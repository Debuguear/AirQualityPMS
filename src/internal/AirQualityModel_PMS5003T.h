#ifndef AIR_QUALITY_SENSOR_MODEL_PMS5003T_H
#define AIR_QUALITY_SENSOR_MODEL_PMS5003T_H
#define PMS5003T_DATA_BLOCKS 12
#include "DataAdapterBase.h"


namespace debuguear {
    

    struct AirQualityModel_PMS5003T {
        
        uint16_t pm10_standard, pm25_standard, pm100_standard; // refers PM1.0, PM2.5 , PM10 concentration unit μ g/m³ （CF=1，standard particle）
        uint16_t pm10_env, pm25_env, pm100_env; // refers to PM1.0, PM2.5  concentration unit μ g/m3（under atmospheric environment）
        uint16_t particles_03um, particles_05um, particles_10um, particles_25um; // indicates the number of particles with diameter beyond (0.3, 0.5, 1.0, 2.5) um in 0.1 L of air. 
        int16_t temperature; // Range: (-20, 99) °C, resolution=0.1 , error=±0.5 
        uint16_t humedity; //  Range: (0, 99) %, resolution=0.1 , error=±2 
        uint16_t reserved; // Byte reserved by protocol.

        /**
         * @brief Resets all fields of the air quality model to zero.
         * 
         * This method clears all the stored sensor values in the `AirQualityModel` structure
         * by setting them to zero. It ensures a clean state for the model before new data 
         * is assigned or processed.
         * @return void
        */
        void clean() {
                pm10_standard = 0;
                pm25_standard = 0;
                pm100_standard = 0;
                pm10_env = 0;
                pm25_env = 0;
                pm100_env = 0;
                particles_03um = 0;
                particles_05um = 0;
                particles_10um = 0;
                particles_25um = 0;
                temperature = 0;
                humedity = 0;
                reserved = 0;
        }


        /**
         * @brief Converts the air quality model data into a formatted string.
         * 
         * This method generates a human-readable string representation of all the 
         * fields in the `AirQualityModel` structure. The string includes both
         * standart and environmental particles measuramentes, particle counts, 
         * temperature, and humedity, each represented in a separate line.
         * 
         * @return String A formatted string containing all the data in the model.
         * 
         * @note The format of the returned string is as follows:
         *       - PM10 (std): <value>
         *       - PM2.5 (std): <value>
         *       - PM100 (std): <value>
         *       - PM10 (env): <value>
         *       - PM2.5 (env): <value>
         *       - PM100 (env): <value>
         *       - >0.3um: <value>
         *       - >0.5um: <value>
         *       - >1.0um: <value>
         *       - >2.5um: <value>
         *       - Temp <value>
         *       - H% <value>
         */
        String toString() const {
            String result = "PM10 (std): " + String(pm10_standard) + "\n";
            result += "PM2.5 (std): " + String(pm25_standard) + "\n";
            result += "PM100 (std): " + String(pm100_standard) + "\n";
            result += "PM10 (env): " + String(pm10_env) + "\n";
            result += "PM2.5 (env): " + String(pm25_env) + "\n";
            result += "PM100 (env): " + String(pm100_env) + "\n";
            result += ">0.3um: " + String(particles_03um) + "\n";
            result += ">0.5um: " + String(particles_05um) + "\n";
            result += ">1.0um: " + String(particles_10um) + "\n";
            result += ">2.5um: " + String(particles_25um) + "\n";
            result += "Temp " + String(temperature) + "\n";
            result += "H% " + String(humedity) + "\n";
            return result;
        }
    };

    class AdapterPMS5003T: public SensorDataAdapterBase<uint16_t, PMS5003T_DATA_BLOCKS, AirQualityModel_PMS5003T> {
        public:
            void adapt(const uint16_t (&data)[PMS5003T_DATA_BLOCKS], AirQualityModel_PMS5003T *dst) override {
                dst->pm10_standard = data[0];
                dst->pm25_standard = data[1];
                dst->pm100_standard = data[2];
                dst->pm10_env = data[3];
                dst->pm25_env = data[4];
                dst->pm100_env = data[5];
                dst->particles_03um = data[6];
                dst->particles_05um = data[7];
                dst->particles_10um = data[8];
                dst->particles_25um = data[9];
                dst->temperature = data[10];
                dst->humedity = data[11];
            }
    };

}

#endif