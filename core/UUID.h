#ifndef ITF_UUID_H
#define ITF_UUID_H


namespace ITF
{
    // Simple implementation of Universally Unique IDentifiers, 128-bit numbers
    // See RFC 4122 <https://tools.ietf.org/html/rfc4122>

    class UUID
    {
    public:
        // The nil UUID (all zeros)
        static const UUID Nil;

        // By default, an UUID object will be initialized as the nil UUID
        UUID() { makeNil(); }

        bool operator < (const UUID& _rhs) const;
        bool operator > (const UUID& _rhs) const;
        bool operator ==(const UUID& _rhs) const { return (m_highPart == _rhs.m_highPart && m_lowPart == _rhs.m_lowPart); }
        bool operator !=(const UUID& _rhs) const { return !(*this == _rhs); }

        bool isNil() const { return (m_highPart == 0 && m_lowPart == 0); }
        void makeNil() { m_highPart = 0; m_lowPart = 0; }

        // Creates a random (version 4) UUID.
        // If _fastRandom is true, use a fast pseudo-random number generator.
        // If _fastRandom is false, use a cryptographically secure pseudo-random number generator instead (if possible)
        void makeRandom(bool _fastRandom = false);

        // Create an UUID from a string.
        // If the string doesn't represent a valid UUID, the object will be set to the nil UUID
        UUID(const char* _uuid);
        UUID(const String8& _uuidString);

        // Set an UUID from a string.
        // If the string doesn't represent a valid UUID, the object will be set to the nil UUID
        UUID operator =(const char* _rhs);
        UUID operator =(const String8& _rhs) { return (*this = _rhs.cStr()); }

        bool fromString(const char*    _rhs);
        bool fromString(const String8& _rhs) { return fromString(_rhs.cStr()); }

        String8 toString() const;

    private:
        u64 m_highPart;
        u64 m_lowPart;
    };

}


#endif // ITF_UUID_H
