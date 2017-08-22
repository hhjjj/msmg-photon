#ifndef MSMG_HPMA115_h
#define MSMG_HPMA115_h

#include "Particle.h"

#define HPMA115_HEAD_SEND             (0x68)
#define HPMA115_HEAD_READ             (0x40)

#define HPMA115_CMD_READ              (0x04)
#define HPMA115_CMD_START             (0x01)
#define HPMA115_CMD_STOP              (0x02)
#define HPMA115_CMD_SET_COEFF         (0x08)
#define HPMA115_CMD_GET_COEFF         (0x10)
#define HPMA115_CMD_STOP_AUTO_SEND    (0x20)
#define HPMA115_CMD_ENABLE_AUTO_SEND  (0x40)

#define HPMA115_ACK                   (0xA5A5)
#define HPMA115_NACK                  (0x9696)



class MSMG_HPMA115
{
  public:
    MSMG_HPMA115(USARTSerial *ser);

    void begin(uint32_t baud);
    void startMeasurement();
    void stopMeasurement();

    void readAutoData();

    bool dataReady();

    byte *getData();

    void setAdjCoefficient(uint8_t val);

    uint8_t readAdjCoefficient();

    void stopAutoSend();
    void enableAutoSend();

  private:
    byte calCheckSum();
    bool checkCheckSum();
    USARTSerial *hpmaSerial;

};

#endif // AssetTracker_h
