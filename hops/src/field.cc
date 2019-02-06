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
#include "src/field.h"

#include <vector>                       // for vector, etc

#include <gflags/gflags.h>      // for DEFINE_bool
#include "src/optic.h"  // for Optic

DEFINE_bool(disable_field_masking, false, "Ignore field shadowing/blocking");

namespace energy_rec {

class Coordinates3D;

// GetPotentialMaskers
//   Adds to potential_maskers every Optic in the field whose radius, when
//   projected in the direction direction_in, overlaps that of the given Optic.
//   This is the set of Optics which could potentially mask the given Optic
//   under reorientation of one or both Optics.
void Field::GetPotentialMaskers(const Coordinates3D &direction_in,
                                const Optic *optic,
                                vector<const Optic *> *potential_maskers) {
  if (FLAGS_disable_field_masking) {
    return;
  }
  // Iterate over all members of this Field:
  for (vector<Optic *>::const_iterator it = members().begin();
       it != members().end(); ++it) {
    // Test for potential masking:
    if (optic->MayBeMaskedBy(direction_in, *it)) {
      potential_maskers->push_back(*it);
    }
  }
}

}  // namespace energy_rec
