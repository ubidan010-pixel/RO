#pragma once


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections;

#include <windows.h>
#include <vcclr.h>
#include "blobWrapper.h"
#include "pluginWrapper.h"

using uint = unsigned int;
using byte = ITF::u8;

struct profileInfo
{
public:

    profileInfo():m_deltaUpdateMS(0.0f),m_deltaDrawMS(0.0f),m_deltaDrawGPUMS(0.0f),m_typePickable(0),m_hit(1) {};

    uint        objectRef;
    ITF::String m_friendlyName;
    ITF::String m_filename;
    float       m_deltaUpdateMS;
    float       m_deltaDrawMS;
    float       m_deltaDrawGPUMS;
    uint        m_typePickable;
    uint        m_nbComponents;
    uint        m_nbBones;
    uint        m_hit;
    uint        m_updateType;
    uint        m_skipUpdate;
    uint        m_skipDraw;
}; 


public ref class profileInfoWrapper
{public:
    profileInfoWrapper() {};

    uint    objectRef;
    String^ m_friendlyName;
    String^ m_filename;
    float   m_deltaUpdateMS;
    float   m_deltaDrawMS;
    float   m_deltaDrawGPUMS;
    uint    m_typePickable;
    uint    m_nbComponents;
    uint    m_nbBones;
    uint    m_nbHit;
    uint    m_updateType;
    uint    m_skipUpdate;
    uint    m_skipDraw;
}; 


public ref class profileStateWrapper
{public:

profileStateWrapper() {};

 uint objectRef;
 bool enableUpdate;
 bool enableDraw;

}; 

public ref class engineWrapper
{
public:
  engineWrapper(String^ name);

  void disconnect();
  void destroy();
  bool connectToHost(String^ address,int port);
  void update();

  void putThisThreadAsMain();

  void initFilemanager();

  void copyToClipboard(blobWrapper^ _blob);
  void addPlugin(String^ _name,pluginWrapper^ _plugin);

  void extractProfileData(const byte * _byte,uint size,List<profileInfoWrapper^> ^%_dstInfo,List<profileStateWrapper^> ^%_dstState);

  void getFileOpen(Dictionary<String^,int> ^%_openedFiles);

  void fillClassName(Hashtable ^%_dstHashClass);

  bool isConnected() {return m_bIsConnected;}
  
  uint computeEngineSignature();

  String^ m_szExename;
protected:

    bool m_bIsConnected;
};


