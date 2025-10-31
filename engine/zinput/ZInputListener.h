#ifndef _ITF_INPUTLISTENER_H_
#define _ITF_INPUTLISTENER_H_

namespace ITF
{
    class IInputListener
    {
    public:
        IInputListener():m_canReceive(btrue)  {}

        virtual void Receive( u32 deviceID /* player */, f32 axis, const StringID& action ) = 0;
        ITF_INLINE bbool canReceive() const  { return m_canReceive; }
        ITF_INLINE void setCanReceive(bbool _value) { m_canReceive = _value; }
    private:
        bbool m_canReceive;
    };
}//namespace ITF


#endif //_ITF_INPUTLISTENER_H_


