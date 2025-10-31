#ifndef _ITF_ANIMATION_SERIALIZE_H_
#define _ITF_ANIMATION_SERIALIZE_H_


namespace ITF
{
    typedef bbool (*cookFileCallback) (const String&/*_szPlatform*/,const String& /*_szFilename*/, bbool /*_bRaiseError*/, bbool /*_forceCook*/);

    class AnimationSerialize
    {
    public: 
        AnimationSerialize();
        virtual ~AnimationSerialize();

        bbool                       openBinary(String _cookFile, bbool _castWarning = btrue);
        bbool                       saveBinary(String _cookFile);

        virtual void                serialize(ArchiveMemory& _pArchive, bbool _compressedData) = 0;
        virtual void                link() {}
        virtual void                clear() = 0;

        bbool                       loadFile(const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin, bbool raiseError);
        virtual bbool               needRecook() { return bfalse; }
        virtual void                postProcess(const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin) {}

    protected:
        cookFileCallback            m_cookCallback;
    };

}


#endif //#_ITF_ANIMATION_SERIALIZE_H_
