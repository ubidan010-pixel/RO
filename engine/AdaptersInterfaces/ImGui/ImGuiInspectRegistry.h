#ifndef _ITF_IMGUIINSPECTREGISTRY_H_
#define _ITF_IMGUIINSPECTREGISTRY_H_

#ifdef ITF_SUPPORT_IMGUI

#ifndef _ITF_IMGUIPROPERTYSYSTEM_H_
#include "engine/imgui/ImGuiPropertySystem.h"
#endif //_ITF_IMGUIPROPERTYSYSTEM_H_

#include <unordered_map>
#include <vector>
#include <mutex>
#include "core/StringID.h"

#define ITF_REG_BEGIN_CRC(T, CRC) \
  static void ITF__RegBuild_##T(ITF::PropertyBag& bag, void* inst) { T* self = static_cast<T*>(inst); (void)self;
#define ITF_REG_END_CRC(T, CRC) \
  } static struct ITF__RegInstall_##T { ITF__RegInstall_##T(){ ITF::InspectRegistry::registerByCRC(CRC, &ITF__RegBuild_##T);} } ITF__RegInstallInst_##T;
#define ITF_REG_BEGIN(T) ITF_REG_BEGIN_CRC(T, T::GetClassCRCStatic())
#define ITF_REG_END(T)   ITF_REG_END_CRC  (T, T::GetClassCRCStatic())

namespace ITF {
    using RegistrarFn = void(*)(PropertyBag&, void* instance);
    class InspectRegistry {
    public:
        static void registerByCRC(StringID::StringIdValueType crc, RegistrarFn fn) {
            std::lock_guard<std::mutex> lk(m_); map_[crc].push_back(fn);
        }
        static void buildFor(StringID::StringIdValueType crc, PropertyBag& bag, void* inst) {
            std::lock_guard<std::mutex> lk(m_);
            if (auto it = map_.find(crc); it != map_.end()) for (auto f : it->second) f(bag, inst);
        }
    private:
        static inline std::unordered_map<StringID::StringIdValueType, std::vector<RegistrarFn>> map_;
        static inline std::mutex m_;
    };
} // namespace ITF

#endif
#endif // _ITF_IMGUIINSPECTREGISTRY_H_
