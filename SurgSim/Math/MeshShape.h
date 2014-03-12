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

// This code is based on David Eberly's paper:
// http://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdf
// which is improving Brian Mirtich previous work (http://www.cs.berkeley.edu/~jfc/mirtich/massProps.html)
// by making the assumption that the polyhedral mesh is composed of triangles.

#ifndef SURGSIM_MATH_MESHSHAPE_H
#define SURGSIM_MATH_MESHSHAPE_H

#include "SurgSim/DataStructures/EmptyData.h"
#include "SurgSim/DataStructures/TriangleMesh.h"
#include "SurgSim/DataStructures/TriangleMeshBase.h"
#include "SurgSim/Framework/Convert.h"
#include "SurgSim/Math/Shape.h"

namespace SurgSim
{

namespace Math
{

/// Mesh shape: shape made of a triangle mesh
/// Various geometrical properties are computed from the triangle mesh using
/// David Eberly's work:
/// http://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdf
class MeshShape : public Shape
{
public:
	/// Constructor
	/// \param mesh The triangle mesh to build the shape from
	template <class VertexData, class EdgeData, class TriangleData>
	explicit MeshShape(const SurgSim::DataStructures::TriangleMeshBase<VertexData, EdgeData, TriangleData>& mesh);

	/// \return the type of the shape
	virtual int getType() override;

	/// Get mesh
	/// \return The collision mesh associated to this MeshShape
	const std::shared_ptr<SurgSim::DataStructures::TriangleMesh> getMesh() const;

	/// Get the volume of the shape
	/// \return The volume of the shape (in m-3)
	virtual double getVolume() const override;

	/// Get the volumetric center of the shape
	/// \return The center of the shape
	virtual Vector3d getCenter() const override;

	/// Get the second central moment of the volume, commonly used
	/// to calculate the moment of inertia matrix
	/// \return The 3x3 symmetric second moment matrix
	virtual Matrix33d getSecondMomentOfVolume() const override;

	/// Get the complete name of the mesh
	virtual std::string getClassName() override;

private:

	/// Compute various integral terms
	/// \note cf http://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdf
	/// \note for more details
	void computeIntegralTerms(const double w0, const double w1, const double w2,
		double* f1, double* f2, double* f3, double* g0, double* g1, double* g2) const;

	/// Compute the mass, center of mass and inertia matrix for the given triangulated mesh
	void compute();

	/// Center
	SurgSim::Math::Vector3d m_center;

	/// Volume (in m-3)
	double m_volume;

	/// Second moment of volume
	SurgSim::Math::Matrix33d m_secondMomentOfVolume;

	/// Collision mesh associated to this MeshShape
	std::shared_ptr<SurgSim::DataStructures::TriangleMesh> m_mesh;
};

}; // Math
}; // SurgSim


#include "SurgSim/Math/MeshShape-inl.h"

#endif // SURGSIM_MATH_MESHSHAPE_H
