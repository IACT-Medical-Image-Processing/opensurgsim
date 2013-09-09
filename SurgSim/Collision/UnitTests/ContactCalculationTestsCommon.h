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

#ifndef SURGSIM_COLLISION_UNITTESTS_CONTACTCALCULATIONTESTSCOMMON_H
#define SURGSIM_COLLISION_UNITTESTS_CONTACTCALCULATIONTESTSCOMMON_H

#include <gtest/gtest.h>
#include <memory>

#include <SurgSim/Collision/UnitTests/RepresentationUtilities.h>
#include <SurgSim/Collision/UnitTests/MockCollisionRepresentation.h>

#include <SurgSim/Math/Vector.h>
#include <SurgSim/Math/Quaternion.h>
#include <SurgSim/Math/RigidTransform.h>

#include <SurgSim/Physics/RigidShape.h>
#include <SurgSim/Physics/BoxShape.h>
#include <SurgSim/Physics/PlaneShape.h>
#include <SurgSim/Physics/DoubleSidedPlaneShape.h>

#include <SurgSim/Collision/CollisionRepresentation.h>
#include <SurgSim/Collision/ContactCalculation.h>
#include <SurgSim/Collision/CollisionPair.h>

#include <SurgSim/Math/Geometry.h>
#include <SurgSim/Collision/RigidShapeCollisionRepresentation.h>

using SurgSim::Math::Vector3d;
using SurgSim::Math::Quaterniond;
using SurgSim::Math::RigidTransform3d;

using SurgSim::Physics::BoxShape;
using SurgSim::Physics::PlaneShape;
using SurgSim::Physics::DoubleSidedPlaneShape;

namespace SurgSim
{
namespace Collision
{

/// Function that compares two 3d vectors and asserts that they are equal.
/// The tolerance for the numerical values is SurgSim::Math::Geometry::DistanceEpsilon.
/// \param left First vector.
/// \param right Second vector.
::testing::AssertionResult eigenEqual(const Vector3d& left, const Vector3d& right);

/// Function that checks if a given contact contains the given normal, depth and
/// penetration points.
/// \param contact The contact object that is being checked.
/// \param expectedDepth The expected depth.
/// \param expectedNormal The expected normal.
/// \param expectedPenetrationPointFirst The expected first penetration point.
/// \param expectedPenetrationPointSecond The expected second penetration point.
void checkContactInfo(std::shared_ptr<Contact> contact, double expectedDepth,
					  Vector3d &expectedNormal, Vector3d &expectedPenetrationPointFirst,
					  Vector3d &expectedPenetrationPointSecond);

/// Function that checks if a given contact is present in the list of given contacts.
/// \param expected The expected contact.
/// \param contactsList The list of contacts.
::testing::AssertionResult isContactPresentInList(std::shared_ptr<Contact> expected,
												  const std::list<std::shared_ptr<Contact>>& contactsList);

/// Function that checks if two given list of contacts are the same.
/// \param expected The expected contact lists.
/// \param contactsList The list of given contact list.
void contactsInfoEqualityTest(const std::list<std::shared_ptr<Contact>>& expectedContacts,
							  const std::list<std::shared_ptr<Contact>>& calculatedContacts);

/// Function that generates (no collision detection performed) the contact information between a box and a plane,
/// given the box vertex indices that are known to be in contact.
/// \param expectedContacts The list where the generated contacts are added.
/// \param expectedNumberOfContacts The number of contacts to be generated.
/// \param expectedBoxIndicesInContacts The vertex indices of the box which are in collision with the plane.
/// \param box The box shape.
/// \param boxTrans The box translation in global co-ordinate system.
/// \param boxQuat The box orientation in global co-ordinate system.
/// \param plane The plane shape.
/// \param planeTrans The plane translation in global co-ordinate system.
/// \param planeQuat The plane orientation in global co-ordinate system.
void generateBoxPlaneContact(std::list<std::shared_ptr<Contact>>& expectedContacts,
							 const int expectedNumberOfContacts,
							 const int* expectedBoxIndicesInContacts,
							 const std::shared_ptr<BoxShape> box,
							 const Vector3d& boxTrans, const Quaterniond& boxQuat,
							 const std::shared_ptr<PlaneShape> plane,
							 const Vector3d& planeTrans, const Quaterniond& planeQuat);

/// Function that generates (no collision detection performed) the contact information between a box and
/// a double-sided plane, given the box vertex indices that are known to be in contact.
/// \param expectedContacts The list where the generated contacts are added.
/// \param expectedNumberOfContacts The number of contacts to be generated.
/// \param expectedBoxIndicesInContacts The vertex indices of the box which are in collision
///										with the double-sided plane.
/// \param box The box shape.
/// \param boxTrans The box translation in global co-ordinate system.
/// \param boxQuat The box orientation in global co-ordinate system.
/// \param plane The double-sided plane shape.
/// \param planeTrans The double-sided plane translation in global co-ordinate system.
/// \param planeQuat The double-sided plane orientation in global co-ordinate system.
/// \param collisionNormalIsPlaneNormal Flag to represent if the box is in collision
void generateBoxDoubleSidedPlaneContact(std::list<std::shared_ptr<Contact>>& expectedContacts,
										const int expectedNumberOfContacts,
										const int* expectedBoxIndicesInContacts,
										const std::shared_ptr<BoxShape> box,
										const Vector3d& boxTrans, const Quaterniond& boxQuat,
										const std::shared_ptr<DoubleSidedPlaneShape> plane,
										const Vector3d& planeTrans, const Quaterniond& planeQuat,
										const bool collisionNormalIsPlaneNormal);

}; // namespace Collision
}; // namespace SurgSim

#endif