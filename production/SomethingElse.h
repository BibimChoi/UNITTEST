#pragma once
#include "HalTypes.h"
#include <cstdint>

// 실제 펌웨어에선 여기에 무거운 HW 의존성이 딸려옴
namespace Firmware {

class SomethingElse {
public:
    virtual ~SomethingElse() = default;
    virtual bool DoWork(int32_t param);
    virtual int32_t GetStatus() const;

private:
    Hal::DmaConfig dmaCfg_{};
    uint32_t counter_{0};
};

} // namespace Firmware
