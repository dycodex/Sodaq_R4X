#include "Sodaq_R4X_UDP.h"

Sodaq_R4X_UDP::Sodaq_R4X_UDP(Sodaq_R4X& nbiot):
 _nbiot(&nbiot),
 _socket(-1),
 _remotePort(0),
 _txBuffer(0),
 _txBufferLength(0),
 _rxBuffer(0) {}

Sodaq_R4X_UDP::~Sodaq_R4X_UDP() {
    stop();
}

uint8_t Sodaq_R4X_UDP::begin(uint16_t port) {
    // UDP server is not supported yet
    return 0;
}

void Sodaq_R4X_UDP::stop() {
    if (_txBuffer) {
        delete _txBuffer;
        _txBuffer = NULL;
    }

    if (_rxBuffer) {
        delete _rxBuffer;
        _rxBuffer = NULL;
    }

    if (_nbiot->socketClose(_socket)) {
        _socket = -1;
    }
}

int Sodaq_R4X_UDP::beginPacket() {
    if (_remotePort == 0) {
        return 0;
    }

    if (!_txBuffer) {
        _txBuffer = new char[512];
        if (!_txBuffer) {
            return 0;
        }
    }

    _txBufferLength = 0;
    if (_socket == -1) {
        _socket = _nbiot->socketCreate(0, (Protocols)1);
        Serial.printf("Socket result: %d\n", _socket);
        if (_socket < 0) {
            _socket = 0;
            // return 1;
        }
    }

    _nbiot->execCommand("AT+UDCONF=1,1");
    return 1;
}

int Sodaq_R4X_UDP::beginPacket(IPAddress ip, uint16_t port) {
    _remoteAddress = ip;
    _remotePort = port;

    return beginPacket();
}

int Sodaq_R4X_UDP::beginPacket(const char* host, uint16_t port) {
    IPAddress ip;
    if (ip.fromString(host)) {
        _remoteAddress = ip;
        _remotePort = port;

        return beginPacket();
    }

    return 0;
}

int Sodaq_R4X_UDP::endPacket() {
    int sent = _nbiot->socketSend(
        _socket,
        _remoteAddress.toString().c_str(),
        _remotePort,
        (const uint8_t*)_txBuffer,
        _txBufferLength
    );

    if (sent < 0) {
        return 0;
    }

    return 1;
}

size_t Sodaq_R4X_UDP::write(uint8_t value) {
    if (_txBufferLength == 512) {
        endPacket();
        _txBufferLength = 0;
    }

    _txBuffer[_txBufferLength++] = value;

    return 1;
}

size_t Sodaq_R4X_UDP::write(const uint8_t* buffer, size_t size) {
    size_t i;
    for (i = 0; i < size; i++) {
        write(buffer[i]);
    }

    return i;
}

int Sodaq_R4X_UDP::parsePacket() {
    if (_rxBuffer && !_rxBuffer->empty()) {
        return 0;
    }

    uint8_t buffer[512] = {0};
    int readLength = _nbiot->socketReceive(_socket, buffer, 512);
    if (readLength < 1) {
        return 0;
    }

    if (!_rxBuffer) {
        _rxBuffer = new cbuf(readLength);
    } else {
        _rxBuffer->flush();
    }

    _rxBuffer->write((const char*)buffer, readLength);

    return readLength;
}

int Sodaq_R4X_UDP::available() {
    if (!_rxBuffer) {
        return 0;
    }

    return _rxBuffer->available();
}

int Sodaq_R4X_UDP::read() {
    if (!_rxBuffer) {
        return -1;
    }

    int out = _rxBuffer->read();
    if (!_rxBuffer->available()) {
        cbuf* b = _rxBuffer;
        _rxBuffer = 0;
        delete b;
    }

    return out;
}

int Sodaq_R4X_UDP::read(unsigned char* buffer, size_t length) {
    return read((char*)buffer, length);
}

int Sodaq_R4X_UDP::read(char* buffer, size_t length) {
    if (!_rxBuffer) {
        return 0;
    }

    int out = _rxBuffer->read(buffer, length);
    if (!_rxBuffer->available()) {
        cbuf *b = _rxBuffer;
        _rxBuffer = 0;
        delete b;
    }

    return out;
}

int Sodaq_R4X_UDP::peek() {
    if (!_rxBuffer) {
        return -1;
    }

    return _rxBuffer->peek();
}

void Sodaq_R4X_UDP::flush() {
    cbuf* b = _rxBuffer;
    _rxBuffer = 0;
    delete b;
}

IPAddress Sodaq_R4X_UDP::remoteIP() {
    return _remoteAddress;
}

uint16_t Sodaq_R4X_UDP::remotePort() {
    return _remotePort;
}
