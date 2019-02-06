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

// A class for storing and manipulating quaternions.

#ifndef ENERGY_REC_UTIL_QUATERNION_H_
#define ENERGY_REC_UTIL_QUATERNION_H_

using namespace std;
#include "math/coordinates.h"

namespace energy_rec {

// The quaternion class:
class Quaternion : public Coordinates<1, 4> {
 public:
  // A nullary constructor which sets the quaternion to zero:
  Quaternion() : SelfType() {}
  // A constructor which sets the coordinates to a given system and tuple:
  explicit Quaternion(const FloatType value[4]) : SelfType(kCartesian, value) {}
  virtual ~Quaternion() {}
  // Extra arithmetic methods:
  void QuaternionMultiply(const Quaternion &b);
 protected:
  virtual void Convert(CoordinateSystem from_system, const FloatType from[4],
                       CoordinateSystem to_system, FloatType to[4]) const {}
};

}  // namespace energy_rec

#endif  // ENERGY_REC_UTIL_QUATERNION_H_
