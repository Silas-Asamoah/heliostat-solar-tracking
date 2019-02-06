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
#include "math/quaternion.h"

#include "gtest/gtest.h"

namespace energy_rec {

class QuaternionTest : public ::testing::Test {
};

TEST_F(QuaternionTest, QuaternionMultiplyTest) {
  Quaternion one;
  one.Set(kCartesian, 0, 1.0);
  Quaternion i;
  i.Set(kCartesian, 1, 1.0);
  Quaternion j;
  j.Set(kCartesian, 2, 1.0);
  Quaternion k;
  k.Set(kCartesian, 3, 1.0);

  // Check the multiplication rules that define the quaternions:
  //   i^2 == j^2 == k^2 == ijk == -1

  // i^2 == -1:
  Quaternion i_squared(i);
  i_squared.QuaternionMultiply(i);
  i_squared.LinearCombination(1.0, 1.0, one);
  EXPECT_NEAR(0.0, i_squared.EuclideanNormSquared(), kFloatTypeError);

  // j^2 == -1:
  Quaternion j_squared(j);
  j_squared.QuaternionMultiply(j);
  j_squared.LinearCombination(1.0, 1.0, one);
  EXPECT_NEAR(0.0, j_squared.EuclideanNormSquared(), kFloatTypeError);

  // k^2 == -1:
  Quaternion k_squared(k);
  k_squared.QuaternionMultiply(k);
  k_squared.LinearCombination(1.0, 1.0, one);
  EXPECT_NEAR(0.0, k_squared.EuclideanNormSquared(), kFloatTypeError);

  // ijk == -1:
  Quaternion ijk(i);
  ijk.QuaternionMultiply(j);
  ijk.QuaternionMultiply(k);
  ijk.LinearCombination(1.0, 1.0, one);
  EXPECT_NEAR(0.0, ijk.EuclideanNormSquared(), kFloatTypeError);
}

}  // namespace energy_rec
