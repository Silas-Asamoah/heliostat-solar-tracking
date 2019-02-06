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

// Sun is an Optic-derived class that represents the sun in the sky.  It is
// capable of moving itself via the calculator_ member, which is an
// implementation of the SunCalculator abstract class.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_SUN_H_
#define ENERGY_REC_OPTICAL_SIMULATION_SUN_H_

using namespace std;
#include <cstddef>

#include "src/common.h"  // for SimulationTime, etc.
#include "src/optic.h"  // for Optic
#include "math/coordinates.h"  // for FloatType
#include "math/coordinates2d.h"  // for k2DNullVector
#include "math/coordinates3d.h"  // for k3DNullVector, etc
#include "math/polygons.h"   // for Polygons

namespace energy_rec {

static const int kSunSides = 12;  // polygonal approximation
static const Polygons kSunPolygons(kSunSides, 6.955e8);  // the sun's radius

// An abstract class for locating the sun in the sky:
class SunCalculator {
 public:
  SunCalculator() {}
  virtual ~SunCalculator() {}

  // Calculates the sun's position in the sky and its direct normal irradiance
  // upon the field.
  // Note:  Distance is returned in metres.
  virtual void Calculate(SimulationTime time_step,
                         FloatType *distance,
                         FloatType *azimuth,
                         FloatType *elevation,
                         FloatType *dni) const = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(SunCalculator);
};

class Sun : public Optic {
 public:
  // Constructor for a fixed-position sun:
  explicit Sun(const Coordinates3D &sun_direction) :
    Optic(sun_direction, k3DNullVector, k2DNullVector, kSunPolygons),
    calculator_(NULL), dni_(0.0) {}
  // Constructor for a sun whose position is governed by a SunCalculator:
  explicit Sun(const SunCalculator *calculator) :
    Optic(k3DNullVector, k3DNullVector, k2DNullVector, kSunPolygons),
    calculator_(calculator), dni_(0.0) {}
  // Destructor:
  virtual ~Sun() {
    if (calculator_) {
      delete calculator_;
    }
  }

  // Overridden Optic methods:
  virtual void UpdateSelf(SimulationTime when);
  virtual FloatType OutputIrradiance(const Optic &target,
                                     const Polygons &destination_mask,
                                     const Polygons &source_mask) const {
    return dni_;
  }

 private:
  // the SunCalculator (which we own):
  const SunCalculator *calculator_;
  // Direct normal irradiance (DNI) at the field:
  FloatType dni_;

  DISALLOW_COPY_AND_ASSIGN(Sun);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_SUN_H_
