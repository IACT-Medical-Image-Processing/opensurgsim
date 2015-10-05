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

#ifndef SURGSIM_PHYSICS_FEMCONSTRAINTFRICTIONLESSCONTACT_INL_H
#define SURGSIM_PHYSICS_FEMCONSTRAINTFRICTIONLESSCONTACT_INL_H

#include "SurgSim/Math/Vector.h"
#include "SurgSim/Physics/ContactConstraintData.h"
#include "SurgSim/Physics/FemElement.h"

using SurgSim::Math::Vector3d;

namespace SurgSim
{

namespace Physics
{

template <class FemLocalization>
FemConstraintFrictionlessContact<FemLocalization>::FemConstraintFrictionlessContact()
{
}

template <class FemLocalization>
FemConstraintFrictionlessContact<FemLocalization>::~FemConstraintFrictionlessContact()
{
}

template <class FemLocalization>
void FemConstraintFrictionlessContact<FemLocalization>::doBuild(double dt,
												 const ConstraintData& data,
												 const std::shared_ptr<Localization>& localization,
												 MlcpPhysicsProblem* mlcp,
												 size_t indexOfRepresentation,
												 size_t indexOfConstraint,
												 ConstraintSideSign sign)
{
	std::shared_ptr<FemRepresentation> fem
		= std::static_pointer_cast<FemRepresentation>(localization->getRepresentation());
	const size_t numDofPerNode = fem->getNumDofPerNode();

	if (!fem->isActive())
	{
		return;
	}

	const double scale = (sign == CONSTRAINT_POSITIVE_SIDE) ? 1.0 : -1.0;
	const SurgSim::Math::Vector3d& n = static_cast<const ContactConstraintData&>(data).getNormal();
	const SurgSim::DataStructures::IndexedLocalCoordinate& coord
		= std::static_pointer_cast<FemLocalization>(localization)->getLocalPosition();
	Vector3d globalPosition = localization->calculatePosition();

	// Update b with new violation
	double violation = n.dot(globalPosition);
	mlcp->b[indexOfConstraint] += violation * scale;

	// m_newH is a SparseVector, so resizing is cheap.  The object's memory also gets cleared.
	m_newH.resize(fem->getNumDof());
	// m_newH is a member variable, so 'reserve' only needs to allocate memory on the first run.
	std::shared_ptr<FemElement> femElement = fem->getFemElement(coord.index);
	size_t numNodes = femElement->getNumNodes();
	size_t numNodeToConstrain = 0;
	for (size_t index = 0; index < numNodes; index++)
	{
		if (coord.coordinate[index] != 0.0)
		{
			numNodeToConstrain++;
		}
	}
	m_newH.reserve(3 * numNodeToConstrain);

	for (size_t index = 0; index < numNodes; index++)
	{
		if (coord.coordinate[index] != 0.0)
		{
			size_t nodeIndex = femElement->getNodeId(index);
			m_newH.insert(numDofPerNode * nodeIndex + 0) = coord.coordinate[index] * n[0] * scale * dt;
			m_newH.insert(numDofPerNode * nodeIndex + 1) = coord.coordinate[index] * n[1] * scale * dt;
			m_newH.insert(numDofPerNode * nodeIndex + 2) = coord.coordinate[index] * n[2] * scale * dt;
		}
	}

	mlcp->updateConstraint(m_newH, fem->getComplianceMatrix() * m_newH.transpose(), indexOfRepresentation,
		indexOfConstraint);
}

template <class FemLocalization>
SurgSim::Physics::ConstraintType FemConstraintFrictionlessContact<FemLocalization>::getConstraintType() const
{
	return SurgSim::Physics::FRICTIONLESS_3DCONTACT;
}

template <class FemLocalization>
size_t FemConstraintFrictionlessContact<FemLocalization>::doGetNumDof() const
{
	return 1;
}

}; //  namespace Physics

}; //  namespace SurgSim

#endif // SURGSIM_PHYSICS_FEMCONSTRAINTFRICTIONLESSCONTACT_INL_H
