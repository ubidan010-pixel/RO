#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_DEBUGFEATURE

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
   ObjectRef DebugInfo::m_breakOnRef = ITF_INVALID_OBJREF;

   #define BEGIN_INFO String tmp;String param;
   #define ADD_TITLE(txt) tmp+="-----------------------------\r\n";tmp+=txt;tmp+="\r\n";
   #define ADD_ONE_STRING(str) tmp+=str;tmp+="\r\n";
   #define ADD_PARAM(__format,...) param.setTextFormat(__format,## __VA_ARGS__);tmp+=param;tmp+="\r\n";
   #define END_EDITORINFO tmp+="-----------------------------\r\n";m_szEditorInfo=tmp;
   #define END_CAMERAINFO tmp+="-----------------------------\r\n";m_szCameraInfo=tmp;
   #define END_UNDOREDOINFO tmp+="-----------------------------\r\n";m_undoRedoInfo=tmp;
   #define END_ENGINEINFO tmp+="-----------------------------\r\n";m_engineInfo=tmp;

   void DebugInfo::setEditorInfo(const String& _info)
   {
#ifdef ITF_WINDOWS
       BEGIN_INFO
       ADD_TITLE("Editor info:");
       ADD_PARAM("%ls", _info.cStr());
       END_EDITORINFO;
#endif //ITF_WINDOWS
   }

   DebugInfo::DebugInfo()
   {
       Synchronize::createCriticalSection(&m_cs);
   };

   DebugInfo::~DebugInfo()
   {
       Synchronize::destroyCriticalSection(&m_cs);
   };

   void DebugInfo::setUndoRedoInfo( const String& _info )
   {
#ifdef ITF_WINDOWS
       BEGIN_INFO
       ADD_TITLE("UndoRedo info:");
       ADD_ONE_STRING(_info.cStr());
       END_UNDOREDOINFO;
#endif //ITF_WINDOWS
   }

   void DebugInfo::setCameraInfo(Camera* _pCamera)
   {
#ifdef ITF_WINDOWS
       //extract some information about the camera;
       const Vec3d& position = _pCamera->getPos();
       BEGIN_INFO
       ADD_TITLE("Camera Info:");
       ADD_PARAM("Position: %f %f %f",position.m_x,position.m_y,position.m_z);
       END_CAMERAINFO
#endif //ITF_WINDOWS
   }

   void DebugInfo::setEngineInfo( const String& _info )
   {
#ifdef ITF_WINDOWS
       BEGIN_INFO
       ADD_TITLE("Engine info:");
       ADD_ONE_STRING(_info.cStr());
       END_ENGINEINFO;
#endif //ITF_WINDOWS
   }


#ifdef ITF_WINDOWS
   void DebugInfo::dump(FILE* _file)
   {
       fputws((wchar_t *) m_szCameraInfo.cStr(),_file);
       fputws((wchar_t *) m_szEditorInfo.cStr(),_file);
       fputws((wchar_t *) m_undoRedoInfo.cStr(),_file);
       fputws((wchar_t *) m_engineInfo.cStr(),_file);

       u32 numGeneric = m_genericInfo.size();

       for ( u32 i = 0; i < numGeneric; i++ )
       {
          fputws((wchar_t *) m_genericInfo[i].cStr(),_file);
       }
   }
#endif //ITF_WINDOWS

   void DebugInfo::addDebugInfoDisplay(const String& _text,f32 _time)
   {
       if(CONFIG->m_noWarnings)
           return;

       csAutoLock cs(m_cs);
       DebugInfoDisplay debugInfoDisplay;
       debugInfoDisplay.text = _text;
       debugInfoDisplay.timeToDisplay = _time;
       m_infoLines.push_back(debugInfoDisplay);
   }

   void DebugInfo::draw()
   {    
       f32 _dt = LOGICDT;
       const f32 textSize = f32(GFX_ADAPTER->getDebugFontSize());
       Color color = Color::black();
       color.m_a = 0.5f;
       const u32 uColor = color.getAsU32();
       f32 y = 62.0f;
       csAutoLock cs(m_cs);

       ITF_VECTOR<DebugInfoDisplay> ::iterator iter = m_infoLines.begin();

       while (iter!=m_infoLines.end())
       {
           DebugInfoDisplay& debugInfoDisplay = *iter;

           if(debugInfoDisplay.timeToDisplay < 0.0f)
                iter = m_infoLines.erase(iter);
           else
           {
                debugInfoDisplay.timeToDisplay -= _dt;

                //display it
                GFX_ADAPTER->draw2dBox(Vec2d(0.0f, y), debugInfoDisplay.text.getLen() * textSize / 2.5f, textSize, uColor, uColor, uColor, uColor);
                GFX_ADAPTER->drawDBGText(debugInfoDisplay.text,0.0f, y, 1, 1, 0, bfalse, FONT_ALIGN_LEFT);
                ++iter;
                y+=textSize;
           }
       }

   }

};

#endif //ITF_SUPPORT_DEBUGFEATURE
