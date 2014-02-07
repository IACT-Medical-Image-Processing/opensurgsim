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

#include <gtest/gtest.h>


#include "SurgSim/Blocks/PoseInterpolator.h"
#include "SurgSim/Framework/Runtime.h"
#include "SurgSim/Framework/Representation.h"
#include "SurgSim/Math/RigidTransform.h"
#include "SurgSim/Math/Vector.h"
#include "SurgSim/Math/Quaternion.h"


using SurgSim::Math::RigidTransform3d;
using SurgSim::Math::Quaterniond;
using SurgSim::Math::Vector3d;
using SurgSim::Math::makeRigidTransform;
using SurgSim::Math::interpolate;

namespace
{
	RigidTransform3d startPose = makeRigidTransform(Quaterniond::Identity(), Vector3d(1.0,2.0,3.0));
	RigidTransform3d endPose = makeRigidTransform(Quaterniond::Identity(), Vector3d(3.0,2.0,1.0));
}

class PoseTestRepresentation : public SurgSim::Framework::Representation
{
public:
	PoseTestRepresentation(const std::string& name) : Representation(name)
	{

	}

	virtual void setInitialPose(const SurgSim::Math::RigidTransform3d& pose)
	{
		m_pose = pose;
	}

	virtual const SurgSim::Math::RigidTransform3d& getInitialPose() const
	{
		return m_pose;
	}

	virtual void setPose(const SurgSim::Math::RigidTransform3d& pose)
	{
		m_pose = pose;
	}

	virtual const SurgSim::Math::RigidTransform3d& getPose() const
	{
		return m_pose;
	}

private:
	RigidTransform3d m_pose;
};



namespace SurgSim
{
namespace Blocks
{

TEST(PoseInterpolatorTests, InitTest)
{
	ASSERT_NO_THROW({auto interpolator = std::make_shared<PoseInterpolator>("test");});
}

TEST(PoseInterpolatorTests, StartAndEndPose)
{
	auto runtime = std::make_shared<SurgSim::Framework::Runtime>();
	auto representation = std::make_shared<PoseTestRepresentation>("representation");
	auto interpolator = std::make_shared<PoseInterpolator>("interpolator");

	interpolator->setStartingPose(startPose);
	interpolator->setEndingPose(endPose);
	interpolator->setTarget(representation);

	interpolator->initialize(runtime);
	interpolator->wakeUp();
	interpolator->update(0.0);

	EXPECT_TRUE(startPose.isApprox(representation->getPose()));
	interpolator->update(0.5);
	
	RigidTransform3d pose = interpolate(startPose, endPose, 0.5);
	EXPECT_TRUE(pose.matrix().isApprox(representation->getPose().matrix())) << pose.matrix() << std::endl << representation->getPose().matrix();
}

TEST(PoseInterpolatorTests, UseOptionalStartPose)
{
	auto runtime = std::make_shared<SurgSim::Framework::Runtime>();
	auto representation = std::make_shared<PoseTestRepresentation>("representation");
	auto interpolator = std::make_shared<PoseInterpolator>("interpolator");

	representation->setInitialPose(startPose);
	interpolator->setEndingPose(endPose);
	interpolator->setTarget(representation);

	interpolator->initialize(runtime);
	interpolator->wakeUp();
	interpolator->update(0.0);

	EXPECT_TRUE(startPose.isApprox(representation->getPose()));
	interpolator->update(0.5);

	RigidTransform3d pose = interpolate(startPose, endPose, 0.5);
	EXPECT_TRUE(pose.matrix().isApprox(representation->getPose().matrix())) << pose.matrix() << std::endl << representation->getPose().matrix();
}

TEST(PoseInterpolatorTests, UseLoop)
{
	auto runtime = std::make_shared<SurgSim::Framework::Runtime>();
	auto representation = std::make_shared<PoseTestRepresentation>("representation");
	auto interpolator = std::make_shared<PoseInterpolator>("interpolator");

	interpolator->setStartingPose(startPose);
	interpolator->setEndingPose(endPose);
	interpolator->setTarget(representation);
	
	interpolator->setPingPong(true);
	interpolator->setLoop(true);

	// Enabling loop should disable pingpong
	EXPECT_TRUE(interpolator->isLoop());
	EXPECT_FALSE(interpolator->isPingPong());

	interpolator->initialize(runtime);
	interpolator->wakeUp();
	interpolator->update(0.0);

	EXPECT_TRUE(startPose.isApprox(representation->getPose()));
	interpolator->update(0.25);

	RigidTransform3d pose = interpolate(startPose, endPose, 0.25);
	EXPECT_TRUE(pose.matrix().isApprox(representation->getPose().matrix())) << pose.matrix() << std::endl << representation->getPose().matrix();

	// We advance by 1.0, this should wrap around to 0.25 again and the poses should be the same
	interpolator->update(1.0);
	EXPECT_TRUE(pose.matrix().isApprox(representation->getPose().matrix())) << pose.matrix() << std::endl << representation->getPose().matrix();

}

TEST(PoseInterpolatorTests, UsePingPong)
{
	auto runtime = std::make_shared<SurgSim::Framework::Runtime>();
	auto representation = std::make_shared<PoseTestRepresentation>("representation");
	auto interpolator = std::make_shared<PoseInterpolator>("interpolator");

	interpolator->setStartingPose(startPose);
	interpolator->setEndingPose(endPose);
	interpolator->setTarget(representation);

	interpolator->setLoop(true);
	interpolator->setPingPong(true);

	// Enabling PingPong should disable Loop
	EXPECT_FALSE(interpolator->isLoop());
	EXPECT_TRUE(interpolator->isPingPong());

	interpolator->initialize(runtime);
	interpolator->wakeUp();
	interpolator->update(0.0);

	EXPECT_TRUE(startPose.isApprox(representation->getPose()));
	interpolator->update(0.25);

	RigidTransform3d pose = interpolate(startPose, endPose, 0.25);
	EXPECT_TRUE(pose.matrix().isApprox(representation->getPose().matrix())) << pose.matrix() << std::endl << representation->getPose().matrix();

	// We advance by 1.0, this should wrap around to 0.25 and the poses should be flipped
	pose = interpolate(endPose, startPose, 0.25);
	interpolator->update(1.0);
	EXPECT_TRUE(pose.matrix().isApprox(representation->getPose().matrix())) << pose.matrix() << std::endl << representation->getPose().matrix();

}

}
}
