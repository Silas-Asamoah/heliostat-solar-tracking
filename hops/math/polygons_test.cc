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

// TODO(tpw):  Bring code coverage from 95% to 100%.

// These unittests make use of the following test components:
// TODO(tpw):  Make the test generate this diagram (semi-)automatically.
//
// (0, 9)                                                                 (9, 9)
// ttrrrrrrrrrrrrrrrrrrrrrrppppppppppppppppp       +       +       +       +
// t tt                    p               p
// t   tt                  p              p
// t     tt    rectangle   p   problem_   p
// t       tt              p    shape    p
// t         tt            p             p
// t           tt          p            p
// t             tt        p            p
// trrrrrrrssssssss1sssssssp       p   p   +       +       +       +       +
// t       s      1 1     2p2     pp3  p                     { r = rectangle
// t       s     1   1   2 p 2   p p 3p                      { t = triangle
// t       s    1     1 2  p  2 p  p  p                      { s = square
// t tria  s   1       2   p   p   p p 3    <=== this area:  { 1 = diamond_one
// t  ngle s  1       2 1  p  p 2  p p  3                    { 2 = diamond_two
// t       s 1       2   1 p p   2 pp    3                   { 3 = diamond_three
// t       s1       2     1pp     2pp     3                  { p = problem_shape
// t       1       2       p       p       3       +       +       +       +
// t      ts1       2     1s3     2       3
// t     t s 1       2   1 s 3   2       3
// t    t  s  1       2 1  s  3 2       3
// t   t   s   1       2   s   3       3
// t  t    s    1     1 2  s  2 3     3-diamond_three
// t t     s     1   1   2 s 2   3   3
// tt      s      1 1     2s2     3 3
// t       ssssssss1sssssss2       b       ooooooooooooooooo       +       +
//            |    |       |      b b     o                 o
//       square    |       |     b   b   o                   o
//                 |       |    b     b o                     o
//       diamond_one       |   b       b                       o
//                         |  b       o b                       o
//               diamond_two b       o   b                       o
//                          b       o     b                       o
// BBBBBBBBBBBBBBBBBBBBBBBBb       o       b11111111       +       o       +
// B                       Bb     o       1 b      1                o
// B   this area:          B b   o       1   b     1   this area:    o
// B   L = left_cup        B  b o       1     b    1   o = octagon    o
// B   R = right_cup       B   b       1       b   1   1 = hexagon_one o
// B   B = (both)          B  o b     1         b  1   2 = hexagon_two  o
// B                       B o   b   1           b 1   b = bar           o
// B                       Bo     b 1             b1                      o
// BRRRRRRRBBBBBBBBBLLLLLLLo       b       +       b       +       +       o
// L       L       R       o       1b             1 b                      o
// L       L       R       o       1 b           1   b                     o
// L       L       R       o       1  b         1     b                    o
// L left  L       R right o       1   b       1       b                   o
// L _cup  L       R  _cup o       1    b     1         b                  o
// L       L       R       o       1     b   1           b                 o
// L       L       R       o       1      b 1             b                o
// L       L       R       o       11111111b       +       b22222222       o
// L       L       R       o                b             2 b      2       o
// L       L       R       o                 b           2   b     2       o
// L       L       R       o                  b         2     b    2       o
// L       L       R       o                   b       2       b   2       o
// L       L       R       o                    b     2         b  2       o
// L       L       R       o                     b   2           b 2       o
// L       L       R       o                      b 2             b2       o
// BRRRRRRRBBBBBBBBBLLLLLLLo       +       +       b       +       b       o
// B                       Bo                      2b             2 b     o
// B                       B o                     2 b           2   b   o
// B                       B  o                    2  b         2     b o
// B                       B   o                   2   b       2       b
// B                       B    o                  2    b     2       o b
// B                       B     o                 2     b   2       o   b
// B                       B      o                2      b 2       o     b
// BBBBBBBBBBBBBBBBBBBBBBBBB       o       +       22222222b       o       b
//                                  o                       b     o       b
//                                   o                       b   o       b
//                                    o                       b o       b
//                                     o                       b       b
//                                      o                     o b     b
//                                       o                   o   b   b
//                                        o                 o     b b
// +       +       +       +       +       ooooooooooooooooo       b       +
// (0, 0)                                                                 (9, 0)
//
// In addition, cut_square_1 is square with the upper-left quarter removed,
// cut_square_2 is square with the lower-right quarter removed, and
// pierced_octagon is octagon with hexagon_one and hexagon_two removed.

#include "config.h"
using namespace std;
#include "math/polygons.h"

#include <cmath>                         // for sqrt, M_PI

#include "gtest/gtest.h"

namespace energy_rec {

class PolygonsTest : public ::testing::Test {
 protected:
  PolygonsTest() {
    // Populate an array of lattice points, to make it easy to define components
    // below:
    for (int i = 0; i < 10; ++i) {
      const FloatType x = (FloatType) i;
      for (int j = 0; j < 10; ++j) {
        const FloatType y = (FloatType) j;
        points_[i][j].Set(kCartesian, 0, x);
        points_[i][j].Set(kCartesian, 1, y);
      }
    }
  }

  virtual void SetUp() {
    // Create a collection of test components:
    Polygon poly;  // A Polygon struct that we'll use
                   // to populate the Polygons objects
    poly.clear();
    poly.push_back(points_[0][9]);
    poly.push_back(points_[0][8]);
    poly.push_back(points_[3][8]);
    poly.push_back(points_[3][9]);
    rectangle_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[0][9]);
    poly.push_back(points_[0][6]);
    poly.push_back(points_[2][8]);
    triangle_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[3][8]);
    poly.push_back(points_[1][8]);
    poly.push_back(points_[1][6]);
    poly.push_back(points_[3][6]);
    square_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[2][8]);
    poly.push_back(points_[2][7]);
    poly.push_back(points_[1][7]);
    poly.push_back(points_[1][6]);
    poly.push_back(points_[3][6]);
    poly.push_back(points_[3][8]);
    cut_square_1_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[1][8]);
    poly.push_back(points_[1][6]);
    poly.push_back(points_[2][6]);
    poly.push_back(points_[2][7]);
    poly.push_back(points_[3][7]);
    poly.push_back(points_[3][8]);
    cut_square_2_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[2][8]);
    poly.push_back(points_[1][7]);
    poly.push_back(points_[2][6]);
    poly.push_back(points_[3][7]);
    diamond_one_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[3][8]);
    poly.push_back(points_[2][7]);
    poly.push_back(points_[3][6]);
    poly.push_back(points_[4][7]);
    diamond_two_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[4][8]);
    poly.push_back(points_[3][7]);
    poly.push_back(points_[4][6]);
    poly.push_back(points_[5][7]);
    diamond_three_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[3][9]);
    poly.push_back(points_[3][7]);
    poly.push_back(points_[4][8]);
    poly.push_back(points_[4][7]);
    poly.push_back(points_[6][9]);
    problem_shape_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[0][5]);
    poly.push_back(points_[0][1]);
    poly.push_back(points_[3][1]);
    poly.push_back(points_[3][2]);
    poly.push_back(points_[1][2]);
    poly.push_back(points_[1][4]);
    poly.push_back(points_[3][4]);
    poly.push_back(points_[3][5]);
    left_cup_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[3][5]);
    poly.push_back(points_[0][5]);
    poly.push_back(points_[0][4]);
    poly.push_back(points_[2][4]);
    poly.push_back(points_[2][2]);
    poly.push_back(points_[0][2]);
    poly.push_back(points_[0][1]);
    poly.push_back(points_[3][1]);
    right_cup_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[9][4]);
    poly.push_back(points_[7][6]);
    poly.push_back(points_[5][6]);
    poly.push_back(points_[3][4]);
    poly.push_back(points_[3][2]);
    poly.push_back(points_[5][0]);
    poly.push_back(points_[7][0]);
    poly.push_back(points_[9][2]);
    octagon_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[6][5]);
    poly.push_back(points_[5][5]);
    poly.push_back(points_[4][4]);
    poly.push_back(points_[4][3]);
    poly.push_back(points_[5][3]);
    poly.push_back(points_[6][4]);
    hexagon_one_.CopyFrom(poly);

    poly.clear();
    poly.push_back(points_[8][3]);
    poly.push_back(points_[7][3]);
    poly.push_back(points_[6][2]);
    poly.push_back(points_[6][1]);
    poly.push_back(points_[7][1]);
    poly.push_back(points_[8][2]);
    hexagon_two_.CopyFrom(poly);

    pierced_octagon_.CopyFrom(octagon_);
    pierced_octagon_.Subtract(hexagon_one_);
    pierced_octagon_.Subtract(hexagon_two_);

    poly.clear();
    poly.push_back(points_[4][6]);
    poly.push_back(points_[3][5]);
    poly.push_back(points_[8][0]);
    poly.push_back(points_[9][1]);
    bar_.CopyFrom(poly);
  }

  Coordinates2D points_[10][10];  // integer lattice points, for convenience
  Polygons rectangle_, triangle_, square_, cut_square_1_, cut_square_2_,
           diamond_one_, diamond_two_, diamond_three_, cut_diamond_,
           problem_shape_, left_cup_, right_cup_,
           octagon_, hexagon_one_, hexagon_two_, pierced_octagon_, bar_;
};

TEST_F(PolygonsTest, TestCircleApproximationConstructor) {
  Polygons triangle_approximation(3, 5.0);
  EXPECT_EQ(1, triangle_approximation.ComponentCount());
  EXPECT_EQ(3, triangle_approximation.VertexCount());
  EXPECT_NEAR(M_PI * 25.0, triangle_approximation.Area(),
              3.0 * kFloatTypeError);
  Polygons square_approximation(4, 3.0);
  EXPECT_EQ(1, square_approximation.ComponentCount());
  EXPECT_EQ(4, square_approximation.VertexCount());
  EXPECT_NEAR(M_PI * 9.0, square_approximation.Area(),
              kFloatTypeError);
  Polygons icosagon_approximation(20, 0.5);
  EXPECT_EQ(1, icosagon_approximation.ComponentCount());
  EXPECT_EQ(20, icosagon_approximation.VertexCount());
  EXPECT_NEAR(M_PI * 0.25, icosagon_approximation.Area(),
              kFloatTypeError);
  Polygons megagon_approximation(1000000, 7.0);
  EXPECT_EQ(1, megagon_approximation.ComponentCount());
  EXPECT_EQ(1000000, megagon_approximation.VertexCount());
  EXPECT_NEAR(M_PI * 49.0, megagon_approximation.Area(),
              1e6 * kFloatTypeError);
}

TEST_F(PolygonsTest, TestComponentCount) {
  EXPECT_EQ(1, rectangle_.ComponentCount());
  EXPECT_EQ(1, triangle_.ComponentCount());
  EXPECT_EQ(1, square_.ComponentCount());
  EXPECT_EQ(1, cut_square_1_.ComponentCount());
  EXPECT_EQ(1, cut_square_2_.ComponentCount());
  EXPECT_EQ(1, diamond_one_.ComponentCount());
  EXPECT_EQ(1, diamond_two_.ComponentCount());
  EXPECT_EQ(1, diamond_three_.ComponentCount());
  EXPECT_EQ(1, problem_shape_.ComponentCount());
  EXPECT_EQ(1, left_cup_.ComponentCount());
  EXPECT_EQ(1, right_cup_.ComponentCount());
  EXPECT_EQ(1, octagon_.ComponentCount());
  EXPECT_EQ(1, hexagon_one_.ComponentCount());
  EXPECT_EQ(1, hexagon_two_.ComponentCount());
  EXPECT_EQ(3, pierced_octagon_.ComponentCount());
  EXPECT_EQ(1, bar_.ComponentCount());
}

TEST_F(PolygonsTest, TestVertexCount) {
  EXPECT_EQ(4, rectangle_.VertexCount());
  EXPECT_EQ(3, triangle_.VertexCount());
  EXPECT_EQ(4, square_.VertexCount());
  EXPECT_EQ(6, cut_square_1_.VertexCount());
  EXPECT_EQ(6, cut_square_2_.VertexCount());
  EXPECT_EQ(4, diamond_one_.VertexCount());
  EXPECT_EQ(4, diamond_two_.VertexCount());
  EXPECT_EQ(4, diamond_three_.VertexCount());
  EXPECT_EQ(5, problem_shape_.VertexCount());
  EXPECT_EQ(8, left_cup_.VertexCount());
  EXPECT_EQ(8, right_cup_.VertexCount());
  EXPECT_EQ(8, octagon_.VertexCount());
  EXPECT_EQ(6, hexagon_one_.VertexCount());
  EXPECT_EQ(6, hexagon_two_.VertexCount());
  EXPECT_EQ(20, pierced_octagon_.VertexCount());
  EXPECT_EQ(4, bar_.VertexCount());
}

TEST_F(PolygonsTest, TestArea) {
  EXPECT_NEAR(3.0, rectangle_.Area(), kFloatTypeError);
  EXPECT_NEAR(3.0, triangle_.Area(), kFloatTypeError);
  EXPECT_NEAR(4.0, square_.Area(), kFloatTypeError);
  EXPECT_NEAR(3.0, cut_square_1_.Area(), kFloatTypeError);
  EXPECT_NEAR(3.0, cut_square_2_.Area(), kFloatTypeError);
  EXPECT_NEAR(2.0, diamond_one_.Area(), kFloatTypeError);
  EXPECT_NEAR(2.0, diamond_two_.Area(), kFloatTypeError);
  EXPECT_NEAR(2.0, diamond_three_.Area(), kFloatTypeError);
  EXPECT_NEAR(3.5, problem_shape_.Area(), kFloatTypeError);
  EXPECT_NEAR(8.0, left_cup_.Area(), kFloatTypeError);
  EXPECT_NEAR(8.0, right_cup_.Area(), kFloatTypeError);
  EXPECT_NEAR(28.0, octagon_.Area(), kFloatTypeError);
  EXPECT_NEAR(3.0, hexagon_one_.Area(), kFloatTypeError);
  EXPECT_NEAR(3.0, hexagon_two_.Area(), kFloatTypeError);
  EXPECT_NEAR(22.0, pierced_octagon_.Area(), kFloatTypeError);
  EXPECT_NEAR(10.0, bar_.Area(), kFloatTypeError);
}

TEST_F(PolygonsTest, TestCentroid) {
  Coordinates2D c;
  rectangle_.GetCentroid(&c);
  EXPECT_NEAR(1.5, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(8.5, c.Get(kCartesian, 1), kFloatTypeError);
  triangle_.GetCentroid(&c);
  EXPECT_NEAR(2.0 / 3.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0 + 2.0 / 3.0, c.Get(kCartesian, 1), kFloatTypeError);
  square_.GetCentroid(&c);
  EXPECT_NEAR(2.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0, c.Get(kCartesian, 1), kFloatTypeError);
  cut_square_1_.GetCentroid(&c);
  EXPECT_NEAR(2.0 + 1.0 / 6.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0 - 1.0 / 6.0, c.Get(kCartesian, 1), kFloatTypeError);
  cut_square_2_.GetCentroid(&c);
  EXPECT_NEAR(2.0 - 1.0 / 6.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0 + 1.0 / 6.0, c.Get(kCartesian, 1), kFloatTypeError);
  diamond_one_.GetCentroid(&c);
  EXPECT_NEAR(2.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0, c.Get(kCartesian, 1), kFloatTypeError);
  diamond_two_.GetCentroid(&c);
  EXPECT_NEAR(3.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0, c.Get(kCartesian, 1), kFloatTypeError);
  diamond_three_.GetCentroid(&c);
  EXPECT_NEAR(4.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(7.0, c.Get(kCartesian, 1), kFloatTypeError);
  octagon_.GetCentroid(&c);
  EXPECT_NEAR(6.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(3.0, c.Get(kCartesian, 1), kFloatTypeError);
  pierced_octagon_.GetCentroid(&c);
  EXPECT_NEAR(6.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(3.0, c.Get(kCartesian, 1), kFloatTypeError);
  bar_.GetCentroid(&c);
  EXPECT_NEAR(6.0, c.Get(kCartesian, 0), kFloatTypeError);
  EXPECT_NEAR(3.0, c.Get(kCartesian, 1), kFloatTypeError);
}

TEST_F(PolygonsTest, TestRadiusFromOrigin) {
  EXPECT_NEAR(sqrt(90.0), rectangle_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(81.0), triangle_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(73.0), square_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(73.0), cut_square_1_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(73.0), cut_square_2_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(68.0), diamond_one_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(73.0), diamond_two_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(80.0), diamond_three_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(117.0), problem_shape_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(34.0), left_cup_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(34.0), right_cup_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(97.0), octagon_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(61.0), hexagon_one_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(73.0), hexagon_two_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(97.0), pierced_octagon_.RadiusFromOrigin(), kFloatTypeError);
  EXPECT_NEAR(sqrt(82.0), bar_.RadiusFromOrigin(), kFloatTypeError);
}

TEST_F(PolygonsTest, TestContainsPoint) {
  EXPECT_TRUE(triangle_.ContainsPoint(points_[0][9]));
  EXPECT_TRUE(triangle_.ContainsPoint(points_[0][8]));
  EXPECT_TRUE(triangle_.ContainsPoint(points_[0][7]));
  EXPECT_TRUE(triangle_.ContainsPoint(points_[0][6]));
  EXPECT_FALSE(triangle_.ContainsPoint(points_[1][9]));
  EXPECT_TRUE(triangle_.ContainsPoint(points_[1][8]));
  EXPECT_TRUE(triangle_.ContainsPoint(points_[1][7]));
  EXPECT_FALSE(triangle_.ContainsPoint(points_[1][6]));
  EXPECT_FALSE(triangle_.ContainsPoint(points_[2][9]));
  EXPECT_TRUE(triangle_.ContainsPoint(points_[2][8]));
  EXPECT_FALSE(triangle_.ContainsPoint(points_[2][7]));

  EXPECT_FALSE(square_.ContainsPoint(points_[1][9]));
  EXPECT_TRUE(square_.ContainsPoint(points_[1][8]));
  EXPECT_TRUE(square_.ContainsPoint(points_[1][7]));
  EXPECT_TRUE(square_.ContainsPoint(points_[1][6]));
  EXPECT_FALSE(square_.ContainsPoint(points_[1][5]));
  EXPECT_FALSE(square_.ContainsPoint(points_[2][9]));
  EXPECT_TRUE(square_.ContainsPoint(points_[2][8]));
  EXPECT_TRUE(square_.ContainsPoint(points_[2][7]));
  EXPECT_TRUE(square_.ContainsPoint(points_[2][6]));
  EXPECT_FALSE(square_.ContainsPoint(points_[2][5]));
  EXPECT_FALSE(square_.ContainsPoint(points_[3][9]));
  EXPECT_TRUE(square_.ContainsPoint(points_[3][8]));
  EXPECT_TRUE(square_.ContainsPoint(points_[3][7]));
  EXPECT_TRUE(square_.ContainsPoint(points_[3][6]));
  EXPECT_FALSE(square_.ContainsPoint(points_[3][5]));

  EXPECT_FALSE(diamond_one_.ContainsPoint(points_[1][8]));
  EXPECT_TRUE(diamond_one_.ContainsPoint(points_[1][7]));
  EXPECT_FALSE(diamond_one_.ContainsPoint(points_[1][6]));
  EXPECT_FALSE(diamond_one_.ContainsPoint(points_[2][9]));
  EXPECT_TRUE(diamond_one_.ContainsPoint(points_[2][8]));
  EXPECT_TRUE(diamond_one_.ContainsPoint(points_[2][7]));
  EXPECT_TRUE(diamond_one_.ContainsPoint(points_[2][6]));
  EXPECT_FALSE(diamond_one_.ContainsPoint(points_[2][5]));
  EXPECT_FALSE(diamond_one_.ContainsPoint(points_[3][8]));
  EXPECT_TRUE(diamond_one_.ContainsPoint(points_[3][7]));
  EXPECT_FALSE(diamond_one_.ContainsPoint(points_[3][6]));

  EXPECT_FALSE(left_cup_.ContainsPoint(points_[2][0]));
  EXPECT_TRUE(left_cup_.ContainsPoint(points_[2][1]));
  EXPECT_TRUE(left_cup_.ContainsPoint(points_[2][2]));
  EXPECT_FALSE(left_cup_.ContainsPoint(points_[2][3]));
  EXPECT_TRUE(left_cup_.ContainsPoint(points_[2][4]));
  EXPECT_TRUE(left_cup_.ContainsPoint(points_[2][5]));
  EXPECT_FALSE(left_cup_.ContainsPoint(points_[2][6]));

  EXPECT_FALSE(right_cup_.ContainsPoint(points_[1][0]));
  EXPECT_TRUE(right_cup_.ContainsPoint(points_[1][1]));
  EXPECT_TRUE(right_cup_.ContainsPoint(points_[1][2]));
  EXPECT_FALSE(right_cup_.ContainsPoint(points_[1][3]));
  EXPECT_TRUE(right_cup_.ContainsPoint(points_[1][4]));
  EXPECT_TRUE(right_cup_.ContainsPoint(points_[1][5]));
  EXPECT_FALSE(right_cup_.ContainsPoint(points_[1][6]));

  EXPECT_TRUE(octagon_.ContainsPoint(points_[5][4]));
  EXPECT_FALSE(pierced_octagon_.ContainsPoint(points_[5][4]));
  EXPECT_TRUE(octagon_.ContainsPoint(points_[7][2]));
  EXPECT_FALSE(pierced_octagon_.ContainsPoint(points_[7][2]));
}

TEST_F(PolygonsTest, TestReverse) {
  Polygons a;

  // Null polygon:
  a.Reverse();
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Single component:
  a.CopyFrom(rectangle_);
  a.Reverse();
  EXPECT_NEAR(-3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Multiple components:
  a.CopyFrom(pierced_octagon_);
  a.Reverse();
  EXPECT_NEAR(-22.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(3, a.ComponentCount());
  EXPECT_EQ(20, a.VertexCount());
}

TEST_F(PolygonsTest, TestUnion) {
  Polygons a;
  Polygons null_polygon;

  // Two null polygons:
  a.Union(null_polygon);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // A null polygon and a non-null polygon:
  a.Union(diamond_one_);
  EXPECT_NEAR(2.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with a single point of intersection:
  a.CopyFrom(rectangle_);
  a.Union(diamond_one_);
  EXPECT_NEAR(5.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // Polygons with a partially shared edge:
  a.CopyFrom(rectangle_);
  a.Union(square_);
  EXPECT_NEAR(7.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());

  // Polygons with several shared edges:
  a.CopyFrom(square_);
  a.Union(cut_square_1_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with several shared edges:
  a.CopyFrom(cut_square_2_);
  a.Union(square_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Interlocked L-shapes:
  a.CopyFrom(cut_square_1_);
  a.Union(cut_square_2_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with a single shared vertex:
  a.CopyFrom(diamond_one_);
  a.Union(diamond_three_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // A two-component polygon and a one-component polygon:
  a.Union(diamond_two_);
  EXPECT_NEAR(5.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(12, a.VertexCount());

  // Polygons with two contact points:
  a.Union(rectangle_);
  EXPECT_NEAR(8.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(16, a.VertexCount());

  // Overlapping polygons that share a vertex and a partial edge:
  a.CopyFrom(rectangle_);
  a.Union(triangle_);
  EXPECT_NEAR(5.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(5, a.VertexCount());

  // Identical polygons with an odd number of vertices:
  a.CopyFrom(triangle_);
  a.Union(triangle_);
  EXPECT_NEAR(3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(3, a.VertexCount());

  // Identical polygons with an even number of vertices:
  a.CopyFrom(square_);
  a.Union(square_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with a vertex from one on an edge of the other:
  a.CopyFrom(triangle_);
  a.Union(square_);
  EXPECT_NEAR(6.5, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(7, a.VertexCount());

  // Simple polygons whose union has a hole:
  a.CopyFrom(left_cup_);
  a.Union(right_cup_);
  EXPECT_NEAR(10.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // Polygons with an edge of one contained by an edge of the other:
  a.CopyFrom(right_cup_);
  a.Union(octagon_);
  EXPECT_NEAR(36.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(16, a.VertexCount());

  // Now with more components:
  a.CopyFrom(right_cup_);
  a.Union(pierced_octagon_);
  EXPECT_NEAR(30.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(3, a.ComponentCount());
  EXPECT_EQ(28, a.VertexCount());

  // Polygons with two points of contact:
  a.CopyFrom(left_cup_);
  a.Union(octagon_);
  EXPECT_NEAR(36.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(16, a.VertexCount());

  // And again with the pierced octagon:
  a.CopyFrom(left_cup_);
  a.Union(pierced_octagon_);
  EXPECT_NEAR(30.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(4, a.ComponentCount());
  EXPECT_EQ(28, a.VertexCount());

  // Polygons with one contained by the other:
  a.CopyFrom(octagon_);
  a.Union(hexagon_one_);
  EXPECT_NEAR(28.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // Disjoint polygons:
  a.CopyFrom(hexagon_one_);
  a.Union(hexagon_two_);
  EXPECT_NEAR(6.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(12, a.VertexCount());

  // Polygons with a degenerate intersection point:
  a.CopyFrom(diamond_three_);
  a.Union(problem_shape_);
  EXPECT_NEAR(5.25, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());

  // Bar through the pierced octagon:
  a.CopyFrom(pierced_octagon_);
  a.Union(bar_);
  EXPECT_NEAR(28.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(5, a.ComponentCount());
  EXPECT_EQ(28, a.VertexCount());

  // Union of a hole with the null polygon:
  a.CopyFrom(square_);
  a.Reverse();
  a.Union(null_polygon);
  // The union should be just the hole:
  EXPECT_NEAR(-4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Union of a null polygon with a hole:
  null_polygon.Clear();
  null_polygon.Union(a);
  // The union should be just the hole:
  EXPECT_NEAR(-4.0, null_polygon.Area(), kFloatTypeError);
  EXPECT_EQ(1, null_polygon.ComponentCount());
  EXPECT_EQ(4, null_polygon.VertexCount());

  // Union of a hole with a solid, disjoint:
  a.CopyFrom(square_);
  a.Reverse();
  a.Union(hexagon_one_);
  // The union should be just the hole:
  EXPECT_NEAR(-4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());
}

TEST_F(PolygonsTest, TestIntersect) {
  Polygons a;
  Polygons null_polygon;

  // A null polygon and a null polygon:
  a.Intersect(null_polygon);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // A null polygon and a non-null polygon:
  a.Intersect(square_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // A non-null polygon and a null polygon:
  a.CopyFrom(square_);
  a.Intersect(null_polygon);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Non-intersecting polygons:
  a.CopyFrom(triangle_);
  a.Intersect(diamond_three_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Polygons with a partially shared edge:
  a.CopyFrom(triangle_);
  a.Intersect(rectangle_);
  EXPECT_NEAR(1.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(3, a.VertexCount());

  // Identical polygons with an odd number of vertices:
  a.CopyFrom(triangle_);
  a.Intersect(triangle_);
  EXPECT_NEAR(3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(3, a.VertexCount());

  // Identical polygons with an even number of vertices:
  a.CopyFrom(square_);
  a.Intersect(square_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with an intersection point on the vertex of one:
  a.CopyFrom(triangle_);
  a.Intersect(square_);
  EXPECT_NEAR(0.5, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(3, a.VertexCount());

  // Tangent polygons with no overlap:
  a.CopyFrom(rectangle_);
  a.Intersect(square_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Polygons with several shared edges:
  a.CopyFrom(square_);
  a.Intersect(cut_square_1_);
  EXPECT_NEAR(3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());

  // Polygons with several shared edges:
  a.CopyFrom(cut_square_2_);
  a.Intersect(square_);
  EXPECT_NEAR(3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());

  // Interlocked L-shapes:
  a.CopyFrom(cut_square_1_);
  a.Intersect(cut_square_2_);
  EXPECT_NEAR(2.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // Polygons with two ordinary intersection points:
  a.CopyFrom(diamond_one_);
  a.Intersect(diamond_two_);
  EXPECT_NEAR(0.5, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with lots of colinear edges:
  a.CopyFrom(left_cup_);
  a.Intersect(right_cup_);
  EXPECT_NEAR(6.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // Polygons with a degenerate intersection point:
  a.CopyFrom(diamond_three_);
  a.Intersect(problem_shape_);
  EXPECT_NEAR(0.25, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());

  a.CopyFrom(rectangle_);
  a.Intersect(diamond_three_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Bar through the pierced octagon:
  a.CopyFrom(pierced_octagon_);
  a.Intersect(bar_);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(3, a.ComponentCount());
  EXPECT_EQ(12, a.VertexCount());

  // Intersection of a hole with a solid, disjoint:
  a.CopyFrom(square_);
  a.Reverse();
  a.Intersect(hexagon_one_);
  // The union should be just the hexagon:
  EXPECT_NEAR(3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());

  // TODO(tpw):  More tests with holes
}

TEST_F(PolygonsTest, TestSubtract) {
  Polygons a;
  Polygons null_polygon;

  // Subtract a null polygon from a null polygon:
  a.Subtract(null_polygon);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Subtract a non-null polygon from a null polygon:
  a.Subtract(square_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Subtract a null polygon from a non-null polygon:
  a.CopyFrom(square_);
  a.Subtract(null_polygon);
  EXPECT_NEAR(4.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Subtract an enclosing polygon:
  a.CopyFrom(hexagon_one_);
  a.Subtract(octagon_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Identical polygons with an odd number of vertices:
  a.CopyFrom(triangle_);
  a.Subtract(triangle_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Identical polygons with an even number of vertices:
  a.CopyFrom(square_);
  a.Subtract(square_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Polygons with several shared edges:
  a.CopyFrom(square_);
  a.Subtract(cut_square_1_);
  EXPECT_NEAR(1.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with several shared edges:
  a.CopyFrom(cut_square_2_);
  a.Subtract(square_);
  EXPECT_NEAR(0.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(0, a.ComponentCount());
  EXPECT_EQ(0, a.VertexCount());

  // Interlocked L-shapes:
  a.CopyFrom(cut_square_1_);
  a.Subtract(cut_square_2_);
  EXPECT_NEAR(1.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Non-intersecting polygons:
  a.CopyFrom(triangle_);
  a.Subtract(diamond_three_);
  EXPECT_NEAR(3.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(3, a.VertexCount());

  // Polygons with two ordinary points of intersection:
  a.CopyFrom(diamond_one_);
  a.Subtract(diamond_two_);
  EXPECT_NEAR(1.5, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());

  // Polygons with a partial shared edge:
  a.CopyFrom(rectangle_);
  a.Subtract(triangle_);
  EXPECT_NEAR(2.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Polygons with a lot of colinear edges:
  a.CopyFrom(left_cup_);
  a.Subtract(right_cup_);
  EXPECT_NEAR(2.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(4, a.VertexCount());

  // Bar through the pierced octagon:
  a.CopyFrom(pierced_octagon_);
  a.Subtract(bar_);
  EXPECT_NEAR(18.0, a.Area(), kFloatTypeError);
  EXPECT_EQ(2, a.ComponentCount());
  EXPECT_EQ(24, a.VertexCount());
}

TEST_F(PolygonsTest, TestRoundingError) {
  // This is a pathological example discovered by lhm and tpw while
  // experimenting with the RE<C optical flux simulation tool:
  Polygon poly;  // A struct used to initialize the Polygons objects a and b
  Polygons a, b;
  Coordinates2D points[6];

  points[0].Set(kCartesian, 0, -0.61074052249552291);
  points[0].Set(kCartesian, 1, 0.38925947750447709);
  points[1].Set(kCartesian, 0, -1.0);
  points[1].Set(kCartesian, 1, 0.0);
  points[2].Set(kCartesian, 0, 0.0);
  points[2].Set(kCartesian, 1, -1.0);
  points[3].Set(kCartesian, 0, 1.0);
  points[3].Set(kCartesian, 1, 0.0);
  points[4].Set(kCartesian, 0, 0.5622018690420616);
  points[4].Set(kCartesian, 1, 0.43779813095793835);
  points[5].Set(kCartesian, 0, -0.048110514402574002);
  points[5].Set(kCartesian, 1, -0.17760385443584165);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  a.CopyFrom(poly);

  points[0].Set(kCartesian, 0, -0.0083017479796730371);
  points[0].Set(kCartesian, 1, 0.99169825202032691);
  points[1].Set(kCartesian, 0, -1.0);
  points[1].Set(kCartesian, 1, 0.0);
  points[2].Set(kCartesian, 0, 0.0);
  points[2].Set(kCartesian, 1, -1.0);
  points[3].Set(kCartesian, 0, 1.0);
  points[3].Set(kCartesian, 1, 0.0);
  points[4].Set(kCartesian, 0, 0.010734592666395093);
  points[4].Set(kCartesian, 1, 0.98926540733360491);
  points[5].Set(kCartesian, 0, 0.0025153802939976329);
  points[5].Set(kCartesian, 1, 0.98107681874778252);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  b.CopyFrom(poly);

  a.Intersect(b);
  EXPECT_NEAR(1.3105368691401, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());
}

TEST_F(PolygonsTest, TestIntersectionsWithNumericalEdgeCaseIntersections) {
  // These pathological examples were discovered by tpw while
  // experimenting with the RE<C optical flux simulation tool:
  Polygon poly;  // A struct used to initialize the Polygons objects a and b
  Polygons a, b;
  Coordinates2D points[6];

  // Case 1:  Intersection of two concave hexagons that share several edges:
  points[0].Set(kCartesian, 0, -0.22945742555325654);
  points[0].Set(kCartesian, 1, -0.77054257444674346);
  points[1].Set(kCartesian, 0,  0.3467556641970837);
  points[1].Set(kCartesian, 1, -0.19466011152326679);
  points[2].Set(kCartesian, 0,  0.57582269143180154);
  points[2].Set(kCartesian, 1, -0.42417730856819846);
  points[3].Set(kCartesian, 0,  1.0);
  points[3].Set(kCartesian, 1,  0.0);
  points[4].Set(kCartesian, 0,  0.0);
  points[4].Set(kCartesian, 1,  1.0);
  points[5].Set(kCartesian, 0, -1.0);
  points[5].Set(kCartesian, 1,  0.0);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  a.CopyFrom(poly);

  points[0].Set(kCartesian, 0, -0.64765203235745972);
  points[0].Set(kCartesian, 1, -0.35234796764254023);
  points[1].Set(kCartesian, 0, -0.41829145290403325);
  points[1].Set(kCartesian, 1, -0.12421477624731797);
  points[2].Set(kCartesian, 0,  0.22751293578127652);
  points[2].Set(kCartesian, 1, -0.77248706421872348);
  points[3].Set(kCartesian, 0,  1.0);
  points[3].Set(kCartesian, 1,  0.0);
  points[4].Set(kCartesian, 0,  0.0);
  points[4].Set(kCartesian, 1,  1.0);
  points[5].Set(kCartesian, 0, -1.0);
  points[5].Set(kCartesian, 1,  0.0);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  b.CopyFrom(poly);

  a.Intersect(b);
  EXPECT_NEAR(1.5449265439571775, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());

  // Case 2:  A concave hexagon intersected with a
  //          rectangle of which it is a subregion.
  points[0].Set(kCartesian, 0,  1.5);
  points[0].Set(kCartesian, 1,  1.0);
  points[1].Set(kCartesian, 0, -1.5);
  points[1].Set(kCartesian, 1,  1.0);
  points[2].Set(kCartesian, 0, -1.5);
  points[2].Set(kCartesian, 1, -1.0);
  points[3].Set(kCartesian, 0,  0.37395927488277003);
  points[3].Set(kCartesian, 1, -1.0);
  points[4].Set(kCartesian, 0,  0.37364170187080303);
  points[4].Set(kCartesian, 1,  0.81777827680997928);
  points[5].Set(kCartesian, 0,  1.5);
  points[5].Set(kCartesian, 1,  0.80231686380696154);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  a.CopyFrom(poly);

  points[0].Set(kCartesian, 0,  1.5);
  points[0].Set(kCartesian, 1,  1.0);
  points[1].Set(kCartesian, 0, -1.5);
  points[1].Set(kCartesian, 1,  1.0);
  points[2].Set(kCartesian, 0, -1.5);
  points[2].Set(kCartesian, 1, -1.0);
  points[3].Set(kCartesian, 0,  1.5);
  points[3].Set(kCartesian, 1, -1.0);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  b.CopyFrom(poly);

  const FloatType a_area = a.Area();
  a.Intersect(b);
  EXPECT_NEAR(a_area, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(6, a.VertexCount());
}

TEST_F(PolygonsTest, TestCaseWithDuplicateIntersections) {
  // An example discovered by tpw while experimenting with optical_simulation.
  // GetIntersections generates a near-duplicate intersection at
  // (-0.88711323870898939, -0.11288676129101061).
  Polygons a, b;
  Polygon poly;  // A struct used to initialize the Polygons objects a and b
  Coordinates2D points[8];

  points[0].Set(kCartesian, 0, -0.24853109551602554);
  points[0].Set(kCartesian, 1, -0.75146890448397463);
  points[1].Set(kCartesian, 0, 0.25034244803605121);
  points[1].Set(kCartesian, 1, -0.25005388692009523);
  points[2].Set(kCartesian, 0, 0.5015873342677506);
  points[2].Set(kCartesian, 1, -0.4984126657322494);
  points[3].Set(kCartesian, 0, 1.0);
  points[3].Set(kCartesian, 1, 0.0);
  points[4].Set(kCartesian, 0, 0.0);
  points[4].Set(kCartesian, 1, 1.0);
  points[5].Set(kCartesian, 0, -0.99727436657018886);
  points[5].Set(kCartesian, 1, 0.0027256334298111362);
  points[6].Set(kCartesian, 0, -0.88425174706701393);
  points[6].Set(kCartesian, 1, -0.11000753907239788);
  points[7].Set(kCartesian, 0, -0.88711323870898939);
  points[7].Set(kCartesian, 1, -0.11288676129101062);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  poly.push_back(points[6]);
  poly.push_back(points[7]);
  a.CopyFrom(poly);

  const FloatType expected_area = a.Area();
  points[0].Set(kCartesian, 0,  0.0);
  points[0].Set(kCartesian, 1,  1.0);
  points[1].Set(kCartesian, 0, -1.0);
  points[1].Set(kCartesian, 1,  0.0);
  points[2].Set(kCartesian, 0,  0.0);
  points[2].Set(kCartesian, 1, -1.0);
  points[3].Set(kCartesian, 0,  1.0);
  points[3].Set(kCartesian, 1,  0.0);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  b.CopyFrom(poly);

  a.Intersect(b);
  EXPECT_NEAR(expected_area, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(8, a.VertexCount());
}

TEST_F(PolygonsTest, TestCaseWithAlmostDegenerateVertices) {
  // An example discovered by tpw while experimenting with optical_simulation,
  // with nearly-degenerate vertices:
  Polygon poly;  // A struct used to initialize the Polygons objects a and b
  Polygons a, b;
  Coordinates2D points[8];

  points[0].Set(kCartesian, 0, -0.2426870708377093);
  points[0].Set(kCartesian, 1, -0.75731292916229087);
  points[1].Set(kCartesian, 0, -0.24268707083770924);
  points[1].Set(kCartesian, 1, -0.75731292916229076);
  points[2].Set(kCartesian, 0,  1.0);
  points[2].Set(kCartesian, 1,  0.0);
  points[3].Set(kCartesian, 0,  0.0);
  points[3].Set(kCartesian, 1,  1.0);
  points[4].Set(kCartesian, 0, -1.0);
  points[4].Set(kCartesian, 1,  0.0);
  points[5].Set(kCartesian, 0, -0.90120171244901759);
  points[5].Set(kCartesian, 1, -0.098798287550982439);
  points[6].Set(kCartesian, 0, -0.81940025606594125);
  points[6].Set(kCartesian, 1, -0.18029782172813807);
  points[7].Set(kCartesian, 0, -0.81955055870818416);
  points[7].Set(kCartesian, 1, -0.18044944129181589);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  poly.push_back(points[4]);
  poly.push_back(points[5]);
  poly.push_back(points[6]);
  poly.push_back(points[7]);
  a.CopyFrom(poly);

  const FloatType expected_area = a.Area();
  points[0].Set(kCartesian, 0,  0.0);
  points[0].Set(kCartesian, 1,  1.0);
  points[1].Set(kCartesian, 0, -1.0);
  points[1].Set(kCartesian, 1,  0.0);
  points[2].Set(kCartesian, 0,  0.0);
  points[2].Set(kCartesian, 1, -1.0);
  points[3].Set(kCartesian, 0,  1.0);
  points[3].Set(kCartesian, 1,  0.0);
  poly.clear();
  poly.push_back(points[0]);
  poly.push_back(points[1]);
  poly.push_back(points[2]);
  poly.push_back(points[3]);
  b.CopyFrom(poly);

  a.Intersect(b);
  EXPECT_NEAR(expected_area, a.Area(), kFloatTypeError);
  EXPECT_EQ(1, a.ComponentCount());
  EXPECT_EQ(7, a.VertexCount());
}

TEST_F(PolygonsTest, TestApplyFunction) {
  // Create a callback object:
  class TestApplyFunction : public PolygonsApplyFunction {
   public:
    virtual void ApplyFunction(Coordinates2D *vertex) const {
      // Rescale the coordinates:
      vertex->Multiply(2.0);
      vertex->Set(kCartesian, 0, vertex->Get(kCartesian, 0) + 1.0);
      vertex->Set(kCartesian, 1, vertex->Get(kCartesian, 1) + 2.0);
    }
  };
  // Apply the callback to some test Polygons:
  TestApplyFunction callback;
  rectangle_.ApplyFunction(callback);
  EXPECT_NEAR(12.0, rectangle_.Area(), kFloatTypeError);
  triangle_.ApplyFunction(callback);
  EXPECT_NEAR(12.0, triangle_.Area(), kFloatTypeError);
}

}  // namespace energy_rec
