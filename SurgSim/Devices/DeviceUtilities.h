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

#ifndef SURGSIM_DEVICES_DEVICEUTILITIES_H
#define SURGSIM_DEVICES_DEVICEUTILITIES_H

#include <memory>
#include <string>
#include <vector>


namespace SurgSim
{
namespace Input
{
class DeviceInterface;
}

namespace Devices
{

/// Attempts to create and initialize a device from a list of devices to try
/// \param classNames The fully-qualified device class names to try. The elements will be tried in order, front to back.
/// \param name The name to be given to the created device
/// \return An initialized device or nullptr if no device was available.
std::shared_ptr<Input::DeviceInterface> createDevice(const std::vector<std::string>& classNames,
		const std::string& name);

/// Attempts to create and initialize a device
/// \param className The fully-qualified device class name to try.
/// \param name The name to be given to the created device
/// \return An initialized device or nullptr if no device was available.
std::shared_ptr<Input::DeviceInterface> createDevice(const std::string& className, const std::string& name);

/// Loads a single device from the file, the first device that successfully initializes.
/// The file format should be just a list of DeviceInterfaces
/// \code
/// - SurgSim::Device::MultiAxisDevice:
///        Name: element1
/// - SurgSim::Device::IdentityPoseDevice:
///        Name: element1
/// \endcode
/// \param fileName the filename of the devices to be loaded, needs to be found
/// \return an initialized device, or nullptr if no device could be created and initialized
/// \throws If the file cannot be found or is an invalid file
std::shared_ptr<Input::DeviceInterface> loadDevice(const std::string& fileName);

};
};

#endif // SURGSIM_DEVICES_DEVICEUTILITIES_H


