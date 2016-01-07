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

#ifndef SURGSIM_PHYSICS_SLIDINGCONSTRAINTDATA_H
#define SURGSIM_PHYSICS_SLIDINGCONSTRAINTDATA_H

#include "SurgSim/Physics/ConstraintData.h"
#include "SurgSim/Math/Vector.h"

namespace SurgSim
{

namespace Physics
{

/// Class for Frictionless sliding constraint (stores two planes, the intersection of which, is the sliding direction)
class SlidingConstraintData : public ConstraintData
{
public:
	/// Default constructor
	SlidingConstraintData();

	/// Constructor
	/// \param point The point through which the sliding direction vector passes.
	/// \param direction The sliding direction vector.
	SlidingConstraintData(const SurgSim::Math::Vector3d& point, const SurgSim::Math::Vector3d& direction);

	/// Destructor
	virtual ~SlidingConstraintData();

	/// Calculate the two plane equations based on the given sliding direction.
	/// \param point The point through which the sliding direction vector passes.
	/// \param direction The sliding direction vector.
	void setSlidingDirection(const SurgSim::Math::Vector3d& point, const SurgSim::Math::Vector3d& direction);

	/// \return The normals of the two planes.
	const std::array<Math::Vector3d, 2>& getNormals() const;

	/// \return The distance from origin of the two planes.
	const std::array<double, 2>& getDistances() const;

private:
	/// The normals of the two planes.
	std::array<Math::Vector3d, 2> m_normals;

	/// The distance from origin of the two planes.
	std::array<double, 2> m_distances;
};

} // namespace Physics

} // namespace SurgSim

#endif // SURGSIM_PHYSICS_SLIDINGCONSTRAINTDATA_H
