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
#include "src/receiver_element.h"

#include <map>                          // for map<>::mapped_type

#include <gflags/gflags.h>      // for <anonymous>, clstring, etc

DEFINE_string(receiver_flux, "", "output name for receiver flux");
DEFINE_string(receiver_irradiance, "", "output name for receiver irradiance");

namespace energy_rec {

// GetOutput
//   If --receiver_irradiance is specified, appends the irradiance incident on
//   this element to output.
void ReceiverElement::GetOutput(SimulationTime sim_time,
                                SimulationOutput *output) const {
  if (!FLAGS_receiver_flux.empty()) {
    const OutputPoint output_point = {
      projection(),
      InputFlux(),
      "Receiver flux (W)"
    };
    (*output)[FLAGS_receiver_flux][sim_time].push_back(output_point);
  }
  if (!FLAGS_receiver_irradiance.empty()) {
    const OutputPoint output_point = {
      projection(),
      InputIrradiance(),
      "Receiver irradiance (W / m^2)"
    };
    (*output)[FLAGS_receiver_irradiance][sim_time].push_back(output_point);
  }
}

}  // namespace energy_rec
