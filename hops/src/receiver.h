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

// Receiver is an OpticContainer-derived class that represents a thermal
// receiver cavity.  Its constructor takes a function that describes a
// parametric surface and uses it to create its own ReceiverElement members.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_RECEIVER_H_
#define ENERGY_REC_OPTICAL_SIMULATION_RECEIVER_H_

using namespace std;
#include <cstddef>
#include <string>                       // for string
#include <vector>                       // for vector

#include "src/common.h"
#include "src/optic_container.h"

namespace energy_rec {

class Coordinates2D;
class Coordinates3D;
class Optic;

// typedef for a function that describes a parametric surface (with 3D spatial
// coordinates and 2D projection coordinates):
typedef void(*ParametricSurface)(double, double,
                                 Coordinates3D *, Coordinates2D *);

class Receiver : public OpticContainer {
 public:
  // Discretization parameters:
  const int u_rows_;
  const double u_step_size_;
  const int v_rows_;
  const double v_step_size_;

  Receiver(int u_rows, int v_rows, ParametricSurface surface,
           const Coordinates3D &location, const Coordinates3D &direction,
           Optic *aperture)
      : OpticContainer(),
        u_rows_(u_rows), u_step_size_(1.0 / u_rows),
        v_rows_(v_rows), v_step_size_(1.0 / v_rows),
        aperture_(aperture) {
    InitializeReceiver(surface, location, direction);
    if (aperture_ != NULL) {
      // aperture_ should be a member of our list of Optics, so that we call
      // its UpdateSelf and GetOutput methods at each simulation step, and so
      // that we delete it when we deconstruct:
      AddMember(aperture_);
    }
  }

  virtual ~Receiver() {}

  // Overridden OpticContainerMethods:
  virtual void GetPotentialMaskers(const Coordinates3D &direction,
                                   const Optic *optic,
                                   vector<const Optic *> *potential_maskers);
  virtual const string name(void) const { return "receiver"; }

 protected:
  void InitializeReceiver(ParametricSurface surface,
                          const Coordinates3D &location,
                          const Coordinates3D &direction);

 private:
  Optic *aperture_;  // memory-managed by superclass's members_ member

  DISALLOW_COPY_AND_ASSIGN(Receiver);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_RECEIVER_H_
