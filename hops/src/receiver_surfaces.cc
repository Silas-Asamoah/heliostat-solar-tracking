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
#include "src/receiver_surfaces.h"

#include <cmath>                        // for M_PI
#include <cstddef>                      // for NULL

#include "math/coordinates.h"  // for FloatType, kCartesian
#include "math/coordinates2d.h"  // for Coordinates2D
#include "math/coordinates3d.h"  // for Coordinates3D, etc

namespace energy_rec {

ParametricSurface FindSurfaceFunction(const string &function_name) {
  if ("flat" == function_name) {
    return &FlatSurface;
  } else if ("alec" == function_name) {
    return &AlecSurface;
  }
  return NULL;
}

void FlatSurface(double u, double v,
                 Coordinates3D *location, Coordinates2D *projection) {
  static const double kReceiverWidth = 3.0;  // x-axis
  static const double kReceiverLength = 3.0;  // y-axis
  const double x = (v - 0.5) * kReceiverWidth;
  const double y = (u - 0.5) * kReceiverLength;
  // This element's location is (x, y, 0.0):
  const FloatType location_array[3] = { x, y, 0.0 };
  location->Set(kCartesian, location_array);
  // Use the equal-area projection (y, x):
  const FloatType projection_array[2] = { y, x };
  projection->Set(kCartesian, projection_array);
}

void AlecSurface(double u, double v,
                 Coordinates3D *location, Coordinates2D *projection) {
  // An example quadratic surface of revolution described by alecbrooks:
  static const double kReceiverLength = 4.0;
  static const double a = 0.0;
  static const double b = 0.0;
  static const double c = -0.084;
  static const double d = 0.0;
  static const double e = 1.68;  // radius at the mouth

  const double z = u * kReceiverLength;
  const double r = ((((a * z) + b) * z + c) * z + d) * z + e;
  // v == 0.0 corresponds to the negative x-axis:
  const double theta = v * 2 * M_PI - M_PI;

  // This element's location is (r, theta, z):
  const FloatType location_array[3] = { r, theta, z };
  location->Set(kCylindrical, location_array);

  // Use the equal-area projection (theta * r, z):
  const FloatType projection_array[2] = { theta * r, z };
  projection->Set(kCartesian, projection_array);
}

}  // namespace energy_rec
