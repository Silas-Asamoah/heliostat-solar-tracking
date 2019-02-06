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

#include "config.h"
using namespace std;
#include "math/coordinates2d.h"

#include <cmath>                       // for atan2, cos, sin, sqrt
#include <cstddef>                     // for NULL

namespace energy_rec {


///////////////////////////////////
// Overridden arithmetic methods //
///////////////////////////////////

FloatType Coordinates2D::EuclideanNorm(void) const {
  return Get(kPolar, 0);
}


//////////////////////////
// Conversion functions //
//////////////////////////

void CartesianToPolar(const FloatType x[2], FloatType y[2]) {
  y[0] = sqrt(x[0] * x[0] + x[1] * x[1]);
  if (y[0] > 0.0) {
    y[1] = atan2(x[1], x[0]);
  } else {
    y[1] = 0.0;
  }
}

void PolarToCartesian(const FloatType x[2], FloatType y[2]) {
  y[0] = x[0] * cos(x[1]);
  y[1] = x[0] * sin(x[1]);
}

// Table of conversion functions:
const Coordinates2D::ConvertFunction
    Coordinates2D::k2DConversions[kNumberOf2DSystems][kNumberOf2DSystems] = {
  {NULL, &CartesianToPolar},
  {&PolarToCartesian, NULL}
};

}  // namespace energy_rec
