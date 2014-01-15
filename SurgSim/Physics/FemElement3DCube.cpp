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

#include "SurgSim/Framework/Log.h"

#include "SurgSim/Physics/DeformableRepresentationState.h"
#include "SurgSim/Physics/FemElement3DCube.h"

using SurgSim::Math::addSubMatrix;
using SurgSim::Math::addSubVector;
using SurgSim::Math::getSubVector;
using SurgSim::Math::Vector;
using SurgSim::Math::Vector3d;

namespace SurgSim
{

namespace Physics
{

FemElement3DCube::FemElement3DCube(std::array<unsigned int, 8> nodeIds,
												 const DeformableRepresentationState& restState)
{
	using SurgSim::Framework::Logger;

	// Set the number of dof per node (3 in this case)
	setNumDofPerNode(3);

	// Set the shape functions coefficients
	// Ni(epsilon, eta, mu) = (1 + epsilon * sign(epsilon_i))(1 + eta * sign(eta_i))(1 + mu * sign(mu_i))/8
	std::array<double, 8> tmpEpsilon = {{-1.0, +1.0, +1.0, -1.0, -1.0, +1.0, +1.0, -1.0}};
	std::array<double, 8> tmpEta     = {{-1.0, -1.0, +1.0, +1.0, -1.0, -1.0, +1.0, +1.0}};
	std::array<double, 8> tmpMu      = {{-1.0, -1.0, -1.0, -1.0, +1.0, +1.0, +1.0, +1.0}};
	m_shapeFunctionsEpsilonSign = tmpEpsilon;
	m_shapeFunctionsEtaSign     = tmpEta;
	m_shapeFunctionsMuSign      = tmpMu;

	// Store the 8 nodeIds in order
	for (auto nodeId = nodeIds.cbegin(); nodeId != nodeIds.cend(); nodeId++)
	{
		SURGSIM_ASSERT(*nodeId >= 0 && *nodeId < restState.getNumNodes()) <<
			"Invalid nodeId " << *nodeId << " expected in range [0.." << restState.getNumNodes()-1 << "]";
		m_nodeIds.push_back(*nodeId);
	}

	// Store the rest state for this cube in m_elementRestPosition
	getSubVector(restState.getPositions(), m_nodeIds, 3, &m_elementRestPosition);

	// Compute the cube rest volume
	m_restVolume = getVolume(restState);
}

void FemElement3DCube::initialize(const DeformableRepresentationState& state)
{
	// Test the validity of the physical parameters
	FemElement::initialize(state);

	// Pre-compute the mass and stiffness matrix
	computeMass(state, &m_mass);
	computeStiffness(state, &m_strain, &m_stress, &m_stiffness);
}

void FemElement3DCube::addForce(const DeformableRepresentationState& state,
									   const Eigen::Matrix<double, 24, 24>& k, SurgSim::Math::Vector* F, double scale)
{
	Eigen::Matrix<double, 24, 1> x, f;

	// K.U = Fext
	// K.(x - x0) = Fext
	// 0 = Fext + Fint     with Fint = -K.(x - x0)
	getSubVector(state.getPositions(), m_nodeIds, 3, &x);
	f = (- scale) * k * (x - m_elementRestPosition);
	addSubVector(f, m_nodeIds, 3, F);
}

void FemElement3DCube::addForce(const DeformableRepresentationState& state, SurgSim::Math::Vector* F,
									   double scale)
{
	addForce(state, m_stiffness, F, scale);
}

void FemElement3DCube::computeMass(const DeformableRepresentationState& state,
										  Eigen::Matrix<double, 24, 24, Eigen::DontAlign>* M)
{
	using SurgSim::Math::gaussQuadrature2Points;

	// From the internal documentation on cube mass matrix calculation, we have:
	// M = rho * integration{over volume} {phi^T.phi} dV
	// with phi = (N0  0  0 N1  0  0...)
	//            ( 0 N0  0  0 N1  0...)
	//            ( 0  0 N0  0  0 N1...)
	// a 3x24 matrix filled with shape functions evaluation at a given point.
	// Using the Gauss-Legendre quadrature to evaluate this integral, we have:
	// M = sum{i=1..2} sum{j=1..2} sum{k=1..2}(w_i * w_j * _w_k * det(J) * rho * phi^T.phi)

	// Zero out the mass matrix
	M->setZero();

	// Build up the mass matrix using a 2-points Gauss-Legendre quadrature
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				addMassMatrixAtPoint(state,
					gaussQuadrature2Points[i], gaussQuadrature2Points[j], gaussQuadrature2Points[k], M);
			}
		}
	}
}

void FemElement3DCube::addMass(const DeformableRepresentationState& state, SurgSim::Math::Matrix* M,
									  double scale)
{
	addSubMatrix(m_mass * scale, m_nodeIds, 3, M);
}

void FemElement3DCube::addDamping(const DeformableRepresentationState& state, SurgSim::Math::Matrix* D,
										 double scale)
{
}

void FemElement3DCube::computeStiffness(const DeformableRepresentationState& state,
											   Eigen::Matrix<double, 6, 24, Eigen::DontAlign>* strain,
											   Eigen::Matrix<double, 6, 24, Eigen::DontAlign>* stress,
											   Eigen::Matrix<double, 24, 24, Eigen::DontAlign>* stiffness)
{
	using SurgSim::Math::gaussQuadrature2Points;

	// Zero out the stiffness matrix
	stiffness->setZero();

	// Build up the stiffness matrix using a 2-points Gauss-Legendre quadrature
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				addStrainStressStiffnessAtPoint(state,
					gaussQuadrature2Points[i], gaussQuadrature2Points[j], gaussQuadrature2Points[k],
					strain, stress, stiffness);
			}
		}
	}
}

void FemElement3DCube::evaluateJ(const DeformableRepresentationState& state, double epsilon, double eta, double mu,
								 SurgSim::Math::Matrix33d *J,
								 SurgSim::Math::Matrix33d *Jinv,
								 double *detJ) const
{
	using SurgSim::Framework::Logger;

	SURGSIM_ASSERT(J != nullptr) << "Trying to evaluate J with a nullptr for matrix J";

	Vector3d p[8];
	for (size_t index = 0; index < 8; index++)
	{
		p[index] = state.getPosition(m_nodeIds[index]);
	}

	// Zero out the matrix J
	J->setZero();

	// Compute J = d(x,y,z)/d(epsilon,eta,mu)
	// Note that (x,y,z) = for(i in {0..7}){ (x,y,z) += (xi,yi,zi).Ni(epsilon,eta,mu)}
	for (size_t index = 0; index < 8; ++index)
	{
		for(size_t axis = 0; axis < 3; ++axis)
		{
			(*J)(0, axis) += p[index][axis] * dShapeFunctiondepsilon(index, epsilon, eta, mu);
			(*J)(1, axis) += p[index][axis] * dShapeFunctiondeta    (index, epsilon, eta, mu);
			(*J)(2, axis) += p[index][axis] * dShapeFunctiondmu     (index, epsilon, eta, mu);
		}
	}

	if (Jinv != nullptr && detJ != nullptr)
	{
		bool invertible;
		J->computeInverseAndDetWithCheck(*Jinv, *detJ, invertible);

		SURGSIM_ASSERT(invertible) <<
			"Found a non invertible matrix J\n" << *J << "\ndet(J)=" << *detJ <<
			") while computing FemElement3DCube stiffness matrix\n";
		SURGSIM_LOG_IF(*detJ <= 1e-8 && *detJ >= -1e-8, Logger::getLogger("Physics"), WARNING) <<
			"Found an invalid matrix J\n" << *J << "\ninvertible, but det(J)=" << *detJ <<
			") while computing FemElement3DCube stiffness matrix\n";
	}
}

void FemElement3DCube::evaluateStrainDisplacement(double epsilon, double eta, double mu,
												  const SurgSim::Math::Matrix33d& Jinv,
												  Eigen::Matrix<double, 6, 24, Eigen::DontAlign> *B) const
{
	SURGSIM_ASSERT(B != nullptr) << "Trying to evaluate the strain-displacmenet with a nullptr";

	// Zero out the strain-displacement
	B->setZero();

	// Set non-zero entries of the strain-displacement
	for (size_t index = 0; index < 8; ++index)
	{
		// Compute dNi/d(x,y,z) = dNi/d(epsilon,eta,mu) d(epsilon,eta,mu)/d(x,y,z)
		//                      = J^{-1}.dNi/d(epsilon,eta,mu)
		Vector3d dNidEpsilonEtaMu(
			dShapeFunctiondepsilon(index, epsilon, eta, mu),
			dShapeFunctiondeta(index, epsilon, eta, mu),
			dShapeFunctiondmu(index, epsilon, eta, mu)
		);
		Vector3d dNidxyz = Jinv * dNidEpsilonEtaMu;

		// B = (dNi/dx      0      0)
		//     (     0 dNi/dy      0)
		//     (     0      0 dNi/dz)
		//     (dNi/dy dNi/dx      0)
		//     (     0 dNi/dz dNi/dy)
		//     (dNi/dz      0 dNi/dx)
		// c.f. http://www.colorado.edu/engineering/CAS/courses.d/AFEM.d/AFEM.Ch11.d/AFEM.Ch11.pdf
		(*B)(0, getNumDofPerNode() * index    ) = dNidxyz[0];
		(*B)(1, getNumDofPerNode() * index + 1) = dNidxyz[1];
		(*B)(2, getNumDofPerNode() * index + 2) = dNidxyz[2];
		(*B)(3, getNumDofPerNode() * index    ) = dNidxyz[1];
		(*B)(3, getNumDofPerNode() * index + 1) = dNidxyz[0];
		(*B)(4, getNumDofPerNode() * index + 1) = dNidxyz[2];
		(*B)(4, getNumDofPerNode() * index + 2) = dNidxyz[1];
		(*B)(5, getNumDofPerNode() * index    ) = dNidxyz[2];
		(*B)(5, getNumDofPerNode() * index + 2) = dNidxyz[0];
	}
}

void FemElement3DCube::buildConstitutiveMaterialMatrix(
	Eigen::Matrix<double, 6, 6, Eigen::DontAlign>* constitutiveMatrix)
{
	// Compute the elasticity material matrix
	// which is commonly based on the Lame coefficients (1st = lambda, 2nd = mu = shear modulus):
	double lambda = m_E * m_nu / ((1.0 + m_nu) * (1.0 - 2.0 * m_nu));
	double mu = m_E / (2.0 * (1 + m_nu));
	constitutiveMatrix->setZero();
	(*constitutiveMatrix)(0, 0) = (*constitutiveMatrix)(1, 1) = (*constitutiveMatrix)(2, 2) = 2.0 * mu + lambda;
	(*constitutiveMatrix)(0, 1) = (*constitutiveMatrix)(0, 2) = (*constitutiveMatrix)(1, 0) = lambda;
	(*constitutiveMatrix)(1, 2) = (*constitutiveMatrix)(2, 0) = (*constitutiveMatrix)(2, 1) = lambda;
	(*constitutiveMatrix)(3, 3) = (*constitutiveMatrix)(4, 4) = (*constitutiveMatrix)(5, 5) = mu;
}

void FemElement3DCube::addStrainStressStiffnessAtPoint(const DeformableRepresentationState& state,
	const SurgSim::Math::gaussQuadraturePoint& epsilon,
	const SurgSim::Math::gaussQuadraturePoint& eta,
	const SurgSim::Math::gaussQuadraturePoint& mu,
	Eigen::Matrix<double, 6, 24, Eigen::DontAlign>* strain,
	Eigen::Matrix<double, 6, 24, Eigen::DontAlign>* stress,
	Eigen::Matrix<double, 24, 24, Eigen::DontAlign>* k)
{
	SurgSim::Math::Matrix33d J, Jinv;
	double detJ;
	Eigen::Matrix<double, 6, 24, Eigen::DontAlign> B;

	evaluateJ(state, epsilon.point, eta.point, mu.point, &J, &Jinv, &detJ);

	evaluateStrainDisplacement(epsilon.point, eta.point, mu.point, Jinv, &B);

	buildConstitutiveMaterialMatrix(&m_constitutiveMaterial);

	*strain += (epsilon.weight * eta.weight * mu.weight * detJ) * B;
	*stress += (epsilon.weight * eta.weight * mu.weight * detJ) * m_constitutiveMaterial * B;
	*k += (epsilon.weight * eta.weight * mu.weight * detJ) * B.transpose() * m_constitutiveMaterial * B;
}

void FemElement3DCube::addMassMatrixAtPoint(const DeformableRepresentationState& state,
	const SurgSim::Math::gaussQuadraturePoint& epsilon,
	const SurgSim::Math::gaussQuadraturePoint& eta,
	const SurgSim::Math::gaussQuadraturePoint& mu,
	Eigen::Matrix<double, 24, 24, Eigen::DontAlign>* m)
{
	// This helper method hels to compute:
	// M = rho * integration{over volume} {phi^T.phi} dV
	//   = sum{i=1..2} sum{j=1..2} sum{k=1..2} (w_i * w_j * w_k * det(J) * rho * phi^T.phi)
	// with phi = (N0  0  0 N1  0  0...)   a 3x24 matrix filled with shape functions
	//            ( 0 N0  0  0 N1  0...)   evaluation at a given point.
	//            ( 0  0 N0  0  0 N1...)
	// by computing the term inside the sums: (w_i * w_j * w_k * det(J) * rho * phi^T.phi)
	SurgSim::Math::Matrix33d J, Jinv;
	SurgSim::Math::Matrix phi(3, 24);
	double detJ;

	evaluateJ(state, epsilon.point, eta.point, mu.point, &J, &Jinv, &detJ);

	phi.setZero();
	for (size_t index = 0; index < 8; ++index)
	{
		double weightPerIndex = shapeFunction(index, epsilon.point, eta.point, mu.point);
		phi(0, getNumDofPerNode() * index    ) += weightPerIndex;
		phi(1, getNumDofPerNode() * index + 1) += weightPerIndex;
		phi(2, getNumDofPerNode() * index + 2) += weightPerIndex;
	}

	*m += (epsilon.weight * eta.weight * mu.weight * detJ * m_rho) * phi.transpose() * phi;
}

void FemElement3DCube::addStiffness(const DeformableRepresentationState& state, SurgSim::Math::Matrix* K,
										   double scale)
{
	addSubMatrix(m_stiffness * scale, getNodeIds(), 3, K);
}

void FemElement3DCube::addFMDK(const DeformableRepresentationState& state,
	SurgSim::Math::Vector* F,
	SurgSim::Math::Matrix* M,
	SurgSim::Math::Matrix* D,
	SurgSim::Math::Matrix* K)
{
	// Assemble the mass matrix
	addMass(state, M);

	// No damping matrix as we are using linear elasticity (not visco-elasticity)

	// Assemble the stiffness matrix
	addStiffness(state, K);

	// Assemble the force vector
	addForce(state, F);
}

void FemElement3DCube::addMatVec(const DeformableRepresentationState& state,
										double alphaM, double alphaD, double alphaK,
										const SurgSim::Math::Vector& x, SurgSim::Math::Vector* F)
{
	using SurgSim::Math::addSubVector;
	using SurgSim::Math::getSubVector;

	if (alphaM == 0.0 && alphaK == 0.0)
	{
		return;
	}

	Eigen::Matrix<double, 24, 1, Eigen::DontAlign> xElement, fElement;
	getSubVector(x, m_nodeIds, 3, &xElement);

	// Adds the mass contribution
	if (alphaM)
	{
		fElement = alphaM * (m_mass * xElement);
		addSubVector(fElement, m_nodeIds, 3, F);
	}

	// Adds the damping contribution (No damping)

	// Adds the stiffness contribution
	if (alphaK)
	{
		fElement = alphaK * (m_stiffness * xElement);
		addSubVector(fElement, m_nodeIds, 3, F);
	}
}

double FemElement3DCube::getVolume(const DeformableRepresentationState& state) const
{
	using SurgSim::Math::gaussQuadrature2Points;

	double v = 0.0;

	// Compute the volume:
	// V = integration{over volume} dV
	// Using a 2-points Gauss-Legendre quadrature:
	// V = for{i in {0..1}} for{j in {0..1}} for{k in {0..1}}
	//        V += weightEpsilon[i] * weightEta[j] * weightMu[k] * det(J(epsilon[i], eta[j], mu[k]))
	for (int i = 0; i < 2; ++i)
	{
		const double &epsilon = gaussQuadrature2Points[i].point;
		const double &weightEpsilon = gaussQuadrature2Points[i].weight;

		for (int j = 0; j < 2; ++j)
		{
			const double &eta= gaussQuadrature2Points[j].point;
			const double &weightEta = gaussQuadrature2Points[j].weight;

			for (int k = 0; k < 2; ++k)
			{
				const double &mu= gaussQuadrature2Points[k].point;
				const double &weightMu = gaussQuadrature2Points[k].weight;

				SurgSim::Math::Matrix33d J, Jinv;
				double detJ;

				evaluateJ(state, epsilon, eta, mu, &J, &Jinv, &detJ);

				v += weightEpsilon * weightEta * weightMu * detJ;
			}
		}
	}

	SURGSIM_ASSERT(v >= 0) << "FemElement3DCube ill-defined, its volume is " << v << std::endl <<
		"Please check the node ordering of your element formed by node ids " <<
		m_nodeIds[0]<<" "<<m_nodeIds[1]<<" "<<m_nodeIds[2]<<" "<<m_nodeIds[3]<<" "<<
		m_nodeIds[4]<<" "<<m_nodeIds[5]<<" "<<m_nodeIds[6]<<" "<<m_nodeIds[7]<<std::endl;

	SURGSIM_ASSERT(v > 1e-12) << "FemElement3DCube ill-defined, its volume is " << v << std::endl <<
		"Please check the node ordering of your element formed by node ids " <<
		m_nodeIds[0]<<" "<<m_nodeIds[1]<<" "<<m_nodeIds[2]<<" "<<m_nodeIds[3]<<" "<<
		m_nodeIds[4]<<" "<<m_nodeIds[5]<<" "<<m_nodeIds[6]<<" "<<m_nodeIds[7]<<std::endl;

	return v;
}

double FemElement3DCube::shapeFunction(size_t i, double epsilon, double eta, double mu) const
{
	return 1.0 / 8.0 *
		(1 + epsilon * m_shapeFunctionsEpsilonSign[i]) *
		(1 + eta * m_shapeFunctionsEtaSign[i]) *
		(1 + mu * m_shapeFunctionsMuSign[i]);
}

double FemElement3DCube::dShapeFunctiondepsilon(size_t i, double epsilon, double eta, double mu) const
{
	return 1.0 / 8.0 *
		m_shapeFunctionsEpsilonSign[i] *
		(1 + eta * m_shapeFunctionsEtaSign[i]) *
		(1 + mu * m_shapeFunctionsMuSign[i]);
}

double FemElement3DCube::dShapeFunctiondeta(size_t i, double epsilon, double eta, double mu) const
{
	return 1.0 / 8.0 *
		(1 + epsilon * m_shapeFunctionsEpsilonSign[i]) *
		m_shapeFunctionsEtaSign[i] *
		(1 + mu * m_shapeFunctionsMuSign[i]);
}

double FemElement3DCube::dShapeFunctiondmu(size_t i, double epsilon, double eta, double mu) const
{
	return 1.0 / 8.0 *
		(1 + epsilon * m_shapeFunctionsEpsilonSign[i]) *
		(1 + eta * m_shapeFunctionsEtaSign[i]) *
		m_shapeFunctionsMuSign[i];
}

bool FemElement3DCube::isValidCoordinate(const SurgSim::Math::Vector& naturalCoordinate) const
{
	SURGSIM_FAILURE() << "FemElement3DCube::isValidCoordinate is not implemented.";
	return false;
}

SurgSim::Math::Vector FemElement3DCube::computeCartesianCoordinate(
	const DeformableRepresentationState& state,
	const SurgSim::Math::Vector& naturalCoordinate) const
{
	SURGSIM_FAILURE() << "FemElement3DCube::computeCartesianCoordinate is not implemented.";
	return Vector3d(0.0, 0.0, 0.0);
}

} // namespace Physics

} // namespace SurgSim