// This file is a part of the OpenSurgSim project.
// Copyright 2012-2015, SimQuest Solutions Inc.
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

/// \file SparseMatrixTests.cpp
/// Tests that exercise the functionality of our sparse matrices, which come
/// straight from Eigen.

#include <gtest/gtest.h>
#include <tuple>

#include "SurgSim/Math/SparseMatrix.h"

using std::tuple;
using std::tuple_element;

template <size_t N> class TypeValue
{
public:
	static const size_t value = N;
};
template <size_t N> const size_t TypeValue<N>::value;

template <typename Tuple>
class SparseMatrices : public ::testing::Test
{
public:
	typedef typename tuple_element<0, Tuple>::type T;
	static const int Opt = tuple_element<1, Tuple>::type::value;
	typedef typename tuple_element<2, Tuple>::type I;

	void SetUp() override
	{
		m_columnId = 6;
		m_rowId = 11;

		m_matrixTooSmall.resize(m_n - 1, m_m - 1);
		m_matrixTooSmall.setZero();

		m_matrixWithoutExtraCoefficients.resize(18, 18);
		m_matrixWithoutExtraCoefficients.setZero();
		m_matrixMissingCoefficients.resize(18, 18);
		m_matrixMissingCoefficients.setZero();
		m_matrixWithoutExtraCoefficientsExpected.resize(18, 18);
		m_matrixWithoutExtraCoefficientsExpected.setZero();
		for (I i = 0; i < m_rowId; i++)
		{
			for (I j = 0; j < m_columnId; j++)
			{
				m_matrixWithoutExtraCoefficients.insert(i, j) = 1.0;
				m_matrixMissingCoefficients.insert(i, j) = 1.0;
				m_matrixWithoutExtraCoefficientsExpected.insert(i, j) = 1.0;
			}
			for (I j = m_columnId + m_m; j < 18; j++)
			{
				m_matrixWithoutExtraCoefficients.insert(i, j) = 1.0;
				m_matrixMissingCoefficients.insert(i, j) = 1.0;
				m_matrixWithoutExtraCoefficientsExpected.insert(i, j) = 1.0;
			}
		}
		for (I i = m_rowId; i < m_rowId + m_n; i++)
		{
			for (I j = m_columnId; j < m_columnId + m_m; j++)
			{
				m_matrixWithoutExtraCoefficients.insert(i , j) = 1.0;
				if (i % 2 == 0 || j % 2 == 0)
				{
					m_matrixMissingCoefficients.insert(i, j) = 1.0;
				}
				m_matrixWithoutExtraCoefficientsExpected.insert(i, j) =
					(i - m_rowId == 0 && j - m_columnId == 3 ? 2 :
					 static_cast<T>(i - m_rowId + 1) * static_cast<T>(j - m_columnId + 1));
			}
		}
		for (I i = m_rowId + m_n; i < 18; i++)
		{
			for (I j = 0; j < m_columnId; j++)
			{
				m_matrixWithoutExtraCoefficients.insert(i, j) = 1.0;
				m_matrixMissingCoefficients.insert(i, j) = 1.0;
				m_matrixWithoutExtraCoefficientsExpected.insert(i, j) = 1.0;
			}
			for (I j = m_columnId + m_m; j < 18; j++)
			{
				m_matrixWithoutExtraCoefficients.insert(i, j) = 1.0;
				m_matrixMissingCoefficients.insert(i, j) = 1.0;
				m_matrixWithoutExtraCoefficientsExpected.insert(i, j) = 1.0;
			}
		}
		m_matrixWithExtraCoefficients = m_matrixWithoutExtraCoefficients;
		m_matrixWithExtraCoefficientsExpected = m_matrixWithoutExtraCoefficientsExpected;
		addElementOnBlockRowsCols(&m_matrixWithExtraCoefficients);
		addElementOnBlockRowsCols(&m_matrixWithExtraCoefficientsExpected);

		m_matrixWithoutExtraCoefficients.makeCompressed();
		m_matrixMissingCoefficients.makeCompressed();
		m_matrixWithoutExtraCoefficientsExpected.makeCompressed();
	}

	void addElementOnBlockRowsCols(Eigen::SparseMatrix<T, Opt, I>* m)
	{
		m->insert(m_rowId, m_columnId - 3) = 1.0;
		m->insert(m_rowId, m_columnId - 1) = 1.0;
		m->insert(m_rowId - 4, m_columnId) = 1.0;
		m->insert(m_rowId - 2, m_columnId) = 1.0;
		m->insert(m_rowId - 3, m_columnId + 1) = 1.0;
		m->makeCompressed();
	}

	template <class Derived>
	void TestSetWithSearchDynamic(const Derived& sub, bool subTooSmall = false, bool success = true)
	{
		using SurgSim::Math::blockWithSearch;
		using SurgSim::Math::Operation;

		SetUp();

		if (subTooSmall)
		{
			EXPECT_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m,\
						  &m_matrixWithExtraCoefficients,\
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::assign)),\
						  SurgSim::Framework::AssertionFailure);
		}
		else
		{
			// No recipient specified
			EXPECT_THROW((blockWithSearch<Derived, T, Opt, I>(sub, m_rowId, m_columnId, m_n, m_m,\
						  nullptr, &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::assign)),\
						  SurgSim::Framework::AssertionFailure);

			// Recipient too small
			EXPECT_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixTooSmall,\
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::assign)),\
						  SurgSim::Framework::AssertionFailure);

			// Recipient does not have all the block coefficients (missing coefficients in the block)
			EXPECT_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixMissingCoefficients,\
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::assign)),\
						  SurgSim::Framework::AssertionFailure);

			// Recipient is correct and sub is correct
			EXPECT_NO_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixWithExtraCoefficients,\
							 &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::assign)));

			if (success)
			{
				EXPECT_TRUE(m_matrixWithExtraCoefficients.isApprox(m_matrixWithExtraCoefficientsExpected));
			}
			else
			{
				EXPECT_FALSE(m_matrixWithExtraCoefficients.isApprox(m_matrixWithExtraCoefficientsExpected));
			}
		}
	}

	template <class Derived>
	void TestAddWithSearchDynamic(const Derived& sub, bool subTooSmall = false, bool success = true)
	{
		using SurgSim::Math::blockWithSearch;
		using SurgSim::Math::Operation;

		SetUp();

		if (subTooSmall)
		{
			EXPECT_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixWithExtraCoefficients,\
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::add)),\
						  SurgSim::Framework::AssertionFailure);
		}
		else
		{
			// No recipient specified
			EXPECT_THROW((blockWithSearch<Derived, T, Opt, I>(sub, m_rowId, m_columnId, m_n, m_m, nullptr,\
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::add)),\
						  SurgSim::Framework::AssertionFailure);

			// Recipient too small
			EXPECT_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixTooSmall,
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::add)),\
						  SurgSim::Framework::AssertionFailure);

			// Recipient does not have all the block coefficients (missing coefficients in the block)
			EXPECT_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixMissingCoefficients,
						  &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::add)),\
						  SurgSim::Framework::AssertionFailure);

			// Recipient is correct and sub is correct
			EXPECT_NO_THROW((blockWithSearch(sub, m_rowId, m_columnId, m_n, m_m, &m_matrixWithExtraCoefficients,
							 &Operation<Derived, Eigen::SparseMatrix<T, Opt, I>>::add)));

			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> dense(m_matrixWithExtraCoefficients);
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> denseSub(sub);
			auto ones = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>::Ones(m_n, m_m);
			auto expectedMatrix = denseSub.block(0, 0, m_n, m_m) + ones;
			if (success)
			{
				EXPECT_TRUE((dense.block(m_rowId, m_columnId, m_n, m_m).isApprox(expectedMatrix)));
			}
			else
			{
				EXPECT_FALSE((dense.block(m_rowId, m_columnId, m_n, m_m).isApprox(expectedMatrix)));
			}
		}
	}

	template <class T, int n, int m, int Opt>
	Eigen::Matrix<T, n, m, Opt> getStaticMatrix()
	{
		Eigen::Matrix<T, n, 1> vec1 = Eigen::Matrix<T, n, 1>::LinSpaced(1.0, n);
		Eigen::Matrix<T, 1, m> vec2 = Eigen::Matrix<T, 1, m>::LinSpaced(1.0, m);
		Eigen::Matrix<T, n, m, Opt> result = vec1 * vec2;
		if (m >= 4)
		{
			result(0, 3) = 2;
		}
		return result;
	}

	template <class T, int n, int m, int Opt>
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Opt> getDynamicMatrix()
	{
		Eigen::Matrix<T, Eigen::Dynamic, 1> vec1 = Eigen::Matrix<T, Eigen::Dynamic, 1>::LinSpaced(n, 1.0, n);
		Eigen::Matrix<T, 1, Eigen::Dynamic> vec2 = Eigen::Matrix<T, 1, Eigen::Dynamic>::LinSpaced(m, 1.0, m);
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Opt> result = vec1 * vec2;
		if (m >= 4)
		{
			result(0, 3) = 2;
		}
		return result;
	}

	template <class T, int n, int m, int Opt>
	Eigen::SparseMatrix<T, Opt> getSparseMatrix()
	{
		typedef typename Eigen::SparseVector<T, Eigen::ColMajor>::Index Index1;
		typedef typename Eigen::SparseVector<T, Eigen::RowMajor>::Index Index2;

		Eigen::SparseVector<T, Eigen::ColMajor> vec1(n);
		Eigen::SparseVector<T, Eigen::RowMajor> vec2(m);
		for (Index1 i = 0; i < n; i++)
		{
			vec1.insert(i) = static_cast<T>(i + 1);
		}
		for (Index2 i = 0; i < m; i++)
		{
			vec2.insert(i) = static_cast<T>(i + 1);
		}

		Eigen::SparseMatrix<T, Opt> result(vec1 * vec2);
		if (m >= 4)
		{
			result.coeffRef(0, 3) = 2;
		}
		return result;
	}

	template <class T, int n, int Opt>
	Eigen::SparseVector<T, Opt> getSparseVector()
	{
		typedef typename Eigen::SparseVector<T, Opt>::Index Index;

		Eigen::SparseVector<T, Opt> vec(n);
		for (Index i = 0; i < n; i++)
		{
			vec.insert(i) = static_cast<T>(i + 1);
		}

		return vec;
	}

protected:
	Eigen::SparseMatrix<T, Opt, I> m_matrixWithoutExtraCoefficients;
	Eigen::SparseMatrix<T, Opt, I> m_matrixWithoutExtraCoefficientsExpected;
	Eigen::SparseMatrix<T, Opt, I> m_matrixWithExtraCoefficients;
	Eigen::SparseMatrix<T, Opt, I> m_matrixWithExtraCoefficientsExpected;
	Eigen::SparseMatrix<T, Opt, I> m_matrixTooSmall;
	Eigen::SparseMatrix<T, Opt, I> m_matrixMissingCoefficients;

	I m_rowId, m_columnId;
	static const I m_n = 4;
	static const I m_m = 4;
};
template <typename Tuple> const typename tuple_element<2, Tuple>::type SparseMatrices<Tuple>::m_n;
template <typename Tuple> const typename tuple_element<2, Tuple>::type SparseMatrices<Tuple>::m_m;

template <typename Tuple>
const int SparseMatrices<Tuple>::Opt;

typedef ::testing::Types <
tuple<double, TypeValue<Eigen::ColMajor>, int>,
	  tuple<double, TypeValue<Eigen::RowMajor>, int>,
	  tuple<double, TypeValue<Eigen::ColMajor>, ptrdiff_t>,
	  tuple<double, TypeValue<Eigen::RowMajor>, ptrdiff_t>,
	  tuple<float, TypeValue<Eigen::ColMajor>, int>,
	  tuple<float, TypeValue<Eigen::RowMajor>, int>,
	  tuple<float, TypeValue<Eigen::ColMajor>, ptrdiff_t>,
	  tuple<float, TypeValue<Eigen::RowMajor>, ptrdiff_t >> MyTypes;
TYPED_TEST_CASE(SparseMatrices, MyTypes);

TYPED_TEST(SparseMatrices, setSubMatrixWithSearchDynamicCall)
{
	typedef typename tuple_element<0, TypeParam>::type T;
	const int Opt = tuple_element<1, TypeParam>::type::value;
	const int OtherOpt = (Opt == Eigen::ColMajor ? Eigen::RowMajor : Eigen::ColMajor);

	{
		SCOPED_TRACE("Test with static dense input sub-matrix");
		this->TestSetWithSearchDynamic(this->template getStaticMatrix<T, 3, 4, Opt>(), true); ///< Sub too small (1D)
		this->TestSetWithSearchDynamic(this->template getStaticMatrix<T, 3, 3, Opt>(), true); ///< Sub too small (2D)
		this->TestSetWithSearchDynamic(this->template getStaticMatrix<T, 4, 4, Opt>());
		this->TestSetWithSearchDynamic(this->template getStaticMatrix<T, 4, 4, OtherOpt>());
		this->TestSetWithSearchDynamic(this->template getStaticMatrix<T, 6, 4, Opt>()); ///< Sub larger (1D)
		this->TestSetWithSearchDynamic(this->template getStaticMatrix<T, 5, 6, Opt>()); ///< Sub larger (2D)
	}

	{
		SCOPED_TRACE("Test with dynamic dense input sub-matrix");
		this->TestSetWithSearchDynamic(this->template getDynamicMatrix<T, 3, 4, Opt>(), true); ///< Sub too small (1D)
		this->TestSetWithSearchDynamic(this->template getDynamicMatrix<T, 3, 3, Opt>(), true); ///< Sub too small (2D)
		this->TestSetWithSearchDynamic(this->template getDynamicMatrix<T, 4, 4, Opt>());
		this->TestSetWithSearchDynamic(this->template getDynamicMatrix<T, 4, 4, OtherOpt>());
		this->TestSetWithSearchDynamic(this->template getDynamicMatrix<T, 6, 4, Opt>()); ///< Sub larger (1D)
		this->TestSetWithSearchDynamic(this->template getDynamicMatrix<T, 5, 6, Opt>()); ///< Sub larger (2D)
	}

	{
		SCOPED_TRACE("Test with sparse input sub-matrix");
		this->TestSetWithSearchDynamic(this->template getSparseMatrix<T, 3, 4, Opt>(), true); ///< Sub too small (1D)
		this->TestSetWithSearchDynamic(this->template getSparseMatrix<T, 3, 3, Opt>(), true); ///< Sub too small (2D)
		this->TestSetWithSearchDynamic(this->template getSparseMatrix<T, 4, 4, Opt>());
		/// Wrong SparseMatrix alignment: in general, this may lead to Eigen failure (program exit).
		//this->TestSetWithSearchDynamic(this->template getSparseMatrix<T, 4, 4, OtherOpt>(), false, false);
		this->TestSetWithSearchDynamic(this->template getSparseMatrix<T, 6, 4, Opt>()); ///< Sub larger (1D)
		this->TestSetWithSearchDynamic(this->template getSparseMatrix<T, 5, 6, Opt>()); ///< Sub larger (2D)
	}
}

TYPED_TEST(SparseMatrices, addSubMatrixWithSearchDynamicCall)
{
	typedef typename tuple_element<0, TypeParam>::type T;
	const int Opt = tuple_element<1, TypeParam>::type::value;
	const int OtherOpt = (Opt == Eigen::ColMajor ? Eigen::RowMajor : Eigen::ColMajor);

	{
		SCOPED_TRACE("Test with static dense input sub-matrix");
		this->TestAddWithSearchDynamic(this->template getStaticMatrix<T, 3, 4, Opt>(), true); ///< Sub too small (1D)
		this->TestAddWithSearchDynamic(this->template getStaticMatrix<T, 3, 3, Opt>(), true); ///< Sub too small (2D)
		this->TestAddWithSearchDynamic(this->template getStaticMatrix<T, 4, 4, Opt>());
		this->TestAddWithSearchDynamic(this->template getStaticMatrix<T, 4, 4, OtherOpt>());
		this->TestAddWithSearchDynamic(this->template getStaticMatrix<T, 6, 4, Opt>()); ///< Sub larger (1D)
		this->TestAddWithSearchDynamic(this->template getStaticMatrix<T, 5, 6, Opt>()); ///< Sub larger (2D)
	}

	{
		SCOPED_TRACE("Test with dynamic dense input sub-matrix");
		this->TestAddWithSearchDynamic(this->template getDynamicMatrix<T, 3, 4, Opt>(), true); ///< Sub too small (1D)
		this->TestAddWithSearchDynamic(this->template getDynamicMatrix<T, 3, 3, Opt>(), true); ///< Sub too small (2D)
		this->TestAddWithSearchDynamic(this->template getDynamicMatrix<T, 4, 4, Opt>());
		this->TestAddWithSearchDynamic(this->template getDynamicMatrix<T, 4, 4, OtherOpt>());
		this->TestAddWithSearchDynamic(this->template getDynamicMatrix<T, 6, 4, Opt>()); ///< Sub larger (1D)
		this->TestAddWithSearchDynamic(this->template getDynamicMatrix<T, 5, 6, Opt>()); ///< Sub larger (2D)
	}

	{
		SCOPED_TRACE("Test with sparse input sub-matrix");
		this->TestAddWithSearchDynamic(this->template getSparseMatrix<T, 3, 4, Opt>(), true); ///< Sub too small (1D)
		this->TestAddWithSearchDynamic(this->template getSparseMatrix<T, 3, 3, Opt>(), true); ///< Sub too small (2D)
		this->TestAddWithSearchDynamic(this->template getSparseMatrix<T, 4, 4, Opt>());
		/// Wrong SparseMatrix alignment: in general, this may lead to Eigen failure (program exit).
		//this->TestAddWithSearchDynamic(this->template getSparseMatrix<T, 4, 4, OtherOpt>(), false, false);
		this->TestAddWithSearchDynamic(this->template getSparseMatrix<T, 6, 4, Opt>()); ///< Sub larger (1D)
		this->TestAddWithSearchDynamic(this->template getSparseMatrix<T, 5, 6, Opt>()); ///< Sub larger (2D)
	}
}


