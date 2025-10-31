#ifndef _ITF_TEXTURECOOKERHRES_H_
#define _ITF_TEXTURECOOKERHRES_H_

namespace ITF
{

#ifdef MODE_HRESTEXTURE_SUPPORTED
class textureCookerHRes
{
public:

    static bbool cookFile(const String& _vPlatform,const String& _szFilename);
    static bbool compress(const String& _vPlatform,const String& _szFilename);
    static bbool binarize(const String& _szFilename);
    static bbool cudaIsEnable();
};
#endif //MODE_HRESTEXTURE_SUPPORTED
}

#endif //_ITF_TEXTURECOOKERHRES_H_