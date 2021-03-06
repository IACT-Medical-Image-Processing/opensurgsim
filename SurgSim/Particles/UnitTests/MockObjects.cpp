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

#include "SurgSim/Particles/UnitTests/MockObjects.h"


MockParticleSystem::MockParticleSystem(const std::string& name) :
	SurgSim::Particles::Representation(name),
	updateCount(0)
{
}

bool MockParticleSystem::doUpdate(double dt)
{
	updateCount++;
	return true;
}

bool MockParticleSystem::doHandleCollisions(double dt, const SurgSim::Collision::ContactMapType& collisions)
{
	return true;
}

MockEmitter::MockEmitter(const std::string& name) :
	SurgSim::Particles::Emitter(name),
	updateCount(0)
{
}

void MockEmitter::update(double dt)
{
	SurgSim::Particles::Emitter::update(dt);
	updateCount++;
}

