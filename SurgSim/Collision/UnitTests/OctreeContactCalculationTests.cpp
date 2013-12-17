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
#include <memory>

#include "SurgSim/DataStructures/OctreeNode.h"
#include "SurgSim/Collision/DcdCollision.h"
#include "SurgSim/Collision/OctreeDcdContact.h"
#include "SurgSim/Math/Quaternion.h"
#include "SurgSim/Math/RigidTransform.h"
#include "SurgSim/Math/Shape.h"
#include "SurgSim/Math/Shapes.h"

using SurgSim::DataStructures::OctreeNode;
using SurgSim::Math::CapsuleShape;
using SurgSim::Math::DoubleSidedPlaneShape;
using SurgSim::Math::makeRigidTransform;
using SurgSim::Math::makeRotationQuaternion;
using SurgSim::Math::OctreeShape;
using SurgSim::Math::OctreePath;
using SurgSim::Math::PlaneShape;
using SurgSim::Math::Shape;
using SurgSim::Math::SphereShape;

namespace SurgSim
{
namespace Collision
{

struct OctreeData
{
	std::string name;
};

std::list<std::shared_ptr<Contact>> doCollision(std::shared_ptr<Shape> octree,
		const SurgSim::Math::Quaterniond& octreeQuat,
		const SurgSim::Math::Vector3d& octreeTrans,
		std::shared_ptr<Shape> shape,
		const SurgSim::Math::Quaterniond& shapeQuat,
		const SurgSim::Math::Vector3d& shapeTrans,
		ContactCalculation& calculator)
{
	std::shared_ptr<Representation> octreeRep = std::make_shared<ShapeCollisionRepresentation>(
		"Collision Octree 0", octree, makeRigidTransform(octreeQuat, octreeTrans));
	std::shared_ptr<Representation> shapeRep = std::make_shared<ShapeCollisionRepresentation>(
		"Collision Capsule 0", shape, makeRigidTransform(shapeQuat, shapeTrans));

	// Perform collision detection.
	std::shared_ptr<CollisionPair> pair = std::make_shared<CollisionPair>(octreeRep, shapeRep);
	calculator.calculateContact(pair);
	return pair->getContacts();
}

bool nodeInContacts(const std::string& name, const std::list<std::shared_ptr<Contact>>& contacts,
		std::shared_ptr<OctreeShape<OctreeData>> octree)
{
	for(auto contact = contacts.cbegin(); contact != contacts.cend(); ++contact)
	{
		OctreePath path = (*contact)->penetrationPoints.first.octreeNodePath.getValue();
		std::shared_ptr<OctreeNode<OctreeData>> node = octree->getNode(path);
		if (node->data.name == name)
		{
			return true;
		}
	}
	return false;
}

std::shared_ptr<OctreeShape<OctreeData>> buildTestOctree()
{
	// To keep things simple, create an 4 level octree with leaf nodes of size 1x1x1.
	// As a result, the root bounding box is 2^4 x 2^4 x 2^4, or 16x16x16
	Eigen::AlignedBox<double, 3> boundingBox;
	const int numLevels = 4;
	boundingBox.min() = Vector3d::Zero();
	boundingBox.max() = Vector3d::Ones() * pow(2.0, numLevels);
	std::shared_ptr<OctreeNode<OctreeData> > rootNode = std::make_shared<OctreeNode<OctreeData> >(boundingBox);

	OctreeData data;
	data.name = "center";
	rootNode->addData(Vector3d( 8.5,  8.5,  8.5), data, numLevels);

	data.name = "corner0";
	rootNode->addData(Vector3d( 0.5,  0.5,  0.5), data, numLevels);

	data.name = "corner1";
	rootNode->addData(Vector3d(15.5,  0.5,  0.5), data, numLevels);

	data.name = "corner2";
	rootNode->addData(Vector3d( 0.5, 15.5,  0.5), data, numLevels);

	data.name = "corner3";
	rootNode->addData(Vector3d(15.5, 15.5,  0.5), data, numLevels);

	data.name = "corner4";
	rootNode->addData(Vector3d( 0.5,  0.5, 15.5), data, numLevels);

	data.name = "corner5";
	rootNode->addData(Vector3d(15.5,  0.5, 15.5), data, numLevels);

	data.name = "corner6";
	rootNode->addData(Vector3d( 0.5, 15.5, 15.5), data, numLevels);

	data.name = "corner7";
	rootNode->addData(Vector3d(15.5, 15.5, 15.5), data, numLevels);

	return std::make_shared<OctreeShape<OctreeData>>(rootNode);
}

TEST(OctreeContactCalculationTests, Capsule)
{
	std::shared_ptr<OctreeShape<OctreeData>> octree = buildTestOctree();
	std::shared_ptr<Shape> capsule = std::make_shared<CapsuleShape>(16.0, 1.0);
	OctreeDcdContact<OctreeData> calculator(std::make_shared<BoxCapsuleDcdContact>());

	std::list<std::shared_ptr<Contact>> contacts;
	{
		SCOPED_TRACE("No intersection, capsule outside octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(5.0, 0.0, 0.0),
				capsule,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				calculator);
		EXPECT_EQ(0, contacts.size());
	}

	{
		SCOPED_TRACE("No intersection, capsule inside octree, but not contacting active nodes");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				capsule,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(5.0, 3.0, 0.0),
				calculator);
		EXPECT_EQ(0, contacts.size());
	}

	{
		SCOPED_TRACE("Intersection, capsule is in the center of the octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				capsule,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(8.0, 8.0, 8.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("center", contacts, octree));
	}

	{
		SCOPED_TRACE("Intersection, capsule intersection 2 nodes");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				capsule,
				makeRotationQuaternion(M_PI_2, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(8.0, 0.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner1", contacts, octree));
	}

	{
		SCOPED_TRACE("Intersection, octree rotated");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(M_PI_4, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				capsule,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 12.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner3", contacts, octree));
	}
}

TEST(OctreeContactCalculationTests, Plane)
{
	std::shared_ptr<OctreeShape<OctreeData>> octree = buildTestOctree();
	std::shared_ptr<Shape> plane = std::make_shared<PlaneShape>();
	OctreeDcdContact<OctreeData> calculator(std::make_shared<BoxPlaneDcdContact>());

	std::list<std::shared_ptr<Contact>> contacts;
	{
		SCOPED_TRACE("No intersection, plane outside octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 5.0, 0.0),
				plane,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				calculator);
		EXPECT_EQ(0, contacts.size());
	}

	{
		SCOPED_TRACE("Intersection, plane inside octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				plane,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 2.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner1", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner4", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner5", contacts, octree));
	}

	{
		SCOPED_TRACE("Intersection, rotated octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(M_PI_4, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				plane,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 2.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner4", contacts, octree));
	}

	{
		SCOPED_TRACE("Intersection, rotated plane");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				plane,
				makeRotationQuaternion(M_PI_4, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 8.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner1", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner3", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner4", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner5", contacts, octree));
	}
}

TEST(OctreeContactCalculationTests, DoubleSidedPlane)
{
	std::shared_ptr<OctreeShape<OctreeData>> octree = buildTestOctree();
	std::shared_ptr<Shape> plane = std::make_shared<DoubleSidedPlaneShape>();
	OctreeDcdContact<OctreeData> calculator(std::make_shared<BoxDoubleSidedPlaneDcdContact>());

	std::list<std::shared_ptr<Contact>> contacts;
	{
		SCOPED_TRACE("No intersection, plane outside octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 5.0, 0.0),
				plane,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				calculator);
		EXPECT_EQ(0, contacts.size());
	}

	{
		SCOPED_TRACE("Intersection, plane along bottom face");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				plane,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner1", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner4", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner5", contacts, octree));
	}

	{
		SCOPED_TRACE("Intersection, plane along diagnol");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				plane,
				makeRotationQuaternion(M_PI_4, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("center", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner3", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner4", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner7", contacts, octree));
	}
}

TEST(OctreeContactCalculationTests, Sphere)
{
	std::shared_ptr<OctreeShape<OctreeData>> octree = buildTestOctree();
	std::shared_ptr<Shape> sphere = std::make_shared<SphereShape>(9);
	OctreeDcdContact<OctreeData> calculator(std::make_shared<BoxSphereDcdContact>());

	std::list<std::shared_ptr<Contact>> contacts;
	{
		SCOPED_TRACE("No intersection, sphere outside octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 10.0, 0.0),
				sphere,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				calculator);
		EXPECT_EQ(0, contacts.size());
	}

	{
		SCOPED_TRACE("Intersection, sphere at center of octree");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				sphere,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(8.0, 8.0, 8.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("center", contacts, octree));
	}

	{
		SCOPED_TRACE("Intersection, sphere center on box face");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 0.0),
				sphere,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(8.0, 8.0, 0.0),
				calculator);
		EXPECT_TRUE(nodeInContacts("corner0", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner1", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner2", contacts, octree));
		EXPECT_TRUE(nodeInContacts("corner3", contacts, octree));
	}


	{
		SCOPED_TRACE("No intersection, sphere inside octree, but not contacting active nodes");
		contacts = doCollision(
				octree,
				makeRotationQuaternion(0.0, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(0.0, 0.0, 4.0),
				sphere,
				makeRotationQuaternion(M_PI_4, Vector3d(0.0, 0.0, 1.0)),
				Vector3d(8.0, 8.0, 0.0),
				calculator);
		EXPECT_EQ(0, contacts.size());
	}
}


}; // namespace Collision
}; // namespace SurgSim
