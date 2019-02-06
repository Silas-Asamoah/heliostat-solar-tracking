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

// OpticContainer is a class that stores a collection of Optic elements and
// provides accessors as well as methods that act on all of its members.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_OPTIC_CONTAINER_H_
#define ENERGY_REC_OPTICAL_SIMULATION_OPTIC_CONTAINER_H_

using namespace std;
#include <string>                       // for string
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationTime, etc

namespace energy_rec {

class Coordinates3D;
class Optic;

// A container for Optics
class OpticContainer {
 public:
  OpticContainer() {}
  virtual ~OpticContainer();

  // Identifier string for use in logging:
  virtual const string name(void) const { return "OpticContainer"; }

  // Member management:
  void AddMember(Optic *optic) { members_.push_back(optic); }
  vector<Optic *>& members(void) { return members_; }  // TODO(tpw): make const!

  // This method should be overridden by subclasses, to populate a vector of
  // Optics which can potentially shadow/block a given Optic in a given
  // direction:
  virtual void GetPotentialMaskers(const Coordinates3D &direction,
                                   const Optic *optic,
                                   vector<const Optic *> *potential_maskers) {}

  // These methods simply call the corresponding methods in their member Optics:
  void ClearIncidentLight(void);
  void UpdateSelf(SimulationTime when);
  void GetOutput(SimulationTime sim_time, SimulationOutput *output);

 private:
  vector<Optic *> members_;

  DISALLOW_COPY_AND_ASSIGN(OpticContainer);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_OPTIC_CONTAINER_H_
