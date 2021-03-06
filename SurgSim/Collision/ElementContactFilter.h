// This file is a part of the OpenSurgSim project.
// Copyright 2013-2016, SimQuest Solutions Inc.
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

#ifndef SURGSIM_COLLISION_ELEMENTCONTACTFILTER_H
#define SURGSIM_COLLISION_ELEMENTCONTACTFILTER_H

#include "SurgSim/Collision/ContactFilter.h"
#include "SurgSim/DataStructures/Location.h"
#include "SurgSim/Framework/LockedContainer.h"

namespace SurgSim
{

namespace Physics
{
class PhysicsManagerState;
}

namespace Collision
{
class CollisionPair;
class Representation;
struct Contact;

SURGSIM_STATIC_REGISTRATION(ElementContactFilter);

/// Given a DeformableCollisionRepresentation this filter can remove contacts on specific elements of that
/// representation. The contacts will be flatly removed.
class ElementContactFilter : public ContactFilter
{
public:
	explicit ElementContactFilter(const std::string& name);

	SURGSIM_CLASSNAME(SurgSim::Collision::ElementContactFilter);

	bool doInitialize() override;

	bool doWakeUp() override;

	/// Set the filter for one representation, this means any contects on the given indices for that representation
	/// will be removed
	/// \param other The other side of the collisionpair
	/// \param indices The indices to filter from the list of contacts
	void setFilter(const std::shared_ptr<Framework::Component>& other, const std::vector<size_t>& indices);

	/// Query the filters for one specific representation
	/// \param other the representation
	/// \return the currently ignored indices for a specific element
	const std::vector<size_t>& getFilter(const std::shared_ptr<Framework::Component>& other) const;

	/// Sets the representation used for filtering, can only be used before initialization
	/// \param val the collision representation to be used for filtering
	void setRepresentation(const std::shared_ptr<SurgSim::Framework::Component>& val);

	/// \return the collision representation used for filtering
	std::shared_ptr<SurgSim::Collision::Representation> getRepresentation() const;

protected:

	typedef std::vector<std::pair<std::shared_ptr<SurgSim::Framework::Component>, std::vector<size_t>>> FilterMapType;

	void setFilterElements(const FilterMapType& filterElements);

	FilterMapType getFilterElements();

	void doFilterContacts(
		const std::shared_ptr<Physics::PhysicsManagerState>& state,
		const std::shared_ptr<CollisionPair>& pair) override;


	void doUpdate(double dt) override;

private:

	std::shared_ptr<SurgSim::Framework::Logger> m_logger;

	/// Representation whose contacts need to be filtered
	std::shared_ptr<Collision::Representation> m_representation;

	mutable boost::mutex m_writeMutex;
	std::unordered_map<Framework::Component*, std::vector<size_t>> m_writeBuffer;

	std::unordered_map<Framework::Component*, std::vector<size_t>> m_filters;

	/// Run the filter over the side of the collision pair indicated by pairIndex
	/// \param pair the collision pair that is being filtered
	/// \param pairIndex 0/1 representing first, and second values of the pair data structure
	/// \param filter indices to filter
	void executeFilter(
		const std::shared_ptr<CollisionPair>& pair,
		size_t pairIndex,
		const std::vector<size_t>& filter);
};

/// Get member of pair data via indexed access, the members of the pair have to have the same type
/// const version
/// \tparam T the member type
/// \param p the pair
/// \param i the index to access
/// \return p.first if index == 0 and p.second if index == 1
template <class T>
const T& pairAt(const std::pair<T, T>& p, size_t i)
{
	SURGSIM_ASSERT(i == 0 || i == 1) << "Index for pair must be 0 or 1.";
	return (i == 0) ? p.first : p.second;
};

/// Get member of pair data via indexed access, the members of the pair have to have the same type
/// non-const version
/// \tparam T the member type
/// \param p the pair
/// \param i the index to access
/// \return p.first if index == 0 and p.second if index == 1
template <class T>
T& pairAt(std::pair<T, T>& p, size_t i) // NOLINT
{
	SURGSIM_ASSERT(i == 0 || i == 1) << "Index for pair must be 0 or 1.";
	return (i == 0) ? p.first : p.second;
};

}
}

#endif
