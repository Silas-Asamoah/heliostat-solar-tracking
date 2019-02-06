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

#include "config.h"
using namespace std;
#include "src/aperture.h"

#include <map>                          // for map<>::mapped_type

#include <gflags/gflags.h>      // for <anonymous>, clstring, etc

DEFINE_string(aperture_flux_spill, "",
              "output filename for flux spilled at aperture");

namespace energy_rec {

// GetOutput
//   If --aperture_flux_spill is specified, appends the irradiance incident on
//   this element to output.
void Aperture::GetOutput(SimulationTime sim_time,
                         SimulationOutput *output) const {
  if (!FLAGS_aperture_flux_spill.empty()) {
    const OutputPoint output_point = {
      projection(),
      InputFlux(),
      "Aperture flux spill (W)"
    };
    (*output)[FLAGS_aperture_flux_spill][sim_time].push_back(output_point);
  }
}

// CreateAperturePolygon
//   This method instantiates and returns a copy of a Polygons object, which
//   is in turn copied and discarded by the constructor, but since this
//   method is called only once per instantiation, it's not too bad.
Polygons Aperture::CreateAperturePolygon(FloatType radius, int sides) {
  Polygons aperture_polygon(sides, radius);
  // Turn aperture_polygon from a solid into a hole:
  aperture_polygon.Reverse();
  return aperture_polygon;
}

}  // namespace energy_rec
