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

#include <cmath>                        // for M_PI
#include <cstddef>                      // for NULL
#include <map>                          // for map<>::mapped_type

#include "math/coordinates3d.h"  // for Coordinates3D

#include "gtest/gtest.h"

namespace energy_rec {

class ApertureTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Set up an Aperture at the origin, pointing down, with radius 1.1m:
    Coordinates3D location;
    Coordinates3D normal;
    normal.Set(kCartesian, 2, -1.0);
    aperture_ = new Aperture(location, normal, 1.1);
  }

  virtual void TearDown() {
    delete aperture_;
  }

  Aperture *aperture_;
};

TEST_F(ApertureTest, TestConstructor) {
  // Check that aperture_ has the expected properties:
  EXPECT_DOUBLE_EQ(0.0, aperture_->location().Get(kCartesian, 0));
  EXPECT_DOUBLE_EQ(0.0, aperture_->location().Get(kCartesian, 1));
  EXPECT_DOUBLE_EQ(0.0, aperture_->location().Get(kCartesian, 2));
  EXPECT_DOUBLE_EQ(0.0, aperture_->normal().Get(kCartesian, 0));
  EXPECT_DOUBLE_EQ(0.0, aperture_->normal().Get(kCartesian, 1));
  EXPECT_DOUBLE_EQ(-1.0, aperture_->normal().Get(kCartesian, 2));
  EXPECT_EQ(1, aperture_->shape().ComponentCount());
  EXPECT_EQ(kApertureSides, aperture_->shape().VertexCount());
  EXPECT_NEAR(-(1.1 * 1.1 * M_PI), aperture_->shape().Area(), kFloatTypeError);
}

TEST_F(ApertureTest, TestGetOutput) {
  // Simulate a couple of time steps and check that --aperture_flux_spill
  // produces the correct output:
  SimulationOutput output;
  FLAGS_aperture_flux_spill = "afs";

  // At time step 0, put no incident light on the Aperture:
  aperture_->ClearIncidentLight();
  aperture_->GetOutput(0, &output);

  // At time step 1, put irradiance of 5 W/m^2 on the Aperture:
  aperture_->ClearIncidentLight();
  Coordinates3D direction;
  direction.Set(kCartesian, 2, 1.0);
  // Only the area of the incident light mask matters to Aperture; in a real
  // application, OpticContainerConnection calculates how much incident light
  // is spilled:
  Polygons mask(3, 1.0);  // using the circle-approxmation constructor for
                          // convenience, so mask's area is pi
  IncidentLight incident_light = { 5.0, direction, mask, NULL };
  aperture_->ReceiveIncidentLight(incident_light);
  aperture_->GetOutput(1, &output);

  // Check the output:
  ASSERT_EQ(1, output.size());
  ASSERT_EQ(2, output["afs"].size());

  // For time step 0:
  ASSERT_EQ(1, output["afs"][0].size());
  // location should be null:
  EXPECT_DOUBLE_EQ(0.0, output["afs"][0][0].location.Get(kCartesian, 0));
  EXPECT_DOUBLE_EQ(0.0, output["afs"][0][0].location.Get(kCartesian, 1));
  // value should be 0:
  EXPECT_DOUBLE_EQ(0.0, output["afs"][0][0].value);

  // For time step 1:
  ASSERT_EQ(1, output["afs"][1].size());
  // location should be null:
  EXPECT_DOUBLE_EQ(0.0, output["afs"][1][0].location.Get(kCartesian, 0));
  EXPECT_DOUBLE_EQ(0.0, output["afs"][1][0].location.Get(kCartesian, 1));
  // value should be the flux on mask:
  EXPECT_NEAR(5.0 * mask.Area(), output["afs"][1][0].value, kFloatTypeError);
  // units should be W:
  EXPECT_EQ("Aperture flux spill (W)", output["afs"][1][0].units);
}

TEST_F(ApertureTest, TestCreateAperturePolygon) {
  // Create Aperture Polygons and check that
  // they are holes with (negative) area pi:
  Polygons polygons = Aperture::CreateAperturePolygon(1.0, 3);
  EXPECT_EQ(1, polygons.ComponentCount());
  EXPECT_EQ(3, polygons.VertexCount());
  EXPECT_NEAR(-M_PI, polygons.Area(), kFloatTypeError);

  polygons = Aperture::CreateAperturePolygon(1.0, 20);
  EXPECT_EQ(1, polygons.ComponentCount());
  EXPECT_EQ(20, polygons.VertexCount());
  EXPECT_NEAR(-M_PI, polygons.Area(), kFloatTypeError);

  polygons = Aperture::CreateAperturePolygon(1.0, 100);
  EXPECT_EQ(1, polygons.ComponentCount());
  EXPECT_EQ(100, polygons.VertexCount());
  EXPECT_NEAR(-M_PI, polygons.Area(), kFloatTypeError);
}

}  // namespace energy_rec
