#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_


#ifndef _ITF_RESOURCELOADER_H_
#include "ResourceLoader.h"
#endif //_ITF_RESOURCELOADER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_XML_H_
#include "engine/XML/XMLFile.h"
#endif //_ITF_XML_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_FONTRESOURCE_H_
#include "engine/display/FontResource.h"
#endif //_ITF_FONTRESOURCE_H_

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_





namespace ITF
{

static u32 emptyImageID = 0;

bbool ResourceLoader::load(Texture *image)
{
    ITF_ASSERT(0); // to be coded
    return btrue;
}

bbool ResourceLoader::load(Sound *sound)
{
    ITF_ASSERT(0); // to be coded
    return btrue;
}

Texture* ResourceLoader::createImage(const Path &filename)
{
    Texture* t = newAlloc(mId_Resource,Texture(filename));
    return t;
}


Texture* ResourceLoader::createEmptyImage()
{
    String name;
    name.setTextFormat("createEmptyImage_%d.dummy", emptyImageID++);
    Texture* t = newAlloc(mId_Resource,Texture(name));
    t->hijackPhysicalLoaded(btrue);  // pretend load() was called without changing physical users
    return t;
}


Sound* ResourceLoader::createSound(const Path &filename)
{
	Sound * s = newAlloc(mId_Resource,Sound(filename));
	return s;
}

#ifdef ITF_SUPPORT_XML
XMLFile* ResourceLoader::createXML(const Path &filename)
{
    XMLFile * s = newAlloc(mId_Resource,XMLFile(filename));
    return s;
}
#endif //ITF_SUPPORT_XML

AnimSkeletonResource *ResourceLoader::createAnimSkeletonResource(const Path &filename)
{
    AnimSkeletonResource * s = newAlloc(mId_Resource,AnimSkeletonResource(filename));
    return s;
}

AnimPatchBankResource *ResourceLoader::createAnimPatchBankResource(const Path &filename)
{
    AnimPatchBankResource * s = newAlloc(mId_Resource,AnimPatchBankResource(filename));
    return s;
}

AnimTrackResource *ResourceLoader::createAnimTrackResource(const Path &filename)
{
    AnimTrackResource * s = newAlloc(mId_Resource,AnimTrackResource(filename));
    return s;
}

AnimDependenciesMapResource *ResourceLoader::createAnimDependenciesMapResource(const Path &filename)
{
    AnimDependenciesMapResource * s = newAlloc(mId_Resource,AnimDependenciesMapResource(filename));
    return s;
}

FontResource* ResourceLoader::createFontResource(const Path &filename)
{
    FontResource * s = newAlloc(mId_Resource,FontResource(filename));
    return s;
}

MusicPart* ResourceLoader::createMusicPart(const Path &filename)
{
    MusicPart * s = newAlloc(mId_Resource, MusicPart(filename));
    return s;
}




} // namespace ITF
