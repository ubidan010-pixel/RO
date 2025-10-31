#ifndef _ITF_AUTOSAVEMANAGER_H_
#define _ITF_AUTOSAVEMANAGER_H_

#ifdef ITF_SUPPORT_EDITOR

namespace ITF
{
//------------------------------------------------------------------------------------------------------------------------

class AutoSaveManager
{
public:
    static const u32 TimeForAutoSave = 180; //in seconds

    AutoSaveManager():m_CurrentTime(0.0f), m_SaveTimeDelay(f32(TimeForAutoSave)) 
    {
#ifdef ITF_WINDOWS
    m_bEnable  = btrue;
#else
    m_bEnable  = bfalse;
#endif  //ITF_WINDOWS
    };

    virtual ~AutoSaveManager()                      {};
    void    init(i32 _autosavetime);

    void update(f32 _dt);

    void disable()  {m_bEnable = bfalse;}

protected:


    bbool m_bEnable;
    f32 m_CurrentTime;
    f32 m_SaveTimeDelay;
    String m_szDirectory;

};
   

} // namespace ITF

#endif // ITF_SUPPORT_EDITOR

#endif // _ITF_AUTOSAVEMANAGER_H_
