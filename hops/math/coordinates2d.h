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

// A class for storing and manipulating coordinates in two dimensions.
// See coordinates.h for more information.

#ifndef ENERGY_REC_UTIL_COORDINATES2D_H_
#define ENERGY_REC_UTIL_COORDINATES2D_H_

using namespace std;
#include "math/coordinates.h"

namespace energy_rec {

// Coordinate systems are enumerated here; kCartesian = 0 is reserved:
const CoordinateSystem kPolar       = 1;  // (r, theta)
const int kNumberOf2DSystems        = 2;

// The 2D coordinate class
class Coordinates2D : public Coordinates<kNumberOf2DSystems, 2> {
 public:
  // A nullary constructor which sets the coordinates to the origin:
  Coordinates2D() : SelfType() {}
  // A constructor which sets the coordinates to a given system and tuple:
  Coordinates2D(const CoordinateSystem system, const FloatType value[2])
      : SelfType(system, value) {}
  // Subtraction constructor
  Coordinates2D(const Coordinates2D &initial_point,
                const Coordinates2D &terminal_point)
      : SelfType(initial_point, terminal_point) {}
  virtual ~Coordinates2D() {}
  // Overridden arithmetic methods:
  virtual FloatType EuclideanNorm(void) const;
 protected:
  virtual void Convert(CoordinateSystem from_system, const FloatType from[2],
                       CoordinateSystem to_system, FloatType to[2]) const {
    k2DConversions[from_system][to_system](from, to);
  }
 private:
  static const ConvertFunction
      k2DConversions[kNumberOf2DSystems][kNumberOf2DSystems];
};

static const Coordinates2D k2DNullVector;

// arrays for standard basis vectors:
static const FloatType k2DXAxisArray[2] = { 1.0, 0.0 };
static const FloatType k2DYAxisArray[2] = { 0.0, 1.0 };

// standard basis vectors:
static const Coordinates2D k2DXAxis(kCartesian, k2DXAxisArray);
static const Coordinates2D k2DYAxis(kCartesian, k2DYAxisArray);

}  // namespace energy_rec

#endif  // ENERGY_REC_UTIL_COORDINATES2D_H_
