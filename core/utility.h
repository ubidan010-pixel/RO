#ifndef _ITF_UTILITY_H_
#define _ITF_UTILITY_H_

namespace ITF
{
    template<typename T>    String8 hex(const T value) {
        char result[2+16+1];	// result character array
        static const char hex_chars[] = "0123456789abcdef";

        unsigned int source = (unsigned int)value;

        // fill the string from the back to front with characters
        int i = 10;
        result[i--] = 0;
        do {
            result[i--] = hex_chars[source & 15];
            source = source >> 4;
        }
        while (source != 0);
        i++;
        return String8(&result[i]);
    }


    // DistInRange is design to be used inside for ranged based loop.
    // It return the index of the element in the range.
    // The range has to support size(range) and data(range)
    // Assert/undefined behavior if the referenced element is not in range.
    // exemple : for(auto & e : myContainer) { auto idx = distInRange(myContainer, e); } // idx go from 0 to myContainer.size()
    template <typename Container, typename T>
    [[nodiscard]] size_t distInRange(Container& rng, const T& _e)
    {
        auto start = std::data(rng);
        ITF_ASSERT((&_e >= start) && (&_e < start + std::size(rng)));
        return &_e - start;
    }

    // Fill on iterable (typed memset)
    template <typename Iterable, typename T>
    constexpr void fill(Iterable&& _iterable, const T& _value)
    {
        std::fill(std::begin(_iterable), std::end(_iterable), _value);
    }


    //////////////////////////////////////////////////////////////////////////
    // final_act allows you to ensure something gets run at the end of a scope
    // use it via finally fct
    template <class F>
    class final_act
    {
    public:
        explicit final_act(F f) noexcept
            : m_f(std::move(f)), m_invoke(true)
        {
        }

        final_act(final_act&& other) noexcept
            : m_f(std::move(other.m_f)), m_invoke(other.m_invoke)
        {
            other.m_invoke = false;
        }

        final_act(const final_act&) = delete;
        final_act& operator=(const final_act&) = delete;

        ~final_act() noexcept { if (m_invoke) m_f(); }

    private:
        F m_f;
        bool m_invoke; // TODO: Can be removed thanks to C++17 guaranteed copy ellision
    };

    // finally() - convenience function to generate a final_act
    template <class F>
    [[nodiscard]] inline final_act<F> finally(const F& f) noexcept
    {
        return final_act<F>(f);
    }

    template <class F>
    [[nodiscard]] inline final_act<F> finally(F&& f) noexcept
    {
        return final_act<F>(std::forward<F>(f));
    }

    // Enum tools
    template <typename Enum>
    constexpr std::underlying_type_t<Enum> toUnderlying(Enum e) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }

    // Tool for C style callbacks (see below)

    // helper, see below
    namespace MethodToCallback_private
    {
        template <typename M>
        class Helper;
        template <typename T, typename... Args>
        class Helper<void (T::*)(Args...)>
        {
        public:
            template <auto Method>
            static constexpr auto GetFunctionPtr()
            {
                return &StaticMethod<Method>;
            }
            template <auto Method>
            static void StaticMethod(Args... args, void* userData)
            {
                auto instance = static_cast<T*>(userData);
                (instance->*Method)(args...);
            }
        };
    }

    // methodToCallback<&Class::Method>()
    // This is an helper to get 'legacy C style' functions that call a method.
    // It returns a function ptr with the signature of Method with an additional last void* parameter assumed as a Class*.
    // The returned function is invoking Method on the Class*.
    // Example:
    // m_profiler.RegisterFrameCallback(methodToCallback<&Controller::onNewFrame>(), this);
    template <auto Method>
    constexpr auto methodToCallback()
    {
        return MethodToCallback_private::Helper<decltype(Method)>::template GetFunctionPtr<Method>();
    }
    // Possible improvement => a version for the convention (void* userData, Args... args)


} //namespace ITF


#endif //_ITF_UTILITY_H_
