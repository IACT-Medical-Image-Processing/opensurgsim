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
#include "SurgSim/Collision/Representation.h"
#include "SurgSim/Framework/Log.h"
#include "SurgSim/Physics/PhysicsManagerState.h"
#include "SurgSim/Physics/PrepareCollisionPairs.h"
#include "SurgSim/Math/Aabb.h"

namespace SurgSim
{
namespace Physics
{

PrepareCollisionPairs::PrepareCollisionPairs(bool doCopyState) :
	Computation(doCopyState),
	m_timeSinceLog(0.0),
	m_logger(Framework::Logger::getLogger("Physics/PrepareCollisionPairs"))
{
}

PrepareCollisionPairs::~PrepareCollisionPairs()
{
}

std::shared_ptr<PhysicsManagerState> PrepareCollisionPairs::doUpdate(
	const double& dt,
	const std::shared_ptr<PhysicsManagerState>& state)
{
	std::shared_ptr<PhysicsManagerState> result = state;
	auto& representations = result->getActiveCollisionRepresentations();

	std::vector<std::shared_ptr<Collision::CollisionPair>> pairs;

	auto end = std::end(representations);
	for (auto first = std::begin(representations); first != end; ++first)
	{
		for (auto second = first; second != end; ++second)
		{
			if (!(*first)->isIgnoring(*second) && !(*second)->isIgnoring(*first))
			{
				auto pair = std::make_shared<Collision::CollisionPair>(*first, *second);
				if (pair->getType() != Collision::COLLISION_DETECTION_TYPE_NONE && pair->mayIntersect())
				{
					pairs.push_back(pair);
				}
			}
		}
	}

	result->setCollisionPairs(pairs);

	if (m_logger->getThreshold() <= SURGSIM_LOG_LEVEL(DEBUG))
	{
		m_timeSinceLog += dt;
		if (m_timeSinceLog > 5.0)
		{
			m_timeSinceLog = 0.0;
			typedef std::tuple<std::string, std::string, Collision::CollisionDetectionType> PairType;
			std::vector<PairType> names;
			for (const auto& pair : pairs)
			{
				std::string first = pair->getFirst()->getFullName();
				std::string second = pair->getSecond()->getFullName();
				if (first < second)
				{
					names.emplace_back(first, second, pair->getType());
				}
				else
				{
					names.emplace_back(second, first, pair->getType());
				}
			}
			std::sort(names.begin(), names.end(), [](const PairType & i, const PairType & j)
			{
				return (std::get<0>(i) < std::get<0>(j)) ||
					   ((std::get<0>(i) == std::get<0>(j)) && (std::get<1>(i) < std::get<1>(j)));
			});
			std::string message = "All collision pairs for testing:\n";
			for (const auto& name : names)
			{
				message += "\t" + std::get<0>(name) + " : " + std::get<1>(name) +
						   (
							   (std::get<2>(name) == SurgSim::Collision::COLLISION_DETECTION_TYPE_CONTINUOUS) ?
							   " CCD" : " DCD"
						   ) + "\n";
			}
			SURGSIM_LOG_DEBUG(m_logger) << message;
		}
	}

	return result;
}

}; // Physics
}; // SurgSim
