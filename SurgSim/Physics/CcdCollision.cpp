// This file is a part of the OpenSurgSim project.
// Copyright 2013-2015, SimQuest Solutions Inc.
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

#include <vector>

#include "SurgSim/Collision/CollisionPair.h"
#include "SurgSim/Collision/ContactCalculation.h"
#include "SurgSim/Collision/CcdDcdCollision.h"
#include "SurgSim/Collision/Representation.h"
#include "SurgSim/Framework/Runtime.h"
#include "SurgSim/Framework/ThreadPool.h"
#include "SurgSim/Physics/CcdCollision.h"
#include "SurgSim/Physics/PhysicsManagerState.h"

using SurgSim::Collision::CollisionPair;
using SurgSim::Collision::ContactCalculation;

namespace SurgSim
{
namespace Physics
{

CcdCollision::CcdCollision(bool doCopyState) :
	Computation(doCopyState)
{
}

CcdCollision::~CcdCollision()
{
}

std::shared_ptr<PhysicsManagerState> CcdCollision::doUpdate(
	const double& dt,
	const std::shared_ptr<PhysicsManagerState>& state)
{
	std::shared_ptr<PhysicsManagerState> result = state;
	auto threadPool = Framework::Runtime::getThreadPool();
	std::vector<std::future<void>> tasks;

	const auto& calculations = ContactCalculation::getCcdContactTable();

	for (auto& pair : result->getCollisionPairs())
	{
		if (pair->getType() == Collision::COLLISION_DETECTION_TYPE_CONTINUOUS)
		{
			tasks.push_back(threadPool->enqueue<void>([&]()
			{
				calculations[pair->getFirst()->getShapeType()]
				[pair->getSecond()->getShapeType()]->calculateContact(pair);
			}));
		}
	}

	std::for_each(tasks.begin(), tasks.end(), [](std::future<void>& p){p.get();});

	return result;
}

}; // Physics
}; // SurgSim

