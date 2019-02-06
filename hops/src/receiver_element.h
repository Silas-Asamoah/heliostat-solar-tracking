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

// An Optic-derived class representing a wall element in a thermal receiver
// cavity.  Overrides GetOutput to provide flux map data.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_RECEIVER_ELEMENT_H_
#define ENERGY_REC_OPTICAL_SIMULATION_RECEIVER_ELEMENT_H_

using namespace std;
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationOutput, etc
#include "src/optic.h"  // for Optic

namespace energy_rec {

class Coordinates2D;
class Coordinates3D;
class Polygons;

class ReceiverElement : public Optic {
 public:
  ReceiverElement(const Coordinates3D &location, const Coordinates3D &normal,
                  const Coordinates2D &projection, const Polygons &shape)
      : Optic(location, normal, projection, shape) {}
  ReceiverElement(const vector<Coordinates3D> &points,
                  const vector<Coordinates2D> &projections)
      : Optic(points, projections) {}
  virtual ~ReceiverElement() {}

  // Overridden Optic methods:
  virtual bool IsATarget(void) const { return true; }
  virtual void GetOutput(SimulationTime sim_time,
                         SimulationOutput *output) const;

 private:
  DISALLOW_COPY_AND_ASSIGN(ReceiverElement);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_RECEIVER_ELEMENT_H_
