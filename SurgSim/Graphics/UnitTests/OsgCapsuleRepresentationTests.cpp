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
/// Tests for the OsgCapsuleRepresentation class.

#include "SurgSim/Graphics/UnitTests/MockOsgObjects.h"

#include "SurgSim/Framework/FrameworkConvert.h"
#include "SurgSim/Graphics/OsgCapsuleRepresentation.h"
#include "SurgSim/Math/Vector.h"

#include <gtest/gtest.h>

using SurgSim::Math::Vector2d;

namespace
{
const double epsilon = 1e-10;
};

namespace SurgSim
{

namespace Graphics
{

TEST(OsgCapsuleRepresentationTests, AccessibleTest)
{
	std::shared_ptr<SurgSim::Framework::Component> component;
	ASSERT_NO_THROW(component = SurgSim::Framework::Component::getFactory().create(
									"SurgSim::Graphics::OsgCapsuleRepresentation",
									"capsule"));

	EXPECT_EQ("SurgSim::Graphics::OsgCapsuleRepresentation", component->getClassName());

	double radius = 4.321;
	double height = 1.234;

	component->setValue("Height", height);
	component->setValue("Radius", radius);

	YAML::Node node(YAML::convert<SurgSim::Framework::Component>::encode(*component));

	auto decoded = std::dynamic_pointer_cast<SurgSim::Graphics::OsgCapsuleRepresentation>(
					   node.as<std::shared_ptr<SurgSim::Framework::Component>>());

	EXPECT_NE(nullptr, decoded);
	EXPECT_DOUBLE_EQ(radius, decoded->getValue<double>("Radius"));
	EXPECT_DOUBLE_EQ(height, decoded->getValue<double>("Height"));
}

TEST(OsgCapsuleRepresentationTests, RadiusTest)
{
	std::shared_ptr<CapsuleRepresentation> capsuleRepresentation =
		std::make_shared<OsgCapsuleRepresentation>("test name");

	double radius = 1.0;
	capsuleRepresentation->setRadius(radius);
	EXPECT_NEAR(capsuleRepresentation->getRadius(), radius, epsilon);
}

TEST(OsgCapsuleRepresentationTests, HeightTest)
{
	std::shared_ptr<CapsuleRepresentation> capsuleRepresentation =
		std::make_shared<OsgCapsuleRepresentation>("test name");

	double height = 1.0;
	capsuleRepresentation->setHeight(height);
	EXPECT_NEAR(capsuleRepresentation->getHeight(), height, epsilon);
}

TEST(OsgCapsuleRepresentationTests, SizeTest)
{
	std::shared_ptr<CapsuleRepresentation> capsuleRepresentation =
		std::make_shared<OsgCapsuleRepresentation>("test name");

	double radius = 1.0, height = 1.0;
	double retrievedRadius, retrievedHeight;

	capsuleRepresentation->setSize(radius, height);
	capsuleRepresentation->getSize(&retrievedRadius, &retrievedHeight);

	EXPECT_NEAR(radius, retrievedRadius, epsilon);
	EXPECT_NEAR(height, retrievedHeight, epsilon);
}

TEST(OsgCapsuleRepresentationTests, SizeVectordTest)
{
	std::shared_ptr<CapsuleRepresentation> capsuleRepresentation =
		std::make_shared<OsgCapsuleRepresentation>("test name");

	Vector2d size(1.0, 1.0);
	capsuleRepresentation->setSize(size);
	EXPECT_TRUE(capsuleRepresentation->getSize().isApprox(size));
}

};  // namespace Graphics

};  // namespace SurgSim
