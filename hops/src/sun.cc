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
#include "src/sun.h"

namespace energy_rec {

// UpdateSelf
//   If calculator_ is not NULL, calls calculator_->Calculate and then updates
//   the sun's position in the sky using SetLocation().
void Sun::UpdateSelf(SimulationTime when) {
  if (calculator_) {
    // Move to the correct position in the sky and update dni_:
    double distance, azimuth, elevation;
    calculator_->Calculate(when, &distance, &azimuth, &elevation, &dni_);
    const FloatType value[3] = { distance, azimuth, elevation };
    Coordinates3D location(kAstronomical, value);
    SetLocation(location, when);
  }
  // Set the normal to point back to the origin (so that the Sun is 'facing'
  // the origin):
  Coordinates3D normal(location());
  normal.Invert();
  SetNormal(normal, when);
}

}  // namespace energy_rec
