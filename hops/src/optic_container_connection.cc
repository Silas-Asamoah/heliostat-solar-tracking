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
#include "src/optic_container_connection.h"

#include <string>                       // for operator<<
#include <utility>

#include <glog/logging.h>               // for operator<<, basic_ostream, etc
#include "src/optic.h"  // for Optic, etc
#include "src/optic_container.h"
#include "src/stl_delete_elements.h"

namespace energy_rec {

// OpticContainerConnection
//   Creates an empty OpticContainerConnection with flags set to refresh
//   itself.
OpticContainerConnection::OpticContainerConnection(
    OpticContainer *sources, OpticContainer *destinations) {
  sources_ = sources;
  destinations_ = destinations;
  need_to_compute_visibilities_ = true;
  sources_locked_ = false;
  destinations_locked_ = false;
}

// ~OpticContainerConnection
//   OpticContainerConnection owns the contents of visibilities_, so we delete
//   them here.
OpticContainerConnection::~OpticContainerConnection() {
  STLDeleteElements(&visibilities_);
}

// Simulate
//   Determines which elements of visibilities_ need to be recalculated due to
//   changed Optics, then does the recomputation, and finally calls
//   InteractOptics to transmit light from sources_ to destinations_.
void OpticContainerConnection::Simulate(SimulationTime when) {
  CheckFreshness(when);
  UpdateStaleVisibilities();
  destinations_->ClearIncidentLight();
  InteractOptics();
}

// AddVisibility
//   Adds a Visibility structure to visibilities_, marks it as stale, and
//   updates source_dependencies_ and destination_dependencies_.
void OpticContainerConnection::AddVisibility(Visibility *visibility) {
  visibilities_.push_back(visibility);
  // Visibilities start off stale:
  stale_visibilities_.insert(visibility);
  // Update dependencies mentioned in visibility:
  for (vector<const Optic *>::const_iterator it =
       visibility->potential_blockers.begin();
       it != visibility->potential_blockers.end(); ++it) {
    source_dependencies_[*it].insert(visibility);
  }
  for (vector<const Optic *>::const_iterator it =
       visibility->potential_shadowers.begin();
       it != visibility->potential_shadowers.end(); ++it) {
    destination_dependencies_[*it].insert(visibility);
  }
}

// DetermineVisibilities
//   Clears and populates visibilities_ from sources_ and destinations_.
void OpticContainerConnection::DetermineVisibilities() {
  LOG(INFO) << "Determining visibilities between " << sources_->name()
            << " and " << destinations_->name() << ".";
  visibilities_.clear();
  Visibility *visibility;
  // Iterate over all source-destination pairs:
  for (vector<Optic *>::iterator src_it = sources_->members().begin();
       src_it != sources_->members().end(); ++src_it) {
    for (vector<Optic *>::iterator dest_it = destinations_->members().begin();
         dest_it != destinations_->members().end(); ++dest_it) {
      visibility = new Visibility;
      // Source data:
      visibility->source = *src_it;
      // We start with the direction_out reversed, to compute blockers:
      visibility->direction_out.CopyFrom((*src_it)->location());
      visibility->direction_out.Subtract((*dest_it)->location());
      sources_->GetPotentialMaskers(visibility->direction_out,
                                    *src_it,
                                    &(visibility->potential_blockers));
      // Now multiply it by -1 to get the actual direction out:
      visibility->direction_out.Invert();
      // source_mask starts out equal to the whole source surface:
      visibility->source_mask = (*src_it)->shape();
      visibility->stale_source = true;
      // Destination data:
      visibility->destination = *dest_it;
      // For now, direction_in is always equal to direction_out:
      visibility->direction_in.CopyFrom(visibility->direction_out);
      destinations_->GetPotentialMaskers(visibility->direction_in,
                                         *dest_it,
                                         &(visibility->potential_shadowers));
      // destination_mask starts out equal to the whole destination surface:
      visibility->destination_mask = (*dest_it)->shape();
      visibility->stale_destination = true;
      // Add this Visibility to visibilities_ and the dependency lists:
      AddVisibility(visibility);
    }
  }
  // Clear the recompute-everything flag:
  need_to_compute_visibilities_ = false;
}

// CheckFreshness
//   Scans sources_ and destinations_ for Optics that have moved or rotated
//   since a given time.
//   If an Optic has moved, it may shadow or block other Optics that didn't
//   have it in their potential_blockers or potential_shadowers lists before,
//   so we must recompute visibilities_ from scratch.
//   If an Optic has rotated, we merely need to recalculate each Visibility
//   struct that has that Optic as a source or destination.
void OpticContainerConnection::CheckFreshness(SimulationTime since) {
  // Check source freshness:
  if (!sources_locked_) {
    // Check for any sources that have been relocated:
    for (vector<Optic *>::const_iterator it = sources_->members().begin();
         it != sources_->members().end(); ++it) {
      if ((*it)->ChangedLocation(since)) {
        // Need to recompute everything:
        LOG(INFO) << "Recomputing visibilities due to moved source in "
                  << sources_->name();
        need_to_compute_visibilities_ = true;
        return;
      }
    }
    // Check for any sources that have been reoriented:
    for (VisibilityDependencies::iterator it = source_dependencies_.begin();
         it != source_dependencies_.end(); ++it) {
      if (it->first->ChangedOrientation(since)) {
        // Mark all the visibilities connected to source as stale at the source
        // end:
        SetSourceUpdateFlags(it->second);
      }
    }
  }
  // Check destination freshness:
  if (!destinations_locked_) {
    // Check for any destinations that have been relocated:
    for (vector<Optic *>::iterator it = destinations_->members().begin();
         it != destinations_->members().end(); ++it) {
      if ((*it)->ChangedLocation(since)) {
        // Need to recompute everything:
        LOG(INFO) << "Recomputing visibilities due to moved destination in "
                  << destinations_->name();
        need_to_compute_visibilities_ = true;
        return;
      }
    }
    // Check for any destinations that have been reoriented:
    for (VisibilityDependencies::iterator it =
         destination_dependencies_.begin();
         it != destination_dependencies_.end(); ++it) {
      if (it->first->ChangedOrientation(since)) {
        // Mark all the visibilities connected to destination as stale at the
        // destination end:
        SetDestinationUpdateFlags(it->second);
      }
    }
  }
}

// UpdateStaleVisibilities
//   If need_to_compute_visibilities_ is true, refreshes the list of Visibility
//   structs.
//   Next, updates all Visibility structs marked as stale.
void OpticContainerConnection::UpdateStaleVisibilities(void) {
  if (need_to_compute_visibilities_) {
    // Recompute visibilities_ from scratch:
    DetermineVisibilities();
  }
  for (set<Visibility *>::iterator it = stale_visibilities_.begin();
       it != stale_visibilities_.end(); ++it) {
    // Update this Visibility struct:
    ComputeVisibility(*it);
  }
  // Clear the list of Visibility structs that needed to be updated:
  stale_visibilities_.clear();
}

// InteractOptics
//   Transmits light from the source to the destination of each Visibility
//   struct.
void OpticContainerConnection::InteractOptics(void) {
  LOG(INFO) << "Interacting " << sources_->name()
            << " and " << destinations_->name() << ".";
  // Iterate over visibilities_:
  for (vector<Visibility *>::const_iterator it = visibilities_.begin();
       it != visibilities_.end(); ++it) {
    const Visibility *vis = *it;
    // Compute the irradiance from this struct's source at the location of its
    // destination:
    const IncidentLight incident_light = {
      vis->source->OutputIrradiance(*(vis->destination),
                                    vis->destination_mask,
                                    vis->source_mask),  // irradiance
      vis->direction_in,  // direction
      vis->destination_mask,  // incident_mask
      vis->source  // source
    };
    // Tell the destination Optic what's happening to it:
    vis->destination->ReceiveIncidentLight(incident_light);
  }
}

// ComputeVisibility
//   Updates a Visibility struct by calculating shadowing and blocking masks.
void OpticContainerConnection::ComputeVisibility(Visibility *visibility) {
  // We assume here that relative to the distance between source and
  // destination, potential_blockers are all close to source and
  // potential_shadowers are all close to destination.  This lets us treat
  // masking at the source and masking at the destination as independent of each
  // other.
  Polygons mask;
  if (visibility->stale_source) {
    // Compute the visible window from the source through the blockers:
    Polygons block;
    // Add up the shadows of all the potential blockers:
    for (vector<const Optic *>::const_iterator it =
         visibility->potential_blockers.begin();
         it != visibility->potential_blockers.end(); ++it) {
      DLOG_IF_EVERY_N(WARNING, !CloseTo(visibility->source->location(),
                                        (*it)->location(),
                                        visibility->destination->location()),
                      50)
          << "A blocker is more than 1/5 the distance to the destination; "
          << "blocking may be approximate.";
      (*it)->Shadow(visibility->direction_out, *(visibility->source), &mask);
      block.Union(mask);
    }
    // Subtract this mask from the outline of the source:
    visibility->source_mask = visibility->source->shape();
    visibility->source_mask.Subtract(block);
    // Clear the staleness flag:
    visibility->stale_source = false;
  }
  if (visibility->stale_destination) {
    // Compute the visible window to the destination through the shadowers:
    Polygons shadow;
    // Add up the shadows of all the potential shadowers:
    for (vector<const Optic *>::const_iterator it =
         visibility->potential_shadowers.begin();
         it != visibility->potential_shadowers.end(); ++it) {
      DLOG_IF_EVERY_N(WARNING, !CloseTo(visibility->destination->location(),
                                        (*it)->location(),
                                        visibility->source->location()), 50)
          << "A shadower is more than 1/5 the distance from the source; "
          << "shadowing may be approximate.";
      (*it)->Shadow(
          visibility->direction_in, *(visibility->destination), &mask);
      shadow.Union(mask);
    }
    // Subtract this mask from the outline of the destination:
    visibility->destination_mask = visibility->destination->shape();
    visibility->destination_mask.Subtract(shadow);
    // Clear the staleness flag:
    visibility->stale_destination = false;
  }
}

// SetSourceUpdateFlags
//   Sets the stale_source flag for each of a set of Visibility structs and
//   adds each struct to the stale_visibilities_ list.
void OpticContainerConnection::SetSourceUpdateFlags(
    const set<Visibility *> &visibilities) {
  for (set<Visibility *>::iterator it = visibilities.begin();
       it != visibilities.end(); ++it) {
    (*it)->stale_source = true;
    stale_visibilities_.insert(*it);
  }
}

// SetDestinationUpdateFlags
//   Sets the stale_destination flag for each of a set of Visibility structs
//   and adds each struct to the stale_visibilities_ list.
void OpticContainerConnection::SetDestinationUpdateFlags(
    const set<Visibility *> &visibilities) {
  for (set<Visibility *>::iterator it = visibilities.begin();
       it != visibilities.end(); ++it) {
    (*it)->stale_destination = true;
    stale_visibilities_.insert(*it);
  }
}

// CloseTo
//   A convenience static method for the DCHECKs in ComputeVisibility.  Returns
//   true iff the distance from subject to object is at most 1/5 the distance
//   from subject to comparison.
bool OpticContainerConnection::CloseTo(const Coordinates3D &subject,
                                       const Coordinates3D &object,
                                       const Coordinates3D &comparison) {
  const Coordinates3D to_object(subject, object);
  const Coordinates3D to_comparison(subject, comparison);
  return to_object.EuclideanNorm() / to_comparison.EuclideanNorm() <= 0.2;
}

}  // namespace energy_rec
