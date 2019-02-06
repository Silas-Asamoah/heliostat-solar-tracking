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
#include "src/sun_calculators.h"

#include <cmath>
#include <ctime>

#include <glog/logging.h>
#include "solpos/solpos.h"

namespace energy_rec {

static const FloatType kAstronomicalUnit = 1.49597870700e11;  // in metres

void SOLPOS::Calculate(SimulationTime time_step,
                       FloatType *distance,
                       FloatType *azimuth,
                       FloatType *elevation,
                       FloatType *dni) const {
  // Convert Unix time to the values SOLPOS needs:
  time_t time_step_time_t = time_step;
  tm time_struct;
  gmtime_r(&time_step_time_t, &time_struct);

  // Populate a SOLPOS data structure:
  struct posdata solpos_data;
  S_init(&solpos_data);
  solpos_data.longitude = field_location_.Get(kAstronomical, 1);
  solpos_data.latitude = field_location_.Get(kAstronomical, 2);
  solpos_data.timezone = 0.0;  // work from UTC
  solpos_data.year = 1900 + time_struct.tm_year;
  solpos_data.daynum = 1 + time_struct.tm_yday;
  solpos_data.hour = time_struct.tm_hour;
  solpos_data.minute = time_struct.tm_min;
  solpos_data.second = time_struct.tm_sec;

  // Call SOLPOS:
  long result = S_solpos(&solpos_data);  // NOLINT (solpos is third-party)
  if (result) {
    S_decode(result, &solpos_data);
    LOG(FATAL) << "SOLPOS error.";
  }

  // Compute sun position:
  *distance = kAstronomicalUnit * solpos_data.erv;
  *azimuth = solpos_data.azim;
  *elevation = solpos_data.elevref;

  // Compute insolation at the top of the Earth's atmosphere:
  FloatType extraterrestrial_irradiance = solpos_data.etrn;

  // Compute DNI from extraterrestrial irradiance:
  const double elevation_sine = sin(*elevation * M_PI / 180.0);
  if (elevation_sine > 0.0) {
    // Compute airmass using Rozenberg's formula:
    // (see http://en.wikipedia.org/wiki/Airmass for this and alternatives):
    const double air_masses =
        1.0 / (elevation_sine + 0.025 * exp(-11.0 * elevation_sine));
    // Compute atmospheric attenuation from airmass
    // (see http://www.asterism.org/tutorials/tut28-1.htm):
    const double attenuation_magnitude = 0.28 * air_masses;
    // Compute attenuated irradiance:
    *dni = extraterrestrial_irradiance * pow(1e2, -attenuation_magnitude / 5.0);
  } else {
    *dni = 0.0;
  }
}

void SunPositionList::Calculate(SimulationTime time_step,
                                FloatType *distance,
                                FloatType *azimuth,
                                FloatType *elevation,
                                FloatType *dni) const {
  CHECK(0 <= time_step && time_step < sun_vectors_.size())
      << "Simulation timestamp " << time_step
      << " out of bounds for sun positions list of length "
      << sun_vectors_.size();
  CHECK(0 <= time_step && time_step < dnis_.size())
      << "Simulation timestamp " << time_step
      << " out of bounds for DNIs list of length " << sun_vectors_.size();
  // Put the sun at 1 AU from the observer and look up its direction from the
  // (assumed unnormalized) entries in sun_vectors_ and its DNI from dnis_:
  *distance = kAstronomicalUnit;
  *azimuth = sun_vectors_[time_step].Get(kAstronomical, 1);
  *elevation = sun_vectors_[time_step].Get(kAstronomical, 2);
  *dni = dnis_[time_step];
}

}  // namespace energy_rec
