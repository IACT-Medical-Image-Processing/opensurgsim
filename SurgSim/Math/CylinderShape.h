// This file is a part of the OpenSurgSim project.
// Copyright 2013, SimQuest Solutions Inc.
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

#ifndef SURGSIM_MATH_CYLINDERSHAPE_H
#define SURGSIM_MATH_CYLINDERSHAPE_H

#include "SurgSim/Math/Shape.h"

namespace SurgSim
{

namespace Math
{

/// Cylinder shape: centered on (0 0 0), aligned along Y,
/// defined with length and radius.
class CylinderShape: public Shape
{
public:
	/// Constructor
	/// \param length The length of the cylinder (in m)
	/// \param radius The cylinder radius (in m)
	CylinderShape(double length = 0.0, double radius = 0.0);

	/// \return the type of the shape
	virtual int getType() override;

	/// Get the cylinder length
	/// \return The cylinder length (in m)
	double getLength() const;

	/// Get the cylinder radius
	/// \return The cylinder radius (in m)
	double getRadius() const;

	/// Calculate the volume of the cylinder
	/// \return The volume of the cylinder (in m-3)
	virtual double calculateVolume() const override;

	/// Calculate the mass center of the cylinder
	/// \return The mass center of the cylinder
	virtual Vector3d calculateMassCenter() const override;

	/// Calculate the inertia of the cylinder
	/// \param rho The mass density (in Kg.m-3)
	/// \return The 3x3 symmetric inertia matrix of the cylinder
	virtual Matrix33d calculateInertia(double rho) const override;

	/// Serialize declarations of the cylinder
	OSS_SERIALIZE(SurgSim::Math::CylinderShape);

private:
	/// The cylinder radius
	double   m_radius;

	/// The cylinder length
	double   m_length;
};

}; // Math

}; // SurgSim

#endif // SURGSIM_MATH_CYLINDERSHAPE_H
