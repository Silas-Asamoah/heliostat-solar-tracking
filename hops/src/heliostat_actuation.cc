// Copyright 2011 Google Inc. All Rights Reserved.
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

#include "config.h"
using namespace std;
#include "src/heliostat_actuation.h"

#include <cmath>                        // for M_PI

#include <glog/logging.h>               // for Check_GTImpl, operator<<, etc
#include "math/coordinates.h"  // for FloatType, kCartesian
#include "math/coordinates3d.h"  // for Coordinates3D, k3DXAxis, etc

namespace energy_rec {

// In all cases, the mirror starts out in the (heliostat's) X-Y plane with its
// lower edge facing in the negative Y-direction.

void DirectActuation(const Coordinates3D &normal,
                     Coordinates3D *axis_angle) {
  DCHECK_GT(normal.EuclideanNormSquared(), 0.0) << "Normal cannot be zero";
  // In direct actuation, we simply tilt the mirror from the X-Y plane
  // to attain the desired normal.
  // The amount of rotation is simply the angle between the normal and the
  // Z-axis:
  const FloatType theta = Coordinates3D::Angle(k3DZAxis,
                                               normal);
  // The axis of rotation is orthogonal to the Z-axis and to the normal:
  axis_angle->CopyFrom(k3DZAxis);
  axis_angle->CrossProduct(normal);
  if (axis_angle->EuclideanNormSquared() == 0.0) {
    // The normal is pointing straight down; choose the X-axis for the
    // rotation:
    axis_angle->CopyFrom(k3DXAxis);
    axis_angle->Multiply(theta);
  } else {
    // Set the angle:
    axis_angle->Set(kSpherical, 0, theta);
  }
}

void AziEleActuation(const Coordinates3D &normal,
                     Coordinates3D *axis_angle) {
  DCHECK_GT(normal.EuclideanNormSquared(), 0.0) << "Normal cannot be zero";
  // First, rotate the mirror about the Z-axis to the desired azimuthal
  // direction, including a constant half-pi rotation to align its lower edge
  // with the X-axis, from which the spherical coordinate phi is measured.
  // Then, rotate it about its intrinsic X-axis to the desired elevation,
  // by an amount equal to the spherical colatitude theta.
  axis_angle->EulerZXZToRotation(
      normal.Get(kSpherical, 2) + M_PI / 2.0,
      normal.Get(kSpherical, 1),
      0.0);
}

void PitchRollActuation(const Coordinates3D &normal,
                        Coordinates3D *axis_angle) {
  DCHECK_GT(normal.EuclideanNormSquared(), 0.0) << "Normal cannot be zero";
  // Find the elevation vector.  This vector lies in the intersection of
  // the mirror plane and the pitch actuation plane, pointing towards the
  // back of the heliostat.  We can find it by taking the cross-product of
  // two vectors to which it's orthogonal, namely the normal and the X-axis
  // (remember that we're working in heliostat-frame-local coordinates).
  Coordinates3D elevation_vector(normal);
  elevation_vector.CrossProduct(k3DXAxis);
  if (elevation_vector.EuclideanNormSquared() == 0.0) {
    // The normal lies along the heliostat's X-axis, so we don't need to pitch
    // at all, so we set the elevation_vector along the heliostat's Y-axis
    // (ie. its longitudinal axis):
    elevation_vector.CopyFrom(k3DYAxis);
  }
  // Calculate the pitch rotation vector (a rotation about the heliostat's
  // X-axis) from this:
  FloatType alpha = Coordinates3D::Angle(elevation_vector, k3DYAxis);
  if (elevation_vector.Get(kCartesian, 2) < 0.0) {
    // If the elevation vector's Z component is negative, then the
    // heliostat's pitch is negative; it's angled back "over its shoulder":
    alpha *= -1.0;
  }
  axis_angle->CopyFrom(k3DXAxis);
  axis_angle->Multiply(alpha);
  // Now change coordinates to align the Z-axis with elevation_vector...
  Coordinates3D normal_relative_to_elevation(normal);
  normal_relative_to_elevation.RodriguesRotation(M_PI / 2.0 - alpha, k3DXAxis);
  // ...and calculate the roll, which is now just the phi coordinate of
  // elevation_vector:
  const FloatType beta =
      normal_relative_to_elevation.Get(kSpherical, 2) + M_PI / 2.0;
  // To enact this roll on the heliostat, we rotate by an amount beta about
  // elevation_vector (which is actually the mirror's Y-axis):
  elevation_vector.Normalize(beta);
  axis_angle->ComposeRotation(elevation_vector);
}

// The utility class's member table of actuation functions:
const HeliostatActuation::ActuationFunction
    HeliostatActuation::kActuationFunctions[] = {
  &DirectActuation,
  &AziEleActuation,
  &PitchRollActuation
};

}  // namespace energy_rec
