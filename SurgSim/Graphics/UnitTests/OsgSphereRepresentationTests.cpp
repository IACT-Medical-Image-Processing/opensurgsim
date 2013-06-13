// This file is a part of the OpenSurgSim project.
// Copyright 2012-2013, SimQuest Solutions Inc.
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

/// \file
/// Tests for the OsgSphereRepresentation class.

#include <SurgSim/Graphics/OsgSphereRepresentation.h>
#include <SurgSim/Math/Quaternion.h>
#include <SurgSim/Math/Vector.h>

#include <gtest/gtest.h>

#include <random>

using SurgSim::Math::Quaterniond;
using SurgSim::Math::RigidTransform3d;
using SurgSim::Math::Vector3d;
using SurgSim::Math::makeRigidTransform;

namespace SurgSim
{

namespace Graphics
{

TEST(OsgSphereRepresentationTests, InitTest)
{
	ASSERT_NO_THROW({std::shared_ptr<Representation> representation =
		std::make_shared<OsgSphereRepresentation>("test name");});

	std::shared_ptr<Representation> representation = std::make_shared<OsgSphereRepresentation>("test name");
	EXPECT_EQ("test name", representation->getName());
}

TEST(OsgSphereRepresentationTests, VisibilityTest)
{
	std::shared_ptr<Representation> representation = std::make_shared<OsgSphereRepresentation>("test name");

	representation->setVisible(true);
	EXPECT_TRUE(representation->isVisible());

	representation->setVisible(false);
	EXPECT_FALSE(representation->isVisible());
}

TEST(OsgSphereRepresentationTests, RadiusTest)
{
	std::shared_ptr<SphereRepresentation> sphereRepresentation = std::make_shared<OsgSphereRepresentation>("test name");

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(1.0, 10.0);

	double randomRadius = distribution(generator);

	sphereRepresentation->setRadius(randomRadius);
	EXPECT_EQ(randomRadius, sphereRepresentation->getRadius());
}

TEST(OsgSphereRepresentationTests, PoseTest)
{
	std::shared_ptr<Representation> representation = std::make_shared<OsgSphereRepresentation>("test name");

	EXPECT_TRUE(representation->getPose().isApprox(RigidTransform3d::Identity()));

	/// Create a random rigid body transform
	Vector3d translation = Vector3d::Random();
	Quaterniond quaternion = Quaterniond(SurgSim::Math::Vector4d::Random());
	quaternion.normalize();
	RigidTransform3d transform = SurgSim::Math::makeRigidTransform(quaternion, translation);

	/// Set the transform and make sure it was set correctly
	representation->setPose(transform);
	EXPECT_TRUE(representation->getPose().isApprox(transform));
}

};  // namespace Graphics

};  // namespace SurgSim