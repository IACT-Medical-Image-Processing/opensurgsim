// This file is a part of the OpenSurgSim project.
// Copyright 2013, SimQuest Solutions Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SURGSIM_MATH_LINEARMOTIONARITHMETIC_INL_H
#define SURGSIM_MATH_LINEARMOTIONARITHMETIC_INL_H

namespace SurgSim
{
namespace Math
{

template <class T>
LinearMotion<T>::LinearMotion() : m_start(static_cast<T>(0)), m_end(static_cast<T>(0)) {}

template <class T>
LinearMotion<T>::LinearMotion(const T& start, const T& end) : m_start(start), m_end(end) {}

template <class T>
LinearMotion<T>::LinearMotion(const LinearMotion<T>& m) : m_start(m.m_start), m_end(m.m_end) {}

template <class T>
LinearMotion<T>::LinearMotion(const LinearMotion<T>&& m)
{
	m_start = m.m_start;
	m_end = m.m_end;
}

template <class T>
LinearMotion<T>& LinearMotion<T>::operator= (const LinearMotion<T>& m)
{
	m_start = m.m_start;
	m_end = m.m_end;
	return *this;
};

template <class T>
LinearMotion<T>& LinearMotion<T>::operator= (const LinearMotion<T>&& m)
{
	m_start = m.m_start;
	m_end = m.m_end;
	return *this;
};

template <class T>
Interval<T> LinearMotion<T>::toInterval() const
{
	return Interval<T>::minToMax(m_start, m_end);
}

template <class T>
Polynomial<1, T> LinearMotion<T>::toPolynomial() const
{
	return Polynomial<1, T>(m_start, m_end - m_start);
}

template <class T>
bool LinearMotion<T>::containsZero() const
{
	return toInterval().containsZero();
}

template <class T>
bool isApprox(const LinearMotion<T>& m, const T& epsilon) const
{
	return (std::abs(m_start - i.m_start) <= epsilon) && (std::abs(m_end - i.m_end) <= epsilon);
}

template <class T>
bool LinearMotion<T>::operator ==(const LinearMotion<T>& m) const
{
	return ((m_start == m.m_start) && (m_end == m.m_end));
}

template <class T>
bool LinearMotion<T>::operator !=(const LinearMotion<T>& m) const
{
	return !(*this == m);
}

template <class T>
LinearMotion<T> LinearMotion<T>::operator +(const LinearMotion<T>& m) const
{
	return LinearMotion<T>(m_start + m.m_start, m_end + m.m_end);
}

template <class T>
LinearMotion<T>& LinearMotion<T>::operator +=(const LinearMotion<T>& m)
{
	m_start += m.m_start;
	m_end += m.m_end;
	return *this;
}

template <class T>
LinearMotion<T> LinearMotion<T>::operator -(const LinearMotion<T>& m) const
{
	return LinearMotion<T>(m_start - m.m_start, m_end - m.m_end);
}

template <class T>
LinearMotion<T>& LinearMotion<T>::operator -=(const LinearMotion<T>& m)
{
	m_start -= m.m_start;
	m_end -= m.m_end;
	return *this;
};

template <class T>
T LinearMotion<T>::getStart() const
{
	return m_start;
}

template <class T>
T LinearMotion<T>::getEnd() const
{
	return m_end;
}

template <class T>
T LinearMotion<T>::atTime(const T& t) const
{
	return ((1 - t) * m_start + t * m_end);
}

template <class T>
LinearMotion<T> LinearMotion<T>::firstHalf() const
{
	return LinearMotion<T>(m_start, (m_start + m_end) * static_cast<T>(0.5));
}

template <class T>
LinearMotion<T> LinearMotion<T>::secondHalf() const
{
	return LinearMotion<T>((m_start + m_end) * static_cast<T>(0.5), m_end);
}

// Class nD
template <class T, int N>
LinearMotion_nD<T, N>::LinearMotion_nD()
{
	for (int i = 0; i < N; i++)
	{
		m_motion[i] = LinearMotion<T>();
	}
}

template <class T, int N>
LinearMotion_nD<T, N>::LinearMotion_nD(const std::array<LinearMotion<T>, N>& x)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] = x[i];
	}
}

template <class T, int N>
LinearMotion_nD<T, N>::LinearMotion_nD(const LinearMotion_nD<T, N>& motion)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] = motion.m_motion[i];
	}
}

template <class T, int N>
LinearMotion_nD<T, N>::LinearMotion_nD(const LinearMotion_nD<T, N>&& motion)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] = motion.m_motion[i];
	}
}

template <class T, int N>
LinearMotion_nD<T, N>::LinearMotion_nD(const std::array<T, N>& a, const std::array<T, N>& b)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] = LinearMotion<T>(a[i], b[i]);
	}
}

template <class T, int N>
LinearMotion_nD<T, N>& LinearMotion<T, N>::operator =(const LinearMotion_nD<T, N>& motion)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] = motion.m_motion[i];
	}
	return *this;
}

template <class T, int N>
LinearMotion_nD<T, N>& LinearMotion<T, N>::operator =(const LinearMotion_nD<T, N>&& motion)
{
	if (this != &motion)
	{
		for (int i = 0; i < N; ++i)
		{
			m_motion[i] = motion.m_motion[i];
		}
	}

	return *this;
}

template <class T, int N>
Interval_nD<T, N> LinearMotion<T, N>::toInterval() const
{
	std::array<Interval<T>, N> motions;
	for (int i = 0; i < N; ++i)
	{
		motions[i] = m_motion[i].toInterval();
	}

	return Interval_nD<T, N>(motions);
}

template <class T, int N>
bool isApprox(const LinearMotion_nD<T, N>& motion, const T& epsilon) const
{
	for (int i = 0; i < N; i++)
	{
		if (!m_motion[i].isApprox(motion.m_interval[i], epsilon))
		{
			return false;
		}
	}
	return true;
}

template <class T, int N>
bool LinearMotion<T, N>::operator ==(const LinearMotion_nD<T, N>& motion) const
{
	for (int i = 0; i < N; ++i)
	{
		if (m_motion[i] != motion.m_motion[i])
		{
			return false;
		}
	}
	return true;
}

template <class T, int N>
bool LinearMotion<T, N>::operator !=(const LinearMotion_nD<T, N>& motion) const
{
	return !(this->operator==(motion));
}

template <class T, int N>
LinearMotion_nD<T, N> LinearMotion<T, N>::operator +(const LinearMotion_nD<T, N>& m) const
{
	LinearMotion_nD<T, N> ret(*this);
	ret += m;
	return ret;
}

template <class T, int N>
LinearMotion_nD<T, N>& LinearMotion<T, N>::operator +=(const LinearMotion_nD<T, N>& m)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] += m.m_motion[i];
	}
	return *this;
}

template <class T, int N>
LinearMotion_nD<T, N> LinearMotion<T, N>::operator -(const LinearMotion_nD<T, N>& m) const
{
	LinearMotion_nD<T, N> ret(*this);
	ret -= m;
	return ret;
}

template <class T, int N>
LinearMotion_nD<T, N>& LinearMotion<T, N>::operator -=(const LinearMotion_nD<T, N>& m)
{
	for (int i = 0; i < N; ++i)
	{
		m_motion[i] -= m.m_motion[i];
	}
	return *this;
}

template <class T, int N>
Interval_nD<T, N> LinearMotion<T, N>::operator *(const LinearMotion_nD<T, N>& m) const
{
	return this->toInterval() * m.toInterval();
}

template <class T, int N>
Interval_nD<T, N> LinearMotion<T, N>::operator /(const LinearMotion_nD<T, N>& m) const
{
	return this->toInterval() / m.toInterval();
};

template <class T, int N>
Interval<T> LinearMotion<T, N>::dotProduct(const Interval_nD<T, N>& motion) const
{
	Interval<T> ret(static_cast<T>(0), static_cast<T>(0));
	for (int i = 0 ; i < N ; i++)
	{
		ret += m_motion[i] * motion.m_motion[i];
	}
	return ret;
}

template <class T, int N>
const LinearMotion<T>& LinearMotion<T, N>::getAxis(size_t i) const
{
	return m_motion[i];
}

template <class T, int N>
void LinearMotion<T, N>::getStart(std::array<T, N>& start) const
{
	for (int i = 0; i < N; ++i)
	{
		start[i] = m_motion[i].getStart();
	}
}

template <class T, int N>
void LinearMotion<T, N>::getEnd(std::array<T, N>& end) const
{
	for (int i = 0; i < N; ++i)
	{
		end[i] = m_motion[i].getEnd();
	}
}

template <class T, int N>
LinearMotion_nD<T, N> LinearMotion<T, N>::firstHalf() const
{
	LinearMotion_nD<T, N> ret;
	for (int i = 0; i < N; ++i)
	{
		ret.m_motion[i] = m_motion[i].firstHalf();
	}
	return ret;
}

template <class T, int N>
LinearMotion_nD<T, N> LinearMotion<T, N>::secondHalf() const
{
	LinearMotion_nD<T, N> ret;
	for (int i = 0; i < N; ++i)
	{
		ret[i] = m_motion[i].secondHalf();
	}
	return ret;
}

// Special case for dimension 3
template <class T>
LinearMotion_nD::LinearMotion_nD<T, 3>()
{
	m_motion[0] = LinearMotion_nD<T>();
	m_motion[1] = LinearMotion_nD<T>();
	m_motion[2] = LinearMotion_nD<T>();
};

template <class T>
LinearMotion_nD<T, 3>::LinearMotion_nD(const std::array<LinearMotion<T>, 3>& x)
{
	m_motion[0] = x[0];
	m_motion[1] = x[1];
	m_motion[2] = x[2];
}

template <class T>
LinearMotion_nD<T, 3>::LinearMotion_nD(const LinearMotion<T>& a, const LinearMotion<T>& b, const LinearMotion<T>& c)
{
	m_motion[0] = a;
	m_motion[1] = b;
	m_motion[2] = c;
}

template <class T>
LinearMotion_nD<T, 3>::LinearMotion_nD(const LinearMotion_nD<T, 3>& motion)
{
	m_motion[0] = motion.m_motion[0];
	m_motion[1] = motion.m_motion[1];
	m_motion[2] = motion.m_motion[2];
}

template <class T>
LinearMotion_nD<T, 3>::LinearMotion_nD(const LinearMotion_nD<T, 3>&& motion)
{
	m_motion[0] = motion.m_motion[0];
	m_motion[1] = motion.m_motion[1];
	m_motion[2] = motion.m_motion[2];
}

template <class T>
LinearMotion_nD<T, 3>::LinearMotion_nD(const std::array<T, 3>& a, const std::array<T, 3>& b);
{
	m_motion[0] = LinearMotion<T>(a[0], b[0]);
	m_motion[1] = LinearMotion<T>(a[1], b[1]);
	m_motion[2] = LinearMotion<T>(a[2], b[2]);
}

template <class T>
LinearMotion_nD<T, 3>& LinearMotion_nD<T, 3>::operator =(const LinearMotion_nD<T, 3>& motion)
{
	m_motion[0] = motion.m_motion[0];
	m_motion[1] = motion.m_motion[1];
	m_motion[2] = motion.m_motion[2];
	return *this;
}

template <class T>
LinearMotion_nD<T, 3>& LinearMotion_nD<T, 3>::operator =(const LinearMotion_nD<T, 3>&& motion)
{
	m_motion[0] = motion.m_motion[0];
	m_motion[1] = motion.m_motion[1];
	m_motion[2] = motion.m_motion[2];
	return *this;
}

template <class T>
Interval_nD<T, 3> LinearMotion_nD<T, 3>::toInterval() const
{
	std::array<Interval<T>, 3> intervals;
	intervals[0] = m_motion[0].toInterval();
	intervals[1] = m_motion[1].toInterval();
	intervals[2] = m_motion[2].toInterval();
	return Interval_nD<T, 3>(motions);
}

template <class T>
bool LinearMotion_nD<T, 3>::isApprox(const LinearMotion_nD<T, 3>& motion, const T& epsilon) const
{
	return (m_motion[0].isApprox(motion.m_motion[0], epsilon) &&
			m_motion[1].isApprox(motion.m_motion[1], epsilon) &&
			m_motion[2].isApprox(motion.m_motion[2], epsilon));
}

template <class T>
bool LinearMotion_nD<T, 3>::operator ==(const LinearMotion_nD<T, 3>& motion) const
{
	return (m_motion[0] == motion.m_motion[0] &&
			m_motion[1] == motion.m_motion[1] &&
			m_motion[2] == motion.m_motion[2]);
}

template <class T>
bool LinearMotion_nD<T, 3>::operator !=(const LinearMotion_nD<T, 3>& motion) const
{
	return !(this->operator==(motion));
}

template <class T>
LinearMotion_nD<T, 3> LinearMotion_nD<T, 3>::operator +(const LinearMotion_nD<T, 3>& m) const
{
	LinearMotion_nD<T, 3> ret(*this);
	ret += m;
	return ret;
}

template <class T>
LinearMotion_nD<T, 3>& LinearMotion_nD<T, 3>::operator +=(const LinearMotion_nD<T, 3>& m)
{
	m_motion[0] += m.m_motion[0];
	m_motion[1] += m.m_motion[1];
	m_motion[2] += m.m_motion[2];
	return *this;
}

template <class T>
LinearMotion_nD<T, 3> LinearMotion_nD<T, 3>::operator -(const LinearMotion_nD<T, 3>& m) const
{
	LinearMotion_nD<T, 3> ret(*this);
	ret -= m;
	return ret;
}

template <class T>
LinearMotion_nD<T, 3>& LinearMotion_nD<T, 3>::operator -=(const LinearMotion_nD<T, 3>& m)
{
	m_motion[0] -= m.m_motion[0];
	m_motion[1] -= m.m_motion[1];
	m_motion[2] -= m.m_motion[2];
	return *this;
};

template <class T>
Interval<T> LinearMotion_nD<T, 3>::dotProduct(const LinearMotion_nD<T, 3>& motion, const Interval<T>& range) const
{
	return valuesOverInterval(analyticDotProduct(*this, motion), range);
}

template <class T>
Interval_nD<T, 3> LinearMotion_nD<T, 3>::crossProduct(const LinearMotion_nD<T, 3>& motion,
		const Interval<T>& range) const
{
	// The naive approach, i.e.
	// toInterval().crossProduct(motion.toInterval())
	// results in intervals that are way too broad.
	return Interval_nD<T, 3>(valuesOverInterval(analyticCrossProductAxis<0>(*this, motion), range),
							 valuesOverInterval(analyticCrossProductAxis<1>(*this, motion), range),
							 valuesOverInterval(analyticCrossProductAxis<2>(*this, motion), range));
}

template <class T>
Interval<T> LinearMotion_nD<T, 3>::magnitudeSquared(const Interval<T>& range) const
{
	return valuesOverInterval(analyticMagnitudeSquared(), range);
}

template <class T>
Interval<T> LinearMotion_nD<T, 3>::magnitude(const Interval<T>& range) const
{
	Interval<T> magnitudeSq = magnitudeSquared(range);
	// Both minimum and maximum are guaranteed to be non-negative.
	return Interval<T>(sqrt(magnitudeSq.getMin()), sqrt(magnitudeSq.getMax()));
}

template <class T>
const LinearMotion<T>& LinearMotion_nD<T, 3>::getAxis(size_t i) const
{
	return m_motion[i];
}

template <class T>
void LinearMotion_nD<T, 3>::getStart(std::array<T, 3>& start) const
{
	start[0] = m_motion[0].getStart();
	start[1] = m_motion[1].getStart();
	start[2] = m_motion[2].getStart();
}

template <class T>
void LinearMotion_nD<T, 3>::getEnd(std::array<T, 3>& end) const
{
	end[0] = m_motion[0].getEnd();
	end[1] = m_motion[1].getEnd();
	end[2] = m_motion[2].getEnd();
}

template <class T>
Vector3<T> LinearMotion_nD<T, 3>::getStart() const
{
	return Vector3<T>(m_motion[0].getStart(), m_motion[1].getStart(), m_motion[2].getStart());
}

template <class T>
Vector3<T> LinearMotion_nD<T, 3>::getEnd() const
{
	return Vector3<T>(m_motion[0].getEnd(), m_motion[1].getEnd(), m_motion[2].getEnd());
}

template <class T>
Vector3<T> LinearMotion_nD<T, 3>::atTime(const T& t) const
{
	return Vector3<T>(m_motion[0].atTime(t), m_motion[1].atTime(t), m_motion[2].atTime(t));
}

template <class T>
LinearMotion_nD<T, 3> LinearMotion_nD<T, 3>::firstHalf() const
{
	LinearMotion_nD<T, 3> ret;
	ret.m_motion[0] = m_motion[0].firstHalf();
	ret.m_motion[1] = m_motion[1].firstHalf();
	ret.m_motion[2] = m_motion[2].firstHalf();
	return ret;
}

template <class T>
LinearMotion_nD<T, 3> LinearMotion_nD<T, 3>::secondHalf() const
{
	LinearMotion_nD<T, 3> ret;
	ret.m_motion[0] = m_motion[0].secondHalf();
	ret.m_motion[1] = m_motion[1].secondHalf();
	ret.m_motion[2] = m_motion[2].secondHalf();
	return ret;
}

// Utility functions not part of any class

// Interval functions
template <typename T>
std::ostream& operator<< (ostream& o, const LinearMotion<T>& motion)
{
	o << "(" << motion.getStart() << " -> " << motion.getEnd() << ")";
	return o;
}

// Interval nD functions
template <typename T, int N>
std::ostream& operator<< (ostream& o, const LinearMotion_nD<T, N>& motion)
{
	o << "([" << motion.getAxis(0).getStart();
	for (int i = 1; i < N; ++i)
	{
		o << "," << motion.getAxis(i).getStart();
	}
	o << "] -> [" << motion.getAxis(0).getEnd();
	for (int i = 1; i < N; ++i)
	{
		o << "," << motion.getAxis(i).getEnd();
	}
	o << "])";
	return o;
}

// Interval 3D functions
template <class T>
Polynomial<2, T> analyticDotProduct(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b)
{
	return a.getAxis(0).toPolynomial() * b.getAxis(0).toPolynomial() +
		   a.getAxis(1).toPolynomial() * b.getAxis(1).toPolynomial() +
		   a.getAxis(2).toPolynomial() * b.getAxis(2).toPolynomial();
}

template <class T, int A>
Polynomial<2, T> analyticCrossProductAxis(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b)
{
	// The labels here are probably a bit confusing for anyone else, but at least this makes sense.
	// For A == 0, the "Y" and "Z" mean what they say, and the output is the X component.
	// For A == 1, they get rotated "down" by one (so Y -> Z, Z -> X), and the output is the Y component.
	// For A == 2, they get rotated "down" by two (so Y -> X, Z -> Y), and the output is the Z component.
	const LinearMotion<T>& aY = a.getAxis((A + 1) % 3);
	const LinearMotion<T>& aZ = a.getAxis((A + 2) % 3);
	const LinearMotion<T>& bY = b.getAxis((A + 1) % 3);
	const LinearMotion<T>& bZ = b.getAxis((A + 2) % 3);
	return aY.toPolynomial() * bZ.toPolynomial() - aZ.toPolynomial() * bY.toPolynomial();
}

template <class T>
Polynomial<2, T> analyticCrossProductXAxis(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b)
{
	return analyticCrossProductAxis<0>(a, b);
}

template <class T>
Polynomial<2, T> analyticCrossProductYAxis(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b)
{
	return analyticCrossProductAxis<1>(a, b);
}

template <class T>
Polynomial<2, T> analyticCrossProductZAxis(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b)
{
	return analyticCrossProductAxis<2>(a, b);
}

template <class T>
void analyticCrossProduct(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b,
						  Polynomial<2, T>& resultXAxis, Polynomial<2, T>& resultYAxis, Polynomial<2, T>& resultZAxis)
{
	resultXAxis = analyticCrossProductXAxis(a, b);
	resultYAxis = analyticCrossProductYAxis(a, b);
	resultZAxis = analyticCrossProductZAxis(a, b);
}

template <class T>
Polynomial << 3, T > analyticTripleProduct(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b,
		   const LinearMotion_nD<T, 3>& c)
{
	const Polynomial<1, T> aX = a.getAxis(0).toPolynomial();
	const Polynomial<1, T> aY = a.getAxis(1).toPolynomial();
	const Polynomial<1, T> aZ = a.getAxis(2).toPolynomial();
	const Polynomial<1, T> bX = b.getAxis(0).toPolynomial();
	const Polynomial<1, T> bY = b.getAxis(1).toPolynomial();
	const Polynomial<1, T> bZ = b.getAxis(2).toPolynomial();
	const Polynomial<1, T> cX = c.getAxis(0).toPolynomial();
	const Polynomial<1, T> cY = c.getAxis(1).toPolynomial();
	const Polynomial<1, T> cZ = c.getAxis(2).toPolynomial();
	return ((bY * cZ - bZ * cY) * aX + (bZ * cX - bX * cZ) * aY + (bX * cY - bY * cX) * aZ);
}

template <class T>
static Interval<T> tripleProduct(const LinearMotion_nD<T, 3>& a, const LinearMotion_nD<T, 3>& b,
								 const LinearMotion_nD<T, 3>& c, const Interval<T>& range)
{
	// The naive approach, i.e.
	// toInterval().dotProduct(motion2.crossProduct(motion3))
	// results in intervals that are way too broad.
	return valuesOverInterval(analyticTripleProduct(a, b, c), range);
}

template <class T>
Polynomial<2, T> analyticMagnitudeSquared(const LinearMotion_nD<T, 3>& motion) const;
{
	return analyticDotProduct(motion, motion);
}

#undef
#endif // SURGSIM_MATH_LINEARMOTIONARITHMETIC_INL_H
