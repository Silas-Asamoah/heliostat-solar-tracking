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

// Optic is a class that represents generic optical elements (the sun, mirrors,
// apertures, etc.).

#ifndef ENERGY_REC_OPTICAL_SIMULATION_OPTIC_H_
#define ENERGY_REC_OPTICAL_SIMULATION_OPTIC_H_

using namespace std;
#include <vector>                       // for vector

#include "src/common.h"  // for SimulationTime, etc
#include "math/coordinates.h"  // for FloatType
#include "math/coordinates2d.h"  // for Coordinates2D
#include "math/coordinates3d.h"  // for Coordinates3D
#include "math/polygons.h"   // for Polygons

namespace energy_rec {

class Optic;

// IncidentLight is a structure that represents a quantum of light traveling
// from one Optic to another:
// TODO(tpw):  Make this into a class with at least the following method:
//   FluxThroughMask() { return irradiance_ * incident_mask_.Area(); }
struct IncidentLight {
  FloatType irradiance;  // not including shadowing
  Coordinates3D direction;  // the direction of the incoming light
  Polygons incident_mask;  // so the receiving Optic knows which part of it is
                           // illuminated; this is important for heliostats
                           // when blocking and shadowing overlap
  const Optic *source;  // used by Heliostat to form reflection shapes
};

class Optic {
 public:
  Optic(const Coordinates3D &location, const Coordinates3D &normal,
        const Coordinates2D &projection, const Polygons &shape)
      : projection_(projection),
        shape_(shape),
        radius_(shape.RadiusFromOrigin()) {
    // Initialize location_ and location_timestamp_:
    SetLocation(location, kInitialTimeStamp);
    // Initialize axis_angle_ and orientation_timestamp_:
    SetNormal(normal, kInitialTimeStamp);
  }
  Optic(const vector<Coordinates3D> &points,
        const vector<Coordinates2D> &projections);
  virtual ~Optic() {}

  // Accessors for coordinate data:
  const Coordinates3D& location(void) const { return location_; }
  const Coordinates3D& normal(void) const { return normal_; }
  const Coordinates3D& axis_angle(void) const { return axis_angle_; }
  const Coordinates2D& projection(void) const { return projection_; }
  const Polygons& shape(void) const { return shape_; }
  FloatType radius(void) const { return radius_; }

  // A method to project an Optic's shadow on another Optic's plane:
  void Shadow(const Coordinates3D &direction, const Optic &target,
              Polygons *shadow) const;

  // Setters:
  void SetLocation(const Coordinates3D &location, SimulationTime when);
  void SetAxisAngle(const Coordinates3D &axis_angle, SimulationTime when);
  void SetNormal(const Coordinates3D &normal, SimulationTime when);

  // Freshness-testing methods:
  bool ChangedLocation(SimulationTime since) const;
  bool ChangedOrientation(SimulationTime since) const;

  // This method should be called before each simulation step, in order to allow
  // Optics to update themselves (for example, for the sun to move in the sky or
  // for a heliostat to track the sun).
  virtual void UpdateSelf(SimulationTime when) {}

  // Methods to handle receiving and emitting light:
  virtual void ClearIncidentLight(void);
  virtual void ReceiveIncidentLight(const IncidentLight &incident_light);
  // Only sources (like the sun) should override this:
  virtual FloatType OutputIrradiance(const Optic &target,
                                     const Polygons &destination_mask,
                                     const Polygons &source_mask) const;
  // Heliostats and other reflective/emissive elements should override this:
  virtual FloatType TransferIrradiance(
      const Optic &target,
      const Polygons &my_mask,
      const Polygons &destination_mask,
      const IncidentLight &incident_light) const { return 0.0; }

  // Optical statistics:
  virtual bool IsATarget(void) const { return false; }
  FloatType InputFlux(void) const { return flux_in_; }
  FloatType InputIrradiance(void) const { return InputFlux() / shape().Area(); }
  FloatType ShadowedIrradiance(void) const;
  virtual void GetOutput(SimulationTime sim_time,
                         SimulationOutput *output) const {}

  // Coordinate conversions between coordinates in space and coordinates within
  // the plane of this Optic:
  void LocalToGlobal(const Coordinates2D &local, Coordinates3D *global) const;
  void GlobalToLocal(const Coordinates3D &global, Coordinates2D *local) const;

  // A method for filtering Optics which may mask each other:
  bool MayBeMaskedBy(const Coordinates3D &direction_in,
                     const Optic *potential_masker) const;

 private:
  Coordinates3D location_;  // Where the element is in the physical world
  Coordinates3D axis_angle_;  // How to rotate from (0, 0, 1) to my normal
  Coordinates3D normal_;  // (0, 0, 1) rotated by axis_angle_;
  Coordinates2D projection_;  // Where the element should appear in map plots
  Polygons shape_;  // fixed; orientation changes are just coordinate changes
  FloatType radius_;  // maximum radius of shape_ from the 2D origin
  SimulationTime location_timestamp_;  // the last time I changed location
  SimulationTime orientation_timestamp_;  // the last time I changed orientation
  vector<IncidentLight> incident_light_;  // light received by this Optic
  FloatType flux_in_;  // total flux impinging on this Optic

  DISALLOW_COPY_AND_ASSIGN(Optic);
};

}  // namespace energy_rec

#endif  // ENERGY_REC_OPTICAL_SIMULATION_OPTIC_H_
