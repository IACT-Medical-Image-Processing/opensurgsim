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

#ifndef EXAMPLES_GRAPHICSSCENE_POSEINTERPOLATOR_H
#define EXAMPLES_GRAPHICSSCENE_POSEINTERPOLATOR_H

#include <memory>
#include <string>
#include "SurgSim/Framework/Behavior.h"
#include "SurgSim/Math/RigidTransform.h"
#include "SurgSim/DataStructures/OptionalValue.h"

namespace SurgSim
{
namespace Framework
{
	class Representation;
}
}

namespace SurgSim
{
namespace Blocks
{

/// Perform linear interpolation on two poses
class PoseInterpolator : public SurgSim::Framework::Behavior
{
public:

	/// Constructor
	explicit PoseInterpolator(const std::string& name);

	/// Set the starting pose. This is optional, if not set the target's pose
	/// will be used as the starting pose.
	/// \param transform The starting pose.
	void setStartingPose(const SurgSim::Math::RigidTransform3d transform);

	/// Set the end pose.
	/// \param transform The end pose.
	void setEndingPose(const SurgSim::Math::RigidTransform3d transform);

	/// Set the target of the interpolation, this is where the interpolated transform
	/// will be applied to. If no starting pose is set, the pose of this representation will
	/// be used as the starting pose
	/// \param target The target that will use the interpolated pose.
	void setTarget(std::shared_ptr<SurgSim::Framework::Representation> target);

	/// Set the duration of the interpolation.
	/// \param t The duration in seconds.
	void setDuration(double t);

	/// Get the duration.
	/// \return The duration in seconds.
	double getDuration() const;

	/// Sets the interpolation to looping, pingpong and loop cannot be used together.
	/// \param val If true the interpolation will loop.
	void setLoop(bool val);

	/// \return true If the interpolation is looping.
	bool isLoop() const;

	/// Sets the interpolation to ping pong back and forth between the starting and ending poses.
	/// pingpong and loop cannot be used together.
	/// \param val If true the interpolation will ping pong.
	void setPingPong(bool val);

	/// \return true If the interpolation is doing ping pong.
	bool isPingPong() const;

	/// Overridden from Behavior
	virtual void update(double dt) override;

private:

	/// Optional value to take the from rigid transform
	SurgSim::DataStructures::OptionalValue<SurgSim::Math::RigidTransform3d> m_optionalStartPose;

	/// Start of the interpolation
	SurgSim::Math::RigidTransform3d m_endingPose;

	/// Target of the interpolation
	SurgSim::Math::RigidTransform3d m_startingPose;

	/// Target for the interpolated RigidTransform 
	std::shared_ptr<SurgSim::Framework::Representation> m_target;

	/// Duration of the interpolation
	double m_duration;

	/// How far through the interpolation we are
	double m_currentTime;

	/// Whether to pingpong
	bool m_pingpong;

	/// Whether to loop
	bool m_loop;

	/// Overridden from Component
	virtual bool doWakeUp() override;

	/// Overridden from Component
	virtual bool doInitialize() override;
};


}; // Blocks
}; // Surgsim

#endif