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
/// Tests for the OsgPlaneRepresentation class.

#include "SurgSim/Graphics/UnitTests/MockOsgObjects.h"

#include "SurgSim/Framework/BasicSceneElement.h"
#include "SurgSim/Graphics/OsgMaterial.h"
#include "SurgSim/Graphics/OsgPlaneRepresentation.h"
#include "SurgSim/Math/Quaternion.h"
#include "SurgSim/Math/Vector.h"
#include "SurgSim/Framework/FrameworkConvert.h"

#include <gtest/gtest.h>

#include <osg/Geode>

#include <random>

using SurgSim::Framework::BasicSceneElement;
using SurgSim::Math::Quaterniond;
using SurgSim::Math::RigidTransform3d;
using SurgSim::Math::Vector3d;
using SurgSim::Math::makeRigidTransform;
using SurgSim::Math::makeRotationQuaternion;

namespace SurgSim
{

namespace Graphics
{

TEST(OsgPlaneRepresentationTests, InitTest)
{
	ASSERT_NO_THROW({std::shared_ptr<Representation> representation =
						 std::make_shared<OsgPlaneRepresentation>("test name");
					});

	std::shared_ptr<Representation> representation = std::make_shared<OsgPlaneRepresentation>("test name");
	EXPECT_EQ("test name", representation->getName());
}

TEST(OsgPlaneRepresentationTests, AccessibleTest)
{
	std::shared_ptr<SurgSim::Framework::Component> component;
	ASSERT_NO_THROW(component = SurgSim::Framework::Component::getFactory().create(
									"SurgSim::Graphics::OsgPlaneRepresentation",
									"plane"));

	EXPECT_EQ("SurgSim::Graphics::OsgPlaneRepresentation", component->getClassName());

	YAML::Node node(YAML::convert<SurgSim::Framework::Component>::encode(*component));

	auto decoded = std::dynamic_pointer_cast<SurgSim::Graphics::OsgPlaneRepresentation>(
					   node.as<std::shared_ptr<SurgSim::Framework::Component>>());

	EXPECT_NE(nullptr, decoded);
}

};  // namespace Graphics

};  // namespace SurgSim
