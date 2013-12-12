// This file is a part of the OpenSurgSim project.
// Copyright 2012-2013, SimQuest Solutions Inc.
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

/// \file
/// Tests for the OsgTextureCubeMap class.

#include "SurgSim/Framework/ApplicationData.h"
#include "SurgSim/Graphics/OsgTexture3d.h"

#include <boost/filesystem.hpp>

#include <gtest/gtest.h>

namespace SurgSim
{
namespace Graphics
{

TEST(OsgTexture3dTests, InitTest)
{
	OsgTexture3d texture;

	EXPECT_NE(nullptr, texture.getOsgTexture());

	EXPECT_EQ(nullptr, texture.getOsgTexture()->getImage(0u));
}

TEST(OsgTexture3dTests, SetSizeTest)
{
	OsgTexture3d texture;

	texture.setSize(256, 512, 1024);

	int width, height, depth;
	texture.getSize(&width, &height, &depth);

	EXPECT_EQ(256, width);
	EXPECT_EQ(512, height);
	EXPECT_EQ(1024, depth);
}

TEST(OsgTexture3dTests, LoadAndClearImageTest)
{
	ASSERT_TRUE(boost::filesystem::exists("Data"));

	std::vector<std::string> paths;
	paths.push_back("Data/OsgTextureTests");
	SurgSim::Framework::ApplicationData data(paths);

	std::string imagePath = data.findFile("CheckerBoard.png");

	ASSERT_NE("", imagePath) << "Could not find image file!";

	// Load the image
	std::shared_ptr<OsgTexture3d> osgTexture = std::make_shared<OsgTexture3d>();
	std::shared_ptr<Texture> texture = osgTexture;

	EXPECT_TRUE(texture->loadImage(imagePath)) << "Failed to load image!";

	EXPECT_EQ(1u, osgTexture->getOsgTexture()->getNumImages());

	EXPECT_NE(nullptr, osgTexture->getOsgTexture()->getImage(0u)) << "Texture should have an image!";

	// Make sure the image has the expected size
	int width, height, depth;
	osgTexture->getSize(&width, &height, &depth);
	EXPECT_EQ(512, width);
	EXPECT_EQ(512, height);
	EXPECT_EQ(1, depth);

	osg::Image* image = osgTexture->getOsgTexture()->getImage(0u);
	EXPECT_EQ(512, image->s());
	EXPECT_EQ(512, image->t());
	EXPECT_EQ(1, image->r());

	// Remove the image
	texture->clearImage();

	EXPECT_EQ(nullptr, osgTexture->getOsgTexture()->getImage(0u)) << "Texture image should have been cleared!";

	// Try to load an image that does not exist
	EXPECT_FALSE(texture->loadImage("NotHere.png")) << "Should not have been able to load image - it does not exist!";
	EXPECT_EQ(nullptr, osgTexture->getOsgTexture()->getImage(0u));
}

TEST(OsgTexture3dTests, LoadImageSlicesTest)
{
	ASSERT_TRUE(boost::filesystem::exists("Data"));

	std::vector<std::string> paths;
	paths.push_back("Data/OsgTextureTests");
	SurgSim::Framework::ApplicationData data(paths);

	std::string slice0Path = data.findFile("Brdf0.png");
	ASSERT_NE("", slice0Path) << "Could not find image file for slice 0!";

	std::string slice1Path = data.findFile("Brdf1.png");
	ASSERT_NE("", slice1Path) << "Could not find image file for slice 1!";

	std::vector<std::string> slicePaths;
	slicePaths.push_back(slice0Path);
	slicePaths.push_back(slice1Path);

	// Load the images
	std::shared_ptr<OsgTexture3d> osgTexture = std::make_shared<OsgTexture3d>();
	std::shared_ptr<Texture> texture = osgTexture;

	EXPECT_TRUE(osgTexture->loadImageSlices(slicePaths)) << "Failed to load images!";

	EXPECT_EQ(1u, osgTexture->getOsgTexture()->getNumImages());

	EXPECT_NE(nullptr, osgTexture->getOsgTexture()->getImage(0u)) << "Texture should have an image!";

	// Make sure the image has the expected size
	int width, height, depth;
	osgTexture->getSize(&width, &height, &depth);
	EXPECT_EQ(256, width);
	EXPECT_EQ(256, height);
	EXPECT_EQ(2, depth);

	osg::Image* image = osgTexture->getOsgTexture()->getImage(0u);
	EXPECT_EQ(256, image->s());
	EXPECT_EQ(256, image->t());
	EXPECT_EQ(2, image->r());

	// Remove the image
	texture->clearImage();

	EXPECT_EQ(nullptr, osgTexture->getOsgTexture()->getImage(0u)) << "Texture image should have been cleared!";

	// Try to load an image that does not exist
	slicePaths.push_back("NotHere.png");
	EXPECT_FALSE(osgTexture->loadImageSlices(slicePaths)) <<
		"Should not have been able to load an image - it does not exist!";
	EXPECT_EQ(nullptr, osgTexture->getOsgTexture()->getImage(0u));
}

}  // namespace Graphics
}  // namespace SurgSim