
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for send data
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Sender class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
(**) - see Storage, Base and Interfaces classes definition
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>                // integer types

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
template <typename Storage, 
          template <typename T> class Base,
          template <typename T_1, template <typename Y> class T_2> class Interface>
class DSrv_Sender : public Interface<Storage, Base>
{

private:

	// Parser of the accepted data 
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		PRINT_ERR("This class must not receive data");
		
		return 0;
	}
	
public:

	DSrv_Sender()
	{
		PRINT_DBG(true, "");
	}
	
	~DSrv_Sender()
	{
		PRINT_DBG(true, "");
	}

	// Loop for the server
	void loop() noexcept
	{
		// Start
		Interface<Storage, Base>::start();
		
		// Send to receiver
		uint8_t data[] {'Q', 'w', 'E', '\0'};
		Interface<Storage, Base>::sendData(
		   typename Base<Storage>::Data_send(data, sizeof(data) / sizeof(uint8_t)), "0.0.0.0");
		
		// 
		while (m_stopLoop == false);
	}
	
	// Flag for stop the loop
	volatile bool m_stopLoop {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

