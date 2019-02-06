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
#include "src/io.h"

#include <glog/logging.h>
#include "src/field_layout.pb.h"
#include "google/protobuf/text_format.h"  // for Parse
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

namespace energy_rec {

// LoadFieldLayoutFile
//   Loads a field configuration file into a FieldLayout protobuf.
//   Returns true if the file was read successfully, false otherwise.
void LoadFieldLayoutFile(const string &filename, FieldLayout *layout) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  FILE *layout_file = fopen(filename.data(), "r");
  CHECK(layout_file) << "Failed to load layout file " << filename.data();
  google::protobuf::io::ZeroCopyInputStream *layout_stream =
      new google::protobuf::io::FileInputStream(fileno(layout_file));
  CHECK(google::protobuf::TextFormat::Parse(layout_stream, layout))
      << "Failed to parse layout file " << filename.data();
  delete layout_stream;
  fclose(layout_file);
}

// OutputFile
//   Opens a file for writing with the given filename, using either a File
//   struct or an fstream struct.
OutputFile::OutputFile(const string &filename) {
  file_stream_ = new fstream(filename.data(), ios::out);
  CHECK(file_stream_->is_open()) << "Failed to open output file";
}

// ~OutputFile
//   Closes the file and deletes its object (either a File or an fstream).
OutputFile::~OutputFile() {
  file_stream_->close();
  delete file_stream_;
}

// WriteDataBlockSeparator
//   Writes a newline (the gnuplot datablock separator) to the output file.
void OutputFile::WriteDataBlockSeparator() const {
  *file_stream_ << "\n";
}

// WriteDataSetSeparator
//   Writes two newlines (the gnuplot dataset separator) to the output file.
void OutputFile::WriteDataSetSeparator() const {
  WriteDataBlockSeparator();
  WriteDataBlockSeparator();
}

// WriteRecord
//   Writes a gnuplot-formatted record to the output file.
void OutputFile::WriteRecord(SimulationTime timestamp,
                             FloatType x,
                             FloatType y,
                             FloatType value,
                             const string &units) const {
  *file_stream_
      << timestamp << " " << x << " " << y << " "
      << value << " \"" << units << "\"\n";
}

}  // namespace energy_rec
