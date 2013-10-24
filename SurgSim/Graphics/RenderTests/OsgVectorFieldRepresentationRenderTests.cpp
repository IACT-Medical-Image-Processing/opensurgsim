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

/// Render Tests for the OsgVectorFieldRepresentation class.

#include <SurgSim/Graphics/RenderTests/RenderTest.h>
#include <SurgSim/Graphics/VectorFieldRepresentation.h>
#include <SurgSim/Graphics/OsgVectorFieldRepresentation.h>
#include <SurgSim/Math/Quaternion.h>
#include <SurgSim/Math/Vector.h>
#include <SurgSim/Math/RigidTransform.h>
#include <SurgSim/Testing/MathUtilities.h>

using SurgSim::DataStructures::Vertex;
using SurgSim::DataStructures::Vertices;
using SurgSim::Graphics::OsgVectorFieldRepresentation;
using SurgSim::Graphics::VectorFieldRepresentation;
using SurgSim::Math::Vector3d;
using SurgSim::Math::Vector4d;
using SurgSim::Math::Quaterniond;
using SurgSim::Math::makeRigidTransform;
using SurgSim::Testing::interpolate;

struct OsgVectorFieldRepresentationRenderTests : public SurgSim::Graphics::RenderTest
{
protected:
	std::vector<Vector3d> makeVertices()
	{
		std::vector<Vector3d> points;
		points.emplace_back(Vector3d(1.0, 0.0, 0.0));
		points.emplace_back(Vector3d(0.0, 1.0, 0.0));

		points.emplace_back(Vector3d(-1.0, 0.0, 0.0));
		points.emplace_back(Vector3d(0.0, -1.0, 0.0));

		points.emplace_back(Vector3d(2.0, 0.0, 0.0));
		points.emplace_back(Vector3d(0.0, 2.0, 0.0));

		points.emplace_back(Vector3d(-2.0, 0.0, 0.0));
		points.emplace_back(Vector3d(0.0, -2.0, 0.0));
		return points;
	}

	std::shared_ptr<VectorFieldRepresentation<void>> makeVectorRepresentation(const std::vector<Vector3d>& points)
	{
		auto vectorRepresentation =
			std::make_shared<OsgVectorFieldRepresentation<void>>("vectorRepresentation representation");

		auto vertices = std::make_shared<Vertices< Vertex<void> > >();
		for (auto it = std::begin(points); it != std::end(points); ++it)
		{
			auto vector = Vertex<void>(*it);
			vertices->addVertex(Vertices< Vertex<void> >::VertexType((*it), vector));
		}

		vectorRepresentation->setVertices(vertices);
		vectorRepresentation->setInitialPose(makeRigidTransform(Quaterniond::Identity(), Vector3d(0.0, 0.0, -8.0)));

		viewElement->addComponent(vectorRepresentation);

		return vectorRepresentation;
	}

	std::shared_ptr<VectorFieldRepresentation<Vector4d>>
		makeVectorRepresentationWithColors(const std::vector<Vector3d>& points)
	{
		auto vectorRepresentation =
			std::make_shared<OsgVectorFieldRepresentation<Vector4d>>("vectorRepresentation representation");

		auto vertices = std::make_shared<Vertices< Vertex<Vector4d> > >();

		// Used to generate different colors for vectors
		unsigned index = 0;

		for (auto it = std::begin(points); it != std::end(points); ++it)
		{
			auto color = Vector4d(1.0, 0.0, 0.0, 0.0);
			color[(index++)%4] = 1.0;
			auto vector = Vertex<Vector4d>(*it, color);
			vertices->addVertex(Vertices< Vertex<Vector4d> >::VertexType((*it), vector));
		}

		vectorRepresentation->setVertices(vertices);
		vectorRepresentation->setInitialPose(makeRigidTransform(Quaterniond::Identity(), Vector3d(0.0, 0.0, -8.0)));

		viewElement->addComponent(vectorRepresentation);

		return vectorRepresentation;
	}
};

TEST_F(OsgVectorFieldRepresentationRenderTests, LineWidth)
{
	std::shared_ptr<VectorFieldRepresentation<void>> vectorRepresentation = makeVectorRepresentation(makeVertices());

	runtime->start();
	EXPECT_TRUE(graphicsManager->isInitialized());
	EXPECT_TRUE(viewElement->isInitialized());
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	int numSteps = 100;
	double startWidth = 0.0;
	double endWidth = 10.0;

	for (int i = 0; i < numSteps; ++i)
	{
		/// Calculate t in [0.0, 1.0]
		double t = static_cast<double>(i) / numSteps;
		vectorRepresentation->setLineWidth(interpolate(startWidth, endWidth, t));
		boost::this_thread::sleep(boost::posix_time::milliseconds(500 / numSteps));
	}
}

TEST_F(OsgVectorFieldRepresentationRenderTests, ColorTest)
{
	std::shared_ptr<VectorFieldRepresentation<Vector4d>> vectorRepresentation =
		makeVectorRepresentationWithColors(makeVertices());

	runtime->start();
	EXPECT_TRUE(graphicsManager->isInitialized());
	EXPECT_TRUE(viewElement->isInitialized());
	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
}
