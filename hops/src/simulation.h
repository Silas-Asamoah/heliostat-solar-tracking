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

// Simulation is a class for reading a text FieldLayout protobuf from disk,
// instantiating the necessary Optic, OpticContainer, and
// OpticContainerConnection objects, running the optical simulation, and
// collating and outputting the results.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_SIMULATION_H_
#define ENERGY_REC_OPTICAL_SIMULATION_SIMULATION_H_

using namespace std;
#include <cstddef>                      // for NULL
#include <string>                       // for string
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationOutput, etc
#include "math/coordinates.h"  // for FloatType
#include "math/polygons.h"   // for Polygon

namespace energy_rec {

class Field;
class Heliostat;
class OpticContainerConnection;
class Receiver;
class Sky;
class Sun;
class Coordinates2D;
class Coordinates3D;

class Simulation {
 public:
  Simulation()
      : sky_(NULL), field_(NULL), receiver_(NULL),
        sky_to_field_(NULL), field_to_receiver_(NULL) {}
  virtual ~Simulation();

  bool LoadConfigurationFromFile(const string &filename);

  void Simulate(SimulationTime timestamp);

  void WriteOutput(void) const;

 protected:
  Sun * AddAutomaticSun(const Coordinates3D &field_location);
  Sun * AddManualSun(const vector<Coordinates3D> &sun_positions,
                     const vector<FloatType> &dnis);
  static void MakeMirrorRectangle(FloatType width, FloatType height,
                                  Polygon *polygon);
  Heliostat * AddHeliostat(const Coordinates3D &location,
                           const Coordinates2D &projection,
                           const Coordinates3D &frame_orientation,
                           const Coordinates3D &aim,
                           const Sun *sun,
                           FloatType reflectivity,
                           FloatType focal_length,
                           int actuation,
                           const Polygon &mirror_polygon);

 private:
  SimulationOutput output_;
  Sky *sky_;
  Field *field_;
  Receiver *receiver_;
  OpticContainerConnection *sky_to_field_;
  OpticContainerConnection *field_to_receiver_;
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_SIMULATION_H_
