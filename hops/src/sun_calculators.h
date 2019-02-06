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

// SOLPOS is an implementation of SunCalculator using the SOLPOS solar
// position calculator from NREL (National Renewable Energy Laboratory).

#ifndef ENERGY_REC_OPTICAL_SIMULATION_SUN_CALCULATORS_H_
#define ENERGY_REC_OPTICAL_SIMULATION_SUN_CALCULATORS_H_

using namespace std;
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationTime
#include "src/sun.h"  // for SunCalculator
#include "math/coordinates.h"  // for FloatType
#include "math/coordinates3d.h"  // for Coordinates3D

namespace energy_rec {

class SOLPOS : public SunCalculator {
 public:
  explicit SOLPOS(const Coordinates3D &field_location)
      : SunCalculator(), field_location_(field_location) {}
  virtual ~SOLPOS() {}

  virtual void Calculate(SimulationTime time_step,
                         FloatType *distance,
                         FloatType *azimuth,
                         FloatType *elevation,
                         FloatType *dni) const;

 private:
  // The observer's location on Earth; longitude and latitude correspond
  // respectively to azimuth and elevation in Coordinates3D's kAstronomical
  // coordinate system:
  const Coordinates3D field_location_;

  DISALLOW_COPY_AND_ASSIGN(SOLPOS);
};

class SunPositionList : public SunCalculator {
 public:
  explicit SunPositionList(const vector<Coordinates3D> &sun_vectors,
                           const vector<FloatType> &dnis)
      : SunCalculator(), sun_vectors_(sun_vectors), dnis_(dnis) {}
  virtual ~SunPositionList() {}

  virtual void Calculate(SimulationTime time_step,
                         FloatType *distance,
                         FloatType *azimuth,
                         FloatType *elevation,
                         FloatType *dni) const;

 private:
  // The list of stored (non-normalized) sun vectors and DNIs (direct normal
  // irradiance):
  const vector<Coordinates3D> sun_vectors_;
  const vector<FloatType> dnis_;

  DISALLOW_COPY_AND_ASSIGN(SunPositionList);
};

// Add other implementations here if desired...

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_SUN_CALCULATORS_H_
