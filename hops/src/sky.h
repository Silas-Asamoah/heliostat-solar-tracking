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

// An OpticContainer representing the sky.  Overrides name().

#ifndef ENERGY_REC_OPTICAL_SIMULATION_SKY_H_
#define ENERGY_REC_OPTICAL_SIMULATION_SKY_H_

using namespace std;
#include <string>                       // for string

#include "src/common.h"
#include "src/optic_container.h"

namespace energy_rec {

class Sky : public OpticContainer {
 public:
  Sky() : OpticContainer() {}
  virtual ~Sky() {}
  virtual const string name(void) const { return "sky"; }

 private:
  DISALLOW_COPY_AND_ASSIGN(Sky);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_SKY_H_
