// Copyright 2011 Google Inc. All Rights Reserved.
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

// An Optic-derived class representing an aperture in front of a Receiver; this
// is a polygonal approximation to a circular hole.  Overrides GetOutput to
// provide spilled flux data.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_APERTURE_H_
#define ENERGY_REC_OPTICAL_SIMULATION_APERTURE_H_

using namespace std;
#include <gflags/gflags_declare.h>  // for DECLARE_string
#include "src/common.h"  // for SimulationOutput
#include "src/optic.h"  // for Optic
#include "math/coordinates.h"  // for FloatType
#include "math/coordinates2d.h"  // for k2DNullVector
#include "math/polygons.h"   // for Polygons

DECLARE_string(aperture_flux_spill);

namespace energy_rec {

class Coordinates3D;

static const int kApertureSides = 20;

class Aperture : public Optic {
 public:
  Aperture(const Coordinates3D &location, const Coordinates3D &normal,
           FloatType radius)
      : Optic(location, normal, k2DNullVector,
              CreateAperturePolygon(radius, kApertureSides)) {}
  virtual ~Aperture() {}

  virtual void GetOutput(SimulationTime sim_time,
                         SimulationOutput *output) const;

  static Polygons CreateAperturePolygon(FloatType radius, int sides);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_APERTURE_H_
