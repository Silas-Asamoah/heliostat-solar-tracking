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
#include "src/optic_container.h"

#include "src/optic.h"  // for Optic
#include "src/stl_delete_elements.h"

namespace energy_rec {

// ~OpticContainer
OpticContainer::~OpticContainer() {
  // OpticContainers own their members!
  STLDeleteElements(&members_);
}

// ClearIncidentLight
//   Calls Optic::ClearIncidentLight for each Optic in members_.
void OpticContainer::ClearIncidentLight(void) {
  for (vector<Optic *>::const_iterator it = members().begin();
       it != members().end();
       ++it) {
    (*it)->ClearIncidentLight();
  }
}

// UpdateSelf
//   Calls Optic::UpdateSelf for each Optic in members_.
void OpticContainer::UpdateSelf(SimulationTime when) {
  for (vector<Optic *>::iterator it = members().begin();
       it != members().end(); ++it) {
    (*it)->UpdateSelf(when);
  }
}

// GetOutput
//   Calls Optic::GetOutput for each Optic in members_.
void OpticContainer::GetOutput(SimulationTime sim_time,
                               SimulationOutput *output) {
  for (vector<Optic *>::iterator it = members().begin();
       it != members().end(); ++it) {
    (*it)->GetOutput(sim_time, output);
  }
}

}  // namespace energy_rec
