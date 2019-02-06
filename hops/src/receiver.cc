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
// TODO(tpw): unittests!

#include "config.h"
using namespace std;
#include "src/receiver.h"

#include <cmath>                        // for M_PI
#include <cstddef>                      // for NULL
#include <map>                          // for map, map<>::mapped_type
#include <utility>                      // for pair, make_pair
#include <vector>                       // for vector

#include "src/receiver_element.h"
#include "math/coordinates.h"  // for FloatType
#include "math/coordinates2d.h"  // for Coordinates2D
#include "math/coordinates3d.h"  // for Coordinates3D, etc

namespace energy_rec {

void Receiver::InitializeReceiver(ParametricSurface surface,
                                  const Coordinates3D &location,
                                  const Coordinates3D &direction) {
  // Inclination of the endcap in radians from zenith:
  const FloatType theta = M_PI - direction.Get(kSpherical, 1);
  // Azimuth of the mouth in radians from east:
  const FloatType phi = direction.Get(kSpherical, 2);

  // Sample a discrete lattice from the parametric surface:
  map<pair<int, int>, Coordinates3D> points;
  map<pair<int, int>, Coordinates2D> projections;
  Coordinates3D point;
  Coordinates2D projection;
  for (int i = 0; i <= u_rows_; i++) {
    for (int j = 0; j <= v_rows_; j++) {
      surface(u_step_size_ * i, v_step_size_ * j, &point, &projection);
      // Rotate about the Y axis to the desired elevation...
      point.RodriguesRotation(-theta, k3DYAxis);
      // ...and then about the Z axis to the desired azimuth:
      point.RodriguesRotation(phi, k3DZAxis);
      // Translate to location:
      point.Add(location);
      // Add the point to points:
      points[make_pair(i, j)] = point;
      projections[make_pair(i, j)] = projection;
    }
  }

  // Create a set of receiver elements from the above points:
  vector<Coordinates3D> element_points;
  vector<Coordinates2D> element_projections;
  for (int i = 0; i < u_rows_; i++) {
    for (int j = 0; j < v_rows_; j++) {
      // Create a trapezoidal element from four adjacent points:
      element_points.clear();
      element_points.push_back(points[make_pair(i, j)]);
      element_points.push_back(points[make_pair(i + 1, j)]);
      element_points.push_back(points[make_pair(i + 1, j + 1)]);
      element_points.push_back(points[make_pair(i, j + 1)]);
      element_projections.clear();
      element_projections.push_back(projections[make_pair(i, j)]);
      element_projections.push_back(projections[make_pair(i + 1, j)]);
      element_projections.push_back(projections[make_pair(i + 1, j + 1)]);
      element_projections.push_back(projections[make_pair(i, j + 1)]);
      AddMember(new ReceiverElement(element_points, element_projections));
    }
  }
}

void Receiver::GetPotentialMaskers(const Coordinates3D &direction,
                                   const Optic *optic,
                                   vector<const Optic *> *potential_maskers) {
  // Note:
  //   We assume that the Aperture is the only Optic needed to compute shadows
  //   on the Receiver.  That is, if you're looking through the Optic, no
  //   ReceiverElement overlaps another.  This requirement is satisfied for any
  //   Receiver that, including the Aperture, is non-concave.
  // TODO(tpw):  Add DCHECKs to validate this assumption.
  // Thus, the Aperture is the sole masker of each ReceiverElement, but it does
  // not mask itself:
  if (aperture_ != NULL && aperture_ != optic) {
    potential_maskers->push_back(aperture_);
  }
}

}  // namespace energy_rec
