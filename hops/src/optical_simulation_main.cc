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
// This is the main executable for optical_simulation, a tool for simulating
// optical flux from a heliostat field to a receiver cavity.
// TODO(tpw):  Regression test!

#include "config.h"
using namespace std;
#include <ctime>
#include <cstdlib>


#include <gflags/gflags.h>      // for <anonymous>, DEFINE_int32, etc
#include <glog/logging.h>               // for operator<<, etc
#include "src/common.h"  // for SimulationTime
#include "src/simulation.h"  // for Simulation

DEFINE_string(layout_file, "", "layout file in field_layout.proto format");
// TODO(tpw):  Add date-string parsing so these flags don't have to be ints:
DEFINE_int32(start_time, 0, "Simulation start time (assumed as Unix time)");
DEFINE_int32(end_time, 0, "Simulation end time (assumed as Unix time)");
DEFINE_int32(time_step, 1, "Simulation time step size (assumed seconds)");

int main(int argc, char **argv) {
  // Initialization:
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  srand(time(NULL));

  // Check that the user has specified a field layout:
  if (FLAGS_layout_file.empty()) {
    LOG(ERROR) << "The flag --layout_file is required.";
    return 1;
  }

  // Set up the simulation framework:
  energy_rec::Simulation simulation;
  CHECK(simulation.LoadConfigurationFromFile(FLAGS_layout_file));

  // Time variables from flags:
  const energy_rec::SimulationTime start_time = FLAGS_start_time;
  const energy_rec::SimulationTime end_time = FLAGS_end_time;
  const energy_rec::SimulationTime time_step = FLAGS_time_step;

  // The main simulation loop:
  energy_rec::SimulationTime simulation_time = 0;
  for (simulation_time = start_time; simulation_time <= end_time;
       simulation_time += time_step) {
    if (end_time > start_time) {
      const double percent =
          1e2 * (simulation_time - start_time) / (end_time - start_time);
      LOG(INFO)
          << "Time step " << simulation_time
          << " (" << static_cast<int>(percent) << "%)";
    }
    simulation.Simulate(simulation_time);
  }

  LOG(INFO) << "Writing output to disk.";
  simulation.WriteOutput();
}
