#ifndef _ITF_DEBUGINFO_H_
#define _ITF_DEBUGINFO_H_

#ifdef ITF_SUPPORT_DEBUGFEATURE

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

namespace ITF
{

    struct DebugInfoDisplay
    {
        String text;
        f32 timeToDisplay;
    };

    class DebugInfo:public TemplateSingleton<DebugInfo>
    {
     public:
        DebugInfo();
        virtual ~DebugInfo();
        
        void setEditorInfo(const String& _info);
        void setCameraInfo(class Camera* _pCamera);

        void setUndoRedoInfo(const String& _info);
        void clearUndoRedoInfo()                        { m_undoRedoInfo.clear(); }

        void setEngineInfo(const String& _info);

        void addGenericInfo( const String& _generic )   { m_genericInfo.push_back(_generic); }

        void addDebugInfoDisplay(const String& _text,f32 _time);

        void draw();

        static void breakOnRef(ObjectRef _ref)      {m_breakOnRef = _ref;}
        static void stopOnRef(BaseObject* _pRef)    {if (m_breakOnRef == _pRef->getRef())  ITF_DEBUG_BREAK(); }
        static void clearRef()                      {m_breakOnRef = ITF_INVALID_OBJREF;}

#ifdef ITF_WINDOWS
        void dump(FILE* _file);
#endif //ITF_WINDOWS

    private:

        ITF_THREAD_CRITICAL_SECTION             m_cs;

        String m_szCameraInfo;
        String m_szEditorInfo;
        String m_undoRedoInfo;
        String m_engineInfo;

        static ObjectRef m_breakOnRef;
        
        ITF_VECTOR<String> m_genericInfo;
        ITF_VECTOR<DebugInfoDisplay> m_infoLines;
    };

#define DEBUGINFO DebugInfo::getptr()
}

#endif //ITF_SUPPORT_DEBUGFEATURE

#endif
