#include "MSMG_HPMA115.h"

MSMG_HPMA115::MSMG_HPMA115(USARTSerial *ser)
{
  hpmaSerial = NULL;
  hpmaSerial = ser;
}

void MSMG_HPMA115::begin(uint32_t baud)
{
  hpmaSerial->begin(baud);
}

void MSMG_HPMA115::startMeasurement()
{
  byte buf[4] = {HPMA115_HEAD_SEND, 1, HPMA115_CMD_START, 0x96};
  hpmaSerial->write(buf,4);
}

void MSMG_HPMA115::stopMeasurement()
{
  byte buf[4] = {HPMA115_HEAD_SEND, 1, HPMA115_CMD_STOP, 0x95};
  hpmaSerial->write(buf,4);
}

void MSMG_HPMA115::stopAutoSend()
{
  byte buf[4] = {HPMA115_HEAD_SEND, 1, HPMA115_CMD_STOP_AUTO_SEND, 0x77};
  hpmaSerial->write(buf,4);
}

void MSMG_HPMA115::enableAutoSend()
{
  byte buf[4] = {HPMA115_HEAD_SEND, 1, HPMA115_CMD_ENABLE_AUTO_SEND, 0x57};
  hpmaSerial->write(buf,4);
}
