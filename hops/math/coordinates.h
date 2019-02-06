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

// This is a dimension-agnostic virtual class for storing coordinate tuples and
// changing coordinate systems.  Derive from it to create a class with a given
// dimension, such as in coordinates2d.h and coordinates3d.h.

// Example usage (with Coordinates3D); see the unittests for more:
//   // Instantiate a coordinate tuple set to the origin:
//   Coordinates3D my_coords;
//   // Store the Cartesian tuple (1, 4, -2) in my_coords:
//   my_coords.Set(kCartesian, 0, 1.0);
//   my_coords.Set(kCartesian, 1, 4.0);
//   my_coords.Set(kCartesian, 2, -2.0);
//   // Convert to spherical coordinates and get theta:
//   double theta = my_coords.Get(kSpherical, 1);
//   // Convert to cylindrical coordinates and change rho:
//   my_coords.Set(kCylindrical, 0, 3.0);
//   // Convert to Cartesian coordinates and get z:
//   double new_z = my_coords.Get(kCartesian, 2);

#ifndef ENERGY_REC_UTIL_COORDINATES_H_
#define ENERGY_REC_UTIL_COORDINATES_H_

using namespace std;
#include <cstddef>                      // for size_t
#include <cstring>                      // for memset, memcpy
#include <cmath>                        // for fabs, pow, acos, sqrt

#include <glog/logging.h>               // for LOG

namespace energy_rec {

// A type to specify a coordinate system:
typedef int CoordinateSystem;

// Cartesian coordinates are defined for any number of dimensions:
const CoordinateSystem kCartesian = 0;

// The standard floating-point type for this class:
typedef double FloatType;

// Error margin for calculations:
const FloatType kFloatTypeError = 1.0e-14;

// The virtual coordinate class, which is dimension-agnostic:
template<size_t systems, size_t dimensions>
class Coordinates {
 public:
  // Coordinate conversion function type:
  typedef void(*ConvertFunction)(
      const FloatType[dimensions], FloatType[dimensions]);

  // Convenience typedef:
  typedef Coordinates<systems, dimensions> SelfType;

  Coordinates();  // Null constructor
  Coordinates(const CoordinateSystem system, const FloatType value[dimensions]);

  // Construct the vector terminal_point - initial_point
  Coordinates(const SelfType &initial_point, const SelfType &terminal_point);
  explicit Coordinates(const SelfType &other);  // Copy constructor
  virtual ~Coordinates() {}

  // Copy operator:
  void CopyFrom(const SelfType &source);

  // Getters:
  void Get(CoordinateSystem system, FloatType destination[dimensions]) const;
  const FloatType * Get(CoordinateSystem system) const;
  FloatType Get(CoordinateSystem system, int index) const;

  // Setters:
  void Set(CoordinateSystem system, const FloatType value[dimensions]);
  void Set(CoordinateSystem system, int index, FloatType value);
  bool Has(CoordinateSystem system) const;

  // Arithmetic:
  virtual void Multiply(FloatType alpha);
  virtual void Invert(void) { Multiply(-1.0); }
  virtual void Normalize(FloatType norm);
  virtual void Normalize(void);
  virtual void LinearCombination(FloatType alpha,
                                 FloatType beta, const SelfType &b);
  virtual void Add(const SelfType &a);
  virtual void Subtract(const SelfType &a);
  virtual void Reflect(const SelfType &normal);
  virtual FloatType PNorm(FloatType p) const;
  virtual FloatType EuclideanNormSquared(void) const;
  virtual FloatType EuclideanNorm(void) const;
  static FloatType DotProduct(const SelfType &a, const SelfType &b);
  static FloatType Cosine(const SelfType &a, const SelfType &b);
  static FloatType Angle(const SelfType &a, const SelfType &b);
  static FloatType LineHyperplaneIntersection(
      const SelfType &line_point, const SelfType &line_direction,
      const SelfType &plane_point, const SelfType &plane_normal);

 protected:
  void AddSystem(CoordinateSystem system) const;
  void AddSystemIfNeeded(CoordinateSystem system) const;
  void CheckValidSystem(CoordinateSystem system) const;
  virtual void Convert(
      CoordinateSystem from_system, const FloatType from[dimensions],
      CoordinateSystem to_system, FloatType to[dimensions]) const = 0;
  int NumberOfSystems() const { return systems; }

 private:
  // Storage for coordinate tuples in each known coordinate system:
  // (mutable to allow coordinate conversion invisible to the user)
  mutable FloatType x_[systems][dimensions];
  mutable bool has_[systems];
};


template<size_t systems, size_t dimensions>
Coordinates<systems, dimensions>::Coordinates() {
  memset(x_, 0, sizeof(x_));
  memset(has_, false, sizeof(has_));
  has_[kCartesian] = true;
}

template<size_t systems, size_t dimensions>
Coordinates<systems, dimensions>::Coordinates(
    const SelfType &initial_point, const SelfType &terminal_point) {
  // We'll need the cartesian system to do the subtraction.
  // We can't do a conversion on the Coordinates being constructed, because
  // that would require to call a pure virtual function.
  // Therefore we do it on the terminal point, before copying
  terminal_point.AddSystemIfNeeded(kCartesian);
  CopyFrom(terminal_point);
  DCHECK(Has(kCartesian));
  Subtract(initial_point);
}

template<size_t systems, size_t dimensions>
Coordinates<systems, dimensions>::Coordinates(
    const CoordinateSystem system, const FloatType value[dimensions]) {
  memcpy(x_[system], value, sizeof(x_[system]));
  memset(has_, false, sizeof(has_));
  has_[system] = true;
}

template<size_t systems, size_t dimensions>
Coordinates<systems, dimensions>::Coordinates(const SelfType &other) {
  // Copy constructor
  CopyFrom(other);
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::CopyFrom(const SelfType &source) {
  memcpy(x_, source.x_, sizeof(x_));
  memcpy(has_, source.has_, sizeof(has_));
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Get(
    CoordinateSystem system, FloatType destination[dimensions]) const {
  CheckValidSystem(system);
  AddSystemIfNeeded(system);
  memcpy(destination, x_[system], sizeof(x_[system]));
}

template<size_t systems, size_t dimensions>
const FloatType * Coordinates<systems, dimensions>::Get(
    CoordinateSystem system) const {
  CheckValidSystem(system);
  AddSystemIfNeeded(system);
  return x_[system];
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::Get(CoordinateSystem system,
                                                int index) const {
  CheckValidSystem(system);
  AddSystemIfNeeded(system);
  return x_[system][index];
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Set(CoordinateSystem system,
                                           const FloatType value[dimensions]) {
  CheckValidSystem(system);
  // Discard the other systems and set the coordinate in this system:
  memcpy(x_[system], value, sizeof(x_[system]));
  memset(has_, false, sizeof(has_));
  has_[system] = true;
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Set(CoordinateSystem system,
                                           int index, FloatType value) {
  CheckValidSystem(system);
  AddSystemIfNeeded(system);
  // Change the coordinate in this system and discard the other systems:
  x_[system][index] = value;
  memset(has_, false, sizeof(has_));
  has_[system] = true;
}

template<size_t systems, size_t dimensions>
bool Coordinates<systems, dimensions>::Has(CoordinateSystem system) const {
  // Returns true iff we currently know our coordinates in the specified system
  return has_[system];
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Multiply(FloatType alpha) {
  // Multiplies this by the scalar alpha
  AddSystemIfNeeded(kCartesian);
  for (int i = 0; i < dimensions; ++i) {
    x_[kCartesian][i] *= alpha;
  }
  memset(has_, false, sizeof(has_));
  has_[kCartesian] = true;
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Normalize(FloatType norm) {
  DCHECK_GT(EuclideanNorm(), 0.0) << "Vector has 0 norm";
  Multiply(norm / EuclideanNorm());
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Normalize(void) {
  Normalize(1.0);
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::LinearCombination(FloatType alpha,
                                                         FloatType beta,
                                                         const SelfType &b) {
  // Multiplies self by alpha, then adds beta times b
  Multiply(alpha);
  AddSystemIfNeeded(kCartesian);
  b.AddSystemIfNeeded(kCartesian);
  for (int i = 0; i < dimensions; ++i) {
    x_[kCartesian][i] += beta * b.x_[kCartesian][i];
  }
  memset(has_, false, sizeof(has_));
  has_[kCartesian] = true;
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Add(const SelfType &a) {
  LinearCombination(1.0, 1.0, a);
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Subtract(const SelfType &a) {
  LinearCombination(1.0, -1.0, a);
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::Reflect(const SelfType &normal) {
  // Reflects this, interpreted as a vector, in the vector hyperplane with the
  // given normal.
  DCHECK_GT(normal.EuclideanNormSquared(), 0.0) << "normal must be nonzero";
  LinearCombination(
      1.0,
      -2.0 * DotProduct(*this, normal) / normal.EuclideanNormSquared(), normal);
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::PNorm(FloatType p) const {
  // Converts this to Cartesian coordinates and returns its p-norm
  DCHECK_GT(p, 0.0) << "p must be greater than zero";
  AddSystemIfNeeded(kCartesian);
  FloatType result = 0.0;
  for (int i = 0; i < dimensions; ++i) {
    result += pow(fabs(x_[kCartesian][i]), p);
  }
  return pow(result, 1 / p);
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::EuclideanNormSquared(void) const {
  // Returns the square of this's Euclidean norm
  return DotProduct(*this, *this);
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::EuclideanNorm(void) const {
  // Returns this's Euclidean norm; equivalent to calling PNorm(2.0)
  return sqrt(EuclideanNormSquared());
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::DotProduct(const SelfType &a,
                                                       const SelfType &b) {
  // Gets a and b in Cartesian coordinates and returns their dot product
  a.AddSystemIfNeeded(kCartesian);
  b.AddSystemIfNeeded(kCartesian);
  FloatType result = 0.0;
  for (int i = 0; i < dimensions; ++i) {
    result += a.x_[kCartesian][i] * b.x_[kCartesian][i];
  }
  return result;
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::Cosine(const SelfType &a,
                                                   const SelfType &b) {
  // Uses DotProduct and EuclideanNorm to determine the cosine of the angle
  // between a and b
  return DotProduct(a, b) / a.EuclideanNorm() / b.EuclideanNorm();
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::Angle(const SelfType &a,
                                                  const SelfType &b) {
  // Uses Cosine to determine the angle between a and b
  return acos(Cosine(a, b));
}

template<size_t systems, size_t dimensions>
FloatType Coordinates<systems, dimensions>::LineHyperplaneIntersection(
    const SelfType &line_point, const SelfType &line_direction,
    const SelfType &plane_point, const SelfType &plane_normal) {
  // Given a point LP on a line and a direction vector LD, and a point PP on a
  // hyperplane and a normal vector PN, returns the value t such that LP + t LD
  // is on the hyperplane.
  // If the line is parallel to the hyperplane, returns NaN if the line lies
  // within the hyperplane and an infinity if it doesn't.
  // Details:
  //   The equation of the line is X = LP + t LD, and the equation of the
  //   hyperplane is PN . (X - PP) = 0.  Substituting the line equation into
  //   the hyperplane equation gives
  //     PN . LP + t PN . LD - PN . PP = 0,
  //   and hence
  //     t = (PN . PP - PN . LP) / (PN . LD).
  const FloatType numerator =
      DotProduct(plane_normal, plane_point) -
      DotProduct(plane_normal, line_point);
  FloatType denominator = DotProduct(plane_normal, line_direction);
  if (fabs(denominator) < kFloatTypeError) {
    // Ensure correct behavior for near-zero denominators:
    denominator = 0.0;
  }
  return numerator / denominator;
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::AddSystem(
    CoordinateSystem system) const {
  // Find a coordinate system that we know and convert from it:
  for (int i = 0; i < systems; ++i) {
    if (has_[i] && i != system) {
      Convert(i, x_[i], system, x_[system]);
      has_[system] = true;
      return;
    }
  }
  // Fatal error; didn't find the necessary conversion:
  LOG(FATAL) << "Coordinates object has no coordinates set";  // COV_NF_LINE
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::AddSystemIfNeeded(
    CoordinateSystem system) const {
  if (!Has(system)) {
    // Convert to this system:
    AddSystem(system);
  }
}

template<size_t systems, size_t dimensions>
void Coordinates<systems, dimensions>::CheckValidSystem(
    CoordinateSystem system) const {
  // These checks are run only in debug builds and not in optimized builds:
  DCHECK_GE(system, 0) << "Unknown coordinate system " << system;
  DCHECK_LT(system, NumberOfSystems())
      << "Unknown coordinate system " << system;
}

}  // namespace energy_rec

#endif  // ENERGY_REC_UTIL_COORDINATES_H_
