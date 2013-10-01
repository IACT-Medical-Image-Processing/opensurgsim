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

#ifndef SURGSIM_PHYSICS_RIGIDREPRESENTATIONBASE_H
#define SURGSIM_PHYSICS_RIGIDREPRESENTATIONBASE_H

#include <SurgSim/Physics/Representation.h>

#include <SurgSim/Physics/Localization.h>
#include <SurgSim/Collision/Location.h>
#include <SurgSim/Physics/RigidRepresentationState.h>
#include <SurgSim/Physics/RigidRepresentationParameters.h>
#include <SurgSim/Physics/RigidRepresentationLocalization.h>

#include <SurgSim/Math/RigidTransform.h>

using SurgSim::Collision::Location;

namespace SurgSim
{
namespace Physics
{

class RigidRepresentationState;

/// The RigidRepresentationBase class defines the base class for
/// all rigid motion based representations (fixed, rigid body, rigid body + vtc,...)
class RigidRepresentationBase : public Representation
{
public:
	typedef SurgSim::Math::RigidTransform3d RigidTransform3d;

	/// Constructor
	/// \param name The rigid representation's name
	explicit RigidRepresentationBase(const std::string& name);

	/// Destructor
	virtual ~RigidRepresentationBase();

	/// Set the initial pose of the rigid representation
	/// \param pose The initial pose (translation + rotation)
	void setInitialPose(const SurgSim::Math::RigidTransform3d& pose);
	/// Set the initial state of the rigid representation
	/// \param state The initial state (pose + lin/ang velocities)
	/// This will also set the current/previous states to the initial state
	void setInitialState(const RigidRepresentationState& state);
	/// Reset the rigid representation state to its initial state
	void resetState();

	/// Get the initial state of the rigid representation
	/// \return The initial state (pose + lin/ang velocities)
	const RigidRepresentationState& getInitialState() const;
	/// Get the current state of the rigid representation
	/// \return The current state (pose + lin/ang velocities)
	const RigidRepresentationState& getCurrentState() const;
	/// Get the previous state of the rigid representation
	/// \return The previous state (pose + lin/ang velocities)
	const RigidRepresentationState& getPreviousState() const;

	/// Get the initial pose of the rigid representation
	/// \return The initial pose (translation + rotation)
	const SurgSim::Math::RigidTransform3d& getInitialPose() const;
	/// Get the previous pose of the rigid representation
	/// \return The previous pose (translation + rotation)
	const SurgSim::Math::RigidTransform3d& getPreviousPose() const;
	/// Get the current pose of the rigid representation
	/// \return The current pose (translation + rotation)
	const SurgSim::Math::RigidTransform3d& getCurrentPose() const;

	/// Get the final pose of the rigid representation
	/// \return The final pose (translation + rotation)
	const SurgSim::Math::RigidTransform3d& getPose() const;

	std::shared_ptr<Localization> createLocalization(const Location& location);

	/// Get the initial parameters of the rigid representation
	/// \return The initial parameters of the rigid representation
	const RigidRepresentationParameters& getInitialParameters() const;
	/// Get the current parameters of the rigid representation
	/// \return The current parameters of the rigid representation
	const RigidRepresentationParameters& getCurrentParameters() const;

protected:
	/// Initial rigid representation state (useful for reset)
	RigidRepresentationState m_initialState;
	/// Previous rigid representation state
	RigidRepresentationState m_previousState;
	/// Current rigid representation state
	RigidRepresentationState m_currentState;
	/// Last valid/final rigid representation state
	RigidRepresentationState m_finalState;

	/// Initial physical parameters
	RigidRepresentationParameters m_initialParameters;
	/// Current physical parameters
	RigidRepresentationParameters m_currentParameters;

	/// Creates typed localization.
	/// \tparam	T	Type of localization to create.
	/// \param	location	The location for the localization.
	/// \return	The new Localization;
	template <class T>
	std::shared_ptr<T> createTypedLocalization(const Location& location);

private:
	virtual void updateGlobalInertiaMatrices(const RigidRepresentationState& state) = 0;
};

}; // Physics

}; // SurgSim

#include <SurgSim/Physics/RigidRepresentationBase-inl.h>

#endif // SURGSIM_PHYSICS_RIGIDREPRESENTATIONBASE_H
