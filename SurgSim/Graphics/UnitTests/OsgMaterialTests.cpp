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
/// Tests for the OsgMaterial class.

#include <SurgSim/Graphics/OsgMaterial.h>
#include <SurgSim/Graphics/OsgShader.h>
#include <SurgSim/Graphics/OsgUniform.h>

#include <gtest/gtest.h>

using SurgSim::Math::Vector2f;

namespace SurgSim
{
namespace Graphics
{

/// Uniform that does not subclass OsgUniformBase
class MockUniform : public UniformBase
{
public:
	/// Constructor
	MockUniform() : UniformBase()
	{
	}
};

/// Shader that does not subclass OsgShader
class MockShader : public Shader
{
public:
	/// Constructor
	MockShader() : Shader()
	{
	}
};


TEST(OsgMaterialTests, InitTest)
{
	OsgMaterial material;

	EXPECT_EQ(0u, material.getNumUniforms());
	EXPECT_EQ(nullptr, material.getShader());

	EXPECT_NE(nullptr, material.getOsgStateSet());
}

TEST(OsgMaterialTests, AddAndRemoveUniformsTest)
{
	std::shared_ptr<OsgMaterial> osgMaterial = std::make_shared<OsgMaterial>();
	std::shared_ptr<Material> material = osgMaterial;

	EXPECT_EQ(0u, material->getNumUniforms());

	std::shared_ptr<OsgUniform<float>> osgUniform1 = std::make_shared<OsgUniform<float>>("float uniform");
	std::shared_ptr<Uniform<float>> uniform1 = osgUniform1;
	std::shared_ptr<OsgUniform<Vector2f>> osgUniform2 = std::make_shared<OsgUniform<Vector2f>>("Vector2f uniform");
	std::shared_ptr<Uniform<Vector2f>> uniform2 = osgUniform2;

	const osg::StateSet::UniformList& uniforms = osgMaterial->getOsgStateSet()->getUniformList();

	// Add a uniform to the material
	EXPECT_TRUE(material->addUniform(uniform1));
	EXPECT_EQ(1u, material->getNumUniforms());
	EXPECT_EQ(uniform1, material->getUniform(0));

	EXPECT_EQ(1u, uniforms.size());
	EXPECT_EQ(osgUniform1->getOsgUniform(), uniforms.at("float uniform").first);

	/// Add another uniform to the material
	EXPECT_TRUE(material->addUniform(uniform2));
	EXPECT_EQ(2u, material->getNumUniforms());
	EXPECT_EQ(uniform2, material->getUniform(1));

	EXPECT_EQ(2u, uniforms.size());
	EXPECT_EQ(osgUniform1->getOsgUniform(), uniforms.at("float uniform").first);
	EXPECT_EQ(osgUniform2->getOsgUniform(), uniforms.at("Vector2f uniform").first);

	/// Remove the first uniform from the material
	EXPECT_TRUE(material->removeUniform(uniform1));
	EXPECT_EQ(1u, material->getNumUniforms());
	EXPECT_EQ(uniform2, material->getUniform(0));

	EXPECT_EQ(1u, uniforms.size());
	EXPECT_EQ(osgUniform2->getOsgUniform(), uniforms.at("Vector2f uniform").first);

	/// Try removing the same uniform again
	EXPECT_FALSE(material->removeUniform(uniform1));
	EXPECT_EQ(1u, material->getNumUniforms());

	/// Try adding a non-OSG Uniform
	std::shared_ptr<MockUniform> nonOsgUniform = std::make_shared<MockUniform>();
	EXPECT_FALSE(material->addUniform(nonOsgUniform)) <<
		"Should not be able to add a uniform that is not a subclass of OsgUniformBase!";
	EXPECT_EQ(1u, material->getNumUniforms());

	/// Try removing a non-OSG Uniform
	EXPECT_FALSE(material->removeUniform(nonOsgUniform)) <<
		"Should not be able to remove a uniform that is not a subclass of OsgUniformBase!";
	EXPECT_EQ(1u, material->getNumUniforms());
}

TEST(OsgMaterialTests, SetShaderTest)
{
	std::shared_ptr<OsgMaterial> osgMaterial = std::make_shared<OsgMaterial>();
	std::shared_ptr<Material> material = osgMaterial;

	EXPECT_EQ(nullptr, material->getShader());

	std::shared_ptr<OsgShader> osgShader = std::make_shared<OsgShader>();
	std::shared_ptr<Shader> shader = osgShader;

	const osg::StateSet::AttributeList& attributes = osgMaterial->getOsgStateSet()->getAttributeList();

	// Set the material's shader
	EXPECT_TRUE(material->setShader(shader));
	EXPECT_EQ(shader, material->getShader());

	EXPECT_EQ(1u, attributes.size());
	EXPECT_EQ(osgShader->getOsgProgram(), attributes.at(osg::StateAttribute::TypeMemberPair(
		osg::StateAttribute::PROGRAM, 0)).first);

	/// Try setting a non-OSG Shader
	std::shared_ptr<MockShader> nonOsgShader = std::make_shared<MockShader>();
	EXPECT_FALSE(material->setShader(nonOsgShader)) <<
		"Should not be able to set a shader that is not a subclass of OsgShader!";
	EXPECT_NE(nonOsgShader, material->getShader());
}

}  // namespace Graphics
}  // namespace SurgSim