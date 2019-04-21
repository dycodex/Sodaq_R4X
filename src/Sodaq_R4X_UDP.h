#ifndef _Sodaq_R4X_UDP_h
#define _Sodaq_R4X_UDP_h

#include <Arduino.h>
#include <Udp.h>
#include <cbuf.h>

#include "Sodaq_R4X.h"

class Sodaq_R4X_UDP: public UDP {
public:
    Sodaq_R4X_UDP(Sodaq_R4X& nbiot);
    ~Sodaq_R4X_UDP();

    virtual uint8_t begin(uint16_t port);
    virtual void stop();
    virtual int beginPacket();
    virtual int beginPacket(IPAddress ip, uint16_t port);
    virtual int beginPacket(const char* host, uint16_t port);
    virtual int endPacket();
    virtual size_t write(uint8_t value);
    virtual size_t write(const uint8_t* buffer, size_t size);
    virtual int parsePacket();
    virtual int available();
    virtual int read();
    virtual int read(unsigned char* buffer, size_t len);
    virtual int read(char* buffer, size_t len);
    virtual int peek();
    virtual void flush();
    virtual IPAddress remoteIP();
    virtual uint16_t remotePort();

private:
    Sodaq_R4X* _nbiot;
    int _socket;
    IPAddress _remoteAddress;
    uint16_t _remotePort;
    char* _txBuffer;
    size_t _txBufferLength;
    cbuf* _rxBuffer;
};

#endif