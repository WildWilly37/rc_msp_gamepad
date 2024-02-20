#pragma once

#include <stdint.h>

#define MSP_MAX_PACKET_SIZE 255

#define MSP_FC_VARIANT                  2    //out message
#define MSP_FC_VERSION                  3    //out message
#define MSP_NAME                        10   //out message          Returns user set board name - betaflight
#define MSP_FILTER_CONFIG               92
#define MSP_SET_PID_ADVANCED            95
#define MSP_STATUS                      101  //out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RC                          105  //out message         rc channels and more
#define MSP_RC_TUNING                   111  //out message         rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_PID                         112  //out message         P I D coeff (9 are used currently)
#define MSP_BATTERY_STATE               130  //out message         Connected/Disconnected, Voltage, Current Used
#define MSP_STATUS_EX                   150  //out message         cycletime, errors_count, CPU load, CPU temperature, sensor present etc


// requests & replies 
#define MSP_API_VERSION            1
#define MSP_BOARD_INFO             4
#define MSP_BUILD_INFO             5
#define MSP_CALIBRATION_DATA      14
#define MSP_FEATURE               36
#define MSP_BOARD_ALIGNMENT       38
#define MSP_CURRENT_METER_CONFIG  40
#define MSP_RX_CONFIG             44
#define MSP_SONAR_ALTITUDE        58
#define MSP_ARMING_CONFIG         61
#define MSP_RX_MAP                64 // get channel map (also returns number of channels total)
#define MSP_LOOP_TIME             73 // FC cycle time i.e looptime parameter
#define MSP_RAW_IMU              102
#define MSP_SERVO                103
#define MSP_MOTOR                104
#define MSP_RAW_GPS              106
#define MSP_COMP_GPS             107 // distance home, direction home
#define MSP_ATTITUDE             108
#define MSP_ALTITUDE             109
#define MSP_ANALOG               110
#define MSP_MISC                 114
#define MSP_SERVO_CONFIGURATIONS 120
#define MSP_NAV_STATUS           121 // navigation status
#define MSP_SENSOR_ALIGNMENT     126 // orientation of acc,gyro,mag
#define MSP_SENSOR_STATUS        151
#define MSP_BOXIDS               119
#define MSP_UID                  160 // Unique device ID
#define MSP_GPSSVINFO            164 // get Signal Strength (only U-Blox)
#define MSP_GPSSTATISTICS        166 // get GPS debugging data
#define MSP_SET_RAW_RC           200    //in message          8 rc chan
#define MSP_SET_PID              202 // set P I D coeff

// External OSD displayport mode messages
#define MSP_DISPLAYPORT                 182

#define MSP_COPY_PROFILE                183

#define MSP_BEEPER_CONFIG               184
#define MSP_SET_BEEPER_CONFIG           185

#define MSP_SET_TX_INFO                 186 // in message           Used to send runtime information from TX lua scripts to the firmware
#define MSP_TX_INFO                     187 // out message          Used by TX lua scripts to read information from the firmware

#define MSP_SET_OSD_CANVAS              188 // in message           Set osd canvas size COLSxROWS
#define MSP_OSD_CANVAS                  189 // out message          Get osd canvas size COLSxROWS

#define START_BYTE  '$'  // Start of MSP packet ('$' character)
#define SECOND_BYTE 'M'  // Start of MSP packet ('$' character)
#define HEADER_LENGTH 6  // Header length (including command and payload size)



#define DISPLAYPORT_BLINK  0x80 // Device local blink bit or'ed into displayPortSeverity_e

typedef enum {
    MSP_PARSE_STATE_IDLE = 0,
    MSP_PARSE_STATE_HEADER,    
    MSP_PARSE_STATE_DIRECTION,    
    MSP_PARSE_STATE_SIZE,    
    MSP_PARSE_STATE_COMMAND,    
    MSP_PARSE_STATE_PAYLOAD,
    MSP_PARSE_STATE_CRC,
} msp_parse_state_t;


typedef struct 
{
  uint8_t direction;
  uint8_t command;
  uint8_t size;
  uint8_t payload[256];
  uint8_t crc;
} msp_packet_t;


typedef struct {
    uint8_t state;
    msp_packet_t packet; 
    uint16_t msp_packet_length;
} msp_parser_t;

// MSP_STATUS_DJI reply
struct msp_status_DJI_t {
  uint16_t cycleTime;
  uint16_t i2cErrorCounter;
  uint16_t sensor;                    // MSP_STATUS_SENSOR_...
  uint32_t flightModeFlags;           // see getActiveModes()
  uint8_t  configProfileIndex;
  uint16_t averageSystemLoadPercent;  // 0...100
  uint16_t armingFlags;   //0x0103 or 0x0301
  uint8_t  accCalibrationAxisFlags;  //0
  uint8_t  DJI_ARMING_DISABLE_FLAGS_COUNT; //25
  uint32_t djiPackArmingDisabledFlags; //(1 << 24)
} __attribute__ ((packed));


struct msp_name_t {
    char craft_name[15];                    //15 characters max possible displayed in the goggles
} __attribute__ ((packed));

struct msp_battery_state_t {
  uint8_t batteryCellCount;
    uint16_t batteryCapacity;
    uint8_t legacyBatteryVoltage;
    uint16_t mAhDrawn;
    uint16_t amperage;
    uint8_t batteryState;
    uint16_t batteryVoltage;
} __attribute__ ((packed));

// MSP_STATUS_EX reply
struct msp_status_ex_t {
  uint16_t cycleTime;
  uint16_t i2cErrorCounter;
  uint16_t sensor;                    // MSP_STATUS_SENSOR_...
  uint32_t flightModeFlags;           // see getActiveModes()
  uint8_t  configProfileIndex;
  uint16_t averageSystemLoadPercent;  // 0...100
  uint16_t armingFlags;
  uint8_t  accCalibrationAxisFlags;
} __attribute__ ((packed));


// MSP_STATUS
struct msp_status_t {
  uint16_t cycleTime;
  uint16_t i2cErrorCounter;
  uint16_t sensor;                    // MSP_STATUS_SENSOR_...
  uint32_t flightModeFlags;           // see getActiveModes()
  uint8_t  configProfileIndex;
} __attribute__ ((packed));

// MSP_SET_RAW_RC
struct msp_set_raw_rc_t {
  uint16_t channels[16];
} __attribute__ ((packed));
