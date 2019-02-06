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

#include <cmath>                        // for M_PI, sqrt, fabs, tan, etc

#include <glog/logging.h>               // for LOG

#include "gtest/gtest.h"

namespace energy_rec {

const int kDimensions = 2;
const int kNumberOfSystems = kNumberOf2DSystems;

const FloatType kTestArrays[][kNumberOfSystems][kDimensions] = {
  {
    // Used by TestReflect and TestLineHyperplaneIntersection:
    {0.0, 0.0},
    {0.0, 0.0}
  }, {
    // Used by TestReflect and TestLineHyperplaneIntersection:
    {2.0, 2.0},
    {sqrt(8.0), M_PI / 4.0}
  }, {
    // Used by TestReflect and TestLineHyperplaneIntersection:
    {-2.0, 2.0},
    {sqrt(8.0), 3.0 * M_PI / 4.0}
  }, {
    // Used by TestReflect and TestLineHyperplaneIntersection:
    {-2.0, -2.0},
    {sqrt(8.0), -3.0 * M_PI / 4.0}
  }, {
    // Used by TestReflect and TestCosine:
    {2.0, -2.0},
    {sqrt(8.0), -M_PI / 4.0}
  }, {
    // Used by TestReflect and TestCosine:
    {1.0, 0.0},
    {1.0, 0.0}
  }, {
    {0.0, 1.0},
    {1.0, M_PI / 2.0}
  }, {
    {-1.0, 0.0},
    {1.0, M_PI}
  }, {
    {0.0, -1.0},
    {1.0, -M_PI / 2.0}
  }
};
const int kNumberOfTestPoints = sizeof(kTestArrays) / sizeof(kTestArrays[0]);

class Coordinates2DTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    for (int i = 0; i < kNumberOfTestPoints; ++i) {
      coordinates_[i].Set(0, kTestArrays[i][0]);
    }
  }

  Coordinates2D coordinates_[kNumberOfTestPoints];
};

TEST_F(Coordinates2DTest, TestSubtractionConstructor) {
  for (int i = 0; i < kNumberOfTestPoints - 1; ++i) {
    const Coordinates2D coordinates(coordinates_[i], coordinates_[i + 1]);

    EXPECT_NEAR(kTestArrays[i + 1][kCartesian][0] -
                kTestArrays[i][kCartesian][0],
                coordinates.Get(kCartesian, 0),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i + 1][kCartesian][1] -
                kTestArrays[i][kCartesian][1],
                coordinates.Get(kCartesian, 1),
                kFloatTypeError);
  }
}

TEST_F(Coordinates2DTest, TestGetWithConversion) {
  // Check that Coordinates2D starts at the origin:
  Coordinates2D zero;
  EXPECT_EQ(0.0, zero.Get(kCartesian, 0));
  EXPECT_EQ(0.0, zero.Get(kCartesian, 1));

  FloatType result[kDimensions];
  // For each point in the test data,
  for (int i = 0; i < kNumberOfTestPoints; i++) {
    // and each pair of coordinate systems,
    for (int j = 0; j < kNumberOfSystems; j++) {
      for (int k = 0; k < kNumberOfSystems; k++) {
        // Test the conversion from one system to the other:
        Coordinates2D coordinates(j, kTestArrays[i][j]);
        // (testing this constructor!)
        if (k != j) {
          // We don't have the coordinates in the new system yet:
          EXPECT_FALSE(coordinates.Has(k));
        }
        coordinates.Get(k, result);
        // And now we do:
        EXPECT_TRUE(coordinates.Has(k));
        // Test the numeric values of the result:
        VLOG(1) << "Testing i=" << i << ", j=" << j << ", k=" << k;
        for (int l = 0; l < kDimensions; l++) {
          EXPECT_NEAR(kTestArrays[i][k][l], result[l], kFloatTypeError);
        }
      }
    }
  }
}

TEST_F(Coordinates2DTest, TestSetWithConversion) {
  FloatType vec[2] = { 3.0, -4.0 };

  // Start in Cartesian coordinates:
  Coordinates2D coordinates(kCartesian, vec);
  EXPECT_EQ(3.0, coordinates.Get(kCartesian, 0));
  EXPECT_EQ(-4.0, coordinates.Get(kCartesian, 1));

  EXPECT_TRUE(coordinates.Has(kCartesian));
  EXPECT_FALSE(coordinates.Has(kPolar));

  // Change one coordinate (r) in polar coordinates:
  coordinates.Set(kPolar, 0, 10.0);

  EXPECT_FALSE(coordinates.Has(kCartesian));
  EXPECT_TRUE(coordinates.Has(kPolar));

  // Convert back to Cartesian:
  EXPECT_NEAR(6.0, coordinates.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(-8.0, coordinates.Get(kCartesian, 1), kFloatTypeError);

  EXPECT_TRUE(coordinates.Has(kCartesian));
  EXPECT_TRUE(coordinates.Has(kPolar));
}

TEST_F(Coordinates2DTest, TestMultiply) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    // Rescale:
    const double kMultiplyFactor = -pow(M_PI, M_PI);
    coordinates_[i].Multiply(kMultiplyFactor);
    // Cartesian coordinates:
    // x:
    EXPECT_NEAR(kMultiplyFactor * kTestArrays[i][kCartesian][0],
                coordinates_[i].Get(kCartesian, 0),
                kFloatTypeError);
    // y:
    EXPECT_NEAR(kMultiplyFactor * kTestArrays[i][kCartesian][1],
                coordinates_[i].Get(kCartesian, 1),
                kFloatTypeError);
    // Polar coordinates:
    // r:
    EXPECT_NEAR(fabs(kMultiplyFactor) * kTestArrays[i][kPolar][0],
                coordinates_[i].Get(kPolar, 0),
                kFloatTypeError);
    // theta:
    if (kTestArrays[i][kCartesian][0] != 0.0) {
      EXPECT_NEAR(tan(kTestArrays[i][kPolar][1]),
                  tan(coordinates_[i].Get(kPolar, 1)),
                  kFloatTypeError);
    }
  }
}

TEST_F(Coordinates2DTest, TestNormalize) {
  // Skip coordinates_[0] because it's the zero vector:
  for (int i = 1; i < kNumberOfTestPoints; ++i) {
    coordinates_[i].Normalize();
    EXPECT_NEAR(1.0, coordinates_[i].EuclideanNorm(), kFloatTypeError);
  }
}

TEST_F(Coordinates2DTest, TestLinearCombination) {
  const double kAlpha = 0.5;
  const double kBeta = 1.5;

  for (int i = 0; i < kNumberOfTestPoints - 1; ++i) {
    coordinates_[i].LinearCombination(kAlpha, kBeta, coordinates_[i + 1]);

    EXPECT_NEAR(kAlpha * kTestArrays[i][kCartesian][0] +
                kBeta * kTestArrays[i + 1][kCartesian][0],
                coordinates_[i].Get(kCartesian, 0),
                kFloatTypeError);
    EXPECT_NEAR(kAlpha * kTestArrays[i][kCartesian][1] +
                kBeta * kTestArrays[i + 1][kCartesian][1],
                coordinates_[i].Get(kCartesian, 1),
                kFloatTypeError);
  }
}

TEST_F(Coordinates2DTest, TestAdd) {
  for (int i = 0; i < kNumberOfTestPoints - 1; ++i) {
    coordinates_[i].Add(coordinates_[i + 1]);

    EXPECT_NEAR(kTestArrays[i][kCartesian][0] +
                kTestArrays[i + 1][kCartesian][0],
                coordinates_[i].Get(kCartesian, 0),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i][kCartesian][1] +
                kTestArrays[i + 1][kCartesian][1],
                coordinates_[i].Get(kCartesian, 1),
                kFloatTypeError);
  }
}


TEST_F(Coordinates2DTest, TestSubtract) {
  for (int i = 0; i < kNumberOfTestPoints - 1; ++i) {
    coordinates_[i].Subtract(coordinates_[i + 1]);

    EXPECT_NEAR(kTestArrays[i][kCartesian][0] -
                kTestArrays[i + 1][kCartesian][0],
                coordinates_[i].Get(kCartesian, 0),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i][kCartesian][1] -
                kTestArrays[i + 1][kCartesian][1],
                coordinates_[i].Get(kCartesian, 1),
                kFloatTypeError);
  }
}

TEST_F(Coordinates2DTest, TestReflect) {
  coordinates_[0].Reflect(coordinates_[1]);
  EXPECT_NEAR(0.0, coordinates_[0].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, coordinates_[0].Get(kCartesian, 1), kFloatTypeError);

  coordinates_[1].Reflect(coordinates_[2]);
  EXPECT_NEAR(2.0, coordinates_[1].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(2.0, coordinates_[1].Get(kCartesian, 1), kFloatTypeError);

  coordinates_[2].Reflect(coordinates_[3]);
  EXPECT_NEAR(-2.0, coordinates_[2].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(2.0, coordinates_[2].Get(kCartesian, 1), kFloatTypeError);

  coordinates_[3].Reflect(coordinates_[5]);
  EXPECT_NEAR(2.0, coordinates_[3].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(-2.0, coordinates_[3].Get(kCartesian, 1), kFloatTypeError);

  coordinates_[5].Reflect(coordinates_[4]);
  EXPECT_NEAR(0.0, coordinates_[5].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(1.0, coordinates_[5].Get(kCartesian, 1), kFloatTypeError);
}

TEST_F(Coordinates2DTest, TestNorms) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    EXPECT_NEAR(fabs(kTestArrays[i][kCartesian][0]) +
                fabs(kTestArrays[i][kCartesian][1]),
                coordinates_[i].PNorm(1.0),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i][kPolar][0],
                coordinates_[i].EuclideanNorm(),
                kFloatTypeError);
  }
}

TEST_F(Coordinates2DTest, TestDotProduct) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    for (int j = 0; j < kNumberOfTestPoints; ++j) {
      EXPECT_NEAR(kTestArrays[i][kCartesian][0] *
                  kTestArrays[j][kCartesian][0] +
                  kTestArrays[i][kCartesian][1] *
                  kTestArrays[j][kCartesian][1],
                  Coordinates2D::DotProduct(coordinates_[i], coordinates_[j]),
                  kFloatTypeError);
    }
  }
}

TEST_F(Coordinates2DTest, TestCosine) {
  EXPECT_NEAR(0.0, Coordinates2D::Cosine(coordinates_[1], coordinates_[2]),
              kFloatTypeError);
  EXPECT_NEAR(-1.0, Coordinates2D::Cosine(coordinates_[1], coordinates_[3]),
              kFloatTypeError);
  EXPECT_NEAR(1.0, Coordinates2D::Cosine(coordinates_[2], coordinates_[2]),
              kFloatTypeError);
  EXPECT_NEAR(1.0 / sqrt(2.0),
              Coordinates2D::Cosine(coordinates_[4], coordinates_[5]),
              kFloatTypeError);
}

TEST_F(Coordinates2DTest, TestAngle) {
  EXPECT_NEAR(M_PI / 2.0,
              Coordinates2D::Angle(coordinates_[1], coordinates_[2]),
              kFloatTypeError);
  EXPECT_NEAR(M_PI, Coordinates2D::Angle(coordinates_[1], coordinates_[3]),
              1e-7);  // Higher tolerance for acos weirdness
}

TEST_F(Coordinates2DTest, TestLineHyperplaneIntersection) {
  const Coordinates2D a(coordinates_[0]);
  const Coordinates2D b(coordinates_[1]);
  const Coordinates2D c(coordinates_[2]);
  const Coordinates2D d(coordinates_[3]);

  EXPECT_NEAR(0.0, Coordinates2D::LineHyperplaneIntersection(a, b, c, d),
              kFloatTypeError);
  EXPECT_NEAR(-1.0, Coordinates2D::LineHyperplaneIntersection(b, b, c, d),
              kFloatTypeError);
  EXPECT_NEAR(1.0, Coordinates2D::LineHyperplaneIntersection(d, b, a, b),
              kFloatTypeError);
  EXPECT_NEAR(2.0, Coordinates2D::LineHyperplaneIntersection(d, b, b, b),
              kFloatTypeError);

  // Near-zero denominator
  //   This is a pathological case discovered by tpw while working on the
  //   RE<C optical flux simulation.
  const FloatType w_array[2] = { 1.0, 0.0 };
  const FloatType x_array[2] = { -1.0, 1.0 };
  const FloatType y_array[2] = { 0.66666666666666663, 0.33333333333333337 };
  const FloatType z_array[2] = { 0.33333333333333337, 0.33333333333333331 };
  const Coordinates2D w(kCartesian, w_array);
  const Coordinates2D x(kCartesian, x_array);
  const Coordinates2D y(kCartesian, y_array);
  const Coordinates2D z(kCartesian, z_array);
  EXPECT_TRUE(isnan(Coordinates2D::LineHyperplaneIntersection(w, x, y, z)));
}

}  // namespace energy_rec
