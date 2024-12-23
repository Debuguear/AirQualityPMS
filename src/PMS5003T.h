
#ifndef AIR_QUALITY_SENSOR_PMS5003T_HELPERS_H
#define AIR_QUALITY_SENSOR_PMS5003T_HELPERS_H
#include "./internal/AirQualityModel_PMS5003T.h"
#include "./internal/AirQualityPMSProcessor.h"


namespace debuguear {

    using PMS5003T_PROCESSOR_T = AirQualitySensor<AdapterPMS5003T, AirQualityModel_PMS5003T, uint16_t, 12>;
    AdapterPMS5003T adapterPMS5003T;

    /**
     * @brief Creates and returns a reference to the PMS5003T air quality sensor processor.
     * 
     * @param sensorStream A reference to a Stream object used for reading sensor data.
     *                      Typically, this will be a hardware serial stream.
     * 
     * @return PMS5003T_PROCESSOR_T&  A reference to the static instance of the air quality sensor processor.
     * 
     * 
     * @note
     * - The airQualitySensor instance is static, ensuring it persists ofr the lifetime of the program.
     * - This design avoids redundant object creation and ensure single, consistent processor instance.
     * - This function is not thread-safe, it may required synchronization in multi-threaded systems.
     * 
     * @example
     * ```cpp
     * PMS5003T_PROCESSOR_T& processor = debuguear::pms5003TProcessor(pmsStreamSerial);
     * processor.loop();
     * ```
     * 
     */
    PMS5003T_PROCESSOR_T& pms5003TProcessor(Stream *sensorStream, size_t max_observers) {
        static PMS5003T_PROCESSOR_T airQualitySensor(sensorStream, adapterPMS5003T, max_observers);
        return airQualitySensor;
    }

}


#endif