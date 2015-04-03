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

#ifndef SURGSIM_DEVICES_LEAP_LEAPSCAFFOLD_H
#define SURGSIM_DEVICES_LEAP_LEAPSCAFFOLD_H

#include <memory>

#include "SurgSim/DataStructures/DataGroup.h"


namespace SurgSim
{

namespace Framework
{
class Logger;
};

namespace Device
{

class LeapDevice;

/// A class that manages Leap devices
///
/// \sa SurgSim::Device::LeapDevice
class LeapScaffold
{
public:
	/// Destructor.
	virtual ~LeapScaffold();

private:
	/// Internal shared state data type.
	struct StateData;

	/// Interal per-device information.
	struct DeviceData;

	/// Class for receiving data from the Leap SDK
	class Listener;

	friend class LeapDevice;

	/// Constructor.
	LeapScaffold();

	/// Gets or creates the scaffold shared by all LeapDevice instances.
	/// The scaffold is managed using a SharedInstance object, so it will be destroyed when all devices are released.
	/// \return the scaffold object.
	static std::shared_ptr<LeapScaffold> getOrCreateSharedInstance();

	/// Registers the specified device object.
	/// \param device The device object to be used
	/// \return True if the initialization succeeds, false if it fails.
	bool registerDevice(LeapDevice* device);

	/// Unregisters the specified device object.
	/// \param device The device object.
	/// \return true on success, false on failure.
	bool unregisterDevice(const LeapDevice* device);

	/// Do the Leap specific registration
	/// \param info The device data
	/// \return true on success, false on failure.
	bool doRegisterDevice(DeviceData* info);

	/// Do the Leap specific unregistration
	/// \param info The device data
	/// \return true on success, false on failure.
	bool doUnregisterDevice(DeviceData* info);

	/// Handle a new frame of data
	void handleFrame();

	/// Builds the data layout for the application input (i.e. device output).
	static SurgSim::DataStructures::DataGroup buildDeviceInputData();

	/// Internal scaffold state.
	std::unique_ptr<StateData> m_state;

	/// Logger used by the scaffold
	std::shared_ptr<SurgSim::Framework::Logger> m_logger;
};

};  // namespace Device
};  // namespace SurgSim

#endif // SURGSIM_DEVICES_LEAP_LEAPSCAFFOLD_H
