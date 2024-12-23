#ifndef AIRQUALITY_TEST_FAKE_STREAM
#define AIRQUALITY_TEST_FAKE_STREAM

#include <Arduino.h>

class FakeStream : public Stream {
private:
    const uint8_t* data;
    size_t size;
    size_t position;

public:
    FakeStream(const uint8_t* frame, size_t length) : data(frame), size(length), position(0) {}

    int available() override {
        return (size - position);
    }

    size_t readBytes(uint8_t* buffer, size_t length) {
        size_t bytesRead = 0;
        while (bytesRead < length && available()) {
            buffer[bytesRead++] = read();
        }
        return bytesRead; // Ensure return value
    }

    int read() override {
        if (position <= size) {
            return data[position++];
        } else {
            return -1; // No more data
        }
    }

    int peek() override {
        if (position < size) {
            return data[position];
        } else {
            return -1;
        }
    }

    void flush() override {
        // No-op for FakeStream
    }

     size_t write(uint8_t) override {
        return 1; // Simulamos que siempre se escribe 1 byte correctamente
    }

};

#endif