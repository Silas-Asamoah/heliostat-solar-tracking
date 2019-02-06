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

// OpticContainerConnection is a class that handles the physical relationship
// between two OpticContainer objects, actually performing the simulation of
// light transmission form one to the other.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_OPTIC_CONTAINER_CONNECTION_H_
#define ENERGY_REC_OPTICAL_SIMULATION_OPTIC_CONTAINER_CONNECTION_H_

using namespace std;
#include <map>                          // for map, map<>::value_compare
#include <set>                          // for set
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationTime
#include "math/coordinates3d.h"  // for Coordinates3D
#include "math/polygons.h"   // for Polygons

namespace energy_rec {

class Optic;
class OpticContainer;

// A structure to describe a link between Optics that can see each other:
struct Visibility {
  // outgoing from source:
  Optic *source;
  Coordinates3D direction_out;
  vector<const Optic *> potential_blockers;
  Polygons source_mask;  // projected onto source
  bool stale_source;  // is this block of data stale?
  // incoming at destination:
  Optic *destination;
  Coordinates3D direction_in;
  vector<const Optic *> potential_shadowers;
  Polygons destination_mask;  // projected onto destination
  bool stale_destination;  // is this block of data stale?
};

// This container maps an Optic to every Visibility that includes it:
// TODO(tpw):  Try making this a hash_map instead; test performance.
typedef map<const Optic *, set<Visibility *> > VisibilityDependencies;

class OpticContainerConnection {
 public:
  OpticContainerConnection(OpticContainer *sources,
                           OpticContainer *destinations);
  virtual ~OpticContainerConnection();

  // The main method; it transmits light from one OpticContainer to the other:
  void Simulate(SimulationTime when);

 private:
  void AddVisibility(Visibility *visibility);
  void DetermineVisibilities();
  void CheckFreshness(SimulationTime when);
  void UpdateStaleVisibilities(void);
  void InteractOptics(void);
  void ComputeVisibility(Visibility *visibility);
  void SetSourceUpdateFlags(const set<Visibility *> &visibilities);
  void SetDestinationUpdateFlags(const set<Visibility *> &visibilities);
  static bool CloseTo(const Coordinates3D &subject, const Coordinates3D &object,
                      const Coordinates3D &comparison);

  // The two groups that are connected by this object:
  OpticContainer *sources_;
  OpticContainer *destinations_;

  // The list of every link between an Optic in one group and an Optic in the
  // other:
  vector<Visibility *> visibilities_;

  // Maps from Optics to the Visibilities that depend on them:
  VisibilityDependencies source_dependencies_;
  VisibilityDependencies destination_dependencies_;

  bool need_to_compute_visibilities_;  // true if we need to refresh everything
  // TODO(tpw):  Eliminate use of set here and just iterate over visibilities_:
  set<Visibility *> stale_visibilities_;  // list of visibilities to be updated
  bool sources_locked_;  // skip checking source freshness
  bool destinations_locked_;  // skip checking destination freshness

  DISALLOW_COPY_AND_ASSIGN(OpticContainerConnection);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_OPTIC_CONTAINER_CONNECTION_H_
