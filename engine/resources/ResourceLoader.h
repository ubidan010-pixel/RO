#ifndef _ITF_RESOURCELOADER_H_
#define _ITF_RESOURCELOADER_H_


namespace ITF
{
    class    Texture;
    class    Sound;
    class    Lua;
    class    AnimSkeletonResource;
    class    AnimPatchBankResource;
    class    AnimTrackResource;
    class    AnimDependenciesMapResource;
    class    FontResource;
    class    XMLFile;
    class    MusicPart;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// The ResourceLoader class is a wrapper to the various adapters
    /// Its role is to trigger physical resources loading. It's a factory
    class ResourceLoader
    {
    public:        
        ResourceLoader(){};
        virtual ~ResourceLoader() {};

        virtual bbool load(Texture *image);
        virtual bbool load(Sound *sound);

    private:
        friend class ResourceManager; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instanciate a resource 

        virtual Texture *createImage(const Path &filename);
        virtual Texture *createEmptyImage();
        virtual Sound *createSound(const Path &filename);

#ifdef ITF_SUPPORT_XML
        virtual XMLFile *createXML(const Path &filename);
#endif //ITF_SUPPORT_XML
        virtual AnimSkeletonResource *createAnimSkeletonResource(const Path &filename);
        virtual AnimPatchBankResource *createAnimPatchBankResource(const Path &filename);
        virtual AnimTrackResource *createAnimTrackResource(const Path &filename);
        virtual AnimDependenciesMapResource *createAnimDependenciesMapResource(const Path &filename);
        virtual FontResource *createFontResource(const Path &filename);
        virtual MusicPart *createMusicPart(const Path &filename);
    };
} // namespace ITF

#endif // _ITF_RESOURCELOADER_H_
