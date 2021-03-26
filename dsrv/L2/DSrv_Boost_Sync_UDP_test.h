
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for DSrv_Boost_Sync_UDP class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------

#include "DSrv_Boost_Sync_UDP.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Sync_UDP_for_test : public DSrv_Boost_Sync_UDP<Storage, Base>
{
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		if (Storage::setData(typename Storage::Data_set(data.first, data.second), true) != 0)
		{
			PRINT_ERR("setData()");
			return -1;
		}
		if (Storage::completeData() != 0)
		{
			PRINT_ERR("completeData()");
			return -1;
		}
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Sync_UDP_test
{
public:

	DSrv_Boost_Sync_UDP_test() = delete;
	
	static void fullTest() noexcept
	{
		DSrv_Boost_Sync_UDP_for_test<Storage, Base> obj;
		
		obj.start(50000);
		
		while (obj.receiveData() == 1);
		
		const uint8_t * const data = (const uint8_t * const)("Answer");
		
		obj.sendData(typename Base<Storage>::Data_send(data, 6), "0.0.0.0", 50001);
		
		obj.stop();
	}
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

