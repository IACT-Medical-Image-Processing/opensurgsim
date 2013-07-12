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

#include <memory>

#include <gtest/gtest.h>
#include <SurgSim/Physics/Constraint.h>
#include <SurgSim/Physics/ConstraintData.h>
#include <SurgSim/Physics/ContactConstraintData.h>
#include <SurgSim/Physics/FixedRepresentation.h>
#include <SurgSim/Physics/FixedRepresentationContact.h>
#include <SurgSim/Physics/FixedRepresentationLocalization.h>
#include <SurgSim/Physics/MlcpPhysicsProblem.h>
#include <SurgSim/Physics/RigidRepresentation.h>
#include <SurgSim/Physics/RigidRepresentationContact.h>
#include <SurgSim/Physics/RigidRepresentationLocalization.h>
#include <SurgSim/Physics/RigidRepresentationParameters.h>
#include <SurgSim/Physics/SphereShape.h>

#include <SurgSim/Math/Vector.h>
#include <SurgSim/Math/Quaternion.h>
#include <SurgSim/Math/RigidTransform.h>

namespace
{
	const double epsilon = 1e-10;
};

namespace SurgSim
{
namespace Physics
{

class ConstraintTests : public ::testing::Test
{
protected:
	/// Fixed plane pose
	SurgSim::Math::RigidTransform3d m_poseFixed;
	/// Rigid sphere pose
	SurgSim::Math::RigidTransform3d m_poseRigid;
	/// Contact normal direction
	SurgSim::Math::Vector3d m_n;
	/// Distance to origin of the contact plane equation
	double m_d;
	/// Sphere radius
	double m_radius;
	/// Simulation time step
	double m_dt;
	/// Mlcp index of the sphere representation
	unsigned int m_indexSphereRepresentation;
	/// Mlcp index of the plane representation
	unsigned int m_indexPlaneRepresentation;
	/// Mlcp index of the constraint (frictionless contact)
	unsigned int m_indexConstraint;
	/// Contact location on the plane (point on the plane with the most penetration)
	SurgSim::Math::Vector3d m_contactPositionPlane;
	/// Contact location on the sphere (point on the sphere with the most penetration)
	SurgSim::Math::Vector3d m_contactPositionSphere;

	/// Rigid sphere
	std::shared_ptr<RigidRepresentation> m_rigid;
	/// Fixed plane
	std::shared_ptr<FixedRepresentation> m_fixed;

	/// Mlcp
	MlcpPhysicsProblem m_mlcpPhysicsProblem;
	/// Constraint
	std::shared_ptr<Constraint> m_constraint;
	/// Constraint data: frictionless contact
	std::shared_ptr<ContactConstraintData> m_constraintData;
	/// Localization on the fixed plane
	std::shared_ptr<Localization> m_locFixedPlane;
	/// Localization on the rigid sphere
	std::shared_ptr<Localization> m_locRigidSphere;
	/// Constraint implementation for the fixed plane
	std::shared_ptr<ConstraintImplementation> m_implementationFixedPlane;
	/// Constraint implementation for the rigid sphere
	std::shared_ptr<ConstraintImplementation> m_implementationRigidSphere;

	/// Total number of degrees of freedom in the system (plane + sphere)
	unsigned int m_numDof;
	/// Total number of atomic constraint in the system (1 for a frictionless contact)
	unsigned int m_numConstraint;

	/// Setup the test case by creating all object
	void SetUp()
	{
		m_d = 0.0;
		m_radius = 0.01;
		m_dt = 1e-3;
		m_numDof = 0;
		m_indexSphereRepresentation = 0;
		m_indexConstraint = 0;
		m_numConstraint = 1;
		m_contactPositionPlane.setZero();
		m_contactPositionSphere.setZero();
		m_contactPositionSphere[1] = -m_radius;

		m_poseFixed.setIdentity();
		m_poseRigid.setIdentity();

		m_rigid = std::make_shared<RigidRepresentation>("Rigid");
		m_rigid->setIsActive(true);
		m_rigid->setIsGravityEnabled(false);
		m_rigid->setInitialPose(m_poseRigid);
		{
			RigidRepresentationParameters param;
			param.setDensity(1000.0);
			std::shared_ptr<SphereShape> shape = std::make_shared<SphereShape>(m_radius);
			param.setShapeUsedForMassInertia(shape);
			m_rigid->setInitialParameters(param);
		}
		m_numDof += m_rigid->getNumDof();

		m_indexPlaneRepresentation = m_indexSphereRepresentation + m_rigid->getNumDof();
		m_fixed = std::make_shared<FixedRepresentation>("Fixed");
		m_fixed->setIsActive(true);
		m_fixed->setIsGravityEnabled(false);
		m_fixed->setInitialPose(m_poseFixed);
		m_numDof += m_fixed->getNumDof();

		std::shared_ptr<FixedRepresentationLocalization> locFixedPlane;
		locFixedPlane = std::make_shared<FixedRepresentationLocalization>(m_fixed);
		m_locFixedPlane = locFixedPlane;
		std::shared_ptr<RigidRepresentationLocalization> locRigidSphere;
		locRigidSphere = std::make_shared<RigidRepresentationLocalization>(m_rigid);
		m_locRigidSphere = locRigidSphere;

		locFixedPlane->setLocalPosition(m_contactPositionPlane);
		locRigidSphere->setLocalPosition(m_contactPositionSphere);
	
		m_implementationFixedPlane = std::make_shared<FixedRepresentationContact>();
		m_implementationRigidSphere = std::make_shared<RigidRepresentationContact>();

		m_constraintData = std::make_shared<ContactConstraintData>();

		clearMlcpPhysicsProblem(m_numDof, m_numConstraint);
	}

	/// Allocate and clear the Mlcp
	/// \param numDof The number of degrees of freedom in the system
	/// \param numConstraint The number of atomic constraints in the system
	void clearMlcpPhysicsProblem(int numDof, int numConstraint)
	{
		// Resize and zero all Eigen types
		m_mlcpPhysicsProblem.A.resize(numConstraint, numConstraint);
		m_mlcpPhysicsProblem.A.setZero();
		m_mlcpPhysicsProblem.b.resize(numConstraint);
		m_mlcpPhysicsProblem.b.setZero();
		m_mlcpPhysicsProblem.mu.resize(numConstraint);
		m_mlcpPhysicsProblem.mu.setZero();
		m_mlcpPhysicsProblem.CHt.resize(numDof, numConstraint);
		m_mlcpPhysicsProblem.CHt.setZero();
		m_mlcpPhysicsProblem.H.resize(numConstraint, numDof);
		m_mlcpPhysicsProblem.H.setZero();

		// Empty all std::vector types
		m_mlcpPhysicsProblem.constraintTypes.clear();
	}
};

TEST_F (ConstraintTests, TestDefaultEmptyConstraint)
{
	std::shared_ptr<ConstraintImplementation> imp0 = std::make_shared<FixedRepresentationContact>();
	std::shared_ptr<ConstraintImplementation> imp1 = std::make_shared<FixedRepresentationContact>();
	m_constraint = std::make_shared<Constraint>(imp0, imp1);

	EXPECT_EQ(nullptr, m_constraint->getData());
	EXPECT_EQ(imp0, m_constraint->getImplementations().first);
	EXPECT_EQ(imp1, m_constraint->getImplementations().second);
}

TEST_F (ConstraintTests, TestGetNumDof)
{
	m_constraint = std::make_shared<Constraint>(m_implementationFixedPlane, m_implementationRigidSphere);

	// 1 for a frictionless contact
	EXPECT_EQ(1u, m_constraint->getNumDof());

	// 1 for a frictionless contact between 2 fixed representations
	{
		std::shared_ptr<Localization> loc1 = std::make_shared<FixedRepresentationLocalization>();
		std::shared_ptr<Localization> loc2 = std::make_shared<RigidRepresentationLocalization>();
		std::shared_ptr<FixedRepresentationContact> implementation1;
		std::shared_ptr<FixedRepresentationContact> implementation2;
		implementation1 = std::make_shared<FixedRepresentationContact>();
		implementation2 = std::make_shared<FixedRepresentationContact>();
		m_constraint->setImplementations(implementation1, implementation2);
		m_constraint->setLocalizations(loc1, loc2);
		EXPECT_EQ(1u, m_constraint->getNumDof());
		EXPECT_EQ(implementation1, m_constraint->getImplementations().first);
		EXPECT_EQ(implementation2, m_constraint->getImplementations().second);
	}

	// 1 for a frictionless contact between 1 fixed representation and 1 rigid representation
	{
		std::shared_ptr<Localization> loc1 = std::make_shared<FixedRepresentationLocalization>();
		std::shared_ptr<Localization> loc2 = std::make_shared<RigidRepresentationLocalization>();
		std::shared_ptr<ConstraintImplementation> implementation1 = std::make_shared<FixedRepresentationContact>();
		std::shared_ptr<ConstraintImplementation> implementation2 = std::make_shared<RigidRepresentationContact>();
		m_constraint->setImplementations(implementation1, implementation2);
		m_constraint->setLocalizations(loc1, loc2);
		EXPECT_EQ(1u, m_constraint->getNumDof());
		EXPECT_EQ(implementation1, m_constraint->getImplementations().first);
		EXPECT_EQ(implementation2, m_constraint->getImplementations().second);
	}
}

TEST_F (ConstraintTests, TestSetGetData)
{
	m_constraint = std::make_shared<Constraint>(m_implementationFixedPlane, m_implementationRigidSphere);

	EXPECT_EQ(nullptr, m_constraint->getData());
	m_constraint->setData(m_constraintData);
	EXPECT_NE(nullptr, m_constraint->getData());
	EXPECT_EQ(m_constraintData, m_constraint->getData());
	EXPECT_EQ(m_implementationFixedPlane, m_constraint->getImplementations().first);
	EXPECT_EQ(m_implementationRigidSphere, m_constraint->getImplementations().second);

	m_constraint->setData(nullptr);
	EXPECT_EQ(nullptr, m_constraint->getData());
}

// Test case: Rigid sphere at (0 0 0) with radius 0.01 colliding with Fixed plane Y=0
// Contact location on the rigid sphere is (0 -0.01 0)
// Contact location on the fixed plane is (0 0 0)
// Constraint: (Sphere - Plane).n >= 0 with n=(0 1 0) The normal should be the contact normal on the 2nd object
TEST_F (ConstraintTests, TestBuildMlcpSpherePlane)
{
	m_constraint = std::make_shared<Constraint>(m_implementationRigidSphere, m_implementationFixedPlane);
	m_constraint->setLocalizations(m_locRigidSphere, m_locFixedPlane);
	m_n.setZero();
	m_n[1] = 1.0;
	m_constraintData->setPlaneEquation(m_n, m_d);
	m_constraint->setData(m_constraintData);

	// Simulate 1 time step...to make sure all representation have a valid compliance matrix...
	{
		m_fixed->beforeUpdate(m_dt);
		m_rigid->beforeUpdate(m_dt);

		m_fixed->update(m_dt);
		m_rigid->update(m_dt);

		m_fixed->afterUpdate(m_dt);
		m_rigid->afterUpdate(m_dt);
	}

	// Fill up the Mlcp
	m_constraint->build(m_dt, &m_mlcpPhysicsProblem, m_indexSphereRepresentation, m_indexPlaneRepresentation,
		m_indexConstraint);

	// Violation b should be exactly -radius (the sphere center is on the plane)
	// This should not depend on the ordering of the object...the violation remains the same no matter what
	EXPECT_NEAR(-m_radius, m_mlcpPhysicsProblem.b[0], epsilon);

	// Constraint H should be
	// H = dt.[nx  ny  nz  nz.GPy-ny.GPz  nx.GPz-nz.GPx  ny.GPx-nx.GPy]
	// The rigid sphere being the 1st representation in the pair, it has the positive sign in the constraint !
	double sign = 1.0;
	SurgSim::Math::Vector3d n_GP = m_n.cross(Vector3d(0.0, -m_radius, 0.0));

	EXPECT_NEAR(sign * m_dt * m_n[0] , m_mlcpPhysicsProblem.H(0, 0), epsilon);
	EXPECT_NEAR(sign * m_dt * m_n[1] , m_mlcpPhysicsProblem.H(0, 1), epsilon);
	EXPECT_NEAR(sign * m_dt * m_n[2] , m_mlcpPhysicsProblem.H(0, 2), epsilon);
	EXPECT_NEAR(sign * m_dt * n_GP[0], m_mlcpPhysicsProblem.H(0, 3), epsilon);
	EXPECT_NEAR(sign * m_dt * n_GP[1], m_mlcpPhysicsProblem.H(0, 4), epsilon);
	EXPECT_NEAR(sign * m_dt * n_GP[2], m_mlcpPhysicsProblem.H(0, 5), epsilon);

	// ConstraintTypes should contain 1 entry SurgSim::Math::MLCP_UNILATERAL_3D_FRICTIONLESS_CONSTRAINT
	ASSERT_EQ(1u, m_mlcpPhysicsProblem.constraintTypes.size());
	EXPECT_EQ(SurgSim::Math::MLCP_UNILATERAL_3D_FRICTIONLESS_CONSTRAINT, m_mlcpPhysicsProblem.constraintTypes[0]);
}

// Test case: Rigid sphere at (0 0 0) with radius 0.01 colliding with Fixed plane Y=0
// Contact location on the rigid sphere is (0 -0.01 0)
// Contact location on the fixed plane is (0 0 0)
// Constraint: (Plane - Sphere).n >= 0 with n=(0 -1 0) The normal should be the contact normal on the 2nd object
TEST_F (ConstraintTests, TestBuildMlcpPlaneSphere)
{
	m_constraint = std::make_shared<Constraint>(m_implementationFixedPlane, m_implementationRigidSphere);
	m_constraint->setLocalizations(m_locFixedPlane,m_locRigidSphere);
	m_n.setZero();
	m_n[1] = -1.0;
	m_constraintData->setPlaneEquation(m_n, m_d);
	m_constraint->setData(m_constraintData);

	// Simulate 1 time step...to make sure all representation have a valid compliance matrix...
	{
		m_fixed->beforeUpdate(m_dt);
		m_rigid->beforeUpdate(m_dt);

		m_fixed->update(m_dt);
		m_rigid->update(m_dt);

		m_fixed->afterUpdate(m_dt);
		m_rigid->afterUpdate(m_dt);
	}

	// Fill up the Mlcp
	m_constraint->build(m_dt, &m_mlcpPhysicsProblem, m_indexPlaneRepresentation, m_indexSphereRepresentation,
		m_indexConstraint);

	// Violation b should be exactly -radius (the sphere center is on the plane)
	// This should not depend on the ordering of the object...the violation remains the same no matter what
	EXPECT_NEAR(-m_radius, m_mlcpPhysicsProblem.b[0], epsilon);

	// Constraint H should be
	// H = dt.[nx  ny  nz  nz.GPy-ny.GPz  nx.GPz-nz.GPx  ny.GPx-nx.GPy]
	// The rigid sphere being the 2nd representation in the pair, it has the negative sign in the constraint !
	double sign = -1.0;
	SurgSim::Math::Vector3d n_GP = m_n.cross(Vector3d(0.0, -m_radius, 0.0));

	EXPECT_NEAR(sign * m_dt * m_n[0] , m_mlcpPhysicsProblem.H(0, 0), epsilon);
	EXPECT_NEAR(sign * m_dt * m_n[1] , m_mlcpPhysicsProblem.H(0, 1), epsilon);
	EXPECT_NEAR(sign * m_dt * m_n[2] , m_mlcpPhysicsProblem.H(0, 2), epsilon);
	EXPECT_NEAR(sign * m_dt * n_GP[0], m_mlcpPhysicsProblem.H(0, 3), epsilon);
	EXPECT_NEAR(sign * m_dt * n_GP[1], m_mlcpPhysicsProblem.H(0, 4), epsilon);
	EXPECT_NEAR(sign * m_dt * n_GP[2], m_mlcpPhysicsProblem.H(0, 5), epsilon);

	// ConstraintTypes should contain 1 entry SurgSim::Math::MLCP_UNILATERAL_3D_FRICTIONLESS_CONSTRAINT
	ASSERT_EQ(1u, m_mlcpPhysicsProblem.constraintTypes.size());
	EXPECT_EQ(SurgSim::Math::MLCP_UNILATERAL_3D_FRICTIONLESS_CONSTRAINT, m_mlcpPhysicsProblem.constraintTypes[0]);
}

};  //  namespace Physics
};  //  namespace SurgSim
