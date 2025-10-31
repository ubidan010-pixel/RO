#ifndef _ITF_VERSIONING_H_
#define _ITF_VERSIONING_H_

namespace ITF
{
    class Blob;
    class ArchiveMemory;

    class Versioning
    {

    public:

        static u32 logicDatabaseCacheVer()          {return 10;}
        static u32 textureVer()                     {return 3;}
        static u32 soundVer()                       {return 1;}
        static u32 animPatchBankVer()               {return 13;}
        static u32 animTrackVer()                   {return 26;}
        static u32 animSkeletonVer()                {return 11;}
        static u32 animDependeniesVer()             {return 8;}
        static u32 gameVer()                        {return 4;}
        static u32 bundleVer()                      {return 3;}
        static u32 friezeConfigVer()                {return 75;}
        static u32 sceneVer()                       {return 5;}
		static u32 atlasVer()                       {return 2;}


        static bbool serializeVersion(ArchiveMemory& __archive, u32 _version);

        static void serializeAll(ArchiveMemory& __archive);

        static bbool Versions(Blob& _blob,bbool _bWrite);

        static u32 getEngineVersion();
    };


} //namespace ITF


#endif
