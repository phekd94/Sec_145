
#include "DSrv_Kowa.h"

#include <exception>  // std::exception class

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Kowa::DSrv_Kowa()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Kowa::~DSrv_Kowa()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Kowa::DSrv_Kowa(DSrv_Kowa && obj) : DSrv_USART_QT(std::move(obj))
{
	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
uint8_t DSrv_Kowa::calcCRC(Data_crc data) const noexcept
{
	uint16_t crc = 0;

	while (data.second--)
	{
		crc += *data.first++;
	}

	return crc % 256;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Kowa::dataParser(DSrv::Data_parser data) noexcept
{
	for (uint32_t i = 0; i < data.second; ++i)
		PRINT_DBG(m_debug, "[*]: 0x%x", *data.first++);

	return 0;
}
