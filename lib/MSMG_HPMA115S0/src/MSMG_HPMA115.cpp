#include "MSMG_HPMA115.h"

#define dust_data_size 32
volatile byte dust_data[dust_data_size];
volatile byte *data_pointer;
volatile uint8_t rcvCount = 0;
volatile bool recvdflag;

MSMG_HPMA115::MSMG_HPMA115(USARTSerial *ser)
{
  hpmaSerial = NULL;
  hpmaSerial = ser;
}

void MSMG_HPMA115::begin(uint32_t baud)
{
  recvdflag = false;
  rcvCount = 0;
  data_pointer = dust_data;
  hpmaSerial->begin(baud);
}

void MSMG_HPMA115::readAutoData()
{

  if(hpmaSerial->available())
  {
    byte c;
    c = hpmaSerial->read();
    if (c == 0x42)
    {
        rcvCount = 0;
    }
    
    dust_data[rcvCount] = c;
    rcvCount++;

    if(rcvCount >= dust_data_size)
    {
      rcvCount = 0;
      recvdflag = true;
    }
  }

}

// char MSMG_HPMA115::read()
// {
//
//   char c = 0;
//
//   if(!hpmaSerial->available()) return c;
//
//   c = hpmaSerial->read();
//
//   return c;
// }

bool MSMG_HPMA115::dataReady()
{
  return recvdflag;
}

byte *MSMG_HPMA115::getData()
{
  recvdflag = false;
  return (byte *)data_pointer;
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
