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

// HeliostatActuation is a utility class for producing the axis-angle vector
// that takes a heliostat mirror from the flat parked position to a desired
// normal, according to that heliostat's particular actuation kinematics.
// The returned axis-angle vector is that which takes the mirror from the flat
// position with the lower edge of the mirror facing in the negative
// Y-direction to the desired normal.

// Usage:
//   Coordinates3D normal, axis_angle;
//   int actuation_method;
//   HeliostatActuation::kActuationFunctions[actuation_method](normal,
//                                                             &axis_angle)

#ifndef ENERGY_REC_OPTICAL_SIMULATION_HELIOSTAT_ACTUATION_H_
#define ENERGY_REC_OPTICAL_SIMULATION_HELIOSTAT_ACTUATION_H_

using namespace std;
#include "src/field_layout.pb.h"

namespace energy_rec {

class Coordinates3D;

class HeliostatActuation {
 public:
  // Actuation function type, mapping from the normal vector to the axis-angle
  // vector needed to aim the mirror to that normal:
  typedef void(*ActuationFunction)(const Coordinates3D &normal,
                                   Coordinates3D *axis_angle);

  // A table of actuation functions, indexed by the HeliostatType::actuation
  // enum in field_layout.proto:
  static const ActuationFunction kActuationFunctions[
      FieldLayout::HeliostatType::Actuation_ARRAYSIZE];
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_HELIOSTAT_ACTUATION_H_
