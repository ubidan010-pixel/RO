#ifndef _ITF_ARCHIVELINKER_H_
#define _ITF_ARCHIVELINKER_H_

#ifndef ITF_MAP_H_
#include "core/Map.h"
#endif 

namespace ITF
{
 class ArchiveMemory;

 class ArchiveLinker
 {
 public:

     ArchiveLinker(ArchiveMemory* _pArchive);
     
     void setArchive(ArchiveMemory* _pArchive) {m_pArchive = _pArchive;};
     void serialize(uPtr _ptr);

     uPtr getLink(uPtr _ptr);
          
 protected :

     Map<uPtr, uPtr> m_References;
     ArchiveMemory* m_pArchive;
    
     bbool m_bReading;
 };

} //namespace ITF

#endif  //_ITF_ARCHIVELINKER_H_