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

// Heliostat is an Optic-derived class that represents a heliostat, including
// reflection and automatic actuation to track the sun.

// Heliostat mirrors are modeled as flat or spherically curved with a specified
// focal length.  (As long as the heliostat size is smaller than the focal
// length, it doesn't matter much whether we think of them as spherical or
// parabolic.)  Images are produced by discrete convolution, the resolution
// of which is controlled by the flag --heliostat_facets_per_side.

#ifndef ENERGY_REC_OPTICAL_SIMULATION_HELIOSTAT_H_
#define ENERGY_REC_OPTICAL_SIMULATION_HELIOSTAT_H_

using namespace std;
#include <cstddef>                      // for NULL
#include <map>                          // for map
#include <utility>                      // for pair
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationTime, etc
#include "src/optic.h"
#include "src/stl_delete_elements.h"
#include "math/coordinates.h"  // for FloatType
#include "math/coordinates2d.h"  // for Coordinates2D
#include "math/coordinates3d.h"  // for Coordinates3D, k3DZAxis
#include "math/polygons.h"   // for Polygon, Polygons

namespace energy_rec {

// HeliostatFacet is a structure for storing information about the heliostat's
// mirror curvature.  Reflection is calculated by discrete convolution; ie,
// mirrors are modeled as lattices of infinitesimal ideal reflectors.
struct HeliostatFacet {
  Polygons shape;  // The shape of the facet, in the same
                   // coordinates as the Heliostat's shape
  Coordinates2D local_centroid;  // centroid in mirror coordinates
  Coordinates3D global_centroid;  // centroid in global coordinates
  Coordinates3D normal;  // The normal vector of the reflector
};

class Heliostat : public Optic {
 public:
  // Constructor for unspecified normal; heliostat starts in the horizontal flat
  // stow position:
  Heliostat(const Coordinates3D &location, const Coordinates2D &projection,
            const Coordinates3D &frame_orientation,
            FloatType reflectivity, FloatType focal_length, int actuation,
            const Polygons &mirror_shape)
      : Optic(location, k3DZAxis, projection, mirror_shape),
        frame_orientation_(frame_orientation),
        reflectivity_(reflectivity),
        focal_length_(focal_length),
        actuation_(actuation),
        tracking_(NULL) {
    InitializeFacets();
  }
  // Constructor for a given normal:
  Heliostat(const Coordinates3D &location, const Coordinates3D &normal,
            const Coordinates2D &projection,
            const Coordinates3D &frame_orientation,
            FloatType reflectivity, FloatType focal_length, int actuation,
            const Polygons &mirror_shape)
      : Optic(location, normal, projection, mirror_shape),
        frame_orientation_(frame_orientation),
        reflectivity_(reflectivity),
        focal_length_(focal_length),
        actuation_(actuation),
        tracking_(NULL) {
    // Set the mirror orientation using CalculateAxisAngle:
    Coordinates3D axis_angle;
    CalculateAxisAngle(normal, &axis_angle);
    SetAxisAngle(axis_angle, kInitialTimeStamp);
    InitializeFacets();
  }
  // Destructor:
  virtual ~Heliostat() {
    // Heliostat owns its facets:
    STLDeleteElements(&facets_);
  }

  // Control over heliostat tracking and aim:
  void StartTracking(const Optic *optic);
  void StopTracking(void);
  void SetAimPoint(const Coordinates3D &aim_point);

  // Overridden Optic methods:
  // TODO(tpw):  Override SetNormal to get actual orientation
  //             based on the physical actuation mechanism.
  virtual void UpdateSelf(SimulationTime when);
  virtual void ClearIncidentLight(void);
  virtual FloatType TransferIrradiance(
      const Optic &target,
      const Polygons &my_mask,
      const Polygons &destination_mask,
      const IncidentLight &incident_light) const;
  virtual void GetOutput(SimulationTime sim_time,
                         SimulationOutput *output) const;

 protected:
  // Heliostat actuation:
  void CalculateAxisAngle(const Coordinates3D &normal,
                          Coordinates3D *axis_angle) const;
  // Heliostat reflection via facets:
  void InitializeFacets(void);
  static void PopulateLattice(const Polygon &quadrilateral,
                              int n,
                              map<pair<int, int>, Coordinates2D> *lattice);
  void PopulateFacets(int n,
                      const map<pair<int, int>, Coordinates2D> &lattice);
  void UpdateFacets(void);
  FloatType FacetIrradiance(const HeliostatFacet &facet,
                            const Optic &target,
                            const Polygons &my_mask,
                            const Polygons &destination_mask,
                            const IncidentLight &incident_light) const;
  static void ReflectSourceShapeInPoint(const Optic &source,
                                        const Coordinates3D &point_location,
                                        const Coordinates3D &point_normal,
                                        const Optic &target,
                                        Polygons *reflected_shape);

 private:
  const Coordinates3D frame_orientation_;  // Rotation from level, south-facing
  const FloatType reflectivity_;
  const FloatType focal_length_;
  const int actuation_;
  // The direction from this object to the aim point:
  Coordinates3D aim_direction_;  // Note:  This is not changed by SetNormal()
  // The Optic we're tracking, or NULL for none:
  const Optic *tracking_;  // pointer-to-const
  // A decomposition of the heliostats into facets, for performing the discrete
  // convolution to make correct reflected images:
  vector<HeliostatFacet *> facets_;
  // The amount of flux from this Heliostat striking a target:
  mutable FloatType flux_on_target_;

  DISALLOW_COPY_AND_ASSIGN(Heliostat);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_HELIOSTAT_H_
