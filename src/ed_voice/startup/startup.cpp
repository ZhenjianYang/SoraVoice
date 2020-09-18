#include "startup.h"

namespace startup {
class StartUpImpl : public StartUp {

public:
    bool Search() override;
    bool Inject() override;
    bool Start() override;
};  // StartUpImpl

std::unique_ptr<StartUp> StartUp::GetInstance() {
    return std::make_unique<StartUpImpl>();
}

bool StartUpImpl::Search() {
    return false;
}

bool StartUpImpl::Inject() {
    return false;
}

bool StartUpImpl::Start() {
    return false;
}

}  // namespace startup
