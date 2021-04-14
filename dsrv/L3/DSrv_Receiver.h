
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for receive data
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Receiver class
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
class DSrv_Receiver : public Interface<Storage, Base>
{

private:

	// Parser of the accepted data 
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		/*
		for (uint32_t i = 0; i < data.second; ++i)
			PRINT_DBG(true, "[%u]: %u ", 
			                static_cast<unsigned int>(i),
			                static_cast<unsigned int>(data.first[i]));
		*/
		
		if (Storage::setData(typename Storage::Data_set(data.first, data.second), true) != 0)
		{
			PRINT_ERR("setData()");
			return -1;
		}
		
		try
		{
			kaitai::kstream ks(Storage::getIstreamPointer());
			
			KaitaiParser kaitaiParser(&ks);
			
			PRINT_DBG(true, "Length: %d", int(kaitaiParser.total_length()));
			PRINT_DBG(true, "Protocol: %d", int(kaitaiParser.protocol()));
			PRINT_DBG(true, "Data: %s", kaitaiParser._raw_body().c_str());
		    
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
		
		return 0;
	}
	
public:

	DSrv_Receiver()
	{
		PRINT_DBG(true, "");
	}
	
	~DSrv_Receiver()
	{
		PRINT_DBG(true, "");
	}

	// Loop for the server
	void loop() noexcept
	{
		// Start
		Interface<Storage, Base>::start();
		
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

