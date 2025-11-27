#ifndef _ITF_IMGUIPROPERTYSYSTEM_H_
#define _ITF_IMGUIPROPERTYSYSTEM_H_

#ifdef ITF_SUPPORT_IMGUI

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#include "imgui/imgui.h"
#include <algorithm>
#include <deque>
#include <initializer_list>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <cmath>
#include <cfloat>
#include <climits>
#include <cstring>

#define ITF_PROP(bag, Owner, Member, Label, ...) \
    ::ITF::AddProp((bag), (Label), &Owner::Member, ##__VA_ARGS__)

#define ITF_PROP_ACC(bag, Type, Label, GetExpr, SetExpr, ...)                          \
    ::ITF::AddPropAcc<std::remove_pointer_t<decltype(this)>, Type>(                   \
        (bag), (Label),                                                                     \
        [this](std::remove_pointer_t<decltype(this)>*) { return (GetExpr); },               \
        [this](std::remove_pointer_t<decltype(this)>*, const Type& _v) { SetExpr; },        \
        ##__VA_ARGS__ )

namespace ITF {
    struct AttrRange { double min, max, step; };
    struct AttrTooltip { const char* text; };
    struct AttrReadOnly { bool value = true; };
    struct AttrCategory { const char* name; };

    struct Attrs {
        const AttrRange* range = nullptr;
        const AttrTooltip* tooltip = nullptr;
        const AttrReadOnly* ro = nullptr;
        const AttrCategory* cat = nullptr;
    };

    inline const char* InternString(const char* s) {
        static std::deque<String> pool;

        pool.emplace_back(s ? s : "");
        return pool.back().getCharCopy();
    }

    inline const AttrRange* MakeRange(double mn, double mx, double st) { static std::deque<AttrRange>    s; s.push_back({ mn,mx,st }); return &s.back(); }
    inline const AttrTooltip* MakeTip(const char* t) { static std::deque<AttrTooltip>  s; s.push_back({ InternString(t) }); return &s.back(); }
    inline const AttrCategory* MakeCat(const char* n) { static std::deque<AttrCategory> s; s.push_back({ InternString(n) }); return &s.back(); }
    inline const AttrReadOnly* MakeRO(bool v = true) { static const AttrReadOnly roT{ true }, roF{ false }; return v ? &roT : &roF; }

    struct ARange { double mn, mx, st; void operator()(Attrs& a) const { a.range = MakeRange(mn, mx, st); } };
    struct ATip { const char* t;     void operator()(Attrs& a) const { a.tooltip = MakeTip(t); } };
    struct ACat { const char* n;     void operator()(Attrs& a) const { a.cat = MakeCat(n); } };
    struct AReadOnly { bool v = true;     void operator()(Attrs& a) const { a.ro = MakeRO(v); } };

    inline ARange     RANGE(double mn, double mx, double st) { return { mn, mx, st }; }
    inline ATip       TIP(const char* t) { return { t }; }
    inline ACat       CAT(const char* n) { return { n }; }
    inline AReadOnly  RO(bool v = true) { return { v }; }

    struct IProperty {
        virtual ~IProperty() = default;
        virtual const char* name()  const = 0;
        virtual const Attrs& attrs() const = 0;
        virtual bbool         draw(void* instance) = 0; // instance = Owner*
    };

    class PropertyBag {
    public:
        template<class P, class... Args>
        P* emplace(Args&&... args) {
            auto up = std::make_unique<P>(std::forward<Args>(args)...);
            P* raw = up.get();
            props_.push_back(std::move(up));
            order_.push_back(raw);
            return raw;
        }
        const ITF_VECTOR<IProperty*>& view() const { return order_; }
        void clear() { props_.clear(); order_.clear(); }
    private:
        ITF_VECTOR<std::unique_ptr<IProperty>> props_;
        ITF_VECTOR<IProperty*>                 order_;
    };

    template<typename T, typename Owner>
    class PropertyMember : public IProperty {
    public:
        using MemberPtr = T Owner::*;
        PropertyMember(std::string name, MemberPtr member, Attrs attrs = {})
            : name_(std::move(name)), member_(member), attrs_(attrs) {
        }

        const char* name()  const override { return name_.c_str(); }
        const Attrs& attrs() const override { return attrs_; }

        bbool draw(void* instance) override {
            Owner* o = static_cast<Owner*>(instance);
            if (attrs_.ro && attrs_.ro->value) {
                ImGui::BeginDisabled(true);
                T tmp = o->*member_;
                (void)drawField(name_.c_str(), tmp, attrs_);
                ImGui::EndDisabled();
                return bfalse;
            }
            return drawField(name_.c_str(), o->*member_, attrs_);
        }

    private:
        std::string name_;
        MemberPtr   member_;
        Attrs       attrs_;
    };

    template<class Owner, class Type, class GetLambda, class SetLambda>
    class PropertyAccessor : public IProperty {
    public:
        PropertyAccessor(std::string name, GetLambda g, SetLambda s, Attrs attrs = {})
            : name_(std::move(name)), get_(std::move(g)), set_(std::move(s)), attrs_(attrs) {
        }

        const char* name()  const override { return name_.c_str(); }
        const Attrs& attrs() const override { return attrs_; }

        bbool draw(void* instance) override {
            Owner* o = static_cast<Owner*>(instance);
            Type v = get_(o);
            const bool ro = (attrs_.ro && attrs_.ro->value);
            if (ro) {
                ImGui::BeginDisabled(true);
                (void)drawField(name_.c_str(), v, attrs_);
                ImGui::EndDisabled();
                return bfalse;
            }
            bbool changed = drawField(name_.c_str(), v, attrs_);
            if (changed) set_(o, v);
            return changed;
        }

    private:
        std::string name_;
        GetLambda   get_;
        SetLambda   set_;
        Attrs       attrs_;
    };

    template<class Owner, class Type, class GetLambda, class SetLambda, class... Builders>
    inline void AddPropAcc(PropertyBag& bag, const char* label,
        GetLambda g, SetLambda s, Builders&&... bs) {
        Attrs a{};
        (void)std::initializer_list<int>{ (bs(a), 0)... };
        bag.emplace<PropertyAccessor<Owner, Type, GetLambda, SetLambda>>(label, g, s, a);
    }

    template<class Owner, class T, class... Builders>
    inline void AddProp(PropertyBag& bag, const char* label,
        T Owner::* member, Builders&&... builders) {
        Attrs a{};
        (void)std::initializer_list<int>{ (builders(a), 0)... };
        bag.emplace<PropertyMember<T, Owner>>(label, member, a);
    }

    class IInspectable {
    public:
        virtual ~IInspectable() = default;
        virtual const char* inspectableName() const = 0;

    protected:
        virtual void buildProperties(PropertyBag& bag) = 0;

    public:
        virtual bbool canShow() const { return bfalse; }

        virtual bbool displayProperties() {
            bbool changed = bfalse;
            ensureBuilt();
            const auto& props = bag_.view();
            const char* currentCat = nullptr;
            for (auto* p : props) {
                const auto& at = p->attrs();
                if (at.cat && at.cat->name &&
                    (!currentCat || std::strcmp(currentCat, at.cat->name) != 0)) {
                    currentCat = at.cat->name;
                    ImGui::SeparatorText(currentCat);
                }
                changed = p->draw(this) | changed;
            }
            return changed;
        }

        template<class P, class... Args>
        P* addRuntimeProperty(Args&&... args) {
            ensureBuilt();
            return bag_.emplace<P>(std::forward<Args>(args)...);
        }

    protected:
        void ensureBuilt() {
            if (built_) return;
            built_ = true;
            buildProperties(bag_);
        }

    private:
        PropertyBag bag_;
        bool        built_ = false;
    };

    inline bbool drawField(const char* label, float& v, const Attrs& a) {
        const float step = a.range ? (float)a.range->step : 0.01f;
        const float minv = a.range ? (float)a.range->min : -FLT_MAX;
        const float maxv = a.range ? (float)a.range->max : FLT_MAX;
        bbool ch = ImGui::DragFloat(label, &v, step, minv, maxv);
        if (a.tooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", a.tooltip->text);
        return ch;
    }

    inline bbool drawField(const char* label, int& v, const Attrs& a) {
        const int   step = a.range ? (int)a.range->step : 1;
        const int   minv = a.range ? (int)a.range->min : INT_MIN;
        const int   maxv = a.range ? (int)a.range->max : INT_MAX;
        bbool ch = ImGui::DragInt(label, &v, (float)step, minv, maxv);
        if (a.tooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", a.tooltip->text);
        return ch;
    }

    inline bbool drawField(const char* label, bool& v, const Attrs& a) {
        (void)a;
        return ImGui::Checkbox(label, &v);
    }

    inline bbool drawField(const char* label, Vec2d& v, const Attrs& a) {
        float f[2] = { (float)v.m_x, (float)v.m_y };
        const float step = a.range ? (float)a.range->step : 0.01f;
        const float minv = a.range ? (float)a.range->min : -FLT_MAX;
        const float maxv = a.range ? (float)a.range->max : FLT_MAX;
        bbool ch = ImGui::DragFloat2(label, f, step, minv, maxv);
        if (ch) { v.m_x = f[0]; v.m_y = f[1]; }
        if (a.tooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", a.tooltip->text);
        return ch;
    }

    inline bbool drawField(const char* label, Vec3d& v, const Attrs& a) {
        float f[3] = { (float)v.m_x, (float)v.m_y, (float)v.m_z };
        const float step = a.range ? (float)a.range->step : 0.1f;
        const float minv = a.range ? (float)a.range->min : -FLT_MAX;
        const float maxv = a.range ? (float)a.range->max : FLT_MAX;
        bool ch = ImGui::DragFloat3(label, f, step, minv, maxv);
        if (ch) { v.m_x = f[0]; v.m_y = f[1]; v.m_z = f[2]; }
        if (a.tooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", a.tooltip->text);
        return ch;
    }

    inline bbool drawField(const char* label, Color& c, const Attrs& a) {
        float f[4] = { c.m_r, c.m_g, c.m_b, c.m_a };
        bbool ch = ImGui::ColorEdit4(label, f);
        if (ch) { c.m_r = f[0]; c.m_g = f[1]; c.m_b = f[2]; c.m_a = f[3]; }
        if (a.tooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", a.tooltip->text);
        return ch;
    }

} // namespace ITF
#else  // !ITF_SUPPORT_IMGUI

#include <string>

namespace ITF {
    class IInspectable
    {
    };
} 

#endif // ITF_SUPPORT_IMGUI

#endif // _ITF_IMGUIPROPERTYSYSTEM_H_
