#pragma once
#include <cstdint>

// FAKE: production의 무거운 SomethingElse.h 대신 껍데기만
namespace Firmware {

class SomethingElse {
public:
    virtual ~SomethingElse() = default;
    virtual bool DoWork(int32_t param);
    virtual int32_t GetStatus() const;
};

} // namespace Firmware
