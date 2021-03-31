
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
(**) - see Storage, Base and Interfaces classes definition
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>        // integer types

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

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
		if (Storage::setData(typename Storage::Data_set(data.first, data.second - 4), true) != 0)
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
		// Start
		Interface<Storage, Base>::start(50000);
		
		// Send to itself
		uint8_t data[] = {'d','a','t','a',0,0,0,0};
		Interface<Storage, Base>::sendData(
		   typename Base<Storage>::Data_send(data, 4), "0.0.0.0", 50000, true);
		
		// Receive
		while (m_stopLoop == false)
			if (Interface<Storage, Base>::receiveData() < 0)
				break;
		
		// Complete data
		Storage::completeData();
		
		// Get data
		uint8_t *dd;
		uint32_t ds;
		typename Storage::Data_get data_get(&dd, &ds);
		Storage::getData(data_get);
		for (uint32_t i = 0; i < ds; ++i)
			std::cout << dd[i] << " ";
		std::cout << std::endl;
		
		// Check CRC
		PRINT_DBG(true, "CRC for receive data: 0x%x", Storage::completeDataCRC());
	}
	
	// Flag for stop the loop
	volatile bool m_stopLoop {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

