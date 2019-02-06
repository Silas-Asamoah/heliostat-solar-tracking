// Copyright 2010-2011 Google Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: tpw@google.com (Tamsyn Waterhouse)
// TODO(tpw):  Unittests!

#include "config.h"
using namespace std;
#include "src/simulation.h"

#include <algorithm>                    // for sort
#include <cstddef>                      // for NULL
#include <map>                          // for _Rb_tree_const_iterator, etc
#include <utility>                      // for pair
#include <vector>                       // for vector, etc

#include <glog/logging.h>               // for operator<<, Check_EQImpl, etc
#include "src/aperture.h"  // for Aperture
#include "src/field.h"  // for Field
#include "src/field_layout.pb.h"
#include "src/heliostat.h"  // for Heliostat
#include "src/io.h"
#include "src/optic_container_connection.h"
#include "src/receiver.h"  // for Receiver, etc
#include "src/receiver_surfaces.h"
#include "src/sky.h"  // for Sky
#include "src/sun.h"  // for Sun
#include "src/sun_calculators.h"  // for SOLPOS, etc
#include "math/coordinates2d.h"  // for Coordinates2D
#include "math/coordinates3d.h"  // for Coordinates3D
#include "math/polygons.h"   // for Polygon, Polygons

namespace energy_rec {

// ~Simulation
Simulation::~Simulation(void) {
  delete sky_;
  delete field_;
  delete receiver_;
  delete sky_to_field_;
  delete field_to_receiver_;
}

// LoadConfigurationFromFile
//   Resets the simulation and sets up a sky, field, and receiver based on the
//   contents of the text protobuf file specified by filename.  Returns true if
//   the file was read successfully, false otherwise.
bool Simulation::LoadConfigurationFromFile(const string &filename) {
  // Delete any existing owned objects:
  delete sky_;
  delete field_;
  delete receiver_;
  delete sky_to_field_;
  delete field_to_receiver_;

  // Load the field layout from the specified file:
  FieldLayout layout;
  LoadFieldLayoutFile(filename, &layout);

  // Sky:
  sky_ = new Sky();
  // TODO(tpw):  Move the Vector message to its own protobuf in energy/rec/util,
  //             and add constructors to the Coordinates classes that use it.
  Sun *sun;
  if (layout.sun().automatic()) {
    // Use the field_location message and an automatic sun calculator:
    CHECK(layout.sun().has_field_location()) << "field_location not specified";
    const Coordinates3D field_location(
        layout.sun().field_location().coordinate_system(),
        layout.sun().field_location().coordinate().data());
    sun = AddAutomaticSun(field_location);
  } else {
    // Use the sun_position messages:
    vector<Coordinates3D> sun_positions;
    vector<FloatType> dnis;
    for (int i = 0; i < layout.sun().sun_config_size(); ++i) {
      const Coordinates3D sun_vector(
          layout.sun().sun_config(i).sun_vector().coordinate_system(),
          layout.sun().sun_config(i).sun_vector().coordinate().data());
      sun_positions.push_back(sun_vector);
      dnis.push_back(layout.sun().sun_config(i).dni());
    }
    sun = AddManualSun(sun_positions, dnis);
  }

  // Field:
  field_ = new Field();
  // Heliostat blocks:
  for (int i = 0; i < layout.heliostat_block_size(); ++i) {
    Coordinates3D start(layout.heliostat_block(i).start().coordinate_system(),
                        layout.heliostat_block(i).start().coordinate().data());
    Coordinates3D x_step(
        layout.heliostat_block(i).x_step().coordinate_system(),
        layout.heliostat_block(i).x_step().coordinate().data());
    Coordinates3D y_step(
        layout.heliostat_block(i).y_step().coordinate_system(),
        layout.heliostat_block(i).y_step().coordinate().data());
    Coordinates3D aim(layout.heliostat_block(i).aim().coordinate_system(),
                      layout.heliostat_block(i).aim().coordinate().data());
    Coordinates3D frame_orientation;
    if (layout.heliostat_block(i).has_frame_orientation()) {
      FloatType alpha = layout.heliostat_block(i).frame_orientation().alpha();
      FloatType beta = layout.heliostat_block(i).frame_orientation().beta();
      FloatType gamma = layout.heliostat_block(i).frame_orientation().gamma();
      frame_orientation.EulerZXZToRotation(alpha, beta, gamma);
    }
    const FloatType reflectivity =
        layout.heliostat_block(i).heliostat_type().reflectivity();
    const FloatType focal_length =
        layout.heliostat_block(i).heliostat_type().focal_length();
    const int actuation =
        layout.heliostat_block(i).heliostat_type().actuation();
    Polygon mirror_polygon;
    Coordinates2D vertex;
    for (int j = 0;
         j < layout.heliostat_block(i).heliostat_type().vertex_size(); ++j) {
      vertex.Set(kCartesian, 0,
                 layout.heliostat_block(i).heliostat_type().vertex(j).x());
      vertex.Set(kCartesian, 1,
                 layout.heliostat_block(i).heliostat_type().vertex(j).y());
      mirror_polygon.push_back(vertex);
    }
    if (mirror_polygon.empty()) {
      // No vertices specified, so use width x height:
      MakeMirrorRectangle(layout.heliostat_block(i).heliostat_type().width(),
                          layout.heliostat_block(i).heliostat_type().height(),
                          &mirror_polygon);
    }
    for (int j = 0; j <= layout.heliostat_block(i).num_x_steps(); ++j) {
      for (int k = 0; k <= layout.heliostat_block(i).num_y_steps(); ++k) {
        Coordinates3D location(start);
        location.LinearCombination(1.0, 1.0 * j, x_step);
        location.LinearCombination(1.0, 1.0 * k, y_step);
        const FloatType projection_value[2] = {
          location.Get(kCartesian, 0),
          location.Get(kCartesian, 1)
        };
        const Coordinates2D projection(kCartesian, projection_value);
        AddHeliostat(location, projection, frame_orientation, aim, sun,
                     reflectivity, focal_length, actuation, mirror_polygon);
      }
    }
  }
  // Individual heliostats:
  for (int i = 0; i < layout.heliostat_size(); ++i) {
    Coordinates3D location(layout.heliostat(i).location().coordinate_system(),
                           layout.heliostat(i).location().coordinate().data());
    Coordinates3D aim(layout.heliostat(i).aim().coordinate_system(),
                      layout.heliostat(i).aim().coordinate().data());
    const FloatType projection_value[2] = {
      location.Get(kCartesian, 0),
      location.Get(kCartesian, 1)
    };
    const Coordinates2D projection(kCartesian, projection_value);
    Coordinates3D frame_orientation;
    if (layout.heliostat(i).has_frame_orientation()) {
      FloatType alpha = layout.heliostat(i).frame_orientation().alpha();
      FloatType beta = layout.heliostat(i).frame_orientation().beta();
      FloatType gamma = layout.heliostat(i).frame_orientation().gamma();
      frame_orientation.EulerZXZToRotation(alpha, beta, gamma);
    }
    const string name = layout.heliostat(i).name();  // TODO(tpw):  Use this!
    const FloatType reflectivity =
        layout.heliostat(i).heliostat_type().reflectivity();
    const FloatType focal_length =
        layout.heliostat(i).heliostat_type().focal_length();
    const FloatType actuation =
       layout.heliostat(i).heliostat_type().actuation();
    Polygon mirror_polygon;
    Coordinates2D vertex;
    for (int j = 0;
         j < layout.heliostat(i).heliostat_type().vertex_size(); ++j) {
      vertex.Set(kCartesian, 0,
                 layout.heliostat(i).heliostat_type().vertex(j).x());
      vertex.Set(kCartesian, 1,
                 layout.heliostat(i).heliostat_type().vertex(j).y());
      mirror_polygon.push_back(vertex);
    }
    if (mirror_polygon.empty()) {
      // No vertices specified, so use width x height:
      MakeMirrorRectangle(layout.heliostat(i).heliostat_type().width(),
                          layout.heliostat(i).heliostat_type().height(),
                          &mirror_polygon);
    }
    AddHeliostat(location, projection, frame_orientation, aim, sun,
                 reflectivity, focal_length, actuation, mirror_polygon);
  }

  // Aperture:
  Aperture *aperture = NULL;
  if (layout.has_aperture()) {
    const Coordinates3D location(
        layout.aperture().location().coordinate_system(),
        layout.aperture().location().coordinate().data());
    const Coordinates3D direction(
        layout.aperture().normal().coordinate_system(),
        layout.aperture().normal().coordinate().data());
    const FloatType radius(layout.aperture().radius());
    aperture = new Aperture(location, direction, radius);
  }

  // Receiver:
  Coordinates3D rec_location(layout.receiver().location().coordinate_system(),
                             layout.receiver().location().coordinate().data());
  Coordinates3D rec_direction(
      layout.receiver().direction().coordinate_system(),
      layout.receiver().direction().coordinate().data());
  ParametricSurface surface = FindSurfaceFunction(layout.receiver().type());
  CHECK(surface) << "Unknown receiver type: " << layout.receiver().type();
  receiver_ = new Receiver(layout.receiver().resolution(),
                           layout.receiver().resolution(),
                           surface, rec_location, rec_direction,
                           aperture);

  // Connect sky to field:
  sky_to_field_ = new OpticContainerConnection(sky_, field_);

  // Connect field to receiver:
  field_to_receiver_ = new OpticContainerConnection(field_, receiver_);

  return true;
}

// Simulate
//   Updates all Optics in the simulation to the time given by timestamp,
//   calculates the optical flux from sky_ to field_ and from field_ to
//   receiver_, and records the output to output_.
void Simulation::Simulate(SimulationTime timestamp) {
  // Let Optic elements do their things:
  if (sky_) {
    sky_->UpdateSelf(timestamp);
  }
  if (field_) {
    field_->UpdateSelf(timestamp);
  }
  if (receiver_) {
    receiver_->UpdateSelf(timestamp);
  }

  // Iterate the simulation:
  if (sky_to_field_) {
    sky_to_field_->Simulate(timestamp);
  }
  if (field_to_receiver_) {
    field_to_receiver_->Simulate(timestamp);
  }

  // Record results in output_:
  LOG(INFO) << "Getting output.";
  sky_->GetOutput(timestamp, &output_);
  field_->GetOutput(timestamp, &output_);
  receiver_->GetOutput(timestamp, &output_);
}

// WriteOutput
//   Dumps output_ in gnuplot-friendly form to files named after output_'s
//   labels (which are, in turn, specified by command-line flags).
// TODO(tpw):  Switch to streaming write each simulation step.
void Simulation::WriteOutput(void) const {
  for (SimulationOutput::const_iterator it1 = output_.begin();
       it1 != output_.end(); ++it1) {
    OutputFile file(it1->first);
    bool has_previous_time = false;
    SimulationTime simulation_time = kInitialTimeStamp;
    for (OutputTimeSeries::const_iterator it2 = it1->second.begin();
         it2 != it1->second.end(); ++it2) {
      if (has_previous_time && simulation_time != it2->first) {
        // Write two newlines to separate data sets:
        file.WriteDataSetSeparator();
      }
      has_previous_time = true;
      simulation_time = it2->first;
      FloatType previous_row;
      FloatType previous_col;
      bool previous_row_set = false;
      bool previous_col_set = false;
      // Sort it2->second by y and then x:
      OutputVector sorted_vector(it2->second);
      sort(sorted_vector.begin(), sorted_vector.end(), OutputPointComp);
      for (OutputVector::const_iterator it3 = sorted_vector.begin();
           it3 != sorted_vector.end(); ++it3) {
        const FloatType row = it3->location.Get(kCartesian, 0);
        const FloatType col = it3->location.Get(kCartesian, 1);
        if (previous_row_set && previous_col_set &&
            previous_row != row && previous_col != col) {
          // Write a newline to separate rows/columns:
          file.WriteDataBlockSeparator();
        }
        // Write the actual record:
        file.WriteRecord(simulation_time,
                         it3->location.Get(kCartesian, 0),
                         it3->location.Get(kCartesian, 1),
                         it3->value,
                         it3->units);
        previous_row = row;
        previous_row_set = true;
        previous_col = col;
        previous_col_set = true;
      }
    }
  }
}

// AddAutomaticSun
//   Adds a Sun to sky_ and returns a pointer to the newly-created object.
Sun * Simulation::AddAutomaticSun(const Coordinates3D &field_location) {
  Sun *sun = new Sun(new SOLPOS(field_location));
  sky_->AddMember(sun);
  return sun;
}

// AddManualSun
//   Adds a Sun to sky_ and returns a pointer to the newly-created object.
Sun * Simulation::AddManualSun(const vector<Coordinates3D> &sun_positions,
                               const vector<FloatType> &dnis) {
  Sun *sun = new Sun(new SunPositionList(sun_positions, dnis));
  sky_->AddMember(sun);
  return sun;
}

// MakeMirrorRectangle
//   Populates polygon with a rectangle of the desired width and height,
//   centered at the origin.
void Simulation::MakeMirrorRectangle(FloatType width, FloatType height,
                                     Polygon *polygon) {
  const FloatType a_array[] = {  width / 2.0,  height / 2.0 };
  const FloatType b_array[] = { -width / 2.0,  height / 2.0 };
  const FloatType c_array[] = { -width / 2.0, -height / 2.0 };
  const FloatType d_array[] = {  width / 2.0, -height / 2.0 };
  const Coordinates2D a(kCartesian, a_array);
  const Coordinates2D b(kCartesian, b_array);
  const Coordinates2D c(kCartesian, c_array);
  const Coordinates2D d(kCartesian, d_array);
  polygon->clear();
  polygon->push_back(a);
  polygon->push_back(b);
  polygon->push_back(c);
  polygon->push_back(d);
}

// AddHeliostat
//   Adds a Heliostat to field_ and returns a pointer to the newly-created
//   object.
Heliostat * Simulation::AddHeliostat(const Coordinates3D &location,
                                     const Coordinates2D &projection,
                                     const Coordinates3D &frame_orientation,
                                     const Coordinates3D &aim,
                                     const Sun *sun,
                                     FloatType reflectivity,
                                     FloatType focal_length,
                                     int actuation,
                                     const Polygon &mirror_polygon) {
  // Upgrade the one-component vertex list Polygon to a Polygons object:
  const Polygons mirror_polygons(mirror_polygon);
  // Create the heliostat and set it up:
  Heliostat *heliostat = new Heliostat(location, projection, frame_orientation,
                                       reflectivity, focal_length, actuation,
                                       mirror_polygons);
  heliostat->SetAimPoint(aim);
  heliostat->StartTracking(sun);
  // Add it to its OpticContainer (field_) and return:
  field_->AddMember(heliostat);
  return heliostat;
}

}  // namespace energy_rec
