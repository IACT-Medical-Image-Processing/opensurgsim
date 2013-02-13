// This file is a part of the OpenSurgSim project.
// Copyright 2012-2013, SimQuest Solutions Inc.
//
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

/** @file
 *  Definitions of small fixed-size vector types.
 */

#ifndef SURGSIM_MATH_VECTOR_H
#define SURGSIM_MATH_VECTOR_H

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace SurgSim
{
namespace Math
{

/// A 2D vector of floats.
/// This type (and any struct that contain it) can be safely allocated via new.
typedef Eigen::Matrix<float,  2, 1, Eigen::DontAlign>  Vector2f;

/// A 3D vector of floats.
/// This type (and any struct that contain it) can be safely allocated via new.
typedef Eigen::Matrix<float,  3, 1, Eigen::DontAlign>  Vector3f;

/// A 4D vector of floats.
/// This type (and any struct that contain it) can be safely allocated via new.
typedef Eigen::Matrix<float,  4, 1, Eigen::DontAlign>  Vector4f;

/// A 2D vector of doubles.
/// This type (and any struct that contain it) can be safely allocated via new.
typedef Eigen::Matrix<double, 2, 1, Eigen::DontAlign>  Vector2d;

/// A 3D vector of doubles.
/// This type (and any struct that contain it) can be safely allocated via new.
typedef Eigen::Matrix<double, 3, 1, Eigen::DontAlign>  Vector3d;

/// A 4D vector of doubles.
/// This type (and any struct that contain it) can be safely allocated via new.
typedef Eigen::Matrix<double, 4, 1, Eigen::DontAlign>  Vector4d;



/// A 2D vector of floats, with vectorized (SIMD) operations enabled if possible.
/// Note that special alignment restrictions may apply to this type; see Eigen documentation for details.
typedef Eigen::Matrix<float,  2, 1, Eigen::AutoAlign>  Vector2fv;

/// A 3D vector of floats, with vectorized (SIMD) operations enabled if possible.
/// Note that special alignment restrictions may apply to this type; see Eigen documentation for details.
typedef Eigen::Matrix<float,  3, 1, Eigen::AutoAlign>  Vector3fv;

/// A 4D vector of floats, with vectorized (SIMD) operations enabled if possible.
/// Note that special alignment restrictions may apply to this type; see Eigen documentation for details.
typedef Eigen::Matrix<float,  4, 1, Eigen::AutoAlign>  Vector4fv;

/// A 2D vector of doubles, with vectorized (SIMD) operations enabled if possible.
/// Note that special alignment restrictions may apply to this type; see Eigen documentation for details.
typedef Eigen::Matrix<double, 2, 1, Eigen::AutoAlign>  Vector2dv;

/// A 3D vector of doubles, with vectorized (SIMD) operations enabled if possible.
/// Note that special alignment restrictions may apply to this type; see Eigen documentation for details.
typedef Eigen::Matrix<double, 3, 1, Eigen::AutoAlign>  Vector3dv;

/// A 4D vector of doubles, with vectorized (SIMD) operations enabled if possible.
/// Note that special alignment restrictions may apply to this type; see Eigen documentation for details.
typedef Eigen::Matrix<double, 4, 1, Eigen::AutoAlign>  Vector4dv;

};  // namespace Math
};  // namespace SurgSim

#endif  // SURGSIM_MATH_VECTOR_H