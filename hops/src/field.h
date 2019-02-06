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

// Field is an OpticContainer-derived class that contains Heliostat objects in a
// field.  It overrides GetPotentialMaskers to find heliostats which could
// shadow or block one another as they rotate.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_FIELD_H_
#define ENERGY_REC_OPTICAL_SIMULATION_FIELD_H_

using namespace std;
#include <string>                       // for string
#include <vector>                       // for vector

#include "src/common.h"
#include "src/optic_container.h"

namespace energy_rec {

class Coordinates3D;
class Optic;

class Field : public OpticContainer {
 public:
  Field() : OpticContainer() {}
  virtual ~Field() {}

  // Overridden OpticContainer methods:
  virtual void GetPotentialMaskers(const Coordinates3D &direction,
                                   const Optic *optic,
                                   vector<const Optic *> *potential_maskers);
  virtual const string name(void) const { return "field"; }

 private:
  DISALLOW_COPY_AND_ASSIGN(Field);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_FIELD_H_
