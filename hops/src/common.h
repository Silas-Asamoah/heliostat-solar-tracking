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

#ifndef ENERGY_REC_OPTICAL_SIMULATION_COMMON_H_
#define ENERGY_REC_OPTICAL_SIMULATION_COMMON_H_

using namespace std;
#include <map>                          // for map, map<>::value_compare
#include <string>                       // for string, operator<
#include <vector>                       // for vector

#include "math/coordinates.h"  // for kCartesian, FloatType
#include "math/coordinates2d.h"  // for Coordinates2D

namespace energy_rec {

//////////////////////////////
// DISALLOW_COPY_AND_ASSIGN //
//////////////////////////////

// This is a handy macro to prevent unintended copy and assignment semantics on
// classes.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)


/////////////////////////////
// Simulation timestamping //
/////////////////////////////

// Simulation time is a simulation-intrinsic clock value used for updating the
// elements of the simulation and for timestamping the output.  Its only
// requirements are that it be non-negative and non-decreasing.
// However, if automatic sun positioning is used, the SOLPOS class will treat
// simulation time as Unix time.
typedef int SimulationTime;

const SimulationTime kInitialTimeStamp = -1;


/////////////////////////////////
// Output types and structures //
/////////////////////////////////

// A single output value:
struct OutputPoint {
  Coordinates2D location;
  FloatType value;
  string units;  // TODO(tpw):  enumerate units
};

// A vector of output values (for example, values from a set of elements at a
// fixed time):
typedef vector<OutputPoint> OutputVector;

// A time series of OutputVectors (for example, an animated heat map):
typedef map<SimulationTime, OutputVector> OutputTimeSeries;

// A collection of labeled time series (comprising all the simulation's
// outputs):
typedef map<string, OutputTimeSeries> SimulationOutput;

// A comparator for sorting OutputPoint structs by y-coordinate and then
// x-coordinate:
inline bool OutputPointComp(const OutputPoint &a, const OutputPoint &b) {
  if (a.location.Get(kCartesian, 1) != b.location.Get(kCartesian, 1)) {
    return a.location.Get(kCartesian, 1) < b.location.Get(kCartesian, 1);
  }
  return a.location.Get(kCartesian, 0) < b.location.Get(kCartesian, 0);
}

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_COMMON_H_
