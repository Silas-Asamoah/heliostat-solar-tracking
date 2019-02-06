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

// This class stores and manipulates sets of simple (ie. non-self-intersecting)
// polygons, which can be either solid (positive-area) or hole (negative-area)
// components.  It does not check that components are simple; this is up to the
// user.

// A Polygons object comprising a solid component which contains a hole
// component represents just that: a component with a hole in it.  With respect
// to a right-handed coordinate system, counterclockwise-oriented components are
// solids and clockwise-oriented components are holes.

// This class includes functions for determining area, point inclusion,
// component inclusion, and binary operations.

#ifndef ENERGY_REC_UTIL_POLYGONS_H_
#define ENERGY_REC_UTIL_POLYGONS_H_

using namespace std;
#include <vector>                           // for vector

#include "math/coordinates.h"    // for FloatType
#include "math/coordinates2d.h"  // for Coordinates2D

namespace energy_rec {

// A single polygon; these objects are not closed under Union and Intersection,
// so we use them as components for the algebraic closure Polygons class below:
typedef vector<Coordinates2D> Polygon;

// A structure to store a position along one edge of one component:
typedef struct PolygonComponentPosition {
  int component_index;  // which component we're on
  int vector_index;     // the index of the vector preceding this location
  FloatType parameter;  // how far along this edge we are
} PolygonComponentPosition;

// A structure to store information about an intersection of two components'
// edges:
typedef struct EdgeIntersection {
  Coordinates2D location;  // physical coordinate position
  PolygonComponentPosition position[2];  // position on each of two components
  int follow_component[2];  // which component goes to the
                            // {outside, inside} of the other
} EdgeIntersection;

// A callback class for Polygons' ApplyFunction method:
class PolygonsApplyFunction {
 public:
  virtual ~PolygonsApplyFunction(void) {}
  virtual void ApplyFunction(Coordinates2D *vertex) const = 0;
};

// A collection of simple (ie. non-self-intersecting) polygons:
class Polygons {
 public:
  // Empty constructor:
  Polygons() {}
  // Constructor from a single component:
  explicit Polygons(const Polygon &polygon);
  // Constructor for a regular n-gon approximating a circle of a given radius:
  Polygons(int sides, FloatType circle_radius);
  // Copy constructor:
  Polygons(const Polygons &polygons);

  virtual ~Polygons() {}

  void Clear(void) { components_.clear(); }

  // Assignment operator:
  Polygons &operator=(const Polygons &other);

  // Copy operators:
  void CopyFrom(const Polygon &source);
  void CopyFrom(const Polygons &source);

  // Components accessor:
  const vector<Polygon> &components(void) const { return components_; }

  // Statistics:
  int ComponentCount(void) const { return components_.size(); }
  int VertexCount(void) const;
  FloatType Area(void) const;
  static FloatType Area(const Polygon &polygon);
  void GetCentroid(Coordinates2D *centroid) const;
  static void GetCentroid(const Polygon &polygon, Coordinates2D *centroid);
  FloatType RadiusFromOrigin(void) const;
  static FloatType RadiusFromOrigin(const Polygon &polygon);

  // Inclusion tests:
  bool ContainsPoint(const Coordinates2D &point) const;
  static bool ContainsPoint(const Polygon &polygon, const Coordinates2D &point);
  static bool ContainsPolygon(const Polygon &a, const Polygon &b);

  // Unary operation:
  void Reverse(void);

  // Binary operations:
  void Union(const Polygons &other);
  void Intersect(const Polygons &other);
  void Subtract(const Polygons &other);

  // A method to apply a given function to each vertex of this:
  void ApplyFunction(const PolygonsApplyFunction &callback);

 protected:
  static bool ComparePolygonArea(const Polygon &a, const Polygon &b);
  static void GetIntersections(const Polygons &a, const Polygons &b,
                               vector<EdgeIntersection> *intersections);
  void BinaryOperation(int operation, const Polygons &other);
  static void RemoveRedundantVertices(Polygon *polygon);

 private:
  static const int kUnionOperation = 0;
  static const int kIntersectOperation = 1;
  vector<Polygon> components_;  // Must be kept sorted by absolute area
};

}  // namespace energy_rec

#endif  // ENERGY_REC_UTIL_POLYGONS_H_
