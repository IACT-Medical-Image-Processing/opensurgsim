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

#include "SurgSim/Devices/Sixense/SixenseThread.h"

#include "SurgSim/Devices/Sixense/SixenseScaffold.h"

namespace SurgSim
{
namespace Devices
{

SixenseThread::~SixenseThread()
{
}

bool SixenseThread::doUpdate(double dt)
{
	m_scaffold->runInputFrame();
	return true;
}

bool SixenseThread::doInitialize()
{
	return true;
}

bool SixenseThread::doStartUp()
{
	return true;
}

};  // namespace Devices
};  // namespace SurgSim
