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
#include <boost/thread.hpp>

#include <SurgSim/Blocks/BasicSceneElement.h>
#include <SurgSim/Blocks/RepresentationPoseBehavior.h>
#include <SurgSim/Framework/ApplicationData.h>
#include <SurgSim/Framework/Behavior.h>
#include <SurgSim/Framework/BehaviorManager.h>
#include <SurgSim/Framework/Runtime.h>
#include <SurgSim/Framework/Scene.h>
#include <SurgSim/Framework/SceneElement.h>
#include <SurgSim/Graphics/OsgCamera.h>
#include <SurgSim/Graphics/OsgManager.h>
#include <SurgSim/Graphics/OsgPlaneRepresentation.h>
#include <SurgSim/Graphics/OsgSphereRepresentation.h>
#include <SurgSim/Graphics/OsgView.h>
#include <SurgSim/Graphics/OsgViewElement.h>
#include <SurgSim/Physics/PhysicsManager.h>
#include <SurgSim/Physics/FixedRepresentation.h>
#include <SurgSim/Physics/RigidRepresentation.h>
#include <SurgSim/Physics/RigidRepresentationParameters.h>
#include <SurgSim/Physics/PlaneShape.h>
#include <SurgSim/Physics/SphereShape.h>
#include <SurgSim/Math/Vector.h>
#include <SurgSim/Math/Quaternion.h>
#include <SurgSim/Math/RigidTransform.h>

using SurgSim::Blocks::BasicSceneElement;
using SurgSim::Blocks::RepresentationPoseBehavior;
using SurgSim::Framework::SceneElement;
using SurgSim::Graphics::OsgPlaneRepresentation;
using SurgSim::Graphics::OsgSphereRepresentation;
using SurgSim::Physics::FixedRepresentation;
using SurgSim::Physics::Representation;
using SurgSim::Physics::RigidRepresentation;
using SurgSim::Physics::PlaneShape;
using SurgSim::Physics::SphereShape;
using SurgSim::Physics::RigidRepresentationParameters;
using SurgSim::Physics::PhysicsManager;

///\file Example of how to put together a very simple demo of  balls colliding with each other
///		 dcd is used in a very simple manner to detect the collisions between the spheres


/// Simple behavior to show that the spheres are moving while we don't have graphics
class PrintoutBehavior : public SurgSim::Framework::Behavior
{
public:
	PrintoutBehavior(std::shared_ptr<RigidRepresentation> representation)
		: Behavior("PrintoutBehavior"), m_representation(representation) {}
	~PrintoutBehavior() {}

	virtual void update(double dt)
	{
		std::shared_ptr<SurgSim::Framework::Logger> logger = getRuntime()->getLogger("printout");
		SURGSIM_LOG_DEBUG(logger) << m_representation->getName() << ": " << m_representation->getPose().translation().transpose();
	}
protected:
	virtual bool doInitialize()
	{
		return true;
	}
	virtual bool doWakeUp()
	{
		return true;
	}

private:
	std::shared_ptr<RigidRepresentation> m_representation;
};

std::shared_ptr<SurgSim::Graphics::ViewElement> createView(const std::string& name, int x, int y, int width, int height)
{
	using SurgSim::Graphics::OsgViewElement;

	std::shared_ptr<OsgViewElement> viewElement = std::make_shared<OsgViewElement>(name);
	viewElement->getView()->setPosition(x, y);
	viewElement->getView()->setDimensions(width, height);

	return viewElement;
}

std::shared_ptr<SceneElement> createPlane(const std::string& name, const SurgSim::Math::RigidTransform3d& pose)
{
	std::shared_ptr<FixedRepresentation> physicsRepresentation = std::make_shared<FixedRepresentation>(name + " Physics");

	physicsRepresentation->setInitialPose(pose);

	std::shared_ptr<OsgPlaneRepresentation> graphicsRepresentation = std::make_shared<OsgPlaneRepresentation>(name + " Graphics");
	graphicsRepresentation->setInitialPose(pose);

	std::shared_ptr<SceneElement> planeElement = std::make_shared<BasicSceneElement>(name);
	planeElement->addComponent(physicsRepresentation);
	planeElement->addComponent(graphicsRepresentation);
	planeElement->addComponent(std::make_shared<RepresentationPoseBehavior>("Physics to Graphics Pose", physicsRepresentation,
		graphicsRepresentation));
	return planeElement;
}

std::shared_ptr<SceneElement> createSphere(const std::string& name, const SurgSim::Math::RigidTransform3d& pose)
{
	std::shared_ptr<RigidRepresentation> physicsRepresentation = std::make_shared<RigidRepresentation>(name + " Physics");

	RigidRepresentationParameters params;
	params.setDensity(700.0); // Wood
	params.setLinearDamping(10.0);

	std::shared_ptr<SphereShape> shape = std::make_shared<SphereShape>(0.1); // 1cm Sphere
	params.setShapeUsedForMassInertia(shape);

	physicsRepresentation->setInitialParameters(params);
	physicsRepresentation->setInitialPose(pose);

	std::shared_ptr<OsgSphereRepresentation> graphicsRepresentation = std::make_shared<OsgSphereRepresentation>(name + " Graphics");
	graphicsRepresentation->setRadius(0.1);
	graphicsRepresentation->setInitialPose(pose);

	std::shared_ptr<SceneElement> sphereElement = std::make_shared<BasicSceneElement>(name);
	sphereElement->addComponent(physicsRepresentation);
	sphereElement->addComponent(graphicsRepresentation);
	sphereElement->addComponent(std::make_shared<PrintoutBehavior>(physicsRepresentation));
	sphereElement->addComponent(std::make_shared<RepresentationPoseBehavior>("Physics to Graphics Pose", physicsRepresentation,
		graphicsRepresentation));
	return sphereElement;
}


int main(int argc, char* argv[])
{
	std::shared_ptr<SurgSim::Framework::Runtime> runtime(new SurgSim::Framework::Runtime());

	std::shared_ptr<SurgSim::Graphics::OsgManager> graphicsManager = std::make_shared<SurgSim::Graphics::OsgManager>();
	std::shared_ptr<PhysicsManager> physicsManager = std::make_shared<PhysicsManager>();
	std::shared_ptr<SurgSim::Framework::BehaviorManager> behaviorManager = 
		std::make_shared<SurgSim::Framework::BehaviorManager>();

	runtime->addManager(physicsManager);
	runtime->addManager(graphicsManager);
	runtime->addManager(behaviorManager);

	std::shared_ptr<SurgSim::Framework::Scene> scene(new SurgSim::Framework::Scene());

	scene->addSceneElement(createSphere("sphere1", 
		SurgSim::Math::makeRigidTransform(SurgSim::Math::Quaterniond::Identity(), Vector3d(0.0,2.0,0.0))));
	scene->addSceneElement(createPlane("plane1", 
		SurgSim::Math::makeRigidTransform(SurgSim::Math::Quaterniond::Identity(), Vector3d(0.0,0.0,0.0))));
	scene->addSceneElement(createView("view1", 0, 0, 1023, 768));

	graphicsManager->getDefaultCamera()->setInitialPose(SurgSim::Math::makeRigidTransform(
		SurgSim::Math::Quaterniond::Identity(), Vector3d(0.0, 0.5, 5.0)));

	runtime->setScene(scene);

	runtime->execute();

	return 0;
}
