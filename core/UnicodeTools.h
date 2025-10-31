#ifndef ITF_CORE_UNICODE_TOOLS_H_
#define ITF_CORE_UNICODE_TOOLS_H_

#pragma once

#include "core/types.h"
#include "core/memory/SSOBuffer.h"

namespace ITF
{

    namespace UnitTest::UnicodeTools
    {
        void runTest();
    }

    using UnicodeChar = u32;
    using UTF16Char = char16_t;
    using UTF32Char = char32_t;

    enum
    {
        UTF8_ACCEPT = 0,
        UTF8_REJECT = 12,
    };

    enum : UnicodeChar
    {
        // every character of an invalid sequence is replaced by 0xFFFD (?)
        UNICODE_TOOLS_REPLACEMENT_CHAR = 0xFFFDu
    };

    // This is the low level decoder.
    // See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
    // Please use getNextUnicodeChar instead, unless you know what you are doing.
    u32 utf8Decode(u32& _state, u32& _codep, char _srcByte);

    UnicodeChar getNextUnicodeChar(const char*& _src);

    // When working in number of unicode character, please be aware than diacritics exist in the unicode standard (http://en.wikipedia.org/wiki/Combining_character)
    ux getNbUnicodeChar(const char* _src);

    // Tools for utf16 - utf8 conversion
    // Prefer to use the UTF8ToUTF16 and UTF16ToUTF8 classes for conversion, they are more convenient and safer.

    enum
    {
        UNICODE_TOOLS_LEAD_SURROGATE_MIN = 0xD800u,
        UNICODE_TOOLS_LEAD_SURROGATE_MAX = 0xDBFFu,
        UNICODE_TOOLS_TRAIL_SURROGATE_MIN = 0xDC00u,
        UNICODE_TOOLS_TRAIL_SURROGATE_MAX = 0xDFFFu,
        UNICODE_TOOLS_LEAD_OFFSET = UNICODE_TOOLS_LEAD_SURROGATE_MIN - (0x10000u >> 10u),
        UNICODE_TOOLS_SURROGATE_OFFSET = 0x10000u - (UNICODE_TOOLS_LEAD_SURROGATE_MIN << 10u) - UNICODE_TOOLS_TRAIL_SURROGATE_MIN,
        UNICODE_TOOLS_MAX_UNICODE = 0x0010FFFFu,
    };

    inline bbool isUnicodeSurrogate(UnicodeChar _uniChar)
    {
        return (_uniChar >= UNICODE_TOOLS_LEAD_SURROGATE_MIN && _uniChar <= UNICODE_TOOLS_TRAIL_SURROGATE_MAX);
    }

    inline bbool isUnicodeValid(UnicodeChar _uniChar)  // in the range and not a surrogate
    {
        return (_uniChar <= UNICODE_TOOLS_MAX_UNICODE && !isUnicodeSurrogate(_uniChar));
    }

    // method for UTF8 encoding
    ux getUTF8FromUnicodeChar(UnicodeChar _uniChar, char _utf8[4]); // return the number of bytes written

    //////////////////////////////////////////////////////////////////////////
    // UTF16
    //////////////////////////////////////////////////////////////////////////

    // UTF16 decoding
    // When working in number of unicode character, please be aware than diacritics exist in the unicode standard (http://en.wikipedia.org/wiki/Combining_character)
    ux getNbUnicodeChar(const UTF16Char* _src);
    UnicodeChar getNextUnicodeChar(const UTF16Char*& _src);

    // UTF16 encoding

    bbool getUTF16FromUnicodeChar(UnicodeChar _uniChar, UTF16Char& _primary, UTF16Char& _secondary); // return btrue if 2 UTF16Char are needed (is a surrogate)
    ux getNbUTF16Word(const char* _src); // including null character
    void buildUTF16FromUTF8(const char* _src, UTF16Char* _u16Buffer, ux _nbMaxU16); // write up to _nbMaxCharacters (including 0 terminal) in _u16Buffer
    ux getNbUTF8Byte(const UTF16Char* _src);
    ux getNbUTF8Byte(const UTF32Char* _src);
    void buildUTF8FromUTF16(const UTF16Char* _src, char* _dest, ux _nbMaxByte); // write up to _nbMaxBytes (including 0 terminal) in _u16Buffer

    // advanceNbUnicodeChar(_str, 12) move _str forward by 12 unicode character or less if the string is shorter.
    // return the effectively number of unicode char it advanced (so less than the parameter if the string end)
    // When working in number of unicode character, please be aware than diacritics exist in the unicode standard (http://en.wikipedia.org/wiki/Combining_character)
    template <typename T>
    inline ux advanceNbUnicodeChar(const T*& _src,
                                   ux _nbUnicodeCharToAdvance)  // return the effectively number of unicode char it advanced (can be less if the string end)
    {
        ux nbUnicodeCharLeft = _nbUnicodeCharToAdvance;

        while (nbUnicodeCharLeft > 0 && *_src != T(0))
        {
            getNextUnicodeChar(_src);
            nbUnicodeCharLeft--;
        }

        return _nbUnicodeCharToAdvance - nbUnicodeCharLeft;
    }

    // UTF32 conversions
    void buildUTF32FromUTF8(const char* _src, UTF32Char* _buffer, ux _nbMaxCodePoint); // write up to _nbMaxCodePoint (including 0 terminal) in _buffer
    void buildUTF32FromUTF16(const UTF16Char* _src, UTF32Char* _buffer, ux _nbMaxCodePoint); // write up to _nbMaxWChar (including 0 terminal) in _buffer
    void buildUTF8FromUTF32(const UTF32Char* _src, char* _buffer, ux _nbMaxChar); // write up to _nbMaxChar (including 0 terminal) in _buffer
    void buildUTF16FromUTF32(const UTF32Char* _src, UTF16Char* _u16Buffer, ux _nbMaxU16); // write up to _nbMaxChar (including 0 terminal) in _buffer

    // wchar_t is 16 bits on Windows, PS5 but 32 bits on Nintendo platforms.
    // But unfortunately, on Nintendo platforms, intellisense defines wchar_t as 16 bits, so excaping the error.
    #if defined(ITF_WINDOWS) || defined(ITF_PS5) || defined(ITF_XBOX_SERIES) || (defined(ITF_NINTENDO) && defined(__INTELLISENSE__))
        #define ITF_WCHAR_IS_16BIT 1
    #endif

    #ifdef ITF_WCHAR_IS_16BIT
        static_assert(sizeof(wchar_t) == 2, "ITF_WCHAR_IS_16BIT is wrongly defined for this platform");
    #else
        static_assert(sizeof(wchar_t) != 2, "ITF_WCHAR_IS_16BIT should be defined for this platform");
    #endif

    // Conversion helper classes
    // UTF8ToUTF16 and UTF16ToUTF8.
    // Be aware that the converted string is owned by the conversion class.
    // Handle the conversion class in a way that the converted string is not used after the conversion class is destroyed.
    // The conversion classes are not allocating for buffers < UNICODE_NO_ALLOC_CONVERT_LEN
    // example of usage:
    // ::CreateFileW(UTF8ToUTF16(myString.c_str()).getAsWChar(),...);
    // printf("Text: %s", UTF16ToUTF8(u16String.c_str()).get());)

    enum : ux
    {
        UNICODE_NO_ALLOC_CONVERT_LEN = 256
    };

    class UTF8ToUTF16
    {
       public:
        UTF8ToUTF16() = default;

        UTF8ToUTF16(UTF8ToUTF16&& _other) : m_buffer(std::move(_other.m_buffer)) {}
        UTF8ToUTF16& operator=(UTF8ToUTF16&& _other)
        {
            m_buffer = std::move(_other.m_buffer);
            return *this;
        }

        UTF8ToUTF16(const char* _utf8Src) { this->operator=(_utf8Src); }

        UTF8ToUTF16& operator=(const char* _utf8Src)
        {
            ux nbU16 = getNbUTF16Word(_utf8Src);
            m_buffer.alloc(nbU16);
            buildUTF16FromUTF8(_utf8Src, m_buffer.get(), nbU16);
            return *this;
        }

        const u16* getAsU16() const { return reinterpret_cast<const u16*>(m_buffer.get()); }
        const UTF16Char* get() const { return m_buffer.get(); }

    #ifdef ITF_WCHAR_IS_16BIT
        const wchar_t* getAsWChar() const { return reinterpret_cast<const wchar_t*>(m_buffer.get()); }
    #endif

       private:
        SSOBuffer<UTF16Char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
    };

    class UTF16ToUTF8
    {
       public:
        UTF16ToUTF8() = default;

        UTF16ToUTF8(const u16* _utf16Src) : UTF16ToUTF8(reinterpret_cast<const UTF16Char*>(_utf16Src)) {}

        UTF16ToUTF8(const UTF16Char* _utf16Src) { this->operator=(_utf16Src); }

        UTF16ToUTF8& operator=(const UTF16Char* _utf16Src)
        {
            ux nbByte = getNbUTF8Byte(_utf16Src);
            m_buffer.alloc(nbByte);
            buildUTF8FromUTF16(_utf16Src, m_buffer.get(), nbByte);
            return *this;
        }

    #ifdef ITF_WCHAR_IS_16BIT
        UTF16ToUTF8(const wchar_t* _utf16Src) { *this = _utf16Src; }

        UTF16ToUTF8& operator=(const wchar_t* _utf16Src) { return this->operator=(reinterpret_cast<const UTF16Char*>(_utf16Src)); }
    #endif

        ~UTF16ToUTF8() = default;

        const char* get() const { return m_buffer.get(); }

       private:
        SSOBuffer<char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
    };


    class UTF8ToUTF32
    {
    public:
        UTF8ToUTF32() = default;

        UTF8ToUTF32(UTF8ToUTF32&& _other) : m_buffer(std::move(_other.m_buffer)) {}
        UTF8ToUTF32& operator=(UTF8ToUTF32&& _other)
        {
            m_buffer = std::move(_other.m_buffer);
            return *this;
        }

        explicit UTF8ToUTF32(const char* _utf8Src) { this->operator=(_utf8Src); }

        UTF8ToUTF32& operator=(const char* _utf8Src)
        {
            ux nbCodePoint = getNbUnicodeChar(_utf8Src);
            m_buffer.alloc(nbCodePoint);
            buildUTF32FromUTF8(_utf8Src, m_buffer.get(), nbCodePoint);
            return *this;
        }

        const UTF32Char * get() const { return m_buffer.get(); }

    #ifndef ITF_WCHAR_IS_16BIT
        const wchar_t* getAsWChar() const { return reinterpret_cast<const wchar_t*>(m_buffer.get()); }
    #endif

    private:
        SSOBuffer<UTF32Char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
    };

    class UTF16ToUTF32
    {
    public:
        UTF16ToUTF32() = default;

        UTF16ToUTF32(UTF16ToUTF32&& _other) : m_buffer(std::move(_other.m_buffer)) {}
        UTF16ToUTF32& operator=(UTF16ToUTF32&& _other)
        {
            m_buffer = std::move(_other.m_buffer);
            return *this;
        }

        explicit UTF16ToUTF32(const u16* _utf16Src) : UTF16ToUTF32(reinterpret_cast<const UTF16Char*>(_utf16Src)) {}
        explicit UTF16ToUTF32(const UTF16Char* _utf16Src) { this->operator=(_utf16Src); }

        UTF16ToUTF32& operator=(const UTF16Char* _utf16Src)
        {
            ux nbCodePoint = getNbUnicodeChar(_utf16Src);
            m_buffer.alloc(nbCodePoint);
            buildUTF32FromUTF16(_utf16Src, m_buffer.get(), nbCodePoint);
            return *this;
        }

        const UTF32Char* get() const { return m_buffer.get(); }

    #ifndef ITF_WCHAR_IS_16BIT
        const wchar_t* getAsWChar() const { return reinterpret_cast<const wchar_t*>(m_buffer.get()); }
    #endif

    private:
        SSOBuffer<UTF32Char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
    };

    class UTF32ToUTF8
    {
    public:
        UTF32ToUTF8() = default;

        UTF32ToUTF8(UTF32ToUTF8&& _other) : m_buffer(std::move(_other.m_buffer)) {}
        UTF32ToUTF8& operator=(UTF32ToUTF8&& _other)
        {
            m_buffer = std::move(_other.m_buffer);
            return *this;
        }

        explicit UTF32ToUTF8(const UTF32Char* _utf32Src) { this->operator=(_utf32Src); }

        UTF32ToUTF8& operator=(const UTF32Char* _utf32Src)
        {
            ux nbByte = getNbUTF8Byte(_utf32Src);
            m_buffer.alloc(nbByte);
            buildUTF8FromUTF32(_utf32Src, m_buffer.get(), nbByte);
            return *this;
        }

        const char * get() const { return m_buffer.get(); }

    private:
        SSOBuffer<char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
    };

    class UTF32ToUTF16
    {
    public:
        UTF32ToUTF16() = default;

        UTF32ToUTF16(UTF32ToUTF16&& _other) : m_buffer(std::move(_other.m_buffer)) {}
        UTF32ToUTF16& operator=(UTF32ToUTF16&& _other)
        {
            m_buffer = std::move(_other.m_buffer);
            return *this;
        }

        explicit UTF32ToUTF16(const UTF32Char* _utf32Src) { this->operator=(_utf32Src); }

        UTF32ToUTF16& operator=(const UTF32Char* _utf32Src)
        {
            ux nbByte = getNbUTF8Byte(_utf32Src);
            m_buffer.alloc(nbByte);
            buildUTF16FromUTF32(_utf32Src, m_buffer.get(), nbByte);
            return *this;
        }

        const UTF16Char* get() const { return m_buffer.get(); }

    private:
        SSOBuffer<UTF16Char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
    };


    // wchar_t conversion
    #ifdef ITF_WCHAR_IS_16BIT

        class UTF16ToWChar
        {
        public:
            UTF16ToWChar() = default;

            UTF16ToWChar(UTF16ToWChar&& _other) : m_buffer(std::move(_other.m_buffer)) {}
            UTF16ToWChar& operator=(UTF16ToWChar&& _other)
            {
                m_buffer = std::move(_other.m_buffer);
                return *this;
            }

            explicit UTF16ToWChar(const u16* _utf16Src) : UTF16ToWChar(reinterpret_cast<const UTF16Char*>(_utf16Src)) {}
            explicit UTF16ToWChar(const UTF16Char* _utf16Src) { this->operator=(_utf16Src); }

            UTF16ToWChar& operator=(const UTF16Char* _utf16Src)
            {
                ux nbChar = 0;
                while (_utf16Src[nbChar] != 0)
                {
                    nbChar++;
                }
                ux bufferSize = nbChar + 1;
                m_buffer.alloc(bufferSize);
                std::copy(_utf16Src, _utf16Src + bufferSize, m_buffer.get());

                return *this;
            }

            const wchar_t* get() const { return reinterpret_cast<const wchar_t*>(m_buffer.get()); }
        private:
            SSOBuffer<wchar_t, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
        };

        class UTF8ToWChar
        {
        public:
            UTF8ToWChar() = default;
            explicit UTF8ToWChar(const char* _utf8Src)
                : m_converter(_utf8Src)
            {
            }

            UTF8ToWChar& operator=(const char* _utf8Src)
            {
                m_converter = _utf8Src;
                return *this;
            }

            const wchar_t* get() const { return m_converter.getAsWChar(); }

        private:
            UTF8ToUTF16 m_converter;
        };

        class WCharToUTF8
        {
        public:
            WCharToUTF8() = default;
            explicit WCharToUTF8(const wchar_t* _src)
                : m_converter(reinterpret_cast<const UTF16Char*>(_src))
            {
            }

            WCharToUTF8& operator=(const wchar_t* _src)
            {
                m_converter = reinterpret_cast<const UTF16Char*>(_src);
                return *this;
            }

            const char* get() const { return m_converter.get(); }

        private:
            UTF16ToUTF8 m_converter;
        };

        class WCharToUTF16
        {
        public:
            WCharToUTF16() = default;

            WCharToUTF16(WCharToUTF16&& _other) : m_buffer(std::move(_other.m_buffer)) {}
            WCharToUTF16& operator=(WCharToUTF16&& _other)
            {
                m_buffer = std::move(_other.m_buffer);
                return *this;
            }

            explicit WCharToUTF16(const wchar_t* _src) { this->operator=(_src); }

            WCharToUTF16& operator=(const wchar_t* _src)
            {
                ux nbChar = 0;
                while (_src[nbChar] != 0)
                {
                    nbChar++;
                }
                ux bufferSize = nbChar + 1;
                m_buffer.alloc(bufferSize);
                std::copy(reinterpret_cast<const UTF16Char*>(_src), reinterpret_cast<const UTF16Char*>(_src) + bufferSize, m_buffer.get());

                return *this;
            }

            const UTF16Char * get() const { return m_buffer.get(); }

        private:
            SSOBuffer<UTF16Char, UNICODE_NO_ALLOC_CONVERT_LEN> m_buffer;
        };

    #else

        class UTF16ToWChar
        {
        public:
            UTF16ToWChar() = default;

            explicit UTF16ToWChar(const u16* _utf16Src) : UTF16ToWChar(reinterpret_cast<const UTF16Char*>(_utf16Src)) {}

            explicit UTF16ToWChar(const UTF16Char* _utf16Src)
                : m_converter(_utf16Src)
            {
            }

            UTF16ToWChar& operator=(const UTF16Char* _utf16Src)
            {
                m_converter = _utf16Src;
                return *this;
            }

            const wchar_t* get() const { return reinterpret_cast<const wchar_t*>(m_converter.get()); }

        private:
            UTF16ToUTF32 m_converter;
        };

        class UTF8ToWChar
        {
        public:
            UTF8ToWChar() = default;
            explicit UTF8ToWChar(const char* _utf8Src)
                : m_converter(_utf8Src)
            {
            }

            UTF8ToWChar& operator=(const char* _utf8Src)
            {
                m_converter = _utf8Src;
                return *this;
            }

            const wchar_t* get() const { return reinterpret_cast<const wchar_t*>(m_converter.get()); }

        private:
            UTF8ToUTF32 m_converter;
        };

        class WCharToUTF8
        {
        public:
            WCharToUTF8() = default;
            explicit WCharToUTF8(const wchar_t* _src)
                : m_converter(reinterpret_cast<const UTF32Char *>(_src))
            {
            }

            WCharToUTF8& operator=(const wchar_t * _src)
            {
                m_converter = reinterpret_cast<const UTF32Char*>(_src);
                return *this;
            }

            const char * get() const { return m_converter.get(); }

        private:
            UTF32ToUTF8 m_converter;
        };

        class WCharToUTF16
        {
        public:
            WCharToUTF16() = default;
            explicit WCharToUTF16(const wchar_t* _src)
                : m_converter(reinterpret_cast<const UTF32Char*>(_src))
            {
            }

            WCharToUTF16& operator=(const wchar_t* _src)
            {
                m_converter = reinterpret_cast<const UTF32Char*>(_src);
                return *this;
            }

            const UTF16Char* get() const { return m_converter.get(); }

        private:
            UTF32ToUTF16 m_converter;
        };

    #endif

}  // namespace ITF

#endif  // ITF_CORE_UNICODE_TOOLS_H_
