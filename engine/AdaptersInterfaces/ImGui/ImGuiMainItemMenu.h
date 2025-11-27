

#include <functional>
#include <string>

namespace ITF
{
    class ImGuiMainItemMenu
    {
#ifdef ITF_SUPPORT_IMGUI
    public:
        using CallbackOnName = std::function<std::string()>;
        using CallbackOnClick = std::function<void()>;

        ImGuiMainItemMenu(std::string name, CallbackOnClick onClick)
            : m_name(std::move(name)), m_onClick(std::move(onClick)) {
        }

        ImGuiMainItemMenu(CallbackOnName onName, CallbackOnClick onClick)
            : m_onName(std::move(onName)), m_onClick(std::move(onClick)) {
        }

        std::string Name() const
        {
            if (m_onName) return m_onName();
            return m_name;
        }

        void onMenuItem() const
        {
            if (m_onClick) m_onClick();
        }

    private:
        std::string        m_name;
        CallbackOnName     m_onName;
        CallbackOnClick    m_onClick;
#endif // ITF_SUPPORT_IMGUI
    };

} // namespace ITF

