
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
#include <cstdint>                // integer types
#include "kaitai/kaitaistream.h"  // kaitai::kstream

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
template <typename Storage, 
          template <typename T> class Base,
          template <typename T_1, template <typename Y> class T_2> class Interface,
          typename KaitaiParser /*TODO: ...*/>
class DSrv_Load : public Interface<Storage, Base>
{

private:

	// Parser of the accepted data 
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		for (uint32_t i = 0; i < data.second; ++i)
			PRINT_DBG(true, "[%u]: %u ", 
			                static_cast<unsigned int>(i),
			                static_cast<unsigned int>(data.first[i]));
	
		if (Storage::setData(typename Storage::Data_set(data.first, data.second), true) != 0)
		{
			PRINT_ERR("setData()");
			return -1;
		}
		
		//std::unique_ptr<KaitaiParser> kaitaiParser;
		
		try
		{
			kaitai::kstream ks(Storage::getIstreamPointer());
			
			//kaitaiParser = std::unique_ptr<KaitaiParser>(new KaitaiParser(&ks));
			KaitaiParser kaitaiParser(&ks);
			
			std::cout << int(kaitaiParser.total_length()) << std::endl;
		    std::cout << int(kaitaiParser.protocol()) << std::endl;
		    std::cout << kaitaiParser._raw_body() << std::endl;
		    
		} catch (std::exception & ex)
		{
			PRINT_ERR("KaitaiParser: %s", ex.what());
			Storage::clearIstream();
			Storage::clearData();
			return -1;
		}
		
		PRINT_DBG(true, "OK");
		
		// Complete data
		if (Storage::completeData() != 0)
		{
			PRINT_ERR("completeData()");
			return -1;
		}
		
		// Get data
		/*typename Storage::Data_get_2 data_get_2;
		Storage::getData(data_get_2);
		for (uint8_t i = 0, SHIFT = 4; i < kaitaiParser->len(); ++i)
		{
			PRINT_DBG(true, "data: %u", unsigned(data_get_2.first[SHIFT + i]));
		}*/
		
		return 0;
	}
	
public:

	DSrv_Load()
	{
		PRINT_DBG(true, "");
	}

	// Loop for the server
	void loop() noexcept
	{
		// Start
		Interface<Storage, Base>::start(); // 50000);
		
		// Send to itself
		//uint8_t data[] {0xFF, 0x01, 0x01, 0x04, 'A', 0x03, 0x01, 0x02};
		uint8_t data[] {'Q', 'w', 'E'};
		Interface<Storage, Base>::sendData(
		   typename Base<Storage>::Data_send(data, 8), "0.0.0.0"); // , 50000, false);
		
		// Receive
		while (m_stopLoop == false)
			if (Interface<Storage, Base>::receiveData() < 0)
				break;
	}
	
	// Flag for stop the loop
	volatile bool m_stopLoop {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

