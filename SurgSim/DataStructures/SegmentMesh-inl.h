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

#ifndef SURGSIM_DATASTRUCTURES_SEGMENTMESH_INL_H
#define SURGSIM_DATASTRUCTURES_SEGMENTMESH_INL_H

#include "SurgSim/Framework/Log.h"


namespace SurgSim
{
namespace DataStructures
{

template <class VertexData, class EdgeData>
SegmentMesh<VertexData, EdgeData>::SegmentMesh()
{
}

template <class VertexData, class EdgeData>
SegmentMesh<VertexData, EdgeData>::SegmentMesh(
const SegmentMesh<VertexData, EdgeData>& other) :
	TriangleMeshType(other)
{
}

template <class VertexData, class EdgeData>
template <class V, class E>
SegmentMesh<VertexData, EdgeData>::SegmentMesh(const SegmentMesh<V, E>& other) :
	TriangleMeshType(other)
{
}

template <class VertexData, class EdgeData>
SegmentMesh<VertexData, EdgeData>::~SegmentMesh()
{
}

template <class VertexData, class EdgeData>
SegmentMesh<VertexData, EdgeData>::SegmentMesh(SegmentMesh<VertexData, EdgeData>&& other) :
	TriangleMeshType(std::move(other))
{
}

template <class VertexData, class EdgeData>
SegmentMesh<VertexData, EdgeData>& SegmentMesh<VertexData, EdgeData>::operator=(
	const SegmentMesh<VertexData, EdgeData>& other)
{
	return TriangleMeshType::operator=(other);
}

template <class VertexData, class EdgeData>
SegmentMesh<VertexData, EdgeData>& SegmentMesh<VertexData, EdgeData>::operator=(
	SegmentMesh<VertexData, EdgeData>&& other)
{
	return TriangleMeshType::operator=(std::move(other));
}

template <class VertexData, class EdgeData>
size_t SegmentMesh<VertexData, EdgeData>::addTriangle(const TriangleType& triangle)
{
	SURGSIM_FAILURE() << "Cannot insert triangle into segment mesh.";
	return static_cast<size_t>(0);
}

template <class VertexData, class EdgeData>
size_t SegmentMesh<VertexData, EdgeData>::getNumTriangles() const
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
	return static_cast<size_t>(0);
}

template <class VertexData, class EdgeData>
const std::vector<typename SegmentMesh<VertexData, EdgeData>::TriangleType>&
	SegmentMesh<VertexData, EdgeData>::getTriangles() const
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
	return TriangleMeshType::getTriangles();
}

template <class VertexData, class EdgeData>
std::vector<typename SegmentMesh<VertexData, EdgeData>::TriangleType>&
	SegmentMesh<VertexData, EdgeData>::getTriangles()
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
	return TriangleMeshType::getTriangles();
}

template <class VertexData, class EdgeData>
const typename SegmentMesh<VertexData, EdgeData>::TriangleType&
	SegmentMesh<VertexData, EdgeData>::getTriangle(size_t id) const
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
	return TriangleMeshType::getTriangle(id);
}

template <class VertexData, class EdgeData>
typename SegmentMesh<VertexData, EdgeData>::TriangleType&
	SegmentMesh<VertexData, EdgeData>::getTriangle(size_t id)
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
	return TriangleMeshType::getTriangle(id);
}

template <class VertexData, class EdgeData>
void SegmentMesh<VertexData, EdgeData>::removeTriangle(size_t id)
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
}

template <class VertexData, class EdgeData>
std::array<SurgSim::Math::Vector3d, 3>
	SegmentMesh<VertexData, EdgeData>::getTrianglePositions(size_t id) const
{
	using SurgSim::Math::Vector3d;
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
	std::array<Vector3d, 3> result =
	{{
		Vector3d::Zero(),
		Vector3d::Zero(),
		Vector3d::Zero()
	}};
	return result;
}

template <class VertexData, class EdgeData>
void SegmentMesh<VertexData, EdgeData>::doClearTriangles()
{
	SURGSIM_FAILURE() << "No triangles present in segment mesh.";
}

template <class VertexData, class EdgeData>
void SegmentMesh<VertexData, EdgeData>::doClear()
{
	TriangleMeshType::doClearEdges();
	TriangleMeshType::doClearVertices();
}

}  // namespace DataStructures
}  // namespace SurgSim

#endif  // SURGSIM_DATASTRUCTURES_SEGMENTMESH_INL_H
