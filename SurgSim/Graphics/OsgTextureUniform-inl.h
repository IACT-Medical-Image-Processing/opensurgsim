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

#ifndef SURGSIM_GRAPHICS_OSGTEXTUREUNIFORM_INL_H
#define SURGSIM_GRAPHICS_OSGTEXTUREUNIFORM_INL_H

#include "SurgSim/Framework/Assert.h"
#include "SurgSim/Graphics/OsgTexture.h"
#include "SurgSim/Graphics/OsgTexture1d.h"
#include "SurgSim/Graphics/OsgTexture2d.h"
#include "SurgSim/Graphics/OsgTexture3d.h"
#include "SurgSim/Graphics/OsgTextureCubeMap.h"
#include "SurgSim/Graphics/OsgTextureRectangle.h"
#include "SurgSim/Graphics/OsgUniformTypes.h"

namespace SurgSim
{

namespace Graphics
{

template <class T>
OsgTextureUniform<T>::OsgTextureUniform(const std::string& name) :
	UniformBase(), Uniform<std::shared_ptr<T>>(), OsgUniformBase(name), m_unit(0), m_minimumTextureUnit(0)
{
	osg::Uniform::Type osgUniformType = getOsgUniformType<std::shared_ptr<T>>();
	SURGSIM_ASSERT(osgUniformType != osg::Uniform::UNDEFINED) << "Failed to get OSG uniform type!";
	SURGSIM_ASSERT(m_uniform->setType(osgUniformType)) << "Failed to set OSG uniform type!";
	m_uniform->setNumElements(1);
}

template <class T>
void OsgTextureUniform<T>::set(const std::shared_ptr<T>& value)
{
	m_texture = value;
}

template <class T>
const std::shared_ptr<T>& OsgTextureUniform<T>::get() const
{
	return m_texture;
}

template <class T>
void OsgTextureUniform<T>::addToStateSet(osg::StateSet* stateSet)
{
	const osg::StateSet::TextureAttributeList& textures = stateSet->getTextureAttributeList();
	int availableUnit = m_minimumTextureUnit;
	if (textures.size() > m_minimumTextureUnit)
	{
		for (auto it = textures.begin() + m_minimumTextureUnit; it != textures.end(); ++it)
		{
			if (it->empty())
			{
				break;
			}
			availableUnit++;
		}
	}

	m_unit = availableUnit;

	SURGSIM_ASSERT(m_texture != nullptr) << "Tried to add this uniform without a valid Texture";
	stateSet->setTextureAttributeAndModes(m_unit, m_texture->getOsgTexture(),
										  osg::StateAttribute::ON);
	SURGSIM_ASSERT(m_uniform->set(static_cast<int>(m_unit))) << "Failed to set OSG texture uniform unit!" <<
			" Uniform: " << getName() << " unit: " << m_unit;
	stateSet->addUniform(m_uniform);
}

template <class T>
void OsgTextureUniform<T>::removeFromStateSet(osg::StateSet* stateSet)
{
	stateSet->removeTextureAttribute(m_unit, m_texture->getOsgTexture());
	stateSet->removeUniform(m_uniform);
}

template <class T>
void OsgTextureUniform<T>::setMinimumTextureUnit(size_t unit)
{
	m_minimumTextureUnit = unit;
}

template <class T>
size_t OsgTextureUniform<T>::getMinimumTextureUnit() const
{
	return m_minimumTextureUnit;
}
};  // namespace Graphics

};  // namespace SurgSim

#endif  // SURGSIM_GRAPHICS_OSGTEXTUREUNIFORM_INL_H
