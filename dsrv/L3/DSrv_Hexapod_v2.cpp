
#include "DSrv_Hexapod_v2.h"

#include <exception>  // std::exception class

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Hexapod_v2::DSrv_Hexapod_v2()
{
	// Filling a vector with data size
	m_data_size.push_back(2);  //   0 - position value            (from)
	m_data_size.push_back(2);  //   1 - speed value               (from)
	m_data_size.push_back(1);  //   2 - error state               (from)
	m_data_size.push_back(1);  //   3 - start                     (to)
	m_data_size.push_back(1);  //   4 - stop                      (to)
	m_data_size.push_back(2);  //   5 - frequency       (osc)     (to)
	m_data_size.push_back(2);  //   6 - speed           (osc)     (to)
	m_data_size.push_back(2);  //   7 - start position  (osc)     (to)
	m_data_size.push_back(1);  //   8 - mode            (osc)     (to)
	m_data_size.push_back(2);  //   9 - size            (script)  (to)
	m_data_size.push_back(2);  //  10 - value           (script)  (to)
	m_data_size.push_back(1);  //  11 - reinitialize              (to)
	m_data_size.push_back(3);  //  12 - answer                    (from)

	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Hexapod_v2::~DSrv_Hexapod_v2()
{
	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
const std::vector<uint32_t>& DSrv_Hexapod_v2::getDataSize() const noexcept
{
	return m_data_size;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2::start() noexcept
{
	// Call a start() from USART class
	if (DSrv_USART_QT::start() != 0)
	{
		PRINT_ERR(true, PREF, "DSrv_USART_QT::start()");
		return -1;
	}

	// Connect a signal and a slot for handle an error
	try {
		connect(getSerialPort(), &QSerialPort::errorOccurred,
		        this, &DSrv_Hexapod_v2::onErrorOccured);
	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, PREF, "Exception from Qt functions has been occured: %s", obj.what());

		// Call a stop() from USART class
		if (DSrv_USART_QT::stop() != 0)
		{
			PRINT_ERR(true, PREF, "DSrv_USART_QT::stop()");
			return -1;
		}

		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2::stop() noexcept
{
	// Call a stop() from USART class
	if (DSrv_USART_QT::stop() != 0)
	{
		PRINT_ERR(true, PREF, "DSrv_USART_QT::stop()");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2::dataParser(uint8_t* data, uint32_t size) noexcept
{
	// Identificator for add data to last record in storage class
	bool add = true;

	// Check the incomming parameters
	if (nullptr == data)
	{
		PRINT_ERR(m_debug, PREF, "nullptr == data");
		return -2;
	}

	while (size > 0)
	{
		if (0 == m_pktRemSize)
		{
			if (-1 == m_motor_id)
			{
				// First byte
				m_motor_id = *data;
				if (static_cast<uint32_t>(m_motor_id) >= HEXAPOD_NUM_OF_MOTORS)
				{
					PRINT_ERR(true, PREF, "Bad motor id");
					return -1;
				}
			}
			else
			{
				// Second byte
				m_cmd_id = *data;
				if (m_cmd_id >= m_data_size.size())
				{
					PRINT_ERR(true, PREF, "Bad command id");
					return -1;
				}
				m_pktRemSize = m_data_size[m_cmd_id];
				add = false;
			}
		}
		else
		{
			// Data
			if (DSrv_Storage::setData(data, 1, add) != 0)
			{
				PRINT_ERR(true, PREF, "DSrv_Storage::setData()");
				return -1;
			}
			--m_pktRemSize;
			if (0 == m_pktRemSize)
			{
				if (DSrv_Storage::completeData() != 0)
				{
					PRINT_ERR(true, PREF, "DSrv_Storage::completeData() != 0");
					return -1;
				}

				// Get complete data
				uint8_t* data_res;
				uint32_t size_res;
				if (DSrv_Storage::getData(&data_res, &size_res) != 0)
				{
					PRINT_ERR(true, PREF, "DSrv_Storage::getData() != 0");
					return -1;
				}
				for (uint32_t i = m_data_size[m_cmd_id]; i < sizeof(uint32_t); ++i)
				{
					data_res[i] = 0;
				}
				emit stateChanged(static_cast<uint32_t>(m_motor_id),
				                  m_cmd_id,
				                  *reinterpret_cast<uint32_t*>(data_res));
				PRINT_DBG(m_debug, PREF, "motor id = %lu, cmd id = %lu, data = %lu",
				                         static_cast<unsigned long>(m_motor_id),
				                         static_cast<unsigned long>(m_cmd_id),
				                         *reinterpret_cast<uint32_t*>(data_res));

				// Set a parameter for the getting a new packet
				m_motor_id = -1;
			}
		}

		// Update a data pointer and a size
		++data;
		--size;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
void DSrv_Hexapod_v2::onErrorOccured(QSerialPort::SerialPortError err) noexcept
{
	switch (err)
	{

	case QSerialPort::NoError:
		PRINT_ERR(true, PREF, "USART error: no error");
		break;

	default:
		PRINT_ERR(true, PREF, "USART error: %ld", static_cast<long>(err));
		break;
	}
}

//-------------------------------------------------------------------------------------------------
void DSrv_Hexapod_v2::onSendCommand(const uint32_t cmd,
                                    const uint32_t* data,
                                    const uint32_t size) noexcept
{
	switch (cmd)
	{

	case HEXAPOD_CMD_CON:
		// Call a start() method
		if (DSrv_Hexapod_v2::start() != 0)
		{
			PRINT_ERR(true, PREF, "DSrv_Hexapod_v2::start()");

			emit stateChanged(0, HEXAPOD_ERR_USART_OPEN, 0);
		}
		else
		{
			emit stateChanged(0, HEXAPOD_DEV_OPENED, 0);
		}
		break;

	case HEXAPOD_CMD_DISCON:
		// Call a stop() method
		if (DSrv_Hexapod_v2::stop() != 0)
		{
			PRINT_ERR(true, PREF, "DSrv_Hexapod_v2::stop()");

			if (true == static_cast<bool>(*data))
			{
				emit stateChanged(0, HEXAPOD_ERR_USART_CLOSE, 0);
			}
			else
			{
				// Set a flag of complete
				m_stop_ok = true;
			}
		}
		else
		{
			if (true == static_cast<bool>(*data))
			{
				emit stateChanged(0, HEXAPOD_DEV_CLOSED, 0);
			}
			else
			{
				// Set a flag of complete
				m_stop_ok = true;
			}
		}
		break;

	case HEXAPOD_CMD_SEND:
		if (DSrv_USART_QT::sendData(reinterpret_cast<const uint8_t*>(data),
		                            size,
		                            nullptr,
		                            0) != 0)
		{
			PRINT_ERR(true, PREF, "sendData()");
			emit stateChanged(0, HEXAPOD_ERR_USART_IO, 1);
		}
		break;

	default:
		PRINT_ERR(true, PREF, "Unknown a command value from widget class");
		break;
	}
}

//=================================================================================================
void DSrv_Hexapod_v2_test::onStateChanged(const uint32_t motor,
                                          const uint32_t state,
                                          const uint32_t value) noexcept
{
	switch (state)
	{

	case HEXAPOD_POS:
		if (value != 1793 || motor != 0)
		{
			PRINT_ERR(true, PREF, "Position value: value != 1793 or motor != 0");
		}
		else
		{
			m_slot_ok = true;
		}
		break;

	case HEXAPOD_SPEED:
		if (value != 1538 || motor != 0)
		{
			PRINT_ERR(true, PREF, "Speed value: value != 1538 or motor != 0");
		}
		else
		{
			m_slot_ok = true;
		}
		break;

	case HEXAPOD_ERR:
		if (value != 5 || motor != 0)
		{
			PRINT_ERR(true, PREF, "Error value: value != 5 or motor != 0");
		}
		else
		{
			m_slot_ok = true;
		}
		break;

	case HEXAPOD_ANSWER:
	{
		uint32_t value_cmd = value & 0xFF;
		uint32_t value_val = value >> 8;
		if ((   value_cmd != HEXAPOD_START
		     && value_cmd != HEXAPOD_STOP
		     && value_cmd != HEXAPOD_FREQUENCY
		     && value_cmd != HEXAPOD_SPEED_TO
		     && value_cmd != HEXAPOD_START_POS
		     && value_cmd != HEXAPOD_MODE)
		    || motor != 0)
		{
			PRINT_ERR(true, PREF, "Error value: "
			                      "value != HEXAPOD_START and "
			                      "value != HEXAPOD_STOP and "
			                      "value != HEXAPOD_FREQUENCY and "
			                      "value != HEXAPOD_SPEED_TO and "
			                      "value != HEXAPOD_START_POS and "
			                      "value != HEXAPOD_MODE "
			                      "or motor != 0");
		}
		else
		{
			if (HEXAPOD_FREQUENCY == value_cmd && 1543 == value_val)
			{
				m_slot_ok = true;
			}
			else if (HEXAPOD_SPEED_TO == value_cmd && 2054 == value_val)
			{
				m_slot_ok = true;
			}
			else if (HEXAPOD_START_POS == value_cmd && 2079 == value_val)
			{
				m_slot_ok = true;
			}
			else if (HEXAPOD_START == value_cmd && value_val <= 4)
			{
				m_slot_ok = true;
			}
			else if (HEXAPOD_STOP == value_cmd || HEXAPOD_MODE == value_cmd)
			{
				m_slot_ok = true;
			}
			else
			{
				PRINT_ERR(true, PREF, "Error value: bad value_val for cmd = %lu",
				                      static_cast<unsigned long>(value_cmd));
			}
		}
	}
		break;

	default:
		PRINT_ERR(true, PREF, "Unknown value");
		break;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_test::logic(DSrv_Hexapod_v2& obj, DSrv_Hexapod_v2_test& obj_test) noexcept
{
	if (obj.start() == 0)
	{
		PRINT_ERR(true, PREF, "start() without hardware");
		return -1;
	}

	if (obj.stop() != 0)
	{
		PRINT_ERR(true, PREF, "stop()");
		return -1;
	}

	uint8_t data[10] = {0};
	data[PKT_HEADER_MOTOR_INDEX] = 0;

	// Position value
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_POS;
	data[PKT_HEADER_SIZE] = 1;
	data[PKT_HEADER_SIZE + 1] = 7;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_POS]) != 0)
	{
		PRINT_ERR(true, PREF, "Position value: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok)
	{
		PRINT_ERR(true, PREF, "Position value: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1)
	{
		PRINT_ERR(true, PREF, "Position value: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_POS)
	{
		PRINT_ERR(true, PREF, "Position value: m_cmd_id != HEXAPOD_POS");
		return -1;
	}
	if (obj.m_pktRemSize != 0)
	{
		PRINT_ERR(true, PREF, "Position value: m_pktRemSize != 0");
		return -1;
	}

	// Speed value
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_SPEED;
	data[PKT_HEADER_SIZE] = 2;
	data[PKT_HEADER_SIZE + 1] = 6;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_SPEED]) != 0)
	{
		PRINT_ERR(true, PREF, "Speed value: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok)
	{
		PRINT_ERR(true, PREF, "Speed value: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1)
	{
		PRINT_ERR(true, PREF, "Speed value: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_SPEED)
	{
		PRINT_ERR(true, PREF, "Speed value: m_cmd_id != HEXAPOD_SPEED");
		return -1;
	}
	if (obj.m_pktRemSize != 0)
	{
		PRINT_ERR(true, PREF, "Speed value: m_pktRemSize != 0");
		return -1;
	}

	// State error
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_ERR;
	data[PKT_HEADER_SIZE] = 5;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_ERR]) != 0)
	{
		PRINT_ERR(true, PREF, "State error: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok)
	{
		PRINT_ERR(true, PREF, "State error: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1)
	{
		PRINT_ERR(true, PREF, "State error: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_ERR)
	{
		PRINT_ERR(true, PREF, "State error: m_cmd_id != HEXAPOD_ERR");
		return -1;
	}
	if (obj.m_pktRemSize != 0)
	{
		PRINT_ERR(true, PREF, "State error: m_pktRemSize != 0");
		return -1;
	}

	// Answer: HEXAPOD_START
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_ANSWER;
	data[PKT_HEADER_SIZE] = HEXAPOD_START;
	data[PKT_HEADER_SIZE + 1] = 0;
	data[PKT_HEADER_SIZE + 2] = 0;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_ANSWER]) != 0) {
		PRINT_ERR(true, PREF, "State error: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok) {
		PRINT_ERR(true, PREF, "State error: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1) {
		PRINT_ERR(true, PREF, "State error: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_ANSWER) {
		PRINT_ERR(true, PREF, "State error: m_cmd_id != HEXAPOD_ANSWER");
		return -1;
	}
	if (obj.m_pktRemSize != 0) {
		PRINT_ERR(true, PREF, "State error: m_pktRemSize != 0");
		return -1;
	}

	// Answer: HEXAPOD_STOP
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_ANSWER;
	data[PKT_HEADER_SIZE] = HEXAPOD_STOP;
	data[PKT_HEADER_SIZE + 1] = 0;
	data[PKT_HEADER_SIZE + 2] = 0;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_ANSWER]) != 0) {
		PRINT_ERR(true, PREF, "State error: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok) {
		PRINT_ERR(true, PREF, "State error: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1) {
		PRINT_ERR(true, PREF, "State error: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_ANSWER) {
		PRINT_ERR(true, PREF, "State error: m_cmd_id != HEXAPOD_ANSWER");
		return -1;
	}
	if (obj.m_pktRemSize != 0) {
		PRINT_ERR(true, PREF, "State error: m_pktRemSize != 0");
		return -1;
	}

	// Answer: HEXAPOD_FREQUENCY
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_ANSWER;
	data[PKT_HEADER_SIZE] = HEXAPOD_FREQUENCY;
	data[PKT_HEADER_SIZE + 1] = 7;
	data[PKT_HEADER_SIZE + 2] = 6;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_ANSWER]) != 0) {
		PRINT_ERR(true, PREF, "State error: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok) {
		PRINT_ERR(true, PREF, "State error: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1) {
		PRINT_ERR(true, PREF, "State error: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_ANSWER) {
		PRINT_ERR(true, PREF, "State error: m_cmd_id != HEXAPOD_ANSWER");
		return -1;
	}
	if (obj.m_pktRemSize != 0) {
		PRINT_ERR(true, PREF, "State error: m_pktRemSize != 0");
		return -1;
	}

	// Answer: HEXAPOD_SPEED_TO
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_ANSWER;
	data[PKT_HEADER_SIZE] = HEXAPOD_SPEED_TO;
	data[PKT_HEADER_SIZE + 1] = 6;
	data[PKT_HEADER_SIZE + 2] = 8;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_ANSWER]) != 0) {
		PRINT_ERR(true, PREF, "State error: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok) {
		PRINT_ERR(true, PREF, "State error: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1) {
		PRINT_ERR(true, PREF, "State error: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_ANSWER) {
		PRINT_ERR(true, PREF, "State error: m_cmd_id != HEXAPOD_ANSWER");
		return -1;
	}
	if (obj.m_pktRemSize != 0) {
		PRINT_ERR(true, PREF, "State error: m_pktRemSize != 0");
		return -1;
	}

	// Answer: HEXAPOD_START_POS
	data[PKT_HEADER_CMD_INDEX] = HEXAPOD_ANSWER;
	data[PKT_HEADER_SIZE] = HEXAPOD_START_POS;
	data[PKT_HEADER_SIZE + 1] = 31;
	data[PKT_HEADER_SIZE + 2] = 8;
	obj_test.m_slot_ok = false;
	if (obj.dataParser(data, PKT_HEADER_SIZE + obj.m_data_size[HEXAPOD_ANSWER]) != 0) {
		PRINT_ERR(true, PREF, "State error: dataParser()");
		return -1;
	}
	if (false == obj_test.m_slot_ok) {
		PRINT_ERR(true, PREF, "State error: bad value in slot");
		return -1;
	}
	if (obj.m_motor_id != -1) {
		PRINT_ERR(true, PREF, "State error: m_motor_id != -1");
		return -1;
	}
	if (obj.m_cmd_id != HEXAPOD_ANSWER) {
		PRINT_ERR(true, PREF, "State error: m_cmd_id != HEXAPOD_ANSWER");
		return -1;
	}
	if (obj.m_pktRemSize != 0) {
		PRINT_ERR(true, PREF, "State error: m_pktRemSize != 0");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_test::pNull(DSrv_Hexapod_v2& obj)
{
	// Check data == nullptr
	if (obj.dataParser(nullptr, 4) != -2) {
		PRINT_ERR(true, PREF, "dataParser(nullptr, ...)");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_test::fullTest()
{
	DSrv_Hexapod_v2 obj;
	obj.setDebug(true, true);
	DSrv_Hexapod_v2_test obj_test;
	connect(&obj, &DSrv_Hexapod_v2::stateChanged,
	        &obj_test, &DSrv_Hexapod_v2_test::onStateChanged);

	if (pNull(obj) != 0) {
		PRINT_ERR(true, PREF, "pNull");
		return -1;
	}

	if (logic(obj, obj_test) != 0) {
		PRINT_ERR(true, PREF, "logic");
		return -1;
	}

	PRINT_DBG(true, PREF, "Test was successful");
	return 0;
}
