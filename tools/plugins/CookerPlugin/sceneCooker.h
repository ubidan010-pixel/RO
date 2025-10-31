#ifndef _ITF_SCENECOOKER_H_
#define _ITF_SCENECOOKER_H_

namespace ITF
{

class Scene;
class ArchiveMemory;
class PolyLine;

class sceneCooker
{
public:

    static bbool sceneCooker::cookFile(const String& _szPlatform,const String& _szFilename);
    static void CookPolyline(PolyLine* _pol, ArchiveMemory* _ArchiveMem);
  };

}

#endif //_ITF_SCENECOOKER_H_