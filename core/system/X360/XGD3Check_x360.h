#ifndef _ITF_XGD3CHECK_H_
#define  _ITF_XGD3CHECK_H_

namespace ITF
{

class xgd3Check:public TemplateSingleton<xgd3Check>
{
public:
    xgd3Check():m_checkStarted(bfalse),m_bInit(bfalse),m_dt(0.0f),m_thread(NULL) {};
    u32 init();
    void shutdown();

    void poll();
    void startCheck();

    void showPiracy();

#ifdef ITF_SUPPORT_CHEAT
    void injectError(u32 _hashError);
#endif //ITF_SUPPORT_CHEAT

private:
    bbool   m_checkStarted;
    bbool   m_bInit;
    float   m_dt;
    Thread* m_thread;

   
};

}

#endif //_ITF_XGD3CHECK_H_