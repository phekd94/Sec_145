
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for test a load
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Load class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
(**) - see Storage and Base classes definition
*/

//-------------------------------------------------------------------------------------------------

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
// Test class definition

//-------------------------------------------------------------------------------------------------
template <typename Storage, 
          template <typename T> class Base,
          template <typename T_1, template <typename Y> class T_2> class Interface>
class DSrv_Load : public Interface<Storage, Base>
{

private:

	// Parser of the accepted data 
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		if (Storage::setData(typename Storage::Data_set(data.first, data.second), true) != 0)
		{
			PRINT_ERR("setData()");
			return -1;
		}
		/*if (Storage::completeData() != 0)
		{
			PRINT_ERR("completeData()");
			return -1;
		}*/
		return 0;
	}
	
public:

	// Loop for the server
	void loop() noexcept
	{
		Interface<Storage, Base>::start(50000);
		
		while (m_stopLoop == false)
			if (Interface<Storage, Base>::receiveData() < 0)
				break;
		
		Storage::completeData();
	}
	
	// Flag for stop the loop
	volatile bool m_stopLoop {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

