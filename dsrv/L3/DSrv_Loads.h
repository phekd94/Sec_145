
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for test a load
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Loads class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
(**) - see Storage, Base and Interfaces classes definition
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>                // integer types
#include <chrono>                 // time library
#include <memory>                 // std::unique_ptr<>
#include "kaitai/kaitaistream.h"  // kaitai::kstream

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
template <typename Storage, 
          template <typename T> class Base,
          template <typename T_1, template <typename Y> class T_2> class Interface,
          typename... KaitaiParsers>
class DSrv_Loads : public Interface<Storage, Base>
{

private:

	int c {0};
	
	template <typename KaitaiParser>
	void parser()
	{ 
		PRINT_DBG(true, "one; counter = %d", c);*/
		
		
		/*std::unique_ptr<KaitaiParser> kaitaiParser;
		
		try
		{
			kaitai::kstream ks(Storage::getIstreamPointer());
			
			kaitaiParser = std::unique_ptr<KaitaiParser>(new KaitaiParser(&ks));
		    
		} catch (std::exception & ex)
		{
			PRINT_ERR("p or new: %s", ex.what());
			Storage::clearIstream();
			//Storage::clearData();
			//return 0;
			
			// parser()
		}
		
		std::cout << int(kaitaiParser->total_length()) << std::endl;
	    std::cout << int(kaitaiParser->protocol()) << std::endl;
	    std::cout << kaitaiParser->_raw_body() << std::endl;
		
		PRINT_DBG(true, "OK");*/
	}

	template <typename KaitaiParser, typename... restKaitaiParsers>
	void parser()
	{
		PRINT_DBG(true, "counter = %d", c++);
		//parser<KaitaiParser>();
		parser<restKaitaiParsers...>();
	}

	// Parser of the accepted data 
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		/*for (uint32_t i = 0; i < data.second; ++i)
			PRINT_DBG(true, "[%u]: %u ", 
			                static_cast<unsigned int>(i),
			                static_cast<unsigned int>(data.first[i]));*/
	
		if (Storage::setData(typename Storage::Data_set(data.first, data.second), true) != 0)
		{
			PRINT_ERR("setData()");
			return -1;
		}
		
		c = 0;
		parser<KaitaiParsers...>();
		
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

	DSrv_Loads()
	{
		PRINT_DBG(true, "");
	}

	// Loop for the server
	void loop(std::chrono::milliseconds period) noexcept
	{
		// Start
		Interface<Storage, Base>::start();
		
		int i = 0;
		
		// Receive
		while (m_stopLoop == false)
		{
			// Send to itself
			uint8_t data_1[] {'Q', 'w', ' ', '1' + i++, '\0'};
			Interface<Storage, Base>::sendData(
			         typename Base<Storage>::Data_send(data_1, sizeof(data_1) / sizeof(uint8_t)), 
			         "0.0.0.0");
			
			/*uint8_t data_2[] {'Q', 'w', ' ', '1' + i++, '\0'};
			Interface<Storage, Base>::sendData(
			         typename Base<Storage>::Data_send(data_2, sizeof(data_2) / sizeof(uint8_t)), 
			         "0.0.0.0");*/
			
			auto start {std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				)};
			
			while (std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now().time_since_epoch()
				   ) - start < period);
			
			if (Interface<Storage, Base>::receiveData() < 0)
				break;
		}
		
		PRINT_DBG(true, "Loop end");
	}
	
	// Flag for stop the loop
	volatile bool m_stopLoop {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

