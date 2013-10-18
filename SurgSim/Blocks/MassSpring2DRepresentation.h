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

#ifndef SURGSIM_BLOCKS_MASSSPRING2DREPRESENTATION_H
#define SURGSIM_BLOCKS_MASSSPRING2DREPRESENTATION_H

#include <SurgSim/Math/Vector.h>
#include <SurgSim/Physics/MassSpringRepresentation.h>
#include <SurgSim/Physics/DeformableRepresentationState.h>

using SurgSim::Math::Vector3d;
using SurgSim::Physics::MassSpringRepresentation;
using SurgSim::Physics::DeformableRepresentationState;

namespace SurgSim
{

namespace Blocks
{

// This class instantiate a simple 2D structures
class MassSpring2DRepresentation : public MassSpringRepresentation
{
public:
	/// Constructor
	/// \param name The name to assign to the model
	explicit MassSpring2DRepresentation(const std::string& name) : MassSpringRepresentation(name)
	{
	}

	/// Initializes a 2D MassSpring
	/// \param extremities 4 positions forming the extremities of the 2D regular model (4 corners)
	/// \param numNodesPerDim The number of nodes to be created for each dimension (here 2)
	/// \param boundaryConditions The list of all boundary conditions (fixed dof)
	/// \param totalMass The total mass of the mass spring (evenly spread out on the masses)
	/// \param stiffnessStretching, dampingStretching The spring param for all stretching springs (edges)
	/// \param stiffnessBending, dampingBending The spring param for all bending springs (edges)
	/// \param stiffnessFaceDiagonal, dampingFaceDiagonal The spring param for all face diagonal springs (faces)
	/// \note Stretching springs are connecting neighbors, bending springs are connecting 1 node
	/// \note to its 2nd degree neighbors, creating a bending force around the middle node.
	/// \note Face diagonal springs aim at maintaining the area of a square
	/// \note extremities are organized as follow:
	/// \note   [0][1] *---* [1][1]
	/// \note          |   |
	/// \note   [0][0] *---* [1][0]
	void init2D(const Vector3d extremities[2][2], unsigned int numNodesPerDim[2],
		std::vector<unsigned int> boundaryConditions,
		double totalMass,
		double stiffnessStretching, double dampingStretching,
		double stiffnessBending, double dampingBending,
		double stiffnessFaceDiagonal, double dampingFaceDiagonal);

private:
	/// Helper method to initialize/add all stretching springs on a 2D structure
	/// \param state The state to initialize the springs with (rest lengths calculation)
	/// \param numNodesPerDim The number of nodes on the 2 dimensions
	/// \param stiffness, damping The spring parameters
	void init2DStretchingSprings(const std::shared_ptr<DeformableRepresentationState> state,
		unsigned int numNodesPerDim[2], double stiffness, double damping);
	/// Helper method to initialize/add all bending springs on a 2D structure
	/// \param state The state to initialize the springs with (rest lengths calculation)
	/// \param numNodesPerDim The number of nodes on the 2 dimensions
	/// \param stiffness, damping The spring parameters
	void init2DBendingSprings(const std::shared_ptr<DeformableRepresentationState> state,
		unsigned int numNodesPerDim[2], double stiffness, double damping);
	/// Helper method to initialize/add all face diagonal springs on a 2D structure
	/// \param state The state to initialize the springs with (rest lengths calculation)
	/// \param numNodesPerDim The number of nodes on the 2 dimensions
	/// \param stiffness, damping The spring parameters
	void init2DFaceDiagonalSprings(const std::shared_ptr<DeformableRepresentationState> state,
		unsigned int numNodesPerDim[2], double stiffness, double damping);
};

};  // namespace Blocks

};  // namespace SurgSim

#endif  // SURGSIM_BLOCKS_MASSSPRING2DREPRESENTATION_H
