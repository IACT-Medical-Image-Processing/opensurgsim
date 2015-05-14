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

#ifndef SURGSIM_PARTICLES_PARTICLESYSTEMREPRESENTATION_H
#define SURGSIM_PARTICLES_PARTICLESYSTEMREPRESENTATION_H

#include <boost/thread.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "SurgSim/Framework/Representation.h"
#include "SurgSim/Math/Vector.h"
#include "SurgSim/Particles/Particles.h"


namespace SurgSim
{

namespace Collision
{
class Representation;
};

namespace Framework
{
class Logger;
};

namespace Particles
{

/// The Representation class defines the base class for all Particle System.
class Representation : public SurgSim::Framework::Representation
{
public:
	/// Constructor
	/// \param name The representation's name
	explicit Representation(const std::string& name);

	/// Destructor
	virtual ~Representation();

	/// Set the maximum number of particles of this system.
	/// \note Once initialized, it can't be changed.
	/// \param maxParticles The maximum number of particles in this system.
	void setMaxParticles(size_t maxParticles);

	/// \return The number of particles allowed in this system.
	size_t getMaxParticles() const;

	/// Add a particle
	/// \param particle The new particle
	/// \return True if the particle was successfully added, false otherwise
	bool addParticle(const Particle& particle);

	/// Add a particle
	/// \param position The position of the new particle
	/// \param velocity The velocity of the new particle
	/// \param lifetime The lenght of time the particle will exist
	/// \return True if the particle was successfully added, false otherwise
	bool addParticle(const Math::Vector3d& position, const Math::Vector3d& velocity, double lifetime);

	/// Get the particles
	/// \return The particles
	Particles& getParticles();

	/// Get the particles, const version
	/// \return The particles
	const Particles& getParticles() const;

	/// Update the particle system
	/// \param dt The time step.
	void update(double dt);

	/// Set the collision representation for this Particle Representation
	/// \param representation The collision representation to be set
	void setCollisionRepresentation(std::shared_ptr<SurgSim::Collision::Representation> representation);

	/// Get the collision representation for this Particle Representation
	/// \return the collision representation
	std::shared_ptr<SurgSim::Collision::Representation> getCollisionRepresentation() const;

protected:
	/// Implementation of the specific behavior of the particle system
	/// \return True if update succeeded, False otherwise.
	virtual bool doUpdate(double dt) = 0;

	bool doInitialize() override;

	/// Maximum amount of particles allowed in this particle system.
	size_t m_maxParticles;

	/// List of particles.
	Particles m_particles;

	/// Logger used by the particle system.
	std::shared_ptr<SurgSim::Framework::Logger> m_logger;

	/// This entity's collision representation
	std::shared_ptr<SurgSim::Collision::Representation> m_collisionRepresentation;
};

};  // namespace Particles
};  // namespace SurgSim

#endif  // SURGSIM_PARTICLES_PARTICLESYSTEMREPRESENTATION_H
