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
#include "src/heliostat.h"

#include "gtest/gtest.h"

// TODO(tpw):  Test the remaining methods of Heliostat.

namespace energy_rec {

class HeliostatTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Make optic_one_ a square object located at (0, 0, 100) and facing
    // down towards the origin:
    Coordinates3D square_location;
    square_location.Set(kCartesian, 2, 100.0);
    Coordinates3D square_normal;
    square_normal.Set(kCartesian, 2, -1.0);
    Polygons square_shape(4, 1.0);
    optic_one_ = new Optic(square_location, square_normal,
                           k2DNullVector, square_shape);

    // Make optic_two_ a square object located at (0, 0, 50) and facing
    // down towards the origin:
    square_location.Set(kCartesian, 2, 50.0);
    optic_two_ = new Optic(square_location, square_normal,
                           k2DNullVector, square_shape);
  }

  virtual void TearDown() {
    delete optic_one_;
    delete optic_two_;
  }

  Optic *optic_one_;
  Optic *optic_two_;
};

// Test that a Heliostat can reflect light without dropping it:
TEST_F(HeliostatTest, SimpleReflection) {
  // Put a heliostat at the origin facing directly upwards:
  const FloatType reflectivity = 0.8;
  const Polygons square_mirror(4, 1.0);
  Heliostat heliostat(k3DNullVector, k2DNullVector, k3DNullVector,
                      reflectivity, 0.0, 0, square_mirror);

  // Send a packet of light from optic_one_ to heliostat:
  const FloatType input_irradiance = 5.0;
  const IncidentLight incident_light = {
    input_irradiance, optic_one_->normal(), heliostat.shape(), &(*optic_one_)
  };
  heliostat.ReceiveIncidentLight(incident_light);

  // Calculate the values we expect for heliostat:
  const FloatType expected_flux_in =
      input_irradiance * heliostat.shape().Area();
  const FloatType expected_irradiance_out =
      expected_flux_in * reflectivity / optic_two_->shape().Area();

  // Check that heliostat counted the input irradiance correctly:
  EXPECT_NEAR(input_irradiance, heliostat.InputIrradiance(), kFloatTypeError);
  // Check that heliostat received all the flux we expect:
  EXPECT_NEAR(expected_flux_in, heliostat.InputFlux(), kFloatTypeError);
  // Check that heliostat reflects all the light back up:
  EXPECT_NEAR(expected_irradiance_out,
              heliostat.OutputIrradiance(*optic_two_,
                                         optic_two_->shape(),
                                         heliostat.shape()),
              kFloatTypeError);
}

}  // namespace energy_rec
