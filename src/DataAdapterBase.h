
#ifndef AIR_QUALITY_SENSOR_DATA_ADAPTER_H
#define AIR_QUALITY_SENSOR_DATA_ADAPTER_H
#include "Arduino.h"

namespace debuguear {

    template <typename T, size_t N, typename R>
    class SensorDataAdapterBase {
        public:
            virtual ~SensorDataAdapterBase() = default;

            /**
             * @brief Adapts a specific sensor frame to its data model.
             * @param frame Pointer to the frame containing raw sensor data.
             * @param size Size of the frame.
             * @return A filled structure.
             */
            virtual void adapt(const T (&values)[N], R *dst) = 0;
    };


}

#endif