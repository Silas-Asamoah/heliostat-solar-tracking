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
// TODO(tpw): unittests!

#include "config.h"
using namespace std;
#include "src/heliostat.h"

#include <cstddef>                      // for NULL
#include <cstdlib>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include "src/heliostat_actuation.h"
#include "math/coordinates3d.h"  // for Coordinates3D, etc

DEFINE_string(heliostat_flux_in, "",
              "output filename for heliostat input flux");
DEFINE_string(heliostat_irradiance_in, "",
              "output filename for heliostat input irradiance");
DEFINE_string(heliostat_shadowing, "",
              "output filename for heliostat shadowing");
DEFINE_string(heliostat_blocking, "",
              "output filename for heliostat blocking; "
              "note that this includes blocking at the receiver end, "
              "not just on the field");
DEFINE_string(heliostat_flux_on_target, "",
              "output filename for heliostat flux delivered to receiver");
DEFINE_string(heliostat_efficiency, "",
              "output filename for heliostat efficiency");

DEFINE_double(heliostat_aiming_error, 0.0,
              "Heliostat aiming error (normal sigma)");


DEFINE_int32(heliostat_facets_per_side, 1,
             "Break each heliostat into this-squared many facets; "
             "heliostats must be quadrilateral");

namespace energy_rec {

// An easy Gaussian generator using the central limit theorem:
double RndGaussian(void) {
  // Note:  Because of rand(), this function is not strictly thread-safe.
  FloatType result = -6.0;
  for (int i = 0; i < 12; ++i) {
    result += (FloatType)rand() / (FloatType)RAND_MAX;  // NOLINT (for rand)
  }
  return result;
}

// StartTracking
//   Tell this heliostat to track a given Optic.  UpdateSelf will change this
//   Heliostat's pointing to keep the reflection of the tracked Optic directed
//   in aim_direction_.
void Heliostat::StartTracking(const Optic *optic) {
  tracking_ = optic;
}

// StopTracking
//   Tell this heliostat to stop tracking.  UpdateSelf will do nothing.
void Heliostat::StopTracking(void) {
  tracking_ = NULL;
}

// SetAimPoint
//   Sets a location to which the heliostat attempts to direct the tracked
//   Optic's reflection.
void Heliostat::SetAimPoint(const Coordinates3D &aim_point) {
  aim_direction_.CopyFrom(aim_point);
  aim_direction_.Subtract(location());
}

// UpdateSelf
//   If we are tracking, changes this heliostat's orientation to send the
//   reflection of the tracked Optic in the direction aim_direction_, plus
//   random noise if --heliostat_aiming_error is set.
//   Does nothing if we are not tracking.
void Heliostat::UpdateSelf(SimulationTime when) {
  if (tracking_ != NULL) {
    // Find the direction to the tracked object:
    const Coordinates3D direction_to_tracked_optic(location(),
                                                   tracking_->location());
    // Set the normal to the mean between the direction to
    // the tracked object and the direction to the target:
    Coordinates3D normal(aim_direction_);
    normal.LinearCombination(1.0 / normal.EuclideanNorm(),
                             1.0 / direction_to_tracked_optic.EuclideanNorm(),
                             direction_to_tracked_optic);
    // If aiming error is turned on, bump the heliostat's normal accordingly:
    if (FLAGS_heliostat_aiming_error > 0.0) {
      // Generate a symmetric normally-distributed three-vector, simply as a
      // triple of normally-distributed scalars:
      const FloatType gaussian_triple[3] = {
        RndGaussian(),
        RndGaussian(),
        RndGaussian()
      };
      Coordinates3D error_axis_angle(kCartesian, gaussian_triple);
      // Now use the cross product to get a random direction vector that is
      // orthogonal to normal:
      error_axis_angle.CrossProduct(normal);
      // Finally, rotate normal_ by a normally-distributed random amount about
      // that axis:
      if (error_axis_angle.EuclideanNormSquared() > 0.0) {
        normal.RodriguesRotation(
            FLAGS_heliostat_aiming_error * RndGaussian(), error_axis_angle);
      }
    }
    Coordinates3D axis_angle;
    CalculateAxisAngle(normal, &axis_angle);
    SetAxisAngle(axis_angle, when);
  }

  // Adjust the heliostat's facets for the Heliostat's new position/orientation:
  UpdateFacets();
}

// ClearIncidentLight
//   In addition to calling the supermethod, clears flux_on_target_
void Heliostat::ClearIncidentLight(void) {
  flux_on_target_ = 0.0;
  Optic::ClearIncidentLight();
}

// TransferIrradiance
//   Given a target, a shadowing mask (ie. due to objects near me blocking my
//   view of the target), a blocking mask at the target, and an IncidentLight
//   struct, calculates the normal irradiance due to the IncidentLight from
//   this Heliostat at the target's location.
// Note:  a mask describes the region of an Optic's surface which is _not_
//        blocked or shadowed.
FloatType Heliostat::TransferIrradiance(
    const Optic &target,
    const Polygons &my_mask,
    const Polygons &destination_mask,
    const IncidentLight &incident_light) const {

  // If the target is completely shadowed, the irradiance is zero:
  if (destination_mask.Area() == 0.0) {
    return 0.0;
  }

  // Calculate the reflection for each assumed-infinitesimal mirror facet:
  FloatType irradiance_out = 0.0;  // the sum of irradiance over all facets
  for (vector<HeliostatFacet *>::const_iterator it = facets_.begin();
       it != facets_.end(); ++it) {
    // Calculate the irradiance for this facet (and update flux_on_target_:
    irradiance_out += FacetIrradiance(*(*it),
                                      target,
                                      my_mask,
                                      destination_mask,
                                      incident_light);
  }

  // Return the total amount of irradiance:
  return irradiance_out;
}

// GetOutput
//   Depending on command line flags, appends a subset of the following values
//   to output:
//   - the irradiance incident on the surface of this heliostat
//   - the total flux incident on the surface of this heliostat
//   - the percentage of incoming flux that is shadowed by nearby heliostats
//     before reaching this one
//   - the percentage of outgoing flux from this heliostat that is blocked by
//     other Optics (including Apertures, not just other heliostats)
//   - the flux delivered by this heliostat to any optic that IsATarget()
//   - the percentage of flux incident on the surface of this heliostat that is
//     delivered to any optic that IsATarget()
void Heliostat::GetOutput(SimulationTime sim_time,
                          SimulationOutput *output) const {
  static const double kHundredPercent = 100.0;
  if (!FLAGS_heliostat_irradiance_in.empty()) {
    const OutputPoint output_point = {
      projection(),
      InputIrradiance(),
      "Heliostat input irradiance (W / m^2)"
    };
    (*output)[FLAGS_heliostat_irradiance_in][sim_time].push_back(output_point);
    // TODO(tpw):  Try indexing on timestamp first, then flag value, and test
    //             the effect on performance.  (Thanks lhm!)
  }
  if (!FLAGS_heliostat_flux_in.empty()) {
    const OutputPoint output_point = {
      projection(),
      InputFlux(),
      "Heliostat input flux (W)"
    };
    (*output)[FLAGS_heliostat_flux_in][sim_time].push_back(output_point);
  }
  if (!FLAGS_heliostat_shadowing.empty()) {
    const FloatType total_irradiance = ShadowedIrradiance() + InputIrradiance();
    const FloatType shadow_proportion =
        total_irradiance > 0.0 ? ShadowedIrradiance() / total_irradiance : 1.0;
    // (for 0 irradiance, we set shadow_proportion to 1.0 for plotting
    // friendliness)
    const OutputPoint output_point = {
      projection(),
      kHundredPercent * shadow_proportion,
      "Heliostat shadowing (%)"
    };
    (*output)[FLAGS_heliostat_shadowing][sim_time].push_back(output_point);
  }
  if (!FLAGS_heliostat_blocking.empty()) {
    const FloatType percent_not_blocked = InputFlux() > 0.0 ?
        kHundredPercent * flux_on_target_ / InputFlux() / reflectivity_ : 0.0;
    // (for 0 input flux, we set efficiency to 0.0 for plotting friendliness)
    const OutputPoint output_point = {
      projection(),
      kHundredPercent - percent_not_blocked,
      "Total blocking (%)"
    };
    (*output)[FLAGS_heliostat_blocking][sim_time].push_back(output_point);
  }
  if (!FLAGS_heliostat_flux_on_target.empty()) {
    const OutputPoint output_point = {
      projection(),
      flux_on_target_,
      "Heliostat flux on target (W)"
    };
    (*output)[FLAGS_heliostat_flux_on_target][sim_time].push_back(output_point);
  }
  if (!FLAGS_heliostat_efficiency.empty()) {
    const FloatType efficiency = InputFlux() > 0.0 ?
        kHundredPercent * flux_on_target_ / InputFlux() : 0.0;
    // (for 0 input flux, we set efficiency to 0.0 for plotting friendliness)
    const OutputPoint output_point = {
      projection(),
      efficiency,
      "Heliostat efficiency (%)"
    };
    (*output)[FLAGS_heliostat_efficiency][sim_time].push_back(output_point);
  }
}

// CalculateAxisAngle
//   Determines the rotation vector necessary to bring this Heliostat to a
//   given normal.  This depends on actuation_.
void Heliostat::CalculateAxisAngle(const Coordinates3D &normal,
                                   Coordinates3D *axis_angle) const {
  if (normal.EuclideanNormSquared() == 0.0) {
    // No normal given; just zero the rotation vector and return:
    axis_angle->CopyFrom(k3DNullVector);
    return;
  }
  // Convert normal to the heliostat frame's local coordinates:
  Coordinates3D normal_relative_to_frame(normal);
  normal_relative_to_frame.RodriguesRotation(
      -frame_orientation_.EuclideanNorm(), frame_orientation_);
  // Call the actuation_-appropriate function to convert
  // normal_relative_to_frame to the correct axis_angle:
  HeliostatActuation::kActuationFunctions[actuation_](normal_relative_to_frame,
                                                      axis_angle);
  // Compose axis_angle with frame_orientation_ to get the mirror orientation
  // in global coordinates:
  axis_angle->ComposeRotation(frame_orientation_);
}

// InitializeFacets
//   Populates facet_shapes_ with a subdivision of shape().
//   If --heliostats_facets_per_side is greater than 1 (the trivial
//   subdivision), shape() must be a quadrilateral.
void Heliostat::InitializeFacets(void) {
  // CHECK that we meet the conditions for performing the subdivision:
  CHECK_GT(FLAGS_heliostat_facets_per_side, 0)
      << "Number of facets must be positive";
  CHECK_EQ(shape().ComponentCount(), 1)
      << "Mirror shape must be single-component";

  const int n = FLAGS_heliostat_facets_per_side;

  // Create an (n+1)-by-(n+1) lattice of points within shape():
  map<pair<int, int>, Coordinates2D> lattice_points;
  PopulateLattice(shape().components()[0], n, &lattice_points);

  // Now populate facets_ with quadrilaterals made up of lattice_points:
  PopulateFacets(n, lattice_points);

  // Finally, set up the facets' other fields:
  UpdateFacets();
}

// PopulateLattice
//   Given a quadrilateral and a resolution n, populates an n-by-n lattice of
//   points within the quadrilateral.  The lattice is stored a map from
//   lattice index pairs (i, j) to a point.
void Heliostat::PopulateLattice(const Polygon &quadrilateral,
                                int n,
                                map<pair<int, int>, Coordinates2D> *lattice) {
  // Pick out the four corners of quadrilateral:
  CHECK_EQ(quadrilateral.size(), 4) << "Mirror shape must be quadrilateral";
  const Coordinates2D &a = quadrilateral[0];
  const Coordinates2D &b = quadrilateral[1];
  const Coordinates2D &c = quadrilateral[2];
  const Coordinates2D &d = quadrilateral[3];

  // Create the lattice:
  lattice->clear();
  for (int i = 0; i <= n; ++i) {
    // alpha parametrizes the lines ad and bc:
    const FloatType alpha =
        static_cast<FloatType>(i) / static_cast<FloatType>(n);
    Coordinates2D point_on_ad(a);
    point_on_ad.LinearCombination(1.0 - alpha, alpha, d);
    Coordinates2D point_on_bc(b);
    point_on_bc.LinearCombination(1.0 - alpha, alpha, c);
    for (int j = 0; j <= n; ++j) {
      // beta parametrizes the line between point_on_ad and point_on_bc:
      const FloatType beta =
          static_cast<FloatType>(j) / static_cast<FloatType>(n);
      Coordinates2D lattice_point(point_on_ad);
      lattice_point.LinearCombination(1.0 - beta, beta, point_on_bc);
      (*lattice)[make_pair(i, j)] = lattice_point;
    }
  }
}

// PopulateFacets
//   Given a lattice (as defined by PopulateLattice), populates facets_ using
//   quadrilaterals made up of adjacent lattice points.
void Heliostat::PopulateFacets(
    int n, const map<pair<int, int>, Coordinates2D> &lattice) {
  facets_.clear();
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      HeliostatFacet *facet = new(HeliostatFacet);
      Polygon component;
      component.push_back(lattice.at(make_pair(i, j)));
      component.push_back(lattice.at(make_pair(i, j + 1)));
      component.push_back(lattice.at(make_pair(i + 1, j + 1)));
      component.push_back(lattice.at(make_pair(i + 1, j)));
      DCHECK_GE(Polygons::Area(component), 0.0) << "Negative-area component!";
      facet->shape.CopyFrom(component);
      facet->shape.GetCentroid(&(facet->local_centroid));
      facets_.push_back(facet);
    }
  }
}

// UpdateFacets
//   Moves the heliostat's facets to keep up with the motion of the Heliostat
//   itself.
void Heliostat::UpdateFacets(void) {
  // Calculate the center of curvature, the point to which every point on the
  // mirror surface is normal:
  Coordinates3D center_of_curvature(location());
  center_of_curvature.LinearCombination(1.0, 2.0 * focal_length_, normal());

  // Iterate over all the facets and update their centroids and locations:
  for (vector<HeliostatFacet *>::iterator it = facets_.begin();
       it != facets_.end(); ++it) {
    // Calculate the facet's centroid position in space using my location and
    // orientation:
    LocalToGlobal((*it)->local_centroid, &((*it)->global_centroid));

    // Calculate the facet's normal using center_of_curvature:
    if (focal_length_ > 0.0) {
      const Coordinates3D facet_normal((*it)->global_centroid,
                                       center_of_curvature);
      (*it)->normal.CopyFrom(facet_normal);
    } else {
      // The mirror is flat, so the facet's normal is equal to the Heliostat's:
      (*it)->normal.CopyFrom(normal());
    }
  }
}

// FacetIrradiance
//   Calculates the irradiance on a target from a single facet.
//   Updates the counter flux_on_target_ accordingly.
FloatType Heliostat::FacetIrradiance(
    const HeliostatFacet &facet,
    const Optic &target,
    const Polygons &my_mask,
    const Polygons &destination_mask,
    const IncidentLight &incident_light) const {
  // Calculate the vector from the facet centroid to the target...
  const Coordinates3D vector_to_target(facet.global_centroid,
                                       target.location());
  // ...and the cosine factor at the target:
  const FloatType target_cosine = -Coordinates3D::Cosine(vector_to_target,
                                                         target.normal());
  // We consider illumination only on the front of the target, not the back:
  if (target_cosine <= 0.0) {
    return 0.0;
  }

  // Calculate the reflection of the source's shape in this facet:
  Polygons reflection_shape;
  ReflectSourceShapeInPoint(*(incident_light.source),
                            facet.global_centroid,
                            facet.normal,
                            target,
                            &reflection_shape);

  // If target is edge-on to the Heliostat, numerical vagaries may put the
  // reflection at a point behind the heliostat despite target_cosine being
  // small and positive.  This is a problem only because reflection_shape
  // is then a hole rather than a solid shape, which we must ignore:
  if (reflection_shape.Area() < 0.0) {
    return 0.0;
  }

  // Restrict this Heliostat's mask to the current facet:
  Polygons mask_on_this_facet(my_mask);
  mask_on_this_facet.Intersect(facet.shape);

  // Calculate how much of the beam falls on the target Optic:
  const FloatType reflection_area = reflection_shape.Area();
  reflection_shape.Intersect(destination_mask);
  const FloatType reflection_area_on_target = reflection_shape.Area();
  const FloatType proportion_on_target =
      reflection_area_on_target / reflection_area;

  // Calculate output flux, including shadowing, blocking, and reflectivity:
  const FloatType flux_out =
      incident_light.irradiance * mask_on_this_facet.Area() *
      -Coordinates3D::Cosine(incident_light.direction, normal()) *
      reflectivity_;

  // Calculate how much of this flux strikes target:
  const FloatType flux_on_target = flux_out * proportion_on_target;

  // Count the amount of flux from this Heliostat that strikes an Optic with
  // IsATarget() equal to true; this is how we compute Heliostat efficiency:
  if (target.IsATarget()) {
    flux_on_target_ += flux_on_target;
  }

  // Calculate and return the irradiance at target:
  return flux_on_target / (destination_mask.Area() * target_cosine);
}

// ReflectSourceShapeInPoint
//   Produces a reflection of source's shape in an ideal infinitesimal
//   reflector located at point_location with normal point_normal upon the
//   plane of a given target.  Returns the result in reflected_shape.
void Heliostat::ReflectSourceShapeInPoint(const Optic &source,
                                          const Coordinates3D &point_location,
                                          const Coordinates3D &point_normal,
                                          const Optic &target,
                                          Polygons *reflected_shape) {
  // Define a PolygonsApplyFunction that maps the source (ie. sun) polygon to
  // its image produced by an ideal infinitesimal reflector on the plane of the
  // given target:
  class ReflectionCallback : public PolygonsApplyFunction {
   public:
    ReflectionCallback(const Optic &source,
                       const Coordinates3D &mirror_location,
                       const Coordinates3D &local_normal,
                       const Optic &target) :
      source_(source),
      mirror_location_(mirror_location),
      local_normal_(local_normal),
      target_(target) {}
    virtual ~ReflectionCallback(void) {}
    virtual void ApplyFunction(Coordinates2D *vertex) const {
      // Get this vertex's position in 3D space:
      source_.LocalToGlobal(*vertex, &reflection_vector_);
      // Calculate the vector from the vertex to the mirror:
      reflection_vector_.LinearCombination(-1.0, 1.0, mirror_location_);
      // Reflect that vector in the mirror:
      reflection_vector_.Reflect(local_normal_);
      // Project reflected_direction onto the target:
      const FloatType intersection = Coordinates3D::LineHyperplaneIntersection(
          mirror_location_, reflection_vector_,
          target_.location(), target_.normal());
      reflection_vector_.LinearCombination(intersection, 1.0, mirror_location_);
      // Find the coordinates of the intersection point within the target:
      target_.GlobalToLocal(reflection_vector_, vertex);
    }
   private:
    const Optic &source_;
    const Coordinates3D &mirror_location_;
    const Coordinates3D &local_normal_;
    const Optic &target_;
    mutable Coordinates3D reflection_vector_;
  };

  // Instantiate the above callback, then call it to produce the reflection of
  // source:
  ReflectionCallback callback(source, point_location, point_normal, target);
  reflected_shape->CopyFrom(source.shape());
  reflected_shape->ApplyFunction(callback);
}

}  // namespace energy_rec
