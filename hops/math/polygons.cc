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
#include "math/polygons.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>

#include <glog/logging.h>  // for DCHECK

namespace energy_rec {


// Single-component constructor
Polygons::Polygons(const Polygon &polygon) {
  components_.push_back(polygon);
}

// Copy constructor
Polygons::Polygons(const Polygons &other) {
  CopyFrom(other);
}

// Circle-approximation constructor
//   This constructor creates an n-gon with area equal to that of a circle of
//   the specified radius.
Polygons::Polygons(int sides, FloatType circle_radius) {
  DCHECK_GT(circle_radius, 0.0) << "Need positive radius.";
  // Use the formula A = 1/2 * n * r^2 * sin(2 * pi / n):
  const FloatType float_sides = static_cast<FloatType>(sides);
  const FloatType alpha = 2.0 * M_PI / float_sides;  // central angle per side
  const FloatType circumradius = circle_radius * sqrt(alpha / sin(alpha));
  const FloatType polar_coordinates[2] = { circumradius, 0.0 };
  Coordinates2D point(kPolar, polar_coordinates);
  Polygon component;
  for (int i = 0; i < sides; ++i) {
    point.Set(kPolar, 1, static_cast<FloatType>(i) * alpha);
    component.push_back(point);
  }
  components_.push_back(component);
}

// Assignment operator
Polygons &Polygons::operator=(const Polygons &other) {
  CopyFrom(other);
  return *this;
}

// Component copy method
void Polygons::CopyFrom(const Polygon &source) {
  components_.clear();
  components_.push_back(source);
}

// Object copy method
void Polygons::CopyFrom(const Polygons &source) {
  components_ = source.components_;
}

// VertexCount
//   Returns the total number of vertices in all components of this object.
int Polygons::VertexCount(void) const {
  int count = 0;
  for (vector<Polygon>::const_iterator it = components_.begin();
       it != components_.end(); ++it) {
    count += it->size();
  }
  return count;
}

// Area (instance method)
//   Returns the total area in all components of this object, counting reversed
//   components (holes) as negative area.
FloatType Polygons::Area(void) const {
  FloatType area = 0.0;
  // Add up the areas of all components:
  for (vector<Polygon>::const_iterator it = components_.begin();
       it != components_.end(); ++it) {
    area += Area(*it);
  }
  return area;
}

// Area (static method)
//   Returns the signed area of a component.
FloatType Polygons::Area(const Polygon &polygon) {
  FloatType area = 0.0;
  // Surveyor's formula:
  const int n = polygon.size();
  for (int i = 0; i < n; ++i) {
    area +=
        polygon[i].Get(kCartesian, 0) *
        polygon[(i + 1) % n].Get(kCartesian, 1) -
        polygon[(i + 1) % n].Get(kCartesian, 0) *
        polygon[i].Get(kCartesian, 1);
  }
  // We don't take the absolute value here; this allows a negatively-oriented
  // "hole" component to have negative area.
  return area / 2.0;
}

// GetCentroid (instance method)
//   Calculates the centroid of this object.
void Polygons::GetCentroid(Coordinates2D *centroid) const {
  centroid->Multiply(0.0);
  Coordinates2D component_centroid;
  FloatType total_area = 0.0;
  for (vector<Polygon>::const_iterator it = components_.begin();
       it != components_.end(); ++it) {
    GetCentroid(*it, &component_centroid);
    const FloatType component_area = Area(*it);
    total_area += component_area;
    centroid->LinearCombination(1.0, component_area, component_centroid);
  }
  centroid->Multiply(1.0 / total_area);
}

// GetCentroid (static method)
//   Calculates the centroid of a component.
void Polygons::GetCentroid(const Polygon &polygon, Coordinates2D *centroid) {
  centroid->Multiply(0.0);
  // Surveyor's formula:
  const int n = polygon.size();
  for (int i = 0; i < n; ++i) {
    Coordinates2D temp(polygon[i]);
    temp.LinearCombination(1.0, 1.0, polygon[(i + 1) % n]);
    temp.Multiply(polygon[i].Get(kCartesian, 0) *
                  polygon[(i + 1) % n].Get(kCartesian, 1) -
                  polygon[(i + 1) % n].Get(kCartesian, 0) *
                  polygon[i].Get(kCartesian, 1));
    centroid->LinearCombination(1.0, 1.0, temp);
  }
  centroid->Multiply(1.0 / 6.0 / Area(polygon));
}

// RadiusFromOrigin (instance method)
//   Returns the object's maximum distance from the coordinate origin.
FloatType Polygons::RadiusFromOrigin(void) const {
  FloatType radius = 0.0;
  for (vector<Polygon>::const_iterator it = components_.begin();
       it != components_.end(); ++it) {
    const FloatType this_radius = RadiusFromOrigin(*it);
    if (this_radius > radius) {
      radius = this_radius;
    }
  }
  return radius;
}

// RadiusFromOrigin (static method)
//   Returns a component's maximum distance from the coordinate origin.
FloatType Polygons::RadiusFromOrigin(const Polygon &polygon) {
  FloatType radius = 0.0;
  const int n = polygon.size();
  for (int i = 0; i < n; ++i) {
    const FloatType this_radius = polygon[i].EuclideanNorm();
    if (this_radius > radius) {
      radius = this_radius;
    }
  }
  return radius;
}

// ContainsPoint (instance method)
//   Returns true iff point is inside this object.
bool Polygons::ContainsPoint(const Coordinates2D &point) const {
  int crossings = 0;
  for (vector<Polygon>::const_iterator it = components_.begin();
       it != components_.end(); ++it) {
    // Count parity from the static method:
    if (ContainsPoint(*it, point)) {
      ++crossings;
    }
  }
  // point is in this object iff it's in an odd number of this object's
  // components:
  return crossings % 2;
}

// ContainsPoint (static method)
//   Returns true iff point is inside a given component polygon (which is
//   assumed topologically closed, meaning edges are counted as inside).
bool Polygons::ContainsPoint(const Polygon &polygon,
                             const Coordinates2D &point) {
  // This method implement the ray-casting test using a ray from point in the
  // positive y-direction.  We count the number of times this ray crosses an
  // edge of the component.  The point is in the polygon iff the number of
  // crossings is odd.
  // For the purposes of this calculation, we reorient each edge left-to-right
  // and count only the right-side endpoint of each edge.  This prevents
  // double-counting at vertices where the ray enters or leaves the component
  // but does (correctly) double-count vertices at which the ray is tangent
  // without crossing.
  int crossings = 0;
  const int n = polygon.size();
  const FloatType x = point.Get(kCartesian, 0);
  const FloatType y = point.Get(kCartesian, 1);
  FloatType x0, x1, y0, y1;
  for (int i = 0; i < n; ++i) {
    // Find the next vertex in sequence:
    const int j = (i + 1) % n;
    // The coordinates of each end of the current edge:
    const FloatType x_i = polygon[i].Get(kCartesian, 0);
    const FloatType y_i = polygon[i].Get(kCartesian, 1);
    const FloatType x_j = polygon[j].Get(kCartesian, 0);
    const FloatType y_j = polygon[j].Get(kCartesian, 1);
    if (x_i == x_j) {
      // The current edge is vertical.
      if ((x == x_i) && ((y_i <= y && y <= y_j) || (y_j <= y && y <= y_i))) {
        // point is on this edge, so it's definitely in the polygon:
        return true;
      }
      // Don't count this edge now; we'll count it as the endpoint of the next
      // edge:
      continue;
    } else if (x_i > x_j) {
      // Orient this edge left-to-right:
      x1 = x_i;
      y1 = y_i;
      x0 = x_j;
      y0 = y_j;
    } else {  // x_j > x_i
      // Orient this edge left-to-right:
      x1 = x_j;
      y1 = y_j;
      x0 = x_i;
      y0 = y_i;
    }
    const FloatType slope = (y1 - y0) / (x1 - x0);
    if (x0 <= x && x <= x1 && y == y0 + slope * (x - x0)) {
      // point is on this edge, so it's definitely in the polygon:
      return true;
    }
    if (x0 < x && x <= x1 &&          // x is in (x_i, x_j]
        y < y0 + slope * (x - x0)) {  // y is below the edge
      // The ray crosses this edge:
      ++crossings;
    }
  }
  return crossings % 2;  // true iff the number of crossings is odd
}

// ContainsPolygon (static method)
//   Returns true iff the component a contains the component b.
bool Polygons::ContainsPolygon(const Polygon &a, const Polygon &b) {
  // a contains b iff every point of b is inside a:
  for (int i = 0; i < b.size(); ++i) {
    if (!ContainsPoint(a, b[i])) {
      return false;
    }
  }
  return true;
}

// Reverse
//   Reorients each component of this object, turning solids to holes and holes
//   to solids.
void Polygons::Reverse(void) {
  for (vector<Polygon>::iterator it = components_.begin();
       it != components_.end();
       ++it) {
    // Reverse the order of this component's vertices:
    std::reverse(it->begin(), it->end());
  }
}

// Union
//   Computes the setwise union of this and other and stores the result in this.
void Polygons::Union(const Polygons &other) {
  BinaryOperation(kUnionOperation, other);
  // Verify that our size did not decrease:
  DCHECK((Area() * other.Area() >= 0.0 &&
          Area() >= other.Area() - kFloatTypeError) ||
         (Area() * other.Area() <= 0.0 &&
          Area() <= other.Area() + kFloatTypeError));
}

// Intersect
//   Computes the setwise intersection of this and other and stores the result
//   in this.
void Polygons::Intersect(const Polygons &other) {
  BinaryOperation(kIntersectOperation, other);
  // Verify that our size did not increase:
  DCHECK((Area() * other.Area() >= 0.0 &&
          Area() <= other.Area() + kFloatTypeError) ||
         (Area() * other.Area() <= 0.0 &&
          Area() >= other.Area() - kFloatTypeError));
}

// Subtract
//   Performs setwise subtraction of other from this and stores the result in
//   this.
void Polygons::Subtract(const Polygons &other) {
  if (components_.empty() || other.components_.empty()) {
    // No-op:
    return;
  }
  // Invert the subtrahend's components...
  Polygons reversed_other;
  reversed_other.CopyFrom(other);
  reversed_other.Reverse();
  // ...and take an intersection:
  BinaryOperation(kIntersectOperation, reversed_other);
}

// ComparePolygonArea
//   A comparator function for sorting polygon components in ascending order by
//   absolute area.  Used by BinaryOperation.
bool Polygons::ComparePolygonArea(const Polygon &a, const Polygon &b) {
  return fabs(Area(a)) < fabs(Area(b));
}

// GetIntersections
//   Populates a vector of EdgeIntersection with the intersection points of the
//   components of two Polygons objects.
// TODO(tpw):  Refactor this method for clarity and concision.
void Polygons::GetIntersections(const Polygons &a, const Polygons &b,
                                vector<EdgeIntersection> *intersections) {
  intersections->clear();
  Coordinates2D a_direction;
  Coordinates2D b_direction;
  Coordinates2D a_normal;
  Coordinates2D b_normal;
  // Iterate over the components of each object:
  for (int a_component_index = 0;
       a_component_index < a.components_.size();
       ++a_component_index) {
    for (int b_component_index = 0;
         b_component_index < b.components_.size();
         ++b_component_index) {
      const Polygon &a_component = a.components_[a_component_index];
      const Polygon &b_component = b.components_[b_component_index];
      const int m = a_component.size();
      const int n = b_component.size();
      // Iterate over the vertices of each component:
      for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
          // Test for the intersection of edge a[i]-a[i+1] with edge
          // b[j]-b[j+1], taking advantage of
          // Coordinates::LineHyperplaneIntersection:
          a_direction.CopyFrom(a_component[(i + 1) % m]);
          a_direction.LinearCombination(1.0, -1.0, a_component[i]);
          a_normal.Set(kCartesian, 0, a_direction.Get(kCartesian, 1));
          a_normal.Set(kCartesian, 1, -a_direction.Get(kCartesian, 0));
          b_direction.CopyFrom(b_component[(j + 1) % n]);
          b_direction.LinearCombination(1.0, -1.0, b_component[j]);
          b_normal.Set(kCartesian, 0, b_direction.Get(kCartesian, 1));
          b_normal.Set(kCartesian, 1, -b_direction.Get(kCartesian, 0));
          FloatType intersection_on_a =
              Coordinates2D::LineHyperplaneIntersection(
                  a_component[i], a_direction, b_component[j], b_normal);
          FloatType intersection_on_b =
              Coordinates2D::LineHyperplaneIntersection(
                  b_component[j], b_direction, a_component[i], a_normal);
          if (isnan(intersection_on_a) || isnan(intersection_on_b)) {
            // Edges a and b are colinear; we should find the first
            // point of the overlapping region, if there is one:
            static const FloatType a0 = 0.0;
            static const FloatType a1 = 1.0;
            FloatType b0 = Coordinates2D::LineHyperplaneIntersection(
                a_component[i], a_direction,
                b_component[j], b_direction);
            FloatType b1 = Coordinates2D::LineHyperplaneIntersection(
                a_component[i], a_direction,
                b_component[(j + 1) % n], b_direction);
            if (a0 == b0) {
              // a[i] == b[j]
              intersection_on_a = 0.0;
              intersection_on_b = 0.0;
            } else if (b0 <= a0 && a0 < b1) {
              // The line segment b[j]-b[j+1] includes the point a[i]
              intersection_on_a = 0.0;
              intersection_on_b = (a0 - b0) / (b1 - b0);
            } else if (b1 < a0 && a0 <= b0) {
              // The line segment b[j+1]-b[j] includes the point a[i]
              intersection_on_a = 0.0;
              intersection_on_b = (a0 - b1) / (b0 - b1);
            } else if (a0 <= b0 && b0 < a1) {
              // The line segment a[i]-a[i+1] includes the point b[j]
              intersection_on_a = (b0 - a0) / (a1 - a0);
              intersection_on_b = 0.0;
            }
          }
          // Now that we have the intersection parameters, test for
          // intersection.
          //   Note:
          //     We offset the intersection parameters here by a small
          //     constant, experimentally determined through the use of this
          //     class in optical_simulation, to make sure that we don't miss
          //     an intersection at the beginning of an edge.  This is because
          //     it's often the case that the previous edges on either
          //     component are parallel, and this algorithm can't tell from
          //     looking at them which component to follow if this intersection
          //     is missed.
          //     For example:
          //       Consider the following case, where = represents coincident
          //       line segments, with edges oriented from left to right:
          //         \                /
          //          \              /
          //           \1          2/
          //            +==========+--------
          //           /
          //          /
          //       We detect intersection 1 but don't know which of the
          //       components is on the inside and which is on the outside,
          //       because the edges leaving 1 are parallel.  If we miss
          //       intersection 2, we can get off track.  So we need to make
          //       sure we don't miss intersection 2!
          static const FloatType kIntersectionOffset = 100.0 * kFloatTypeError;
          if (0.0 <= intersection_on_a + kIntersectionOffset &&
              intersection_on_a + kIntersectionOffset < 1.0 &&
              0.0 <= intersection_on_b + kIntersectionOffset &&
              intersection_on_b + kIntersectionOffset < 1.0) {
            // Vectors to store the (reversed) directions of inbound edges:
            Coordinates2D a_in(a_component[i]);
            Coordinates2D b_in(b_component[j]);
            if (intersection_on_a == 0.0) {
              // A has a vertex here; calculate the reverse of the inbound
              // A-edge:
              a_in.CopyFrom(a_component[i]);
              a_in.LinearCombination(-1.0, 1.0, a_component[(i - 1 + m) % m]);
            } else {
              // We're in the interior of an A-edge, so just use the reverse of
              // a_direction:
              a_in.CopyFrom(a_direction);
              a_in.Multiply(-1.0);
            }
            if (intersection_on_b == 0.0) {
              // B has a vertex here; calculate the reverse of the inbound
              // B-edge:
              b_in.CopyFrom(b_component[j]);
              b_in.LinearCombination(-1.0, 1.0, b_component[(j - 1 + n) % n]);
            } else {
              // We're in the interior of an B-edge, so just use the reverse of
              // b_direction:
              b_in.CopyFrom(b_direction);
              b_in.Multiply(-1.0);
            }

            // Calculate polar angles for the in- and out-vectors, relative to
            // a_in:
            const FloatType a_in_angle = a_in.Get(kPolar, 1);
            FloatType a_out_angle = a_direction.Get(kPolar, 1) - a_in_angle;
            if (a_out_angle < 0.0) {
              a_out_angle += 2.0 * M_PI;
            }
            FloatType b_in_angle = b_in.Get(kPolar, 1) - a_in_angle;
            if (b_in_angle < 0.0) {
              b_in_angle += 2.0 * M_PI;
            }
            FloatType b_out_angle = b_direction.Get(kPolar, 1) - a_in_angle;
            if (b_out_angle < 0.0) {
              b_out_angle += 2.0 * M_PI;
            }

            // Store the coordinate location of this intersection:
            Coordinates2D location(a_component[i]);
            location.LinearCombination(1.0, intersection_on_a, a_direction);

            // If a_out_angle == b_out_angle, or if a_in_angle == b_out_angle
            // and a_out_angle == b_in_angle, then this intersection is
            // ambiguous.
            // Otherwise, there are six possible orderings for the four polar
            // angles (with a_in_angle fixed at 0.0):
            //   A enters B at this intersection:
            //     a_in  < b_out < a_out <  b_in
            //     a_in  <  b_in < b_out < a_out
            //   B enters A at this intersection:
            //     a_in  < a_out < b_out <  b_in
            //     a_in  <  b_in < a_out < b_out
            //   A and B are inside-tangent at this intersection:
            //     a_in  < a_out <  b_in < b_out
            //   A and B are outside-tangent at this intersection:
            //     a_in  < b_out <  b_in < a_out
            if (b_out_angle == a_out_angle ||
                (b_out_angle == 0.0 && a_out_angle == b_in_angle)) {
              // Ambiguous intersection
              const EdgeIntersection intersection = {
                location,
                { { a_component_index, i, intersection_on_a },
                  { b_component_index, j, intersection_on_b } },
                { -1, -1 }
              };
              intersections->push_back(intersection);
            } else if (b_out_angle < a_out_angle && a_out_angle <= b_in_angle) {
              // A enters B at this intersection
              const EdgeIntersection intersection = {
                location,
                { { a_component_index, i, intersection_on_a },
                  { b_component_index, j, intersection_on_b } },
                { b_out_angle == 0.0 ? -1 : 1,
                  a_out_angle == b_in_angle ? -1 : 0 }
              };
              intersections->push_back(intersection);
            } else if (b_in_angle <= b_out_angle && b_out_angle < a_out_angle) {
              // A enters B at this intersection
              const EdgeIntersection intersection = {
                location,
                { { a_component_index, i, intersection_on_a },
                  { b_component_index, j, intersection_on_b } },
                { 1, 0 }
              };
              intersections->push_back(intersection);
            } else if (a_out_angle < b_out_angle && b_out_angle <= b_in_angle) {
              // B enters A at this intersection
              const EdgeIntersection intersection = {
                location,
                { { a_component_index, i, intersection_on_a },
                  { b_component_index, j, intersection_on_b } },
                { 0, 1 }
              };
              intersections->push_back(intersection);
            } else if (b_in_angle <= a_out_angle && a_out_angle < b_out_angle) {
              // B enters A at this intersection
              const EdgeIntersection intersection = {
                location,
                { { a_component_index, i, intersection_on_a },
                  { b_component_index, j, intersection_on_b } },
                { b_in_angle == a_out_angle ? -1 : 0,
                  1 }
              };
              intersections->push_back(intersection);
            } else if (a_out_angle <= b_in_angle && b_in_angle <= b_out_angle) {
              if (a_out_angle == b_in_angle) {
                // A and B are inside-tangent along B's next edge:
                const EdgeIntersection intersection = {
                  location,
                  { { a_component_index, i, intersection_on_a },
                    { b_component_index, j, intersection_on_b } },
                  { -1, 1 }
                };
                intersections->push_back(intersection);
              } else {
                // A and B are inside-tangent at this intersection:
                const EdgeIntersection intersection = {
                  location,
                  { { a_component_index, i, intersection_on_a },
                    { b_component_index, j, intersection_on_b } },
                  { -1, -1 }
                };
                intersections->push_back(intersection);
              }
            } else if (b_out_angle <= b_in_angle && b_in_angle <= a_out_angle) {
              // A and B are outside-tangent:
              if (b_out_angle == 0.0) {
                const EdgeIntersection intersection = {
                  location,
                  { { a_component_index, i, intersection_on_a },
                    { b_component_index, j, intersection_on_b } },
                  { 0, -1 }
                };
                intersections->push_back(intersection);
              } else if (a_out_angle == b_in_angle) {
                const EdgeIntersection intersection = {
                  location,
                  { { a_component_index, i, intersection_on_a },
                    { b_component_index, j, intersection_on_b } },
                  { 1, -1 }
                };
                intersections->push_back(intersection);
              }
            }
          }
        }
      }
    }
  }
}

// BinaryOperation
//   Implements union and intersection operations between this and other.
//   This is a generalization of the Weiler-Atherton algorithm to work with
//   arbitrary sets of components.  The WA algorithm works on two components,
//   starting on one, traversing its vertices, and switching to the other
//   component when it encounters an intersection of edges.  If you start it on
//   a vertex that's outside the other component, you get the union; if you
//   start it on a vertex that's inside the other component, you get the
//   intersection.
void Polygons::BinaryOperation(int operation, const Polygons &other) {
  // Find the list of edge intersections between this and other:
  vector<EdgeIntersection> intersections;
  GetIntersections(*this, other, &intersections);

  // New components that result from the intersection of existing components:
  vector<Polygon> new_components;

  // Track which components we've seen from each of this and other, so that we
  // can include the missed ones after computing the binary operation on the
  // overlapping components:
  set<int> used_components[2];

  // Track which component intersection points we've already dealt with in the
  // loop below:
  set<int> used_intersections;

  // Create new components from the existing intersecting ones until we've used
  // every intersection point:
  while (intersections.size() > used_intersections.size()) {
    Polygon new_component;
    // Find an unused and unambiguous intersection to start from:
    int starting_intersection = 0;
    while (starting_intersection < intersections.size() &&
           (used_intersections.count(starting_intersection) ||
            intersections[starting_intersection].follow_component[operation] ==
            -1)) {
      ++starting_intersection;
    }
    if (starting_intersection == intersections.size()) {
      // There are no unused unambiguous intersections left, so we're done:
      break;
    }
    int next_intersection = starting_intersection;
    // A flag to track which component (0 or A for this, 1 or B for other) we're
    // on:
    int a_or_b =
        intersections[starting_intersection].follow_component[operation];
    int current_component;
    // A map from vertex number in the new component to intersection number in
    // intersections:
    map<int, int> intersections_in_this_component;
    // Track which intersection numbers are used by the current new component:
    set<int> used_intersections_in_this_component;
    // Loop to traverse the two intersecting components; this is the WA
    // algorithm itself:
    do {
      const EdgeIntersection &intersection = intersections[next_intersection];
      // Add this intersection point:
      intersections_in_this_component[new_component.size()] = next_intersection;
      new_component.push_back(intersection.location);
      used_intersections.insert(next_intersection);
      used_intersections_in_this_component.insert(next_intersection);
      // Follow the component that's on the outside (for kUnionOperation) or
      // inside (for kIntersectOperation) after this intersection:
      if (intersection.follow_component[operation] != -1) {
        a_or_b = intersection.follow_component[operation];
      } else {
        // For union, flip to the other component;
        // for intersection, stay on the current component:
        a_or_b = (a_or_b + 1) % 2;
      }
      // working_component is an alias to the current Polygon component:
      const Polygon &working_component = a_or_b == 0 ?
          components_[intersection.position[0].component_index] :
          other.components_[intersection.position[1].component_index];
      // Record that this component is part of the new structure:
      used_components[a_or_b].insert(
          intersection.position[a_or_b].component_index);

      // Track our current component index (on A or B) and our position
      // traversing that component (where each edge is parametrized with
      // parameter length 1):
      current_component = intersection.position[a_or_b].component_index;
      FloatType current_position =
          FloatType(intersection.position[a_or_b].vector_index) +
          intersection.position[a_or_b].parameter;

      // Find the next intersection on the working component:
      FloatType next_intersection_position = current_position;
      FloatType distance_to_next_intersection =
          FloatType(working_component.size());
      // Look for the closest intersection following this one:
      for (int i = 0; i < intersections.size(); ++i) {
        if (current_component !=
            intersections[i].position[a_or_b].component_index) {
          // This intersection doesn't touch the current component, so skip it:
          continue;
        }
        const FloatType position =
            FloatType(intersections[i].position[a_or_b].vector_index) +
            intersections[i].position[a_or_b].parameter;
        // Count positive distance around the component to this position:
        FloatType distance = position - current_position;
        if (distance <= 0.0) {
          distance += FloatType(working_component.size());
        }
        // Track the smallest distance yet found:
        // NOTE:  It is possible, due to rounding errors, for GetIntersections
        //        to return the same intersection at the start of one line
        //        segment (correctly) and at the end of another (spuriously).
        //        Rather than do epsilon-second-guessing in GetIntersections, we
        //        work around this by ensuring that if the same intersection
        //        appears on two segments, we choose the one with the higher
        //        parameter value (ie. the one that appears at the end of the
        //        first segment) before the one with the lower value.
        if (distance < distance_to_next_intersection ||
            (distance == distance_to_next_intersection &&
             intersections[i].position[a_or_b].parameter >
             intersections[next_intersection].position[a_or_b].parameter)) {
          distance_to_next_intersection = distance;
          next_intersection_position = position;
          next_intersection = i;
        }
      }

      // Add the vertices between the current intersection
      // and the next intersection to the new component:
      if (current_position >= next_intersection_position) {
        next_intersection_position += FloatType(working_component.size());
      }
      while (current_position < next_intersection_position) {
        current_position = floor(current_position) + 1.0;
        if (current_position < next_intersection_position) {
          const int index =
              static_cast<int>(current_position) % working_component.size();
          new_component.push_back(working_component[index]);
        }
      }
    } while (!used_intersections_in_this_component.count(next_intersection));

    // Clean up the new component and add it to the list of new components:
    //   It's possible to start on a vertex and never return to it; thus the new
    //   component comes out of the above loop as a rho-shape.  To chop off the
    //   tail, we search the component for the point where next_intersection
    //   first appears and delete everything prior to that point.
    for (map<int, int>::const_iterator
         it = intersections_in_this_component.begin();
         it != intersections_in_this_component.end();
         ++it) {
      if (it->second == next_intersection) {
        // Chop off the tail:
        new_component.erase(new_component.begin(),
                            new_component.begin() + it->first);
        // Remove vertices with internal angle equal to pi:
        RemoveRedundantVertices(&new_component);
        // Add this component to the list of result components:
        new_components.push_back(new_component);
        break;
      }
    }
  }

  // Now we add the leftover components that had no intersections.
  // This is subtle.  Two disjoint components, neither of which includes the
  // other, are both kept in a Union operation and both discarded in an
  // Intersect operation.  But for included components, things get complicated.
  // Here's how it works:
  //   Union:
  //     Solids:  discard if parent on the other list has the same sign
  //     Holes:  discard if parent on the other list has the opposite sign
  //   Intersect:
  //     Solids:  include if parent on the other list has the same sign
  //     Holes:  include if parent on the other list has the opposite sign

  // First we scan the A-list and keep the appropriate components:
  for (int a_index = 0; a_index < ComponentCount(); ++a_index) {
    if (used_components[0].count(a_index)) {
      continue;
    }
    const Polygon &a = components_[a_index];
    bool has_parent = false;
    bool parent_has_same_sign = Area(a) > 0.0;
    for (int b_index = 0; b_index < other.ComponentCount(); ++b_index) {
      const Polygon &b = other.components_[b_index];
      parent_has_same_sign = Area(a) * Area(b) > 0.0;  // Track largest B member
      if (fabs(Area(b)) < fabs(Area(a))) {
        continue;
      }
      if (ContainsPolygon(b, a)) {
        has_parent = true;
        break;
      }
    }
    if (!has_parent) {
      // Use an implicit parent with sign opposite that
      // of the largest member of the b-list:
      parent_has_same_sign = !parent_has_same_sign;
    }
    if (operation == kUnionOperation) {
      if (Area(a) > 0.0) {
        // Solid
        if (!parent_has_same_sign) {
          new_components.push_back(a);
        }
      } else {
        // Hole
        if (parent_has_same_sign) {
          new_components.push_back(a);
        }
      }
    } else if (operation == kIntersectOperation) {
      if (Area(a) > 0.0) {
        // Solid
        if (parent_has_same_sign) {
          new_components.push_back(a);
        }
      } else {
        // Hole
        if (!parent_has_same_sign) {
          new_components.push_back(a);
        }
      }
    }
  }
  // Now we do the same for the B-list:
  for (int b_index = 0; b_index < other.ComponentCount(); ++b_index) {
    if (used_components[1].count(b_index)) {
      continue;
    }
    const Polygon &b = other.components_[b_index];
    bool has_parent = false;
    bool parent_has_same_sign = Area(b) > 0.0;
    for (int a_index = 0; a_index < ComponentCount(); ++a_index) {
      const Polygon &a = components_[a_index];
      parent_has_same_sign = Area(b) * Area(a) > 0.0;  // Track largest A member
      if (fabs(Area(a)) <= fabs(Area(b))) {  // Note:  < here, <= above
        continue;
      }
      if (ContainsPolygon(a, b)) {
        has_parent = true;
        break;
      }
    }
    if (!has_parent) {
      // Use an implicit parent with sign opposite that
      // of the largest member of the a-list:
      parent_has_same_sign = !parent_has_same_sign;
    }
    if (operation == kUnionOperation) {
      if (Area(b) > 0.0) {
        // Solid
        if (!parent_has_same_sign) {
          new_components.push_back(b);
        }
      } else {
        // Hole
        if (parent_has_same_sign) {
          new_components.push_back(b);
        }
      }
    } else if (operation == kIntersectOperation) {
      if (Area(b) > 0.0) {
        // Solid
        if (parent_has_same_sign) {
          new_components.push_back(b);
        }
      } else {
        // Hole
        if (!parent_has_same_sign) {
          new_components.push_back(b);
        }
      }
    }
  }

  // Prune any degenerate components in new_components:
  for (int i = new_components.size() - 1; i >= 0; --i) {
    if (Area(new_components[i]) == 0.0) {
      new_components.erase(new_components.begin() + i);
    }
  }

  // Finally, sort new_components by absolute area...
  sort(new_components.begin(), new_components.end(), ComparePolygonArea);

  // ...and store the result in this:
  components_ = new_components;
}

// RemoveRedundantVertices
//   Removes vertices of the input component with internal angle equal to pi
//   and those that are degenerate
void Polygons::RemoveRedundantVertices(Polygon *polygon) {
  Coordinates2D vector_in;
  Coordinates2D vector_out;
  set<int> to_delete;
  const int n = polygon->size();
  // Iterate over the component, marking redundant vertices as we go:
  for (int i = 0; i < n; ++i) {
    vector_in.CopyFrom((*polygon)[i]);
    vector_in.LinearCombination(1.0, -1.0, (*polygon)[(i - 1 + n) % n]);
    vector_out.CopyFrom((*polygon)[i]);
    vector_out.LinearCombination(-1.0, 1.0, (*polygon)[(i + 1) % n]);
    if (vector_in.EuclideanNormSquared() == 0.0 ||
        Coordinates2D::Cosine(vector_in, vector_out) == 1.0) {
      to_delete.insert(i);
    }
  }
  // Delete the marked vertices:
  for (set<int>::const_reverse_iterator delete_it = to_delete.rbegin();
       delete_it != to_delete.rend();
       ++delete_it) {
    polygon->erase(polygon->begin() + *delete_it);
  }
}

// ApplyFunction
//   Applies callback to each vertex in this
void Polygons::ApplyFunction(const PolygonsApplyFunction &callback) {
  // Iterate over components:
  for (vector<Polygon>::iterator component_iterator = components_.begin();
       component_iterator != components_.end(); ++component_iterator) {
    // Iterate over vertices:
    for (Polygon::iterator vertex_iterator = component_iterator->begin();
         vertex_iterator != component_iterator->end(); ++vertex_iterator) {
      callback.ApplyFunction(&(*vertex_iterator));
    }
  }
}

}  // namespace energy_rec
