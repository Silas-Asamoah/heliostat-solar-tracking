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
// A self-contained implementation of STLDeleteElements

#ifndef ENERGY_REC_OPTICAL_SIMULATION_STL_DELETE_ELEMENTS_H_
#define ENERGY_REC_OPTICAL_SIMULATION_STL_DELETE_ELEMENTS_H_

using namespace std;
namespace energy_rec {

template <class T>
void STLDeleteElements(T *stl_container) {
  if (stl_container) {
    for (typename T::const_iterator i = stl_container->begin();
         i != stl_container->end();
         ++i) {
      delete *i;
    }
    stl_container->clear();
  }
}

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_STL_DELETE_ELEMENTS_H_
