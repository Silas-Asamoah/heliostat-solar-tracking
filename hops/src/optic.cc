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
#include "src/optic.h"

#include <cmath>  // for fabs

#include <glog/logging.h>  // for DCHECK_LE

namespace energy_rec {

///////////
// Optic //
///////////

// Optic
//   This constructor creates an Optic from a vector of Coordinates3D (assumed
//   coplanar) and a vector of Coordinates2D.  The Optic's location is set to be
//   the centroid of the Coordinates3D points, and its normal is taken from the
//   cross product of the first three of these points.  The Optic's 2D
//   projection, intended for use by output logging functions, is set to be the
//   centroid of the Coordinate2D points.
Optic::Optic(const vector<Coordinates3D> &points,
             const vector<Coordinates2D> &projections) {
  DCHECK_GE(points.size(), 3) << "Need at least 3 points";
  DCHECK_EQ(points.size(), projections.size());
  // Set location to the centroid of the input points:
  Coordinates3D location;
  const FloatType weight = 1.0 / points.size();
  for (vector<Coordinates3D>::const_iterator it = points.begin();
       it != points.end(); ++it) {
    location.LinearCombination(1.0, weight, *it);
  }
  SetLocation(location, kInitialTimeStamp);
  // Use the cross product of the vectors from points[1] to points[0] and
  // points[2] as the normal:
  // NOTE:  This establishes a right-hand rule relating the polygon's
  //        perimeter to its normal.
  Coordinates3D normal(points[1], points[2]);
  const Coordinates3D other_vector(points[1], points[0]);
  normal.CrossProduct(other_vector);
  // Initialize axis_angle_ and orientation_timestamp_:
  SetNormal(normal, kInitialTimeStamp);
  // Rotate points backwards by axis_angle_ to get shape_:
  Polygon polygon;

  for (vector<Coordinates3D>::const_iterator it = points.begin();
       it != points.end(); ++it) {
    // Compute this point's location relative to the centroid of the object:
    Coordinates3D rotated_point(location, *it);
    // Rotate this point by the reverse of axis_angle to bring it into the XY
    // plane:
    rotated_point.RodriguesRotation(-axis_angle_.EuclideanNorm(), axis_angle_);
    // Copy the X and Y coordinates to create a polygon point to store the shape
    // of the object:
    Coordinates2D polygon_point;
    polygon_point.Set(kCartesian, 0, rotated_point.Get(kCartesian, 0));
    polygon_point.Set(kCartesian, 1, rotated_point.Get(kCartesian, 1));
    polygon.push_back(polygon_point);
  }
  shape_.CopyFrom(polygon);
  // Calculate the maximum distance from the origin in shape coordinates:
  radius_ = shape_.RadiusFromOrigin();
  // Set projection_ to the centroid of the input projection points:
  for (vector<Coordinates2D>::const_iterator it = projections.begin();
       it != projections.end(); ++it) {
    projection_.LinearCombination(1.0, weight, *it);
  }
}

// Shadow
//   Projects this Optic's shape onto the plane of another Optic, along a line
//   in a given direction.  (Invariant under rescaling or reversing the
//   direction vector.)
void Optic::Shadow(const Coordinates3D &direction,
                   const Optic &target,
                   Polygons *shadow) const {
  // Create a concrete callback from the PolygonsApplyFunction abstract class,
  // defining it to perform the shadow operation on each vertex:
  class ShadowCallback : public PolygonsApplyFunction {
   public:
    ShadowCallback(const Optic &source,
                   const Coordinates3D &direction,
                   const Optic &target) :
      source_(source), direction_(direction), target_(target) {}
    virtual ~ShadowCallback(void) {}
    virtual void ApplyFunction(Coordinates2D *vertex) const {
      // Find the global coordinates of this vertex:
      source_.LocalToGlobal(*vertex, &global_position_);
      // Find the intersection of the ray from this vertex in
      // the given direction with the plane of the target:
      const FloatType intersection = Coordinates3D::LineHyperplaneIntersection(
          global_position_, direction_, target_.location(), target_.normal());
      global_position_.LinearCombination(1.0, intersection, direction_);
      // Find the coordinates of the intersection
      // point in the plane of the target:
      target_.GlobalToLocal(global_position_, vertex);
    }
   private:
    const Optic &source_;
    const Coordinates3D &direction_;
    const Optic &target_;
    mutable Coordinates3D global_position_;
  };

  // Instantiate the callback, then call it to produce a shadow of this Optic:
  ShadowCallback callback(*this, direction, target);
  shadow->CopyFrom(shape());
  shadow->ApplyFunction(callback);

  // Ensure that shadow has the same sign as shape():
  if (shadow->Area() * shape().Area() < 0.0) {
    shadow->Reverse();
  }
}

// SetLocation
//   Changes the Optic's location in space.
void Optic::SetLocation(const Coordinates3D &location, SimulationTime when) {
  location_.CopyFrom(location);
  location_timestamp_ = when;
}

// SetAxisAngle
//   Changes the orientation of this Optic by changing its orientation axis and
//   angle.  The input vector axis_angle gives the axis, and its magnitude
//   gives the angle.
void Optic::SetAxisAngle(const Coordinates3D &axis_angle, SimulationTime when) {
  axis_angle_.CopyFrom(axis_angle);
  normal_.CopyFrom(k3DZAxis);
  normal_.RodriguesRotation(axis_angle.EuclideanNorm(), axis_angle);
  orientation_timestamp_ = when;
}

// SetNormal
//   Changes the orientation of this Optic by changing its normal vector.
//   Normalizes the input vector if necessary.
void Optic::SetNormal(const Coordinates3D &normal, SimulationTime when) {
  normal_.CopyFrom(normal);
  if (normal_.EuclideanNormSquared() > 0.0) {
    normal_.Normalize();
  }
  axis_angle_.CopyFrom(k3DZAxis);
  const FloatType theta = Coordinates3D::Angle(axis_angle_, normal);
  axis_angle_.CrossProduct(normal);
  if (axis_angle_.EuclideanNormSquared() == 0.0) {
    // Normal is pointing straight down; choose
    // the x-axis for the rotation:
    axis_angle_.CopyFrom(k3DXAxis);
    axis_angle_.Multiply(theta);
  } else {
    // Set the angle:
    axis_angle_.Set(kSpherical, 0, theta);
  }
  orientation_timestamp_ = when;
}

// ChangedLocation
//   Returns true iff this Optic's location has changed at or since the given
//   time.
bool Optic::ChangedLocation(SimulationTime since) const {
  return location_timestamp_ >= since;
}

// ChangedOrientation
//   Returns true iff this Optic's orientation has changed at or since the given
//   time.
bool Optic::ChangedOrientation(SimulationTime since) const {
  return orientation_timestamp_ >= since;
}

// ClearIncidentLight
//   Wipes all of this Optic's IncidentLight packets.
void Optic::ClearIncidentLight(void) {
  incident_light_.clear();
  flux_in_ = 0.0;
}

// ReceiveIncidentLight
//   Accepts an IncidentLight packet.
void Optic::ReceiveIncidentLight(const IncidentLight &incident_light) {
  incident_light_.push_back(incident_light);
  flux_in_ += incident_light.irradiance * incident_light.incident_mask.Area() *
      -Coordinates3D::Cosine(incident_light.direction, normal());
}

// OutputIrradiance
//   Returns the irradiance emitted by this Optic at the location of a given
//   target Optic, subject to a given mask at the point of emission (ie. near
//   this Optic).  Masks are treated as positive; ie., light passes through
//   filled parts of the mask and is blocked by non-filled parts.
FloatType Optic::OutputIrradiance(const Optic &target,
                                  const Polygons &destination_mask,
                                  const Polygons &source_mask) const {
  FloatType irradiance = 0.0;
  for (vector<IncidentLight>::const_iterator it = incident_light_.begin();
       it != incident_light_.end(); ++it) {
    // Compute the intersection of it->incident_mask and source_mask:
    Polygons total_mask(it->incident_mask);
    total_mask.Intersect(source_mask);
    // Calculate the irradiance for this incident_light packet and total_mask:
    irradiance += TransferIrradiance(target, total_mask, destination_mask, *it);
  }
  return irradiance;
}

// ShadowedIrradiance
//   Returns the irradiance directed at this Optic that is shadowed by
//   intervening Optics.
FloatType Optic::ShadowedIrradiance() const {
  FloatType shadowed_irradiance = 0.0;
  for (vector<IncidentLight>::const_iterator it = incident_light_.begin();
       it != incident_light_.end(); ++it) {
    shadowed_irradiance += it->irradiance *
        -Coordinates3D::Cosine(normal(), it->direction) *
        (1.0 - it->incident_mask.Area() / shape().Area());
  }
  return shadowed_irradiance;
}

// LocalToGlobal
//   Converts a Coordinates2D in the shape coordinates of this Optic to a
//   Coordinates3D in space.
void Optic::LocalToGlobal(const Coordinates2D &local,
                          Coordinates3D *global) const {
  // Find the global coordinates of this point:
  const FloatType global_value[3] = {
    local.Get(kCartesian, 0),
    local.Get(kCartesian, 1),
    0.0
  };
  global->Set(kCartesian, global_value);
  global->RodriguesRotation(axis_angle_.EuclideanNorm(), axis_angle_);
  global->Add(location());
}

// GlobalToLocal
//   Converts a Coordinates3D in space to a Coordinates2D in the shape
//   coordinates of this Optic.
void Optic::GlobalToLocal(const Coordinates3D &global,
                          Coordinates2D *local) const {
  Coordinates3D global_mutable(location(), global);
  global_mutable.RodriguesRotation(-axis_angle().EuclideanNorm(),
                                   axis_angle());
  // Verify that global actually lies within the plane of this Optic, subject to
  // a generous rounding tolerance:
  DCHECK_LE(fabs(global_mutable.Get(kCartesian, 2)), 1e-8)
      << "Trying to convert a point that isn't in the Optic's plane";
  // Take advantage of Coordinates::Set to copy the first two elements of
  // global_mutable into local:
  local->Set(kCartesian, global_mutable.Get(kCartesian));
}

// MayBeMaskedBy
//   Returns true iff the specified Optic's radius, projected in the direction
//   direction_in, overlaps the radius of this Optic.
bool Optic::MayBeMaskedBy(const Coordinates3D &direction_in,
                          const Optic *potential_masker) const {
  if (potential_masker == this) {
    // An Optic can't mask itself:
    return false;
  }
  // Calculate the vector from potential_masker to this Optic:
  const Coordinates3D offset(potential_masker->location(), location());
  // Determine whether the projection of a circle around potential_masker in
  // the direction direction_in overlaps a circle around this:
  const FloatType angle = Coordinates3D::Angle(offset, direction_in);
  if (angle > M_PI / 2.0) {
    // potential_masker is behind this Optic, so it cannot mask this:
    return false;
  }
  const FloatType distance = offset.EuclideanNorm() * sin(angle);
  if (distance > radius() + potential_masker->radius()) {
    // The two Optics are too far apart laterally for one to mask the other:
    return false;
  }
  // If we get this far, then masking is possible:
  return true;
}

}  // namespace energy_rec
