#ifndef __STARTUP_SCAN_GROUP_H__
#define __STARTUP_SCAN_GROUP_H__

#include <memory>
#include <string>
#include <vector>

namespace startup {
class ScanGroup {
public:
    virtual bool IsValid() const = 0;
    virtual const std::string& Name() const = 0;
    virtual const std::string& GameTitle() const = 0;
    virtual const std::string& GameBuiltDate() const = 0;
    virtual std::vector<std::unique_ptr<char[]>>&& MovableStrings() = 0;
    virtual bool Scan() = 0;
    virtual bool Inject() = 0;

    virtual ~ScanGroup() { }

    static std::vector<std::unique_ptr<ScanGroup>> GetScanGroups();
};  // ScanGroup
}  // namespace startup

#endif // __STARTUP_SCAN_GROUP_H__
