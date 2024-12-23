#define DEBUG
#include "FakeStream.h"
#include <Arduino.h>
#include <unity.h>
#include "../src/PMS5003T.h"

uint8_t validFrame[32] = {
        0x42, 0x4D,  // Header (Frame Start)
        0x00, 0x1C,  // Frame Length: 28 bytes (Big-Endian: 0x001C)
        0x00, 0x32,  // PM1.0 Standard: 50 (Big-Endian: 0x0032)
        0x00, 0x64,  // PM2.5 Standard: 100 (Big-Endian: 0x0064)
        0x00, 0x96,  // PM10 Standard: 150 (Big-Endian: 0x0096)
        0x00, 0x32,  // PM1.0 Env: 50 (Big-Endian)
        0x00, 0x64,  // PM2.5 Env: 100 (Big-Endian)
        0x00, 0x96,  // PM10 Env: 150 (Big-Endian)
        0x00, 0x32,  // Particles >0.3um: 50
        0x00, 0x64,  // Particles >0.5um: 100
        0x00, 0x96,  // Particles >1.0um: 150
        0x00, 0x96,  // Particles >2.5um: 150
        0x00, 0x01,  // Temperature: 1 (Big-Endian: 0x0001)
        0x00, 0x02,  // Humidity: 2 (Big-Endian: 0x0002)
        0x00, 0x00,  // Reserved
        0x04, 0xc8   // Checksum: 0x04c8 (Big-Endian)
    };

debuguear::AirQualityModel_PMS5003T PMS5003TData;
debuguear::AirQualityModel_PMS5003T *PMS5003TDataPtr = &PMS5003TData;



bool observerWasCalled = false;


void observerFunction(debuguear::AirQualityModel_PMS5003T* data) {
    observerWasCalled = true;
    Serial.println("Observer called!");
}

void test_pms5003t_processor_loop() {
    FakeStream fakeSerial(validFrame, sizeof(validFrame));
    debuguear::PMS5003T_PROCESSOR_T processor = debuguear::pms5003TProcessor(&fakeSerial, (size_t)1);
    processor.addObserver(observerFunction);
    processor.loop();
    TEST_ASSERT_TRUE(observerWasCalled);
}



void test_pms5003t_processor_read_frame() {
    debuguear::PMS5003T_PROCESSOR_T processor = debuguear::pms5003TProcessor(&Serial, 0);
    
    bool result = processor.processFrame(validFrame, &PMS5003TData);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(50, PMS5003TData.pm10_env);
    TEST_ASSERT_EQUAL_UINT16(2, PMS5003TData.humedity);
    TEST_ASSERT_EQUAL_UINT16(1, PMS5003TData.temperature);    
}


void setUp(void){}

void tearDown(void) {
    observerWasCalled = false;
}


void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    Serial.println("Starting test...");
    UNITY_BEGIN();
}

void loop(){
    RUN_TEST(test_pms5003t_processor_loop);
    RUN_TEST(test_pms5003t_processor_read_frame);
    UNITY_END(); // stop unit testing
}

