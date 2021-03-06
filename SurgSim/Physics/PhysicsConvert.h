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

#ifndef SURGSIM_PHYSICS_PHYSICSCONVERT_H
#define SURGSIM_PHYSICS_PHYSICSCONVERT_H

#include <yaml-cpp/yaml.h>

namespace SurgSim
{
namespace Physics
{
class RigidState;
}
}

namespace YAML
{

template <>
struct convert<SurgSim::Physics::RigidState>
{
	static Node encode(const SurgSim::Physics::RigidState& rhs);
	static bool decode(const Node& node, SurgSim::Physics::RigidState& rhs); //NOLINT
};


}; // namespace YAML

#endif // SURGSIM_PHYSICS_PHYSICSCONVERT_H
