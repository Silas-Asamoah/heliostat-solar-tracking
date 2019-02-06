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

#include <cstdio>

#include "math/coordinates.h"  // for FloatType, kCartesian
#include "math/coordinates3d.h"  // for Coordinates3D, k3DZAxis

#include "gtest/gtest.h"

namespace energy_rec {

const FloatType kTestArrays[][3] = {
  {  1.0,  0.0,  0.0 },
  {  0.0,  1.0,  0.0 },
  {  0.0,  0.0,  1.0 },
  { -1.0,  0.0,  0.0 },
  {  0.0, -1.0,  0.0 },
  {  0.0,  0.0, -1.0 },
  {  1.0,  3.2, -3.5 },
  {  0.5, -0.2,  1.0 },
  { -1.2,  2.0,  0.2 }
};
const int kNumberOfTestPoints = sizeof(kTestArrays) / sizeof(kTestArrays[0]);

class HeliostatActuationTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    for (int i = 0; i < kNumberOfTestPoints; ++i) {
      coordinates_[i].Set(kCartesian, kTestArrays[i]);
    }
  }

  Coordinates3D coordinates_[kNumberOfTestPoints];
};

TEST_F(HeliostatActuationTest, TestCorrectNormal) {
  char buffer[100];
  Coordinates3D normal, axis_angle;
  for (int actuation = 0;
       actuation < FieldLayout::HeliostatType::Actuation_ARRAYSIZE;
       ++actuation) {
    snprintf(buffer, sizeof(buffer), "Testing actuation method %i", actuation);
    SCOPED_TRACE(buffer);
    for (int i = 0; i < kNumberOfTestPoints; ++i) {
      snprintf(buffer, sizeof(buffer), "Testing point %i", i);
      SCOPED_TRACE(buffer);
      normal.CopyFrom(coordinates_[i]);
      // Get the axis_angle for this normal-actuation pair:
      HeliostatActuation::kActuationFunctions[actuation](normal, &axis_angle);
      // Rotate normal by the reverse of axis_angle...
      normal.RodriguesRotation(-axis_angle.EuclideanNorm(), axis_angle);
      // ...and check that it is now lying along the Z-axis:
      EXPECT_NEAR(normal.EuclideanNorm(),
                  Coordinates3D::DotProduct(normal, k3DZAxis),
                  kFloatTypeError);
    }
  }
}

// TODO(tpw):  Add tests to check that the mirror orientation is actually
//             correct.

TEST_F(HeliostatActuationTest, TestZeroNormal) {
  char buffer[100];
  Coordinates3D zero, axis_angle;
  for (int actuation = 0;
       actuation < FieldLayout::HeliostatType::Actuation_ARRAYSIZE;
       ++actuation) {
    snprintf(buffer, sizeof(buffer), "Testing actuation method %i", actuation);
    SCOPED_TRACE(buffer);
    // Call the actuation function with zero norm, expecting the DCHECK to fail:
    EXPECT_DEATH(
        HeliostatActuation::kActuationFunctions[actuation](zero, &axis_angle),
        "zero");
  }
}

}  // namespace energy_rec
