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

#include "config.h"
using namespace std;
#include "math/coordinates3d.h"

#include <cstddef>                       // for NULL
#include <cmath>                         // for cos, sin, M_PI, acos, sqrt, etc

#include "math/quaternion.h"  // for Quaternion

namespace energy_rec {


///////////////////////////////////
// Overridden arithmetic methods //
///////////////////////////////////

FloatType Coordinates3D::EuclideanNorm(void) const {
  if (Has(kSpherical)) {
    return Get(kSpherical, 0);
  }
  // It's not worth converting to spherical if we don't have it, because
  // this is expensive
  return SelfType::EuclideanNorm();
}


//////////////////////////////
// Extra arithmetic methods //
//////////////////////////////

// CrossProduct
//   Cross-multiplies this by b.
void Coordinates3D::CrossProduct(const Coordinates3D &b) {
  const FloatType *a_value = Get(kCartesian);
  const FloatType *b_value = b.Get(kCartesian);
  const FloatType new_value[3] = {
    a_value[1] * b_value[2] - a_value[2] * b_value[1],
    a_value[2] * b_value[0] - a_value[0] * b_value[2],
    a_value[0] * b_value[1] - a_value[1] * b_value[0]
  };
  Set(kCartesian, new_value);
}

// RodriguesRotation
//   Rotates this about axis_angle by an amount equal to the Euclidean norm of
//   axis_angle, using Rodrigues' formula, where r is the unit vector in the
//   direction of axis:
//     this' = this * cos(theta) +
//             (r x this) * sin(theta) +
//             r * (r . this) * (1 - cos(theta))
//   The handedness of the rotation matches that of the coordinate system.
void Coordinates3D::RodriguesRotation(const Coordinates3D &axis_angle) {
  RodriguesRotation(axis_angle.EuclideanNorm(), axis_angle);
}

// RodriguesRotation
//   Rotates this by an angle theta about axis using Rodrigues' formula.
void Coordinates3D::RodriguesRotation(FloatType theta,
                                      const Coordinates3D &axis) {
  const FloatType magnitude = axis.EuclideanNorm();
  if (theta == 0.0 || magnitude == 0.0) {
    // No rotation:
    return;
  }
  // Normalize axis to create the unit vector r:
  Coordinates3D r(axis);
  r.Multiply(1.0 / magnitude);

  // Compute the vector products used in Rodrigues' formula:
  Coordinates3D cross_product(r);
  cross_product.CrossProduct(*this);
  const FloatType dot_product = Coordinates3D::DotProduct(r, *this);

  // First and second terms:
  LinearCombination(cos(theta), sin(theta), cross_product);

  // Third term:
  LinearCombination(1.0, dot_product * (1 - cos(theta)), r);
}

// ComposeRotation
//   Composes this (treated as a rotation vector) with another (rotation)
//   vector, storing the resulting rotation vector in this.
void Coordinates3D::ComposeRotation(const Coordinates3D &other) {
  // Convert both rotations to quaternions:
  Quaternion p;
  RotationToQuaternion(&p);
  Quaternion q;
  other.RotationToQuaternion(&q);
  // Compose the rotations (p first, then q):
  q.QuaternionMultiply(p);
  // Convert back to a rotation vector:
  QuaternionToRotation(q);
}

// EulerZXZToRotation
//   Converts Euler angles in the ZXZ convention to a rotation vector, storing
//   the result in this.
void Coordinates3D::EulerZXZToRotation(FloatType alpha,
                                       FloatType beta,
                                       FloatType gamma) {
  // ZXZ rotation means:
  //   1) Rotate by alpha about the Z-axis.
  //   2) Roatte by beta about the intrinsic X-axis.
  //   3) Rotate by gamma about the intrinsic Z-axis.
  // This is embodied in the following code:
  //   // Rotate by alpha about the Z-axis:
  //   CopyFrom(k3DZAxis);
  //   Multiply(alpha);
  //   // Rotate by beta about the intrinsic X-axis:
  //   Coordinates3D x_axis(k3DXAxis);
  //   x_axis.RodriguesRotation(*this);
  //   x_axis.Multiply(beta);
  //   ComposeRotation(x_axis);
  //   // Rotate by gamma about the intrinsic Z-axis:
  //   Coordinates3D z_axis(k3DZAxis);
  //   z_axis.RodriguesRotation(*this);
  //   z_axis.Multiply(gamma);
  //   ComposeRotation(z_axis);
  // However, this is equivalent to the slightly simpler prescription:
  //   1) Rotate by gamma about the Z-axis.
  //   2) Roatte by beta about the extrinsic X-axis.
  //   3) Rotate by alpha about the extrinsic Z-axis.
  // Thus, we do the following:
  // Rotate by gamma about the Z-axis:
  CopyFrom(k3DZAxis);
  Multiply(gamma);
  // Rotate by beta about the extrinsic X-axis:
  Coordinates3D x_axis(k3DXAxis);
  x_axis.Multiply(beta);
  ComposeRotation(x_axis);
  // Rotate by alpha about the extrinsic Z-axis:
  Coordinates3D z_axis(k3DZAxis);
  z_axis.Multiply(alpha);
  ComposeRotation(z_axis);
}

// RotationToQuaternion
//   Converts this (interpreted as a rotation vector) into a quaternion rotation
//   representation
void Coordinates3D::RotationToQuaternion(Quaternion *quaternion) const {
  const FloatType theta = EuclideanNorm();
  if (theta > 0.0) {
    Coordinates3D axis(*this);
    axis.Normalize();
    const FloatType sine = sin(theta / 2.0);
    const FloatType cosine = cos(theta / 2.0);
    const FloatType result[4] = {
      cosine,
      axis.Get(kCartesian, 0) * sine,
      axis.Get(kCartesian, 1) * sine,
      axis.Get(kCartesian, 2) * sine
    };
    quaternion->Set(kCartesian, result);
  } else {
    quaternion->Multiply(0.0);
    quaternion->Set(kCartesian, 0, 1.0);
  }
}

// QuaternionToRotation
//   Converts a quaternion rotation representation into a rotation vector,
//   storing the result in this.
void Coordinates3D::QuaternionToRotation(const Quaternion &quaternion) {
  const FloatType theta = quaternion.Get(kCartesian, 0);
  if (fabs(theta) <= 1.0) {
    Set(kCartesian, 0, quaternion.Get(kCartesian, 1));
    Set(kCartesian, 1, quaternion.Get(kCartesian, 2));
    Set(kCartesian, 2, quaternion.Get(kCartesian, 3));
    if (EuclideanNorm() > 0.0) {
      Normalize();
    }
    Multiply(2.0 * acos(quaternion.Get(kCartesian, 0)));
  } else {
    Multiply(0.0);
  }
}


//////////////////////////
// Conversion functions //
//////////////////////////

// Cartesian, Cylindrical, Spherical:

void CartesianToCylindrical(const FloatType x[3], FloatType y[3]) {
  y[0] = sqrt(x[0] * x[0] + x[1] * x[1]);
  if (y[0] > 0.0) {
    y[1] = atan2(x[1], x[0]);
  } else {
    y[1] = 0.0;
  }
  y[2] = x[2];
}

void CartesianToSpherical(const FloatType x[3], FloatType y[3]) {
  y[0] = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
  if (y[0] > 0.0) {
    y[1] = acos(x[2] / y[0]);
  } else {
    y[1] = 0.0;
  }
  if (x[0] != 0.0 || x[1] != 0.0) {
    y[2] = atan2(x[1], x[0]);
  } else {
    y[2] = 0.0;
  }
}

void CylindricalToCartesian(const FloatType x[3], FloatType y[3]) {
  y[0] = x[0] * cos(x[1]);
  y[1] = x[0] * sin(x[1]);
  y[2] = x[2];
}

void CylindricalToSpherical(const FloatType x[3], FloatType y[3]) {
  y[0] = sqrt(x[0] * x[0] + x[2] * x[2]);
  if (y[0] > 0.0) {
    y[1] = acos(x[2] / y[0]);
  } else {
    y[1] = 0.0;
  }
  y[2] = x[1];
}

void SphericalToCartesian(const FloatType x[3], FloatType y[3]) {
  y[0] = x[0] * sin(x[1]) * cos(x[2]);
  y[1] = x[0] * sin(x[1]) * sin(x[2]);
  y[2] = x[0] * cos(x[1]);
}

void SphericalToCylindrical(const FloatType x[3], FloatType y[3]) {
  y[0] = x[0] * sin(x[1]);
  y[1] = x[2];
  y[2] = x[0] * cos(x[1]);
}

// Astronomical (closely tied to Spherical):

void SphericalToAstronomical(const FloatType x[3], FloatType y[3]) {
  y[0] = x[0];
  y[1] = 0.0;
  y[2] = 0.0;
  if (x[0]) {
    if (x[1] > 0.0 && x[1] < M_PI) {
      y[1] = 90.0 - x[2] * 180.0 / M_PI;
    }
    y[2] = 90.0 - x[1] * 180.0 / M_PI;
  }
  if (y[1] < 0.0) {
    y[1] += 360.0;
  }
}

void CartesianToAstronomical(const FloatType x[3], FloatType y[3]) {
  FloatType temp[3];
  CartesianToSpherical(x, temp);
  SphericalToAstronomical(temp, y);
}

void CylindricalToAstronomical(const FloatType x[3], FloatType y[3]) {
  FloatType temp[3];
  CylindricalToSpherical(x, temp);
  SphericalToAstronomical(temp, y);
}

void AstronomicalToSpherical(const FloatType x[3], FloatType y[3]) {
  y[0] = x[0];
  y[1] = 0.0;
  y[2] = 0.0;
  if (x[0]) {
    y[1] = M_PI / 2.0 - x[2] * M_PI / 180.0;
    if (fabs(x[2]) < 90.0) {
      y[2] = M_PI / 2.0 - x[1] * M_PI / 180.0;
    }
  }
  if (y[2] <= -M_PI) {
    y[2] += 2 * M_PI;
  }
}

void AstronomicalToCartesian(const FloatType x[3], FloatType y[3]) {
  FloatType temp[3];
  AstronomicalToSpherical(x, temp);
  SphericalToCartesian(temp, y);
}

void AstronomicalToCylindrical(const FloatType x[3], FloatType y[3]) {
  FloatType temp[3];
  AstronomicalToSpherical(x, temp);
  SphericalToCylindrical(temp, y);
}


// Table of conversion functions:
const Coordinates3D::ConvertFunction
    Coordinates3D::k3DConversions[kNumberOf3DSystems][kNumberOf3DSystems] = {
  {NULL, &CartesianToCylindrical,
    &CartesianToSpherical, &CartesianToAstronomical},
  {&CylindricalToCartesian, NULL,
    &CylindricalToSpherical, &CylindricalToAstronomical},
  {&SphericalToCartesian, &SphericalToCylindrical,
    NULL, &SphericalToAstronomical},
  {&AstronomicalToCartesian, &AstronomicalToCylindrical,
    &AstronomicalToSpherical, NULL}
};

}  // namespace energy_rec
