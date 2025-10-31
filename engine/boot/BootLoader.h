#ifndef _ITF_BOOT_LOADER_H_
#define _ITF_BOOT_LOADER_H_

namespace ITF
{

class BootLoader
{
public:
    typedef bbool ( *UpdateProc )( u32 );

    static void init( UpdateProc proc );
    static void update();

private:
    static UpdateProc   m_proc;
    static u32          m_jmpId;
};

}

#endif  //_ITF_BOOT_LOADER_H_
