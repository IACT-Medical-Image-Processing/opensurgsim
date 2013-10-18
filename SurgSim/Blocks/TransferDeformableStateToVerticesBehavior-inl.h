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

#ifndef SURGSIM_BLOCKS_TRANSFERDEFORMABLESTATETOVERTICESBEHAVIOR_INL_H
#define SURGSIM_BLOCKS_TRANSFERDEFORMABLESTATETOVERTICESBEHAVIOR_INL_H

#include <SurgSim/Physics/DeformableRepresentationState.h>
#include <SurgSim/DataStructures/Vertices.h>
#include <SurgSim/DataStructures/Vertex.h>

namespace SurgSim
{

namespace Blocks
{

template <class VertexData>
TransferDeformableStateToVerticesBehavior<VertexData>::TransferDeformableStateToVerticesBehavior(
	const std::string& name,
	std::shared_ptr<SurgSim::Physics::DeformableRepresentationState> from,
	unsigned int numDofPerNode,
	std::shared_ptr<SurgSim::DataStructures::Vertices<VertexData>> to) :
	SurgSim::Framework::Behavior(name),
	m_from(from),
	m_numDofPerNode(numDofPerNode),
	m_to(to)
{
}

template <class VertexData>
void TransferDeformableStateToVerticesBehavior<VertexData>::update(double dt)
{
	transfer();
}

template <class VertexData>
bool TransferDeformableStateToVerticesBehavior<VertexData>::doInitialize()
{
	return true;
}

template <class VertexData>
bool TransferDeformableStateToVerticesBehavior<VertexData>::doWakeUp()
{
	transfer();
	return true;
}

template <class VertexData>
void TransferDeformableStateToVerticesBehavior<VertexData>::transfer()
{
	const unsigned int numNodes = m_from->getNumNodes();

	// If vertices is empty, let's populate it properly
	if (m_to->getNumVertices() == 0 && numNodes != 0)
	{
		for (unsigned int nodeId = 0; nodeId < numNodes; nodeId++)
		{
			SurgSim::DataStructures::Vertex<VertexData> v(m_from->getPosition(nodeId), VertexData());
			m_to->addVertex(v);
		}
	}
	else
	{
		for (unsigned int nodeId = 0; nodeId < numNodes; nodeId++)
		{
			m_to->setVertexPosition(nodeId, m_from->getPosition(nodeId));
		}
	}
}

template <>
void TransferDeformableStateToVerticesBehavior<void>::transfer();

}; //namespace Blocks

}; //namespace SurgSim

#endif // SURGSIM_BLOCKS_TRANSFERDEFORMABLESTATETOVERTICESBEHAVIOR_INL_H
