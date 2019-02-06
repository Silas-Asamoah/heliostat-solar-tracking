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
#include "math/coordinates.h"

#include "gtest/gtest.h"

namespace energy_rec {

// The trivial one-dimensional coordinate class:
class Coordinates1D : public Coordinates<1, 1> {
 public:
  Coordinates1D() : Coordinates<1, 1>() {}
  Coordinates1D(const CoordinateSystem system, const FloatType value[1])
      : Coordinates<1, 1>(system, value) {}
  // Subtraction constructor
  Coordinates1D(const Coordinates1D &initial_point,
                const Coordinates1D& terminal_point)
      : SelfType(initial_point, terminal_point) {}
 protected:
  virtual void Convert(CoordinateSystem from_system, const FloatType from[1],
                       CoordinateSystem to_system, FloatType to[1]) const {}
};

class CoordinatesTest : public ::testing::Test {
};

TEST_F(CoordinatesTest, ConstructorsAndAssignment) {
  FloatType vec[1] = { 2.0 };
  Coordinates1D one(kCartesian, vec);
  Coordinates1D two(one);
  Coordinates1D three;
  Coordinates1D four(three, one);

  EXPECT_NEAR(2.0, one.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(2.0, two.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, three.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(2.0, four.Get(kCartesian, 0), kFloatTypeError);
  three = two;
  EXPECT_NEAR(2.0, three.Get(kCartesian, 0), kFloatTypeError);
}

TEST_F(CoordinatesTest, TestInvert) {
  FloatType vec[1] = { -3.0 };
  Coordinates1D coordinates(kCartesian, vec);
  EXPECT_NEAR(-3.0, coordinates.Get(kCartesian, 0), kFloatTypeError);
  coordinates.Invert();
  EXPECT_NEAR(3.0, coordinates.Get(kCartesian, 0), kFloatTypeError);
}

TEST_F(CoordinatesTest, TestEuclideanNorm) {
  FloatType vec[1] = { -2.0 };
  Coordinates1D coordinates(kCartesian, vec);
  EXPECT_NEAR(4.0, coordinates.EuclideanNormSquared(), kFloatTypeError);
  EXPECT_NEAR(2.0, coordinates.EuclideanNorm(), kFloatTypeError);
}

TEST_F(CoordinatesTest, TestNormalize) {
  FloatType vec[1] = { -2.0 };
  Coordinates1D coordinates(kCartesian, vec);
  coordinates.Normalize();
  EXPECT_NEAR(1.0, coordinates.EuclideanNorm(), kFloatTypeError);
}

}  // namespace energy_rec
