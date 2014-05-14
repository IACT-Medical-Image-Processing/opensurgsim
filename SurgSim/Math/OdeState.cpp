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

#include "SurgSim/Math/OdeState.h"
#include "SurgSim/Math/Valid.h"

namespace SurgSim
{

namespace Math
{

OdeState::OdeState() : m_numDofPerNode(0u), m_numNodes(0u)
{
}

OdeState::~OdeState()
{
}

bool OdeState::operator ==(const OdeState& state) const
{
	return m_x == state.m_x && m_v == state.m_v && m_boundaryConditionsPerDof == state.m_boundaryConditionsPerDof;
}

bool OdeState::operator !=(const OdeState& state) const
{
	return ! ((*this) == state);
}

void OdeState::reset()
{
	m_x.setZero();
	m_v.setZero();
	m_boundaryConditionsPerDof.setConstant(false);
	m_boundaryConditionsAsDofIds.clear();
}

void OdeState::setNumDof(unsigned int numDofPerNode, unsigned int numNodes)
{
	const unsigned int numDof = numDofPerNode * numNodes;

	m_numDofPerNode = numDofPerNode;
	m_numNodes = numNodes;

	m_x.resize(numDof);
	m_v.resize(numDof);
	m_boundaryConditionsPerDof.resize(numDof);

	// Zero-out everything
	reset();
}

unsigned int OdeState::getNumDof() const
{
	const unsigned int numDof = m_numDofPerNode * m_numNodes;

	SURGSIM_ASSERT(m_x.size() == m_v.size() &&
		m_x.size() == m_boundaryConditionsPerDof.size() && m_x.size() == static_cast<int>(numDof));

	return numDof;
}

unsigned int OdeState::getNumNodes() const
{
	return m_numNodes;
}

SurgSim::Math::Vector& OdeState::getPositions()
{
	return m_x;
}

const SurgSim::Math::Vector& OdeState::getPositions() const
{
	return m_x;
}

const SurgSim::Math::Vector3d OdeState::getPosition(unsigned int nodeId) const
{
	return SurgSim::Math::getSubVector(m_x, nodeId, m_numDofPerNode).segment(0, 3);
}

SurgSim::Math::Vector& OdeState::getVelocities()
{
	return m_v;
}

const SurgSim::Math::Vector& OdeState::getVelocities() const
{
	return m_v;
}

const SurgSim::Math::Vector3d OdeState::getVelocity(unsigned int nodeId) const
{
	return SurgSim::Math::getSubVector(m_v, nodeId, m_numDofPerNode).segment(0, 3);
}

void OdeState::addBoundaryCondition(unsigned int nodeId)
{
	SURGSIM_ASSERT(m_numDofPerNode != 0u) <<
		"Number of dof per node = 0. Make sure to call setNumDof() prior to adding boundary conditions.";

	for (unsigned int nodeDofId = 0; nodeDofId < m_numDofPerNode; ++nodeDofId)
	{
		addBoundaryCondition(nodeId, nodeDofId);
	}
}

void OdeState::addBoundaryCondition(unsigned int nodeId, unsigned int nodeDofId)
{
	SURGSIM_ASSERT(m_numDofPerNode != 0u) <<
		"Number of dof per node = 0. Make sure to call setNumDof() prior to adding boundary conditions.";
	SURGSIM_ASSERT(nodeId < m_numNodes) << "Invalid nodeId " << nodeId << " number of nodes is " << m_numNodes;
	SURGSIM_ASSERT(nodeDofId < m_numDofPerNode) <<
		"Invalid nodeDofId " << nodeDofId << " number of dof per node is " << m_numDofPerNode;

	unsigned int globalDofId = nodeId * m_numDofPerNode + nodeDofId;
	if (! m_boundaryConditionsPerDof[globalDofId])
	{
		m_boundaryConditionsPerDof[globalDofId] = true;
		m_boundaryConditionsAsDofIds.push_back(globalDofId);
	}
}

unsigned int OdeState::getNumBoundaryConditions() const
{
	return m_boundaryConditionsAsDofIds.size();
}

const std::vector<unsigned int>& OdeState::getBoundaryConditions() const
{
	return m_boundaryConditionsAsDofIds;
}

bool OdeState::isBoundaryCondition(unsigned int dof) const
{
	return m_boundaryConditionsPerDof[dof];
}

void OdeState::applyBoundaryConditionsToVector(Vector* vector) const
{
	SURGSIM_ASSERT(static_cast<unsigned int>(vector->size()) == getNumDof()) <<
		"Invalid vector to apply boundary conditions on";

	for (std::vector<unsigned int>::const_iterator it = getBoundaryConditions().cbegin();
		it != getBoundaryConditions().cend();
		++it)
	{
		(*vector)[*it] = 0.0;
	}
}

void OdeState::applyBoundaryConditionsToMatrix(Matrix* matrix) const
{
	SURGSIM_ASSERT(static_cast<unsigned int>(matrix->rows()) == getNumDof() &&
		static_cast<unsigned int>(matrix->cols()) == getNumDof()) << "Invalid matrix to apply boundary conditions on";

	for (std::vector<unsigned int>::const_iterator it = getBoundaryConditions().cbegin();
		it != getBoundaryConditions().cend();
		++it)
	{
		(*matrix).middleRows(*it, 1).setZero();
		(*matrix).middleCols(*it, 1).setZero();
		(*matrix)(*it, *it) = 1.0;
	}
}

bool OdeState::isValid() const
{
	return SurgSim::Math::isValid(getPositions()) && SurgSim::Math::isValid(getVelocities());
}

}; // namespace Math

}; // namespace SurgSim