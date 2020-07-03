
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class for represent a disjoint set
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE:

Sec_145::DisjointSet class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|       NO      |    YES(*)  |
+---------------+------------+
(*) - received sets are not valid after clearDisjointSet() is called
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>             // integer types
#include <vector>              // std::vector

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
template <typename T>
class DisjointSet
{

public:
	explicit DisjointSet(const uint32_t metric = 30);

	// Adds a member to set
	int32_t addMember(const T& member);

	// Clears a disjoint set
	void clearDisjointSet();

	// Get an index of set of set contains object
	virtual int32_t getIndexOfObjectSet()
	{ return getIndexOfSetWithMaxSize(); }


	// ***** Getters *****

	// Gets a disjoint set
	const std::vector<std::vector<T>>& getDisjointSet() const
	{ return m_d_set; }

	// Gets a size of disjoint set
	uint32_t getDisjointSetSize() const
	{ return static_cast<uint32_t>(m_d_set.size()); }

	// Gets a set from disjoint set
	const std::vector<T>& getSet(const uint32_t index) const;

	// Gets an index of set with maximum size
	int32_t getIndexOfSetWithMaxSize() const;

	// *******************


	// ***** Setters *****

	// Sets a metric
	void setMetric(const uint32_t metric)
	{ m_metric = metric; }

	// Enable/disable debug messages
	void setDebug(const bool d)
	{ m_debug = d; }

	// *******************

protected:
	// The disjoint set
	std::vector<std::vector<T>> m_d_set;

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug;

	// Metric
	uint32_t m_metric;

	// Compares members
	virtual bool compareMembers(const T&, const T&) const
	{ return false; }

private:

	// Preface in debug message
	static const char* PREF;

	// Empty set
	const std::vector<T> empty_set;

	// Unites sets in a disjoint set
	int32_t unionSets(uint32_t index_dest, uint32_t index_src);
};

//=================================================================================================
// DisjointSet class implementation

#include <cstdlib>  // std::abs()

//-------------------------------------------------------------------------------------------------
template <typename T>
const char* DisjointSet<T>::PREF = "[DisjointSet]: ";

//-------------------------------------------------------------------------------------------------
template <typename T>
DisjointSet<T>::DisjointSet(const uint32_t metric) : m_metric(metric),
                                                     m_debug(true)
{
	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t DisjointSet<T>::addMember(const T& member)
{
	// Flag; suitable set was not found
	bool added = false;

	// Flag; some suitable sets were found
	bool added_tmp = false;
	// Indices of some suitable sets
	std::vector<uint32_t> sets;

	// Sets in disjoint set
	for (uint32_t i = 0; i < m_d_set.size(); ++i) {

		// Members in set
		for (uint32_t j = 0; j < m_d_set[i].size(); ++j) {

			// Compare a member in set and a new member
			if (compareMembers(m_d_set[i][j], member) == true) {

				// If member has not been added
				if (false == added) {

					// Add a new member
					m_d_set[i].push_back(member);

					// Set a flag (new member was added)
					added = true;
				}

				// Add index of set and set a flag
				sets.push_back(i);
				added_tmp = true;
			}

			// Break the set loop if members are coincided
			if (true == added_tmp) {
				added_tmp = false;
				break;
			}
		}
	}

	// Add new set in disjoint set
	if (false == added) {
		m_d_set.push_back(std::vector<T>(1, member));
		return 0;
	}

	// Unite sets which have common member
	if (sets.size() > 1) {
		uint32_t vector_dest = sets[0];
		for (uint32_t i = 1; i < sets.size(); ++i) {
			if (unionSets(vector_dest, sets[i]) != 0) {
				return -1;
			}
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void DisjointSet<T>::clearDisjointSet()
{
	m_d_set.clear();
}

//-------------------------------------------------------------------------------------------------
template <typename T>
const std::vector<T>& DisjointSet<T>::getSet(const uint32_t index) const
{
	if (index >= getDisjointSetSize()) {

		// Index is larger or equal than the size of the disjoint set; return an empty set
		PRINT_ERR(true, PREF, "Index is larger or equal than the size of the disjoint set");
		return empty_set;

	} else {

		// Return a specified set
		return m_d_set[index];
	}
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t DisjointSet<T>::getIndexOfSetWithMaxSize() const
{
	// Check a size of disjoint set
	if (getDisjointSetSize() == 0) {
		return -1;
	}

	// Search for the largest set
	uint32_t index = 0, max_size = 0;
	for (uint32_t i = 0; i < m_d_set.size(); ++i) {
		if (m_d_set[i].size() > max_size) {
			index = i;
			max_size = static_cast<uint32_t>(m_d_set[i].size());
		}
	}

	// Return an index in disjoint set
	return index;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t DisjointSet<T>::unionSets(uint32_t index_dest, uint32_t index_src)
{
	// Check the incoming parameters
	if (index_dest >= m_d_set.size() || index_src >= m_d_set.size()) {
		PRINT_ERR(true, PREF, "index_dest(%lu) or index_src(%lu) > size(%lu)",
		          static_cast<unsigned long>(index_dest),
		          static_cast<unsigned long>(index_src),
		          static_cast<unsigned long>(m_d_set.size()));
		return -1;
	}

	// Unite sets
	m_d_set[index_dest].insert(m_d_set[index_dest].end(),
	                           m_d_set[index_src].begin(),
	                           m_d_set[index_src].end());

	// Clear a source set
	m_d_set[index_src].clear();

	PRINT_DBG(false, PREF, "Set %lu has been inserted in set %lu",
	          static_cast<unsigned long>(index_dest),
	          static_cast<unsigned long>(index_src));
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
