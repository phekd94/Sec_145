
#ifndef SEC_145_ALGORITHMS_DISJOINTSET_ELLIPSES_H
#define SEC_145_ALGORITHMS_DISJOINTSET_ELLIPSES_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class for represent a disjoint set with ellipses
TODO: * test class
	  * logic() in test
	  * (*) below
FIXME:
DANGER:
NOTE:

Sec_145::DisjointSet_Ellipses class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |    YES(*)  |
+---------------+------------+
(*) -
*/

//-------------------------------------------------------------------------------------------------
#include "DisjointSet.h"  // DisjointSet template class (for inheritance)
#include "Geometry.h"     // Ellipse struct

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
#define PREF  "[DisjointSet_Ellipses]: "

//-------------------------------------------------------------------------------------------------
class DisjointSet_Ellipses : public DisjointSet<Ellipse> {

private:

	// Compares ellipses
	virtual bool compareMembers(const Ellipse& m_1, const Ellipse& m_2) const final
	{
		// Compare a member in set and a new member
		for (uint32_t i = 0; i < m_1.points.size(); ++i) {
			for (uint32_t j = 0; j < m_2.points.size(); ++j) {
				if (   std::abs(static_cast<int>(m_1.points[i].x) -
				                static_cast<int>(m_2.points[j].x)) <
				       static_cast<int>(m_metric)
				    && std::abs(static_cast<int>(m_1.points[i].y) -
				                static_cast<int>(m_2.points[j].y)) <
				       static_cast<int>(m_metric)
				    ) {

					// Members are near
					return true;
				}
			}
		}

		// Members are not near
		return false;
	}
};

//-------------------------------------------------------------------------------------------------
#undef PREF

//=================================================================================================
// Test class

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_DISJOINTSET_ELLIPSES_H
