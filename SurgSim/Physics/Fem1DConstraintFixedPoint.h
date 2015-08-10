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

#ifndef SURGSIM_PHYSICS_FEM1DCONSTRAINTFIXEDPOINT_H
#define SURGSIM_PHYSICS_FEM1DCONSTRAINTFIXEDPOINT_H

#include "SurgSim/Physics/ConstraintImplementation.h"

namespace SurgSim
{

namespace Physics
{

/// Fem1DRepresentation bilateral 3d constraint implementation.
///
/// The family of FixedPoint constraints enforce equality between two points.
class Fem1DConstraintFixedPoint : public ConstraintImplementation
{
public:
	/// Constructor
	Fem1DConstraintFixedPoint();

	/// Destructor
	virtual ~Fem1DConstraintFixedPoint();

	SurgSim::Physics::ConstraintType getConstraintType() const override;

private:
	size_t doGetNumDof() const override;

	void doBuild(double dt,
				 const ConstraintData& data,
				 const std::shared_ptr<Localization>& localization,
				 MlcpPhysicsProblem* mlcp,
				 size_t indexOfRepresentation,
				 size_t indexOfConstraint,
				 ConstraintSideSign sign) override;
};

}; // namespace Physics

}; // namespace SurgSim

#endif // SURGSIM_PHYSICS_FEM1DCONSTRAINTFIXEDPOINT_H