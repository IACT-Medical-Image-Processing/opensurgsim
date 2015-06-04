// This file is a part of the OpenSurgSim project.
// Copyright 2015, SimQuest Solutions Inc.
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

#include "SurgSim/Devices/Oculus/OculusScaffold.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <list>
#include <memory>

#include <OVR_Version.h>
#if 6 == OVR_MAJOR_VERSION
#include <OVR_CAPI_0_6_0.h>
#elif 5 == OVR_MAJOR_VERSION
#include <OVR_CAPI_0_5_0.h>
#endif


#include "SurgSim/DataStructures/DataGroup.h"
#include "SurgSim/DataStructures/DataGroupBuilder.h"
#include "SurgSim/Devices/Oculus/OculusDevice.h"
#include "SurgSim/Framework/Log.h"
#include "SurgSim/Framework/SharedInstance.h"
#include "SurgSim/Math/Quaternion.h"
#include "SurgSim/Math/RigidTransform.h"

using SurgSim::DataStructures::DataGroup;
using SurgSim::DataStructures::DataGroupBuilder;
using SurgSim::Framework::Logger;
using SurgSim::Math::makeRigidTransform;
using SurgSim::Math::Quaterniond;
using SurgSim::Math::RigidTransform3d;
using SurgSim::Math::Vector3d;

namespace SurgSim
{
namespace Device
{

struct OculusScaffold::DeviceData
{
	/// Constructor
	/// \param device Device to be wrapped
	/// \param index Index of Oculus device to be created. If exists, a valid handle will be held in 'handle'.
	DeviceData(OculusDevice* device, int index) :
		deviceObject(device),
		handle(nullptr)
	{
#if 6 == OVR_MAJOR_VERSION
		ovrHmd_Create(index, &handle);
#elif 5 == OVR_MAJOR_VERSION
		handle = ovrHmd_Create(index);
#endif
	}

	~DeviceData()
	{
		if (handle != nullptr)
		{
			ovrHmd_Destroy(handle);
		}
	}

	/// The device object wrapped in this class.
	OculusDevice* const deviceObject;

	/// Device handle
	ovrHmd handle;
};

struct OculusScaffold::StateData
{
	/// List of registered devices.
	std::list<std::unique_ptr<OculusScaffold::DeviceData>> registeredDevices;

	/// The mutex that protects the list of registered devices.
	boost::mutex mutex;
};

OculusScaffold::OculusScaffold() :
	SurgSim::Framework::BasicThread("OculusScaffold"),
	m_logger(Logger::getLogger("Devices/Oculus")),
	m_state(new StateData)
{
	SURGSIM_LOG_DEBUG(m_logger) << "Oculus: Shared scaffold created.";

	// Oculus DK2 tracks Gyroscope, Accelerometer, Magnetometer at 1000Hz and
	// positional tracking is done at 60Hz.
	setRate(1000.0);

#if 6 == OVR_MAJOR_VERSION
	SURGSIM_ASSERT(ovrSuccess == ovr_Initialize(nullptr)) << "Oculus SDK initialization failed.";
#elif 5 == OVR_MAJOR_VERSION
	SURGSIM_ASSERT(ovrTrue == ovr_Initialize(nullptr)) << "Oculus SDK initialization failed.";
#endif
}

OculusScaffold::~OculusScaffold()
{
	ovr_Shutdown();
}

bool OculusScaffold::registerDevice(OculusDevice* device)
{
	bool result = false;

	int numOfAvailableDevices = ovrHmd_Detect();
	if (numOfAvailableDevices < 1)
	{
		SURGSIM_LOG_SEVERE(m_logger) << __FUNCTION__ << "No available Oculus device detected.";
	}
	else
	{
		boost::lock_guard<boost::mutex> lock(m_state->mutex);
		SURGSIM_ASSERT(m_state->registeredDevices.size() < 1) << "There is one registered Oculus device already." <<
																 "OculusScaffold only supports one device right now";

		std::unique_ptr<DeviceData> info(new DeviceData(device, 0));

		if (info->handle == nullptr)
		{
			SURGSIM_LOG_SEVERE(m_logger) << __FUNCTION__ << "Failed to obtain a handle to Oculus Device." <<
															"Is an Oculus device plugged in?";
		}
		else
		{
#if 6 == OVR_MAJOR_VERSION
			if (ovrSuccess == ovrHmd_ConfigureTracking(info->handle, ovrTrackingCap_Orientation |
																	 ovrTrackingCap_MagYawCorrection |
																	 ovrTrackingCap_Position, 0))
#elif 5 == OVR_MAJOR_VERSION
			if (ovrTrue == ovrHmd_ConfigureTracking(info->handle, ovrTrackingCap_Orientation |
																  ovrTrackingCap_MagYawCorrection |
																  ovrTrackingCap_Position, 0))
#endif
			{
				DataGroup& inputData = info->deviceObject->getInputData();

				// Query the HMD for the left and right projection matrices.
				ovrFovPort defaultLeftFov = info->handle->DefaultEyeFov[ovrEyeType::ovrEye_Left];
				ovrFovPort defaultRightFov = info->handle->DefaultEyeFov[ovrEyeType::ovrEye_Right];

				float nearPlane = info->deviceObject->getNearPlane();
				float farPlane = info->deviceObject->getFarPlane();

				ovrMatrix4f leftProjection = ovrMatrix4f_Projection(defaultLeftFov, nearPlane, farPlane,
																	ovrProjectionModifier::ovrProjection_RightHanded);
				ovrMatrix4f rightProjection = ovrMatrix4f_Projection(defaultRightFov, nearPlane, farPlane,
																	ovrProjectionModifier::ovrProjection_RightHanded);

				inputData.matrices().set(SurgSim::DataStructures::Names::LEFT_PROJECTION_MATRIX,
											Eigen::Map<const Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>
												(&leftProjection.M[0][0]).cast<double>());
				inputData.matrices().set(SurgSim::DataStructures::Names::RIGHT_PROJECTION_MATRIX,
											Eigen::Map<const Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>
												(&rightProjection.M[0][0]).cast<double>());

				m_state->registeredDevices.emplace_back(std::move(info));
				SURGSIM_LOG_INFO(m_logger) << "Device " << getName() << ": registered.";

				result = true;
			}
			else
			{
				SURGSIM_LOG_SEVERE(m_logger) << __FUNCTION__ << "Failed to configure an Oculus Device." <<
																" Registration failed";
			}
		}

		if (result && !isRunning())
		{
			start(); // Start the scaffold thread if not running.
		}
	}

	return result;
}

bool OculusScaffold::unregisterDevice(const OculusDevice* const device)
{
	bool result = false;
	{
		boost::lock_guard<boost::mutex> lock(m_state->mutex);
		auto match = std::find_if(m_state->registeredDevices.begin(), m_state->registeredDevices.end(),
								  [&device](const std::unique_ptr<DeviceData>& info)
		{
			return info->deviceObject == device;
		});

		if (match != m_state->registeredDevices.end())
		{
			m_state->registeredDevices.erase(match);
			// the iterator is now invalid but that's OK
			SURGSIM_LOG_INFO(m_logger) << "Device " << getName() << ": unregistered.";
			result = true;
		}
	}

	// #threadsafety After unregistering, another thread could be in the process of registering.
	if (isRunning() && m_state->registeredDevices.size() == 0)
	{
		stop();
	}

	SURGSIM_LOG_IF(!result, m_logger, SEVERE) << __FUNCTION__ << "Attempted to release a non-registered device.";

	return result;
}

bool OculusScaffold::doInitialize()
{
	return true;
}

bool OculusScaffold::doStartUp()
{
	return true;
}

bool OculusScaffold::doUpdate(double dt)
{
	boost::lock_guard<boost::mutex> lock(m_state->mutex);

	for (auto& device : m_state->registeredDevices)
	{
		DataGroup& inputData = device->deviceObject->getInputData();

		// Query the HMD for the current tracking state.
		// If time in 2nd parameter is now or earlier, no pose prediction is made.
		// Pose is reported in a right handed coordinate system, X->RIGHT, Y->UP, Z->OUT.
		// Positive rotation is counterclockwise.
		// The XZ plane is ALWAYS aligned with the ground regardless of camera orientation.
		// Default tracking origin is one meter away from the camera.
		ovrTrackingState ts = ovrHmd_GetTrackingState(device->handle, ovr_GetTimeInSeconds());
		if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
		{
			ovrPosef ovrPose = ts.HeadPose.ThePose;

			Vector3d position(ovrPose.Position.x, ovrPose.Position.y, ovrPose.Position.z);
			Quaterniond orientation(ovrPose.Orientation.w, ovrPose.Orientation.x,
									ovrPose.Orientation.y, ovrPose.Orientation.z);
			RigidTransform3d pose = makeRigidTransform(orientation, position);

			inputData.poses().set(SurgSim::DataStructures::Names::POSE, pose);
		}
		else
		{
			inputData.poses().reset(SurgSim::DataStructures::Names::POSE);
		}

		device->deviceObject->pushInput();
	}

	return true;
}

SurgSim::DataStructures::DataGroup OculusScaffold::buildDeviceInputData()
{
	DataGroupBuilder builder;
	builder.addPose(SurgSim::DataStructures::Names::POSE);
	builder.addMatrix(SurgSim::DataStructures::Names::LEFT_PROJECTION_MATRIX);
	builder.addMatrix(SurgSim::DataStructures::Names::RIGHT_PROJECTION_MATRIX);
	return builder.createData();
}

std::shared_ptr<OculusScaffold> OculusScaffold::getOrCreateSharedInstance()
{
	static auto creator = []()
	{
		return std::shared_ptr<OculusScaffold>(new OculusScaffold);
	};
	static SurgSim::Framework::SharedInstance<OculusScaffold> sharedInstance(creator);
	return sharedInstance.get();
}

};  // namespace Device
};  // namespace SurgSim
