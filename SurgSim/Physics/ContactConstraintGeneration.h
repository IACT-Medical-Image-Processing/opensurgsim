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

#ifndef SURGSIM_PHYSICS_CONTACTCONSTRAINTGENERATION_H
#define SURGSIM_PHYSICS_CONTACTCONSTRAINTGENERATION_H

#include  <memory>
#include <SurgSim/Framework/SharedInstance.h>
#include <SurgSim/Physics/Computation.h>
#include <SurgSim/Physics/ConstraintImplementationFactory.h>
#include <SurgSim/Framework/Log.h>

namespace SurgSim
{
namespace Physics
{

class PhysicsManagerState;
class Representation;
struct Location;
class Localization;
class CollisionRepresentation;

/// Generate a constraint for every contact that was calculated.
/// The general algorithm is such, for each pair of CollisionRepresentations that has Contacts
/// For each contact:
/// - Generate a localization for each side of the pair
/// - Find the correct CollisionImplementation for the Representation and type of constraint, note that currently
///   we use only one type of constraint for the collision
/// - Create the ConstraintData for the contact with
/// - Create a constraint from, the ContactImplmentation, Localization and ConstraintData
/// - Add it to the list of Constraints
/// At the end those constraints are added as contact constraints to the physics state
class ContactConstraintGeneration : public Computation
{
public:
	/// Constructor
	ContactConstraintGeneration();
	~ContactConstraintGeneration();

private:

	/// For looking up instances of constrain implementations
	ConstraintImplementationFactory m_factory;

	/// The logger for this class
	std::shared_ptr<SurgSim::Framework::Logger> m_logger;

	/// Overriden function from Computation, the actual work is done here
	virtual std::shared_ptr<PhysicsManagerState> doUpdate(
		const double& dt,
		const std::shared_ptr<PhysicsManagerState>& state) override;

	/// Generate a localization from a CollisionRepresentation
	std::shared_ptr<Localization> makeLocalization(
		const std::shared_ptr<CollisionRepresentation>& representation,
		const Location& location);

};

}; // Physics
}; // SurgSim

#endif