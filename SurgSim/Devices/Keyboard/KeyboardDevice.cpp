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

#include "SurgSim/Devices/Keyboard/KeyboardDevice.h"

#include "SurgSim/Devices/Keyboard/KeyboardScaffold.h"
#include "SurgSim/Devices/Keyboard/OsgKeyboardHandler.h"
#include "SurgSim/Framework/Log.h"

namespace SurgSim
{
namespace Devices
{

SURGSIM_REGISTER(SurgSim::Input::DeviceInterface, SurgSim::Devices::KeyboardDevice, KeyboardDevice);

KeyboardDevice::KeyboardDevice(const std::string& deviceName) :
	Input::CommonDevice(deviceName, KeyboardScaffold::buildDeviceInputData())
{
}

KeyboardDevice::~KeyboardDevice()
{
	if (isInitialized())
	{
		finalize();
	}
}

bool KeyboardDevice::initialize()
{
	SURGSIM_ASSERT(!isInitialized()) << getName() << " already initialized.";
	auto scaffold = KeyboardScaffold::getOrCreateSharedInstance();
	SURGSIM_ASSERT(scaffold != nullptr);
	bool registered = false;
	if (scaffold->registerDevice(this))
	{
		m_scaffold = std::move(scaffold);
		registered = true;
	}
	return registered;
}

bool KeyboardDevice::finalize()
{
	SURGSIM_ASSERT(isInitialized()) << getName() << " is not initialized, cannot finalize.";
	bool unregistered = m_scaffold->unregisterDevice();
	m_scaffold.reset();
	return unregistered;
}

bool KeyboardDevice::isInitialized() const
{
	return (m_scaffold != nullptr);
}

OsgKeyboardHandler* KeyboardDevice::getKeyboardHandler() const
{
	return m_scaffold->getKeyboardHandler();
}


};  // namespace Devices
};  // namespace SurgSim
