#ifndef _ITF_ENGINESIGNATURE_H_
#define _ITF_ENGINESIGNATURE_H_

namespace ITF
{

class EngineSignature
{
public:
    static u32 getEngineSignature() {return m_EngineSignature;};


    static void computeEngineSignature();
protected:
    static u32 m_EngineSignature;

};
}


#endif //_ITF_ENGINESIGNATURE_H_