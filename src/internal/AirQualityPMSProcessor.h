#ifdef DEBUG
    #define DEBUG_PRINT(x) Serial.print("[DEBUG] "); Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.print("[DEBUG] "); Serial.println(x)
    #define DEBUG_PRINT_HEX(x) Serial.print("0x"); if ((x) < 0x10) Serial.print("0"); Serial.print((x), HEX)

    #define PRINT_FRAME_HEX(frame, length)                                   \
        do {                                                                \
            Serial.print("[DEBUG] Frame data in hex: ");                    \
            for (size_t i = 0; i < (length); i++) {                         \
                if ((frame)[i] < 0x10) {                                    \
                    Serial.print('0');                                      \
                }                                                           \
                Serial.print((frame)[i], HEX);                              \
                Serial.print(i < (length) - 1 ? " " : "\n");                \
            }                                                               \
        } while (0)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINT_HEX(x)
    #define PRINT_FRAME_HEX(frame, length)
#endif


#ifndef AIR_QUALITY_SENSOR_H
#define AIR_QUALITY_SENSOR_H
#include "AirQualityModel_PMS5003T.h"

#define BIG_ENDIAN_16(hi, lo) (((hi) << 8) | (lo))

namespace debuguear {

    template <typename Adapter, typename AdapteeType, typename T, size_t DATA_N>
    class AirQualitySensor {
        public:

                AirQualitySensor(Stream *sensorStream, Adapter& adapterRef, size_t maxHandlers)
                    : sensorStream(sensorStream), adapter(adapterRef), maxHandlers(maxHandlers) {
    
                    if (maxHandlers > MAX_OBSERVERS_PHYSIC) {
                        maxHandlers = MAX_OBSERVERS_PHYSIC;
                    }
                    this->maxHandlers = maxHandlers;
                    for (uint8_t i = 0; i < maxHandlers; ++i) {
                        observers[i] = nullptr;
                    }
            };

            /**
             * @brief Main loop function for reading and processing sensor data.
             * 
             * This method continuously attempts to read and process a complete frame from the sensor stream.
             * If a valid frame is read, the processed sensor data is passed to all registered observers.
             * The method retries reading up to three times or until a timeout occurs.
             * 
             * @note
             * - Ensure the `sensorStream` is initialized and provides data from the sensor.
             * - Observers registered using `addObserver` will be notified upon successful data processing.
             */
            void loop() {
                AdapteeType data = {};
                int retry = 3;
                uint8_t buffer[FRAME_LENGHT];
                long unsigned timeoutTime = millis() + 1000;
                // Block the this loop until read the whole stream.
                // or timeout.
                DEBUG_PRINTLN("::loop");
                while ((retry != 0) && (timeoutTime >= millis())) {
                    retry--;
                    if (sensorStream->available() > 0) {
                        DEBUG_PRINTLN("::readPMSdata");
                        if (this->readPMSAvailableData(&data, buffer))  {
                            DEBUG_PRINTLN("::readPMSdata succeed");
                            this->notifyNewValue(&data);
                            break;
                        } else {
                            DEBUG_PRINTLN("::readPMSdata failed");
                        }
                    }
                    DEBUG_PRINTLN("--retry");
                } 
            }

            void addObserver(void (*fn)(AdapteeType *data) ) {
                if (fn != nullptr && observersCount < maxHandlers) {
                    observers[observersCount++] = fn;
                }
            }


            /**
             * @brief Processes and validates a raw data frame from the sensor.
             * 
             * This method validates the structure, header, length, and checksum of a raw data frame.
             * If valid, the sensor values are extracted and stored in the provided `dataDst` structure.
             * 
             * @param frame A pointer to a buffer containing the raw sensor data frame (32 bytes).
             * @param dataDst A reference to an `AdapteeType` structure to store the processed sensor values.
             *                The structure is cleared before storing new data.
             * 
             * @return `true` if the frame is valid and processed successfully.
             *         `false` if the frame is invalid, incomplete, or fails checksum validation.
             * 
             * @note
             * - This method does not read directly from the stream; it operates on the provided `frame`.
             * - Ensure the `frame` buffer contains exactly 32 bytes of data before calling this method.
             */
            bool processFrame(uint8_t *frame, AdapteeType *dataDst) {
                PRINT_FRAME_HEX(frame, FRAME_LENGHT);
                uint16_t checksum = 0;
                for (size_t i = 0; i < FRAME_LENGHT -2; i++) { // avoid sum the last 2 bytes (checksun)
                    checksum += frame[i];
                }
                // ensure data is clean;
                dataDst->clean();

                if (frame[0] != AirQualitySensor::FRAME_STARTING_BYTE_1 || frame[1] != AirQualitySensor::FRAME_STARTING_BYTE_2) {
                    DEBUG_PRINTLN("Frame header is incorrect!");
                    return false;
                }

                uint16_t framelen = BIG_ENDIAN_16(frame[2], frame[3]); // Bytes 2 y 3 para la longitud del frame (según protocolo).

                if (framelen != 28) {
                    // Longitud de frame inválida
                    DEBUG_PRINT("invalid framelen ");
                    DEBUG_PRINTLN(framelen);
                    return false;
                }

                T theData[DATA_N];
                for (size_t idx = 0; idx < DATA_N; ++idx) {
                    theData[idx] = BIG_ENDIAN_16(frame[4 + idx * 2], frame[5 + idx * 2]);
                }

                //uint16_t reserved = BIG_ENDIAN_16(frame[28], frame[29]); // Bytes 28 y 29 (Reservado) Firmware version && error code
                uint16_t expectedChecksum = BIG_ENDIAN_16(frame[30], frame[31]);

                DEBUG_PRINT("Calculated checksum: ");
                DEBUG_PRINTLN(checksum);

                DEBUG_PRINT("Expected checksum: ");
                DEBUG_PRINTLN(expectedChecksum);

                if (checksum != expectedChecksum) {
                    DEBUG_PRINTLN("Bad checksum!");
                    return false;
                }

                // now the data is safe to read it.
                this->adapter.adapt(theData, dataDst);

                return true;
            }

            ~AirQualitySensor() {
            }

        private:
            Stream *sensorStream;
            static const uint8_t MAX_OBSERVERS_PHYSIC = 10; //physic limit
            Adapter& adapter;
            static constexpr uint8_t FRAME_STARTING_BYTE_1 = 0x42;
            static constexpr uint8_t FRAME_STARTING_BYTE_2 = 0x4D;
            static constexpr uint8_t FRAME_LENGHT = 32;
            size_t maxHandlers;
            void (*observers[MAX_OBSERVERS_PHYSIC])(AdapteeType *data);
            size_t observersCount;

            bool resynchronizeStream() {
                bool found = false;
                DEBUG_PRINT("Resynchronizing: discarding bytes...");
                // Mientras no se haya encontrado el byte de inicio y haya datos en el buffer
                while (!found && this->sensorStream->available()) {
                    if (this->sensorStream->peek() == AirQualitySensor::FRAME_STARTING_BYTE_1) {
                        // Si encontramos el primer byte de inicio, intentamos leer el segundo byte
                        this->sensorStream->read(); // Consumir el primer byte
                        if (this->sensorStream->peek() == AirQualitySensor::FRAME_STARTING_BYTE_2) {
                            // Encontramos el encabezado completo, estamos alineados
                            this->sensorStream->read(); // Consumir el segundo byte
                            found = true;
                            DEBUG_PRINTLN("OK");
                        }
                    } else {
                        // discard byte
                        this->sensorStream->read();
                    }
                }

                DEBUG_PRINTLN("end");

                if (!found) {
                    DEBUG_PRINTLN("Desynchronized");
                    this->sensorStream->flush();
                }

                return found;
            }

            /**
             * @brief Reads available data from the sensor stream and processes it into a frame.
             * 
             * This method reads data from the sensor stream, resynchronizes the stream if necessary, 
             * and validates the complete frame. If successful, the frame is processed and stored in the
             * provided data structure.
             * 
             * @param data A pointer to an `AdapteeType` structure where the processed sensor values will be stored.
             * @param buffer A pointer to a 32-byte buffer used to store the raw sensor data frame.
             * 
             * @return `true` if the frame was read, validated, and processed successfully.
             *         `false` if the frame is invalid, incomplete, or the stream is desynchronized.
             * 
             * @note
             * - The method resynchronizes the stream if it detects a desynchronization based on the header bytes.
             * - Ensure the `buffer` has enough space (32 bytes) to hold the raw frame data.
             */
            bool readPMSAvailableData(AdapteeType *data, uint8_t *buffer) {
                DEBUG_PRINTLN("::readPMSdata");

                if (this->sensorStream->available() < AirQualitySensor::FRAME_LENGHT) {
                    DEBUG_PRINTLN(":::Not enough data available.");
                    return false;
                }

                // Check if the buffer contains more data than the frame size. This could 
                // occur if the stream is not read at regular intervals.
                // Failing to read the stream periodically may result in buffer overflow
                // and desynchronization.

                // Resynchronize if there are more bytes than the frame size in the buffer
                if (this->sensorStream->available() > AirQualitySensor::FRAME_LENGHT) {
                    DEBUG_PRINTLN("::Invalid frame lenght");

                    if (!resynchronizeStream()) {
                        return false;
                    }

                    // Verificar que haya datos suficientes después de la resincronización
                    if (this->sensorStream->available() < AirQualitySensor::FRAME_LENGHT -2) {

                        DEBUG_PRINT("Not enough data available after resynchronization. ");
                        DEBUG_PRINTLN(this->sensorStream->available());

                        return false;
                    }

                    buffer[0] = AirQualitySensor::FRAME_STARTING_BYTE_1;
                    buffer[1] = AirQualitySensor::FRAME_STARTING_BYTE_2;
                    if (this->sensorStream->readBytes(&buffer[2], FRAME_LENGHT-2) != FRAME_LENGHT-2) {
                        // Couldn't read all bytes
                        DEBUG_PRINT("failed to extract all frame bytes after sync");
                        return false;
                    }
                } else {
                    DEBUG_PRINTLN("::reading bytes");
                    if (this->sensorStream->readBytes(buffer, FRAME_LENGHT) != FRAME_LENGHT) {
                        // Couldn't read all bytes
                        DEBUG_PRINT("failed to extract all frame bytes");
                        return false;
                    }
                }
                return this->processFrame(buffer, data);
            }


            /**
             * @brief Notifies all registered observers with the latest sensor data.
             * 
             * This method iterates through all registered observers (callbacks) and invokes each one,
             * passing the processed sensor data as a parameter.
             * 
             * @param data A pointer to the `AdapteeType` structure containing the latest processed sensor data.
             * 
             * @note
             * - Observers are stored as function pointers and must be registered using `addObserver`.
             * - Observers are only called if they are non-null.
             */
            void notifyNewValue(AdapteeType *data) {
                for (uint8_t idx = 0; idx < observersCount; ++idx){
                    if (this->observers[idx] != nullptr){
                        this->observers[idx](data);                
                    }
                }
            }
        };
}
#endif