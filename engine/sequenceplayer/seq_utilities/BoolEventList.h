
#ifndef _ITF_BOOLEVENTLIST_H_
#define _ITF_BOOLEVENTLIST_H_



namespace ITF
{



class BoolEventList
{
    struct BoolEvent;

    DECLARE_SERIALIZE()

public:
    BoolEventList () { m_pointIdCounter = 0; }
    void addKey(BoolEvent& _boolEvent, bbool _computeId = true);
    void fillFromBlob (Blob* _blob);
    void serializeToBlob (Blob* _blob);
    void clear();
    u32 size() const;
    const BoolEvent& getAt (u32 _index) const;
    void postLoadForId();

private:
    struct BoolEvent
    {
        DECLARE_SERIALIZE()

        BoolEvent(): m_id(0), m_value(bfalse), m_frame(0) { }
        BoolEvent(bbool _value, i32 _frame): m_id(0), m_value(_value), m_frame(_frame) {}

        void            serializeToBlob (Blob* _blob);
        void            fillFromBlob (Blob* _blob);

        u32   m_id;
        bbool m_value;
        i32   m_frame;
    };

    SafeArray<BoolEvent> m_keys;
    u32 m_pointIdCounter;


};


} // namespace ITF

#endif // _ITF_BOOLEVENTLIST_H_