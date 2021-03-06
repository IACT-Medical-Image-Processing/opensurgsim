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

#include "SurgSim/Physics/FixedConstraintFixedPoint.h"
#include "SurgSim/Physics/FixedRepresentation.h"
#include "SurgSim/Physics/Localization.h"

using SurgSim::Math::Vector3d;

namespace SurgSim
{

namespace Physics
{

FixedConstraintFixedPoint::FixedConstraintFixedPoint()
{
}

FixedConstraintFixedPoint::~FixedConstraintFixedPoint()
{
}

void FixedConstraintFixedPoint::doBuild(double dt,
											 const ConstraintData& data,
											 const std::shared_ptr<Localization>& localization,
											 MlcpPhysicsProblem* mlcp,
											 size_t indexOfRepresentation,
											 size_t indexOfConstraint,
											 ConstraintSideSign sign)
{
	std::shared_ptr<Representation> representation = localization->getRepresentation();

	if (!representation->isActive())
	{
		return;
	}

	const double scale = (sign == CONSTRAINT_POSITIVE_SIDE) ? 1.0 : -1.0;

	Vector3d globalPosition = localization->calculatePosition();

	// Fill up b with the constraint equation...
	mlcp->b.segment<3>(indexOfConstraint) += globalPosition * scale;
}

SurgSim::Physics::ConstraintType FixedConstraintFixedPoint::getConstraintType() const
{
	return SurgSim::Physics::FIXED_3DPOINT;
}

size_t FixedConstraintFixedPoint::doGetNumDof() const
{
	return 3;
}

}; //  namespace Physics

}; //  namespace SurgSim
