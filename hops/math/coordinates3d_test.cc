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
#include "math/coordinates3d.h"

#include <cmath>                         // for sqrt, M_PI, atan2, acos, etc

#include <glog/logging.h>                // for LOG
#include "math/quaternion.h"  // for Quaternion

#include "gtest/gtest.h"

namespace energy_rec {

const int kDimensions = 3;
const int kNumberOfSystems = kNumberOf3DSystems;

const FloatType kTestArrays[][kNumberOfSystems][kDimensions] = {
  {
    // Used by TestReflect and TestRodriguesRotation:
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0}
  }, {
    // Used by TestReflect, TestCosine, TestLineHyperplaneIntersection,
    // TestRodriguesRotation, and TestComposeRotation:
    {1.0, 2.0, -1.0},
    {sqrt(5.0), atan2(2.0, 1.0), -1.0},
    {sqrt(6.0), M_PI / 2.0 - atan2(-1.0, sqrt(5.0)), atan2(2.0, 1.0)},
    {sqrt(6.0), 26.56505117707799, -24.094842552110705}
  }, {
    // Used by TestReflect, TestCosine, TestLineHyperplaneIntersection, and
    // TestRodriguesRotation:
    {3.0, -2.0, 1.0},
    {sqrt(13.0), atan2(-2.0, 3.0), 1.0},
    {sqrt(14.0), M_PI / 2.0 - atan2(1.0, sqrt(13.0)), atan2(-2.0, 3.0)},
    {sqrt(14.0), 123.69006752597979, 15.501359566937},
  }, {
    // Used by TestReflect, TestLineHyperplaneIntersection, and
    // TestRodriguesRotation:
    {1.0, 1.0, 0.0},
    {sqrt(2.0), M_PI / 4.0, 0.0},
    {sqrt(2.0), M_PI / 2.0, M_PI / 4.0},
    {sqrt(2.0), 45.0, 0.0},
  }, {
    // Used by TestReflect, TestLineHyperplaneIntersection, and
    // TestRodriguesRotation:
    {0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 90.0}
  }, {
    // Used by TestReflect:
    {2.0, 2.0, 1.0},
    {sqrt(8.0), M_PI / 4.0, 1.0},
    {3.0, acos(1.0 / 3.0), M_PI / 4.0},
    {3.0, 45.0, 19.47122063449069}
  }, {
    {-2.0, 2.0, -1.0},
    {sqrt(8.0), 3.0 * M_PI / 4.0, -1.0},
    {3.0, acos(-1.0 / 3.0), 3.0 * M_PI / 4.0},
    {3.0, 315.0, -19.47122063449069}
  }, {
    {-2.0, -2.0, 1.0},
    {sqrt(8.0), -3.0 * M_PI / 4.0, 1.0},
    {3.0, acos(1.0 / 3.0), -3.0 * M_PI / 4.0},
    {3.0, 225.0, 19.47122063449069}
  }, {
    {2.0, -2.0, -1.0},
    {sqrt(8.0), -M_PI / 4.0, -1.0},
    {3.0, acos(-1.0 / 3.0), -M_PI / 4.0},
    {3.0, 135.0, -19.47122063449069}
  }, {
    {1.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, M_PI / 2.0, 0.0},
    {1.0, 90.0, 0.0}
  }, {
    {0.0, 1.0, 0.0},
    {1.0, M_PI / 2.0, 0.0},
    {1.0, M_PI / 2.0, M_PI / 2.0},
    {1.0, 0.0, 0.0}
  }, {
    {-1.0, 0.0, 0.0},
    {1.0, M_PI, 0.0},
    {1.0, M_PI / 2.0, M_PI},
    {1.0, 270.0, 0.0}
  }, {
    {0.0, -1.0, 0.0},
    {1.0, -M_PI / 2.0, 0.0},
    {1.0, M_PI / 2.0, -M_PI / 2.0},
    {1.0, 180.0, 0.0}
  }, {
    {0.0, 0.0, -1.0},
    {0.0, 0.0, -1.0},
    {1.0, M_PI, 0.0},
    {1.0, 0.0, -90.0}
  }
};
const int kNumberOfTestPoints = sizeof(kTestArrays) / sizeof(kTestArrays[0]);

class Coordinates3DTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    for (int i = 0; i < kNumberOfTestPoints; ++i) {
      coordinates_[i].Set(0, kTestArrays[i][0]);
    }
  }

  Coordinates3D coordinates_[kNumberOfTestPoints];
};

TEST_F(Coordinates3DTest, TestSubtractionConstructor) {
  for (int i = 0; i < kNumberOfTestPoints - 1; ++i) {
    const Coordinates3D coordinates(coordinates_[i], coordinates_[i + 1]);

    EXPECT_NEAR(kTestArrays[i + 1][kCartesian][0] -
                kTestArrays[i][kCartesian][0],
                coordinates.Get(kCartesian, 0),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i + 1][kCartesian][1] -
                kTestArrays[i][kCartesian][1],
                coordinates.Get(kCartesian, 1),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i + 1][kCartesian][2] -
                kTestArrays[i][kCartesian][2],
                coordinates.Get(kCartesian, 2),
                kFloatTypeError);
  }
}

TEST_F(Coordinates3DTest, TestGetWithConversion) {
  // Check that coordinates_ starts at the origin:
  Coordinates3D zero;
  EXPECT_EQ(0.0, zero.Get(kCartesian, 0));
  EXPECT_EQ(0.0, zero.Get(kCartesian, 1));
  EXPECT_EQ(0.0, zero.Get(kCartesian, 2));

  FloatType result[kDimensions];
  // For each point in the test data,
  for (int i = 0; i < kNumberOfTestPoints; i++) {
    // and each pair of coordinate systems,
    for (int j = 0; j < kNumberOfSystems; j++) {
      for (int k = 0; k < kNumberOfSystems; k++) {
        // test the conversion from one system to the other:
        Coordinates3D coordinates(j, kTestArrays[i][j]);
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
          EXPECT_NEAR(kTestArrays[i][k][l], result[l], 2.0 * kFloatTypeError);
        }
      }
    }
  }
}

TEST_F(Coordinates3DTest, TestSetWithConversion) {
  FloatType vec[3] = {2.0, -2.0, 1.0};

  // Start in Cartesian coordinates:
  Coordinates3D coordinates(kCartesian, vec);
  EXPECT_EQ(2.0, coordinates.Get(kCartesian, 0));
  EXPECT_EQ(-2.0, coordinates.Get(kCartesian, 1));
  EXPECT_EQ(1.0, coordinates.Get(kCartesian, 2));

  EXPECT_TRUE(coordinates.Has(kCartesian));
  EXPECT_FALSE(coordinates.Has(kCylindrical));
  EXPECT_FALSE(coordinates.Has(kSpherical));
  EXPECT_FALSE(coordinates.Has(kAstronomical));

  // Change one coordinate (r) in spherical coordinates:
  coordinates.Set(kSpherical, 0, 6.0);

  EXPECT_FALSE(coordinates.Has(kCartesian));
  EXPECT_FALSE(coordinates.Has(kCylindrical));
  EXPECT_TRUE(coordinates.Has(kSpherical));
  EXPECT_FALSE(coordinates.Has(kAstronomical));

  // Convert back to Cartesian:
  EXPECT_NEAR(4.0, coordinates.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(-4.0, coordinates.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(2.0, coordinates.Get(kCartesian, 2), kFloatTypeError);

  EXPECT_TRUE(coordinates.Has(kCartesian));
  EXPECT_FALSE(coordinates.Has(kCylindrical));
  EXPECT_TRUE(coordinates.Has(kSpherical));
  EXPECT_FALSE(coordinates.Has(kAstronomical));

  // Now change one coordinate (z) in cylindrical coordinates:
  coordinates.Set(kCylindrical, 2, 10.0);

  EXPECT_FALSE(coordinates.Has(kCartesian));
  EXPECT_TRUE(coordinates.Has(kCylindrical));
  EXPECT_FALSE(coordinates.Has(kSpherical));
  EXPECT_FALSE(coordinates.Has(kAstronomical));

  // Convert to spherical:
  EXPECT_NEAR(sqrt(132.0), coordinates.Get(kSpherical, 0), kFloatTypeError);
  EXPECT_NEAR(acos(10.0 / sqrt(132.0)), coordinates.Get(kSpherical, 1),
              kFloatTypeError);
  EXPECT_NEAR(-M_PI / 4.0, coordinates.Get(kSpherical, 2), kFloatTypeError);

  EXPECT_FALSE(coordinates.Has(kCartesian));
  EXPECT_TRUE(coordinates.Has(kCylindrical));
  EXPECT_TRUE(coordinates.Has(kSpherical));
  EXPECT_FALSE(coordinates.Has(kAstronomical));

  // Convert to astronomical:
  EXPECT_NEAR(135.0, coordinates.Get(kAstronomical, 1), kFloatTypeError);

  EXPECT_FALSE(coordinates.Has(kCartesian));
  EXPECT_TRUE(coordinates.Has(kCylindrical));
  EXPECT_TRUE(coordinates.Has(kSpherical));
  EXPECT_TRUE(coordinates.Has(kAstronomical));

  // Change the azimuth in astronomical coordinates:
  coordinates.Set(kAstronomical, 1, 45.0);

  EXPECT_FALSE(coordinates.Has(kCartesian));
  EXPECT_FALSE(coordinates.Has(kCylindrical));
  EXPECT_FALSE(coordinates.Has(kSpherical));
  EXPECT_TRUE(coordinates.Has(kAstronomical));

  // Convert to Cartesian:
  EXPECT_NEAR(4.0, coordinates.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(4.0, coordinates.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(10.0, coordinates.Get(kCartesian, 2), kFloatTypeError);

  EXPECT_TRUE(coordinates.Has(kCartesian));
  EXPECT_FALSE(coordinates.Has(kCylindrical));
  EXPECT_FALSE(coordinates.Has(kSpherical));
  EXPECT_TRUE(coordinates.Has(kAstronomical));
}

TEST_F(Coordinates3DTest, TestMultiply) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    VLOG(1) << "Testing i=" << i;
    // Rescale:
    const double kMultiplyFactor = -sqrt(M_PI);
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
    // z:
    EXPECT_NEAR(kMultiplyFactor * kTestArrays[i][kCartesian][2],
                coordinates_[i].Get(kCartesian, 2),
                kFloatTypeError);
    // Cylindrical coordinates:
    // rho:
    EXPECT_NEAR(fabs(kMultiplyFactor) * kTestArrays[i][kCylindrical][0],
                coordinates_[i].Get(kCylindrical, 0),
                kFloatTypeError);
    // phi:
    if (kTestArrays[i][kCartesian][0] != 0.0) {
      EXPECT_NEAR(tan(kTestArrays[i][kCylindrical][1]),
                  tan(coordinates_[i].Get(kCylindrical, 1)),
                  kFloatTypeError);
    }
    // z:
    EXPECT_NEAR(kMultiplyFactor * kTestArrays[i][kCylindrical][2],
                coordinates_[i].Get(kCylindrical, 2),
                kFloatTypeError);
    // Spherical coordinates:
    // r:
    EXPECT_NEAR(fabs(kMultiplyFactor) * kTestArrays[i][kSpherical][0],
                coordinates_[i].Get(kSpherical, 0),
                kFloatTypeError);
    // theta:
    EXPECT_NEAR(sin(kTestArrays[i][kSpherical][1]),
                sin(coordinates_[i].Get(kSpherical, 1)),
                kFloatTypeError);
    // phi:
    if (kTestArrays[i][kCartesian][0] != 0.0) {
      EXPECT_NEAR(tan(kTestArrays[i][kSpherical][2]),
                  tan(coordinates_[i].Get(kSpherical, 2)),
                  kFloatTypeError);
    }
  }
}

TEST_F(Coordinates3DTest, TestNormalize) {
  // Skip coordinates_[0] because it's the zero vector:
  for (int i = 1; i < kNumberOfTestPoints; ++i) {
    coordinates_[i].Normalize();
    EXPECT_NEAR(1.0, coordinates_[i].EuclideanNorm(), kFloatTypeError);
  }
}

TEST_F(Coordinates3DTest, TestLinearCombination) {
  const double kAlpha = 0.5;
  const double kBeta = 1.5;

  for (int i = 0; i < kNumberOfTestPoints - 1; ++i) {
    VLOG(1) << "Testing i=" << i;
    coordinates_[i].LinearCombination(kAlpha, kBeta, coordinates_[i + 1]);

    EXPECT_NEAR(kAlpha * kTestArrays[i][kCartesian][0] +
                kBeta * kTestArrays[i + 1][kCartesian][0],
                coordinates_[i].Get(kCartesian, 0),
                kFloatTypeError);
    EXPECT_NEAR(kAlpha * kTestArrays[i][kCartesian][1] +
                kBeta * kTestArrays[i + 1][kCartesian][1],
                coordinates_[i].Get(kCartesian, 1),
                kFloatTypeError);
    EXPECT_NEAR(kAlpha * kTestArrays[i][kCartesian][2] +
                kBeta * kTestArrays[i + 1][kCartesian][2],
                coordinates_[i].Get(kCartesian, 2),
                kFloatTypeError);
  }
}

TEST_F(Coordinates3DTest, TestAdd) {
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
    EXPECT_NEAR(kTestArrays[i][kCartesian][2] +
                kTestArrays[i + 1][kCartesian][2],
                coordinates_[i].Get(kCartesian, 2),
                kFloatTypeError);
  }
}

TEST_F(Coordinates3DTest, TestSubtract) {
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
    EXPECT_NEAR(kTestArrays[i][kCartesian][2] -
                kTestArrays[i + 1][kCartesian][2],
                coordinates_[i].Get(kCartesian, 2),
                kFloatTypeError);
  }
}

TEST_F(Coordinates3DTest, TestReflect) {
  coordinates_[0].Reflect(coordinates_[1]);
  EXPECT_NEAR(0.0, coordinates_[0].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, coordinates_[0].Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(0.0, coordinates_[0].Get(kCartesian, 2), kFloatTypeError);

  coordinates_[1].Reflect(coordinates_[2]);
  EXPECT_NEAR(13.0 / 7.0, coordinates_[1].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(10.0 / 7.0, coordinates_[1].Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(-5.0 / 7.0, coordinates_[1].Get(kCartesian, 2), kFloatTypeError);

  coordinates_[2].Reflect(coordinates_[3]);
  EXPECT_NEAR(2.0, coordinates_[2].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(-3.0, coordinates_[2].Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(1.0, coordinates_[2].Get(kCartesian, 2), kFloatTypeError);

  coordinates_[3].Reflect(coordinates_[4]);
  EXPECT_NEAR(1.0, coordinates_[3].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(1.0, coordinates_[3].Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(0.0, coordinates_[3].Get(kCartesian, 2), kFloatTypeError);

  coordinates_[4].Reflect(coordinates_[5]);
  EXPECT_NEAR(-4.0 / 9.0, coordinates_[4].Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(-4.0 / 9.0, coordinates_[4].Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(7.0 / 9.0, coordinates_[4].Get(kCartesian, 2), kFloatTypeError);
}

TEST_F(Coordinates3DTest, TestNorms) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    VLOG(1) << "Testing i=" << i;
    EXPECT_NEAR(fabs(kTestArrays[i][kCartesian][0]) +
                fabs(kTestArrays[i][kCartesian][1]) +
                fabs(kTestArrays[i][kCartesian][2]),
                coordinates_[i].PNorm(1.0),
                kFloatTypeError);
    EXPECT_NEAR(kTestArrays[i][kSpherical][0],
                coordinates_[i].EuclideanNorm(),
                kFloatTypeError);
    EXPECT_NEAR(pow(kTestArrays[i][kSpherical][0], 2.0),
                coordinates_[i].EuclideanNormSquared(),
                kFloatTypeError);
    // Exercise the Get(kSpherical, 0) code path in EuclideanNorm():
    (void) coordinates_[i].Get(kSpherical, 1);
    EXPECT_NEAR(kTestArrays[i][kSpherical][0],
                coordinates_[i].EuclideanNorm(),
                kFloatTypeError);
  }
}

TEST_F(Coordinates3DTest, TestDotProduct) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    for (int j = 0; j < kNumberOfTestPoints; ++j) {
      VLOG(1) << "Testing i=" << i << ", j=" << j;
      EXPECT_NEAR(kTestArrays[i][kCartesian][0] *
                  kTestArrays[j][kCartesian][0] +
                  kTestArrays[i][kCartesian][1] *
                  kTestArrays[j][kCartesian][1] +
                  kTestArrays[i][kCartesian][2] *
                  kTestArrays[j][kCartesian][2],
                  Coordinates3D::DotProduct(coordinates_[i], coordinates_[j]),
                  kFloatTypeError);
    }
  }
}

TEST_F(Coordinates3DTest, TestCosine) {
  EXPECT_NEAR(-1.0 / sqrt(21.0),
              Coordinates3D::Cosine(coordinates_[1], coordinates_[2]),
              kFloatTypeError);
}

TEST_F(Coordinates3DTest, TestAngle) {
  EXPECT_NEAR(-1.0 / sqrt(21.0),
              cos(Coordinates3D::Angle(coordinates_[1], coordinates_[2])),
              kFloatTypeError);
}

TEST_F(Coordinates3DTest, TestLineHyperplaneIntersection) {
  const Coordinates3D a(coordinates_[1]);
  const Coordinates3D b(coordinates_[2]);
  const Coordinates3D c(coordinates_[3]);
  const Coordinates3D d(coordinates_[4]);

  // Some simple examples:
  EXPECT_NEAR(1.0, Coordinates3D::LineHyperplaneIntersection(a, b, c, d),
              kFloatTypeError);
  EXPECT_NEAR(0.0, Coordinates3D::LineHyperplaneIntersection(a, b, a, d),
              kFloatTypeError);
  EXPECT_NEAR(2.0, Coordinates3D::LineHyperplaneIntersection(a, b, b, d),
              kFloatTypeError);
  EXPECT_NEAR(-1.0, Coordinates3D::LineHyperplaneIntersection(d, d, c, d),
              kFloatTypeError);
  // Parallel line and plane:
  EXPECT_TRUE(isinf(Coordinates3D::LineHyperplaneIntersection(a, c, b, d)));
  // Line lying within the plane:
  EXPECT_TRUE(isnan(Coordinates3D::LineHyperplaneIntersection(a, c, a, d)));
}

TEST_F(Coordinates3DTest, TestCrossProduct) {
  Coordinates3D cross_product;
  Coordinates3D reverse_product;
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    for (int j = i + 1; j < kNumberOfTestPoints; ++j) {
      VLOG(1) << "Testing i=" << i << ", j=" << j;
      // Calculate X x Y:
      cross_product.CopyFrom(coordinates_[i]);
      cross_product.CrossProduct(coordinates_[j]);
      // Check that X x Y is orthogonal to X and to Y:
      EXPECT_NEAR(Coordinates3D::DotProduct(cross_product, coordinates_[i]),
                  0.0, kFloatTypeError);
      EXPECT_NEAR(Coordinates3D::DotProduct(cross_product, coordinates_[j]),
                  0.0, kFloatTypeError);
      // Check that ||X x Y||^2 + (a . b)^2 = ||A||^2 * ||B||^2:
      FloatType dot_product = Coordinates3D::DotProduct(coordinates_[i],
                                                        coordinates_[j]);
      EXPECT_NEAR(pow(cross_product.EuclideanNorm(), 2.0) +
                  pow(dot_product, 2.0),
                  pow(coordinates_[i].EuclideanNorm(), 2.0) *
                  pow(coordinates_[j].EuclideanNorm(), 2.0),
                  1e-13);
      // Check that X x Y + Y x X = 0:
      reverse_product.CopyFrom(coordinates_[j]);
      reverse_product.CrossProduct(coordinates_[i]);
      reverse_product.LinearCombination(1.0, 1.0, cross_product);
      EXPECT_NEAR(reverse_product.EuclideanNorm(), 0.0, kFloatTypeError);
    }
  }
}

TEST_F(Coordinates3DTest, TestRodriguesRotation) {
  Coordinates3D a(coordinates_[1]);
  Coordinates3D b(coordinates_[2]);
  Coordinates3D c(coordinates_[3]);
  Coordinates3D d(coordinates_[4]);

  // Check that null rotations do nothing:
  a.RodriguesRotation(0.0, d);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(2.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(-1.0, a.Get(kCartesian, 2), kFloatTypeError);
  a.RodriguesRotation(1.0, coordinates_[0]);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(2.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(-1.0, a.Get(kCartesian, 2), kFloatTypeError);

  // Rotate a about d:
  a.RodriguesRotation(M_PI / 2.0, d);
  EXPECT_NEAR(-2.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(-1.0, a.Get(kCartesian, 2), kFloatTypeError);

  // Flip the new a about c:
  a.RodriguesRotation(M_PI, c);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(-2.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 2), kFloatTypeError);

  // Rotate b about d:
  b.RodriguesRotation(M_PI / 2.0, d);
  EXPECT_NEAR(2.0, b.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(3.0, b.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(1.0, b.Get(kCartesian, 2), kFloatTypeError);

  // Flip d about c using the one-parameter form of the method:
  c.Normalize(M_PI);
  d.RodriguesRotation(c);
  EXPECT_NEAR(0.0, d.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, d.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(-1.0, d.Get(kCartesian, 2), kFloatTypeError);
}

TEST_F(Coordinates3DTest, TestComposeRotation) {
  for (int i = 0; i < kNumberOfTestPoints; ++i) {
    for (int j = 0; j < kNumberOfTestPoints; ++j) {
      for (int k = 0; k < kNumberOfTestPoints; ++k) {
        VLOG(1) << "Testing i=" << i << ", j=" << j << ", k=" << k;
        Coordinates3D a(coordinates_[i]);
        Coordinates3D b(coordinates_[j]);
        Coordinates3D c(coordinates_[k]);
        // Rotate a by -c, then -b:
        a.RodriguesRotation(-c.EuclideanNorm(), c);
        a.RodriguesRotation(-b.EuclideanNorm(), b);
        // Compose b and c:
        b.ComposeRotation(c);
        // Rotate a by the composition:
        a.RodriguesRotation(b.EuclideanNorm(), b);
        // Check that we're back where we started:
        EXPECT_NEAR(coordinates_[i].Get(kCartesian, 0), a.Get(kCartesian, 0),
                    kFloatTypeError);
        EXPECT_NEAR(coordinates_[i].Get(kCartesian, 1), a.Get(kCartesian, 1),
                    kFloatTypeError);
        EXPECT_NEAR(coordinates_[i].Get(kCartesian, 2), a.Get(kCartesian, 2),
                    kFloatTypeError);
      }
    }
  }
}

TEST_F(Coordinates3DTest, TestEulerZXZToRotation) {
  Coordinates3D a;

  // Simple cases:
  a.EulerZXZToRotation(1.0, 0.0, 0.0);
  EXPECT_NEAR(0.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 2), kFloatTypeError);
  a.EulerZXZToRotation(0.0, 1.0, 0.0);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(0.0, a.Get(kCartesian, 2), kFloatTypeError);
  a.EulerZXZToRotation(0.0, 0.0, 1.0);
  EXPECT_NEAR(0.0, a.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, a.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(1.0, a.Get(kCartesian, 2), kFloatTypeError);

  // Complex case:
  a.EulerZXZToRotation(1.0, 2.0, 3.0);
  // Check that a has the correct action on the three coordinate axis vectors:
  Coordinates3D x(k3DXAxis);
  Coordinates3D y(k3DYAxis);
  Coordinates3D z(k3DZAxis);
  // First rotate each axis vector according to the zxz scheme:
  x.RodriguesRotation(1.0, z);
  y.RodriguesRotation(1.0, z);
  z.RodriguesRotation(1.0, z);
  x.RodriguesRotation(2.0, x);
  y.RodriguesRotation(2.0, x);
  z.RodriguesRotation(2.0, x);
  x.RodriguesRotation(3.0, z);
  y.RodriguesRotation(3.0, z);
  z.RodriguesRotation(3.0, z);
  // Now rotate them back using a:
  x.RodriguesRotation(-a.EuclideanNorm(), a);
  y.RodriguesRotation(-a.EuclideanNorm(), a);
  z.RodriguesRotation(-a.EuclideanNorm(), a);
  // Now check that we got back to where we started:
  EXPECT_NEAR(1.0, x.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, x.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(0.0, x.Get(kCartesian, 2), kFloatTypeError);
  EXPECT_NEAR(0.0, y.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(1.0, y.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(0.0, y.Get(kCartesian, 2), kFloatTypeError);
  EXPECT_NEAR(0.0, z.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(0.0, z.Get(kCartesian, 1), kFloatTypeError);
  EXPECT_NEAR(1.0, z.Get(kCartesian, 2), kFloatTypeError);
}

TEST_F(Coordinates3DTest, TestQuaternionConversion) {
  // We're not testing the Quaternion class here, so just check that we can
  // convert an arbitrary rotation vector to a quaternion and back again:
  Coordinates3D a, b;
  Quaternion q;
  // Test with a null vector:
  a.RotationToQuaternion(&q);
  a.QuaternionToRotation(q);
  EXPECT_NEAR(0.0, a.EuclideanNormSquared(), kFloatTypeError);
  // Test with a non-null vector:
  a.CopyFrom(coordinates_[1]);
  a.RotationToQuaternion(&q);
  b.QuaternionToRotation(q);
  a.LinearCombination(1.0, -1.0, b);
  EXPECT_NEAR(0.0, a.EuclideanNormSquared(), kFloatTypeError);
}

}  // namespace energy_rec
