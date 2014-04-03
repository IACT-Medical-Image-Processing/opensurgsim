# This file is a part of the OpenSurgSim project.
# Copyright 2013, SimQuest Solutions Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES} SurgSimFramework SurgSimInput)

include_directories(
	${Boost_INCLUDE_DIR}
	${EIGEN3_INCLUDE_DIR}
	${YAML_CPP_INCLUDE_DIR}
)

set(DEVICE_FILTERS_SOURCES
	PoseTransform.cpp
)

set(DEVICE_FILTERS_HEADERS
	PoseTransform.h
)

surgsim_add_library(
	SurgSimDeviceFilters
	"${DEVICE_FILTERS_SOURCES}" "${DEVICE_FILTERS_HEADERS}"
	SurgSim/Devices/DeviceFilters
)
if(BUILD_TESTING)
	# The unit tests will be built whenever the library is built.
	add_subdirectory(UnitTests)
endif()

set_target_properties(SurgSimDeviceFilters PROPERTIES FOLDER "Devices")