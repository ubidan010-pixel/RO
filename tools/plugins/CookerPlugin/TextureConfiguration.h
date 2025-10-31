#ifndef _ITF_TEXTURECONFIGURATION_H_
#define _ITF_TEXTURECONFIGURATION_H_

#include <nvtt/nvtt.h>

namespace ITF
{
	class TextureConfiguration 
	{
	public:
        enum eTernaryValue
        {
            eNO =   0,
            eYES =  1,
            eAUTO = 2
        };

        enum eAnisotropic
        {
            eMAX =  0,
            eLOW =  1,
            eNONE = 2
        };

		TextureConfiguration():m_useMipmap(btrue),m_mipmapStart(0),m_autoDegrad(btrue),m_lod_bias(0.f),m_autoDegradAlpha(bfalse),m_oneColMode(eAUTO),m_Anisotropy(eMAX) {};

		void setUseMipmap(bbool _val)                       {m_useMipmap = _val;}
		const bbool isUseMipmap() const                     {return m_useMipmap;}

		const String& getCompressionMode() const            {return m_compressionMode;}
		void setCompressionMode(const String& _val)         {m_compressionMode = _val;}

		const String& getWrapMode() const                   {return m_wrapMode;}
		void setWrapMode(const String& _val)                {m_wrapMode = _val;}

		const String& getMipmapFilter() const               {return m_mipmapFilter;}
		void setMipmapFilter(const String& _val)            {m_mipmapFilter = _val;}

		const u32    getMipmapStart() const                 {return m_mipmapStart;}
		void setMipmapStart(u32 _val)                       {m_mipmapStart = _val;}

		const bbool getAutoDegrad() const                   {return m_autoDegrad;}
		void setAutoDegrad(bbool _val)                      {m_autoDegrad = _val;}

        const f32 getLodBias() const                        {return m_lod_bias;}
        void setLodBias(f32 _val)                           {m_lod_bias = _val;}

		const bbool getAutoDegradAlpha()         const       {return m_autoDegradAlpha;}
		void setAutoDegradAlpha(bbool _val)                  {m_autoDegradAlpha = _val;}
        const eTernaryValue getOneColMode()      const       {return m_oneColMode;}
        void setOneColMode(eTernaryValue _v)                 {m_oneColMode = _v;}
        void setAnisotropy(eAnisotropic _v)                  {m_Anisotropy = _v;}
        const eAnisotropic getAnisotropy()      const        {return m_Anisotropy;}

		nvtt::Format getnvttFormat() const;
		nvtt::WrapMode getnvttWrapMode() const;
		nvtt::MipmapFilter getnvttMipMapFilter() const;

        String getTexConvFormatOption() const;
        String getTexConvWrapModeOption() const;
        String getTexConvMipMapFilterOption() const;

		static TextureConfiguration* openTextureConfiguration(const String& _szPlatform,const String& _szFilename);

	protected:

		String  m_compressionMode;
		String  m_wrapMode;
		String  m_mipmapFilter;
		bbool   m_useMipmap;
		u32     m_mipmapStart;
        bbool   m_autoDegrad;
        f32     m_lod_bias;
        bbool   m_autoDegradAlpha;
        eTernaryValue m_oneColMode;
        eAnisotropic m_Anisotropy;
	};

}

#endif //_ITF_TEXTURECONFIGURATION_H_
