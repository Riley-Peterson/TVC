#include <quaternion_type.h>
#include <vector_type.h>

enum class FlightPhase {PAD_IDLE, ASCENT, COAST, APOGEE, MAIN, GROUND};

struct FlightData {
  uint32_t time;
  float temperature;
  float pressure;
  float altitude;
  quat_t orientation;
  vec3_t acceleration;
  float servoActuationX;
  float servoActuationY;
  FlightPhase phase;
};

constexpr const char *FLASH_MAIN_HEADER = "Time (ms), Temperature (*C), Pressure (mbar), Altitude (ft), Orientation[w], Orientation[x], Orientation[y], Orientation[z], Acceleration[x] (m/s^2), Acceleration[y] (m/s^2), Acceleration[z] (m/s^2), ServoX Actuation (deg), ServoY Actuation (deg), Flight phase";