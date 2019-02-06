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

// A class for storing and manipulating coordinates in three dimensions.
// See coordinates.h for more information.

#ifndef ENERGY_REC_UTIL_COORDINATES3D_H_
#define ENERGY_REC_UTIL_COORDINATES3D_H_

using namespace std;
#include "math/coordinates.h"

namespace energy_rec {

// Coordinate systems are enumerated here; kCartesian = 0 is reserved:
const CoordinateSystem kCylindrical   = 1;  // (rho,   phi,   z)
const CoordinateSystem kSpherical     = 2;  // (  r, theta, phi)
const CoordinateSystem kAstronomical  = 3;  // (altitude, azimuth, elevation)
// phi .................... is measured in radians from the x-axis, increasing
//                          towards the y-axis, and is in [-pi, pi].
// theta .................. is inclination (colatitude) measured in radians and
//                          is in [0, pi].
// altitude ............... is the same as r (vector magnitude).
// azimuth (aka longitude)  is measured in degrees from the y-axis, increasing
//                          towards the x-axis, and is in [0, 360].
// elevation (aka latitude) is measured in degrees and is in [-90, 90].
const int kNumberOf3DSystems          = 4;

class Quaternion;  // for rotation representations

// The 3D coordinate class
class Coordinates3D : public Coordinates<kNumberOf3DSystems, 3> {
 public:
  // A nullary constructor which sets the coordinates to the origin:
  Coordinates3D() : SelfType() {}
  // A constructor which sets the coordinates to a given system and tuple:
  Coordinates3D(const CoordinateSystem system, const FloatType value[3])
      : SelfType(system, value) {}
  // Subtraction constructor
  Coordinates3D(const Coordinates3D &initial_point,
                const Coordinates3D &terminal_point)
      : SelfType(initial_point, terminal_point) {}
  virtual ~Coordinates3D() {}
  // Overridden arithmetic methods:
  virtual FloatType EuclideanNorm(void) const;
  // Extra arithmetic methods:
  void CrossProduct(const Coordinates3D &b);
  void RodriguesRotation(const Coordinates3D &axis_angle);
  void RodriguesRotation(FloatType theta, const Coordinates3D &axis);
  void ComposeRotation(const Coordinates3D &other);
  void EulerZXZToRotation(FloatType alpha, FloatType beta, FloatType gamma);
  void RotationToQuaternion(Quaternion *quaternion) const;
  void QuaternionToRotation(const Quaternion &quaternion);
 protected:
  virtual void Convert(CoordinateSystem from_system, const FloatType from[3],
                       CoordinateSystem to_system, FloatType to[3]) const {
    k3DConversions[from_system][to_system](from, to);
  }
 private:
  static const ConvertFunction
      k3DConversions[kNumberOf3DSystems][kNumberOf3DSystems];
};

static const Coordinates3D k3DNullVector;

// arrays for standard basis vectors:
static const FloatType k3DXAxisArray[3] = { 1.0, 0.0, 0.0 };
static const FloatType k3DYAxisArray[3] = { 0.0, 1.0, 0.0 };
static const FloatType k3DZAxisArray[3] = { 0.0, 0.0, 1.0 };

// standard basis vectors:
static const Coordinates3D k3DXAxis(kCartesian, k3DXAxisArray);
static const Coordinates3D k3DYAxis(kCartesian, k3DYAxisArray);
static const Coordinates3D k3DZAxis(kCartesian, k3DZAxisArray);

}  // namespace energy_rec

#endif  // ENERGY_REC_UTIL_COORDINATES3D_H_
