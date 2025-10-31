#ifndef _ITF_TEXTURE_H_
#define _ITF_TEXTURE_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{
    class UVAtlas;

    // Texture Flag.
    const int TEX_F_RenderTexture = 0x00000001;

    class Texture : public Resource
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Texture,Resource,3846959819);
    public:
        enum PixFormat
        {
            //! Each pixel is 3 bytes. RGB in that order.
            PF_RGB,
            //! Each pixel is 4 bytes. RGBA in that order.
            PF_RGBA,

            PF_DXT1, // aka. BC1
            PF_DXT2, // aka. BC2 (alpha is not pre-multiplied)
            PF_DXT3, // aka. BC2 (alpha is pre-multiplied)
            PF_DXT4, // aka. BC3 (alpha is not pre-multiplied)
            PF_DXT5, // aka. BC3 (alpha is pre-multiplied)

			PF_RGB_NATIVE_CTR,
			PF_RGBA_NATIVE_CTR,
			PF_ETC1_CTR,
			PF_ETC1_A4_CTR,
			
			PF_CMPA, // WII : DXT1 for RGB + DXT1 for Alpha
            PF_L8,
            PF_LA8,
            PF_A8,
            ENUM_FORCE_SIZE_32(0)
        };

#ifndef ITF_WII
        u32         m_alpha;
#endif        
        void *      m_adapterimplementationData;
#ifdef ITF_WII
		GXTexObj	m_TexObj;
		GXTexObj	m_TexObjA; // used for CMPA
#endif
       
        u32         m_datasizeX, m_datasizeY;
        Vec2d       m_texelScaling;
        PixFormat   m_pixFormat;
        u32         m_mipMapLevels;

        ResourceType getType()const {return ResourceType_Texture;}
        
        ITF_INLINE const UVAtlas* getUVAtlas()  const {return m_uvAtlas;};
        void    createAtlasIfExist();

        ITF_INLINE const f32 getRatioXY()    const {return m_ratio;}
        ITF_INLINE void setSize(u32 _sizeX,u32 _sizeY) 
        {
              m_sizeX = _sizeX;
              m_sizeY = _sizeY;
              if (_sizeY!=0)
                m_ratio = (f32) _sizeX / (f32)_sizeY;
        }

        ITF_INLINE const u32 getSizeX()    const {return m_sizeX;}
        ITF_INLINE const u32 getSizeY()    const {return m_sizeY;}

        virtual void    postCreate();
#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()      const   {return btrue;}
        virtual void setUseDefault(Resource* _pDefaultResource);
        virtual void cancelDefault();
#endif //ITF_SUPPORT_COOKING


#ifdef  MODE_HRESTEXTURE_SUPPORTED
        const bbool hasHResVersion() {return m_HResVersion;}
#endif //MODE_HRESTEXTURE_SUPPORTED

    private:
         
        f32         m_ratio;
        const UVAtlas*    m_uvAtlas;
#ifndef ITF_WII
        u8*         m_fileContent;
        u32         m_fileContentSize;
#endif        
        bbool       m_createdGFXObject;

        u32         m_sizeX, m_sizeY;
 


#ifdef MODE_HRESTEXTURE_SUPPORTED
        bbool      m_HResVersion;
#endif //MODE_HRESTEXTURE_SUPPORTED

        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instantiate a resource

        Texture(const Path &path);
        virtual ~Texture();

        void    flushPhysicalData        (                        );
        bbool   tryLoadFile       (                        );
        void    onResourceProcessingFromMainThread();


#ifndef ITF_FINAL
        virtual String getCustomInfo();
        public:
        char m_dbgNfo[8];
#endif //!ITF_FINAL
    };

} // namespace ITF

#endif // _ITF_TEXTURE_H_
