#ifndef __STARTUP_STARTUP_H__
#define __STARTUP_STARTUP_H__

#include <memory>

namespace startup {
class StartUp {
public:
    virtual bool Search() = 0;
    virtual bool Inject() = 0;
    virtual bool Start() = 0;

    virtual ~StartUp() {};

    static std::unique_ptr<StartUp> GetInstance();
};  // StartUp
}  // namespace startup

#endif  // __STARTUP_STARTUP_H__
