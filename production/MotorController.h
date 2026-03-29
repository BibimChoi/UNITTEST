#pragma once
#include <cstdint>

namespace Hal {
class HalGpio;  // forward declaration
}

namespace Firmware {
class SomethingElse;  // forward declaration

enum class MotorState : uint8_t {
    STOPPED = 0,
    RUNNING_FWD,
    RUNNING_REV,
    ERROR,
};

enum class MotorError : uint8_t {
    NONE = 0,
    OVER_TEMP,
    SENSOR_FAIL,
    NOT_CALIBRATED,
    LOG_FAIL,
};

class MotorController {
public:
    MotorController();
    ~MotorController() = default;

    MotorError RunSequence();
    MotorError EmergencyStop();
    MotorState GetState() const;
    bool SetGpio(Hal::HalGpio* gpio);

private:
    void StartMotor(bool forward);
    void StopMotor();

    Hal::HalGpio* gpio_{nullptr};
    SomethingElse* something_;
    MotorState state_{MotorState::STOPPED};
    int32_t maxTemp_{80};
};

} // namespace Firmware
