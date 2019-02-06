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
//
// This file wraps file IO methods for the optical simulation, allowing us to
// switch out file interfaces cleanly.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_IO_H_
#define ENERGY_REC_OPTICAL_SIMULATION_IO_H_

using namespace std;
#include <fstream>

#include <string>

#include "src/common.h"
#include "math/coordinates.h"


namespace energy_rec {

class FieldLayout;

// LoadConfigurationFromFile
//   Loads a field configuration file into a FieldLayout protobuf.
//   Asserts that the file was read successfully and terminates otherwise.
void LoadFieldLayoutFile(const string &filename, FieldLayout *layout);

// switched by MOE directives.
class OutputFile {
 public:
  explicit OutputFile(const string &filename);
  virtual ~OutputFile();
  void WriteDataBlockSeparator() const;  // a newline in gnuplot
  void WriteDataSetSeparator() const;  // two newlines in gnuplot
  void WriteRecord(SimulationTime timestamp,
                   FloatType x,
                   FloatType y,
                   FloatType value,
                   const string &units) const;
 private:
  fstream *file_stream_;
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_IO_H_
