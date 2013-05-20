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

#ifndef SURGSIM_GRAPHICS_ACTOR_H
#define SURGSIM_GRAPHICS_ACTOR_H

#include <SurgSim/Framework/Representation.h>

#include <SurgSim/Math/RigidTransform.h>

namespace SurgSim
{

namespace Graphics
{

class Manager;
class Material;

/// Base graphics actor class, which defines the interface that all graphics actors must implement.
///
/// A Graphics::Actor is the visual Framework::Representation of a Framework::SceneElement in the Framework::Scene.
class Actor : public SurgSim::Framework::Representation
{
public:
	/// Constructor
	/// \param	name	Name of the actor
	explicit Actor(const std::string& name) : SurgSim::Framework::Representation(name)
	{
	}

	/// Sets whether the actor is currently visible
	/// \param	visible	True for visible, false for invisible
	virtual void setVisible(bool visible) = 0;

	/// Gets whether the actor is currently visible
	/// \return	visible	True for visible, false for invisible
	virtual bool isVisible() const = 0;

	/// Sets the pose of the actor
	/// \param	transform	Rigid transformation that describes the pose of the actor
	virtual void setPose(const SurgSim::Math::RigidTransform3d& transform) = 0;

	/// Gets the pose of the actor
	/// \return	Rigid transformation that describes the pose of the actor
	virtual const SurgSim::Math::RigidTransform3d& getPose() const = 0;

	/// Updates the actor
	/// \param	dt	The time in seconds of the preceding timestep.
	virtual void update(double dt) = 0;
};

};  // namespace Graphics

};  // namespace SurgSim

#endif  // SURGSIM_GRAPHICS_ACTOR_H
