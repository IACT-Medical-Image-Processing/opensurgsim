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
#include <vector>

#include <SurgSim/Physics/FreeMotion.h>
#include <SurgSim/Physics/Actors/Actor.h>

namespace SurgSim
{
namespace Physics
{


FreeMotion::FreeMotion(std::shared_ptr<std::vector<std::shared_ptr<Actor>>> actors) :
	m_actors(actors)
{

}

FreeMotion::~FreeMotion()
{

}

void FreeMotion::doUpdate(double dt)
{
	std::shared_ptr< std::vector<std::shared_ptr<Actor>>> actors = m_actors.lock();

	SURGSIM_ASSERT(actors != nullptr) << "Actors data structure was deallocated";

	auto it = actors->begin();
	auto itEnd = actors->end();
	for (; it != itEnd; ++it)
	{
		(*it)->update(dt);
	}
}


}; // Physics
}; // SurgSim
