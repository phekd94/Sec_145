
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class is a disjoint set for given types
TODO: 
 * test class (logic() method)
FIXME:
DANGER:
NOTE:

Sec_145::DisjointSet<> class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      NO       |    YES(*)  |
+---------------+------------+
(*) - set is not valid after clearDisjointSet() is called
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>    // integer types
#include <vector>     // std::vector
#include <exception>  // std::exception
#include <stdexcept>  // std::logic_error

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Template class is a disjoint set for given types
template <typename T> class DisjointSet
{
public:

	explicit DisjointSet()
	{
		PRINT_DBG(m_debug, "");
	}

	// Adds a member to disjoint set
	int32_t addMember(const T& member) noexcept;

	// Clears a disjoint set
	void clearDisjointSet() noexcept
	{
		m_d_set.clear();
	}

	// ***********************
	// ******* Getters *******
	// ***********************

	// Gets a disjoint set
	const std::vector<std::vector<T>>& getDisjointSet() const noexcept
	{ 
		return m_d_set; 
	}

	// Gets a size of disjoint set
	uint32_t getDisjointSetSize() const noexcept
	{ 
		return static_cast<uint32_t>(m_d_set.size()); 
	}

	// Gets a set from disjoint set
	// (std::out_of_range exception can be thrown)
	const std::vector<T>& getSet(const uint32_t index) const
	{
		return m_d_set.at(index);
	}

	// Gets a size of set from disjoint set
	// (std::out_of_range exception can be thrown)
	uint32_t getSetSize(const uint32_t index) const
	{
		return static_cast<uint32_t>(m_d_set.at(index).size());
	}

	// Gets an index of set with maximum size
	// (std::logic_error exception can be thrown)
	uint32_t getIndexOfSetWithMaxSize() const;

	// ***********************
	// ******* Setters *******
	// ***********************

	// Sets a metric
	void setMetric(const uint32_t metric) noexcept
	{ 
		m_metric = metric; 
	}

	// Sets a debug enable flag
	void setDebug(const bool d) noexcept
	{ 
		m_debug = d; 
	}

protected:
	
	// Enable a debug output
	bool m_debug {false};

private:

	// Disjoint set
	std::vector<std::vector<T>> m_d_set;

	// Metric
	uint32_t m_metric {30};

	// Empty set
	const std::vector<T> empty_set;

	// Unites sets in a disjoint set
	int32_t unionSets(const uint32_t index_dest, const uint32_t index_src) noexcept;
};

//=================================================================================================
// ************************************************
// ******* DisjointSet class implementation *******
// ************************************************

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t DisjointSet<T>::addMember(const T& member) noexcept
{
	// Catch a push_back() exception
	try {

	// Flag; set for add was not found
	bool added {false};

	// Flag; some suitable sets were found
	bool addSuit {false};

	// Indices of some suitable sets
	std::vector<uint32_t> suitSets;

	// Sets in disjoint set
	for (uint32_t i = 0; i < m_d_set.size(); ++i) 
	{
		// Members in set
		for (uint32_t j = 0; j < m_d_set[i].size(); ++j) 
		{
			// Compare a member in set and a new member
			if (member.compareWithMetric(m_d_set[i][j], m_metric) == true)
			{
				// If member has not been added
				if (false == added) 
				{
					// Add a new member
					m_d_set[i].push_back(member);

					// Set a flag (new member was added)
					added = true;
				}

				// Add index of set and set a flag
				suitSets.push_back(i);
				addSuit = true;
			}

			// Break the set loop if members are coincided
			if (true == addSuit)
			{
				addSuit = false;
				break;
			}
		}
	}

	// Add new set in disjoint set
	if (false == added) 
	{
		m_d_set.push_back(std::vector<T>(1, member));
		return 0;
	}

	// Unite sets which have common member
	if (suitSets.size() > 1)
	{
		uint32_t vector_dest = suitSets[0];
		for (uint32_t i = 1; i < suitSets.size(); ++i)
		{
			if (unionSets(vector_dest, suitSets[i]) != 0)
			{
				PRINT_ERR(true, "unionSets(%lu, %lu)", 
						  static_cast<unsigned long>(vector_dest),
				          static_cast<unsigned long>(suitSets[i]));
				return -1;
			}
		}
	}

	return 0;

	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, "Exception (%s) during push_back() member has been occured",
		          obj.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename T>
uint32_t DisjointSet<T>::getIndexOfSetWithMaxSize() const
{
	// Check a size of disjoint set
	if (getDisjointSetSize() == 0) 
	{
		throw std::logic_error("disjoint set is empty");
	}

	// Search for the largest set
	uint32_t index {0}, max_size {0};
	for (uint32_t i = 0; i < m_d_set.size(); ++i)
	{
		if (static_cast<uint32_t>(m_d_set[i].size()) > max_size)
		{
			index = i;
			max_size = static_cast<uint32_t>(m_d_set[i].size());
		}
	}

	// Return an index in disjoint set
	return index;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t DisjointSet<T>::unionSets(const uint32_t index_dest, const uint32_t index_src) noexcept
{
	// Check the incoming parameters
	if (index_dest >= m_d_set.size() || index_src >= m_d_set.size()) 
	{
		PRINT_ERR(true, "index_dest(%lu) or index_src(%lu) > size(%lu)",
		          static_cast<unsigned long>(index_dest),
		          static_cast<unsigned long>(index_src),
		          static_cast<unsigned long>(m_d_set.size()));
		return -1;
	}

	// Unite sets
	//  Catch an insert() exception
	try
	{
		m_d_set[index_dest].insert(m_d_set[index_dest].end(),
		                           m_d_set[index_src].begin(), m_d_set[index_src].end());
	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, "Exception (%s) during insert() set has been occured", obj.what());
		return -1;
	}

	// Clear a source set
	m_d_set[index_src].clear();

	PRINT_DBG(m_debug, "Set %lu has been inserted in set %lu",
	          static_cast<unsigned long>(index_dest),
	          static_cast<unsigned long>(index_src));
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
