#ifndef _ITF_UVATLAS_H_
#define _ITF_UVATLAS_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

namespace ITF
{

struct UVdata
{
public:

    UVdata()
    {
        m_UV0 = Vec2d(0.f, 0.f);
        m_UV1 = Vec2d(1.f, 1.f);
    }
    UVdata(u32 /*_id*/)
    {
        m_UV0 = Vec2d(0.f, 0.f);
        m_UV1 = Vec2d(1.f, 1.f);
    }

    UVdata(u32 /*_id*/, Vec2d _UV0, Vec2d _UV1)
    {
        m_UV0 = _UV0;
        m_UV1 = _UV1;
    }

    ITF_INLINE void setUV0(const Vec2d& _UV0) { m_UV0 = _UV0;};
    ITF_INLINE void setUV1(const Vec2d& _UV1) { m_UV1 = _UV1;};

    ITF_INLINE const Vec2d& getUV0() const {return m_UV0;};
    ITF_INLINE const Vec2d& getUV1() const {return m_UV1;};

	void serialize(ArchiveMemory& _pArchive)
	{
		m_UV0.serialize(_pArchive);
		m_UV1.serialize(_pArchive);
	}

    void setUVdata(const Vec2d& _UV0, const Vec2d& _UV1)
    {
        m_UV0 = _UV0;
        m_UV1 = _UV1;
    }

private:
    Vec2d m_UV0;
    Vec2d m_UV1;
};

class UVAtlas
{
public:

    UVAtlas()
    {
        m_atlasName = "noname";
    }

    UVAtlas(const String& _name)
    {
        m_atlasName = _name;
    }

    ~UVAtlas()
    {
        m_uvMap.clear();
    }


    const UVdata&  getUVDatabyIndex(i32 _index) const;
    const UVdata&  getUVDataAt(u32 _n) const;
    void get4UVbyIndex(i32 _index, Vec2d *m_UV) const;
    void get4UVAt(i32 _n, Vec2d *m_UV) const ;
   
    void addUVdata(i32 _index, UVdata _data);
    void removeUVdataByIndex(i32 _index);
    void removeUVdataAt(i32 _n);
    void deleteAllUVdata();
	static bbool openFile(UVAtlas& _atlas,const String& _path);

    ITF_INLINE void setName(String _name) {m_atlasName = _name;};
    ITF_INLINE const String& getName() const { return m_atlasName;};
    ITF_INLINE const u32 getNumberUV() const {return m_uvMap.size();}

	 void serialize(ArchiveMemory& _pArchive);

private:
    String m_atlasName;
    ITF_MAP<i32, UVdata>  m_uvMap;
    
};


//-----------------------------------------------------------------------------------------------------------------

class UVAtlasManager:public TemplateSingleton<UVAtlasManager>
{
public:

    typedef ITF_MAP<StringID,UVAtlas> UVAtlasKey;
    UVAtlasManager():m_bContainerAccess(bfalse) {};

    void serialize(ArchiveMemory& _archive,UVAtlasKey& _atlasMap);

    void getCookedNamePlatform(const String& _src,const String& _platform, String& _atlName,String& _cookedname );
    void loadDefault();
    void load(const String& _filename);
    const UVAtlas* getAtlas(const String& _filesTexture);

#ifdef ITF_WINDOWS
    void build(const String& _filename,const String& _platform,const ITF_VECTOR<String>& _files);
    void save(const String& _filenameCooked,UVAtlasKey& _atlasMap);

#endif //ITF_WINDOWS

protected:

    UVAtlasKey m_atlasKey;
    bbool m_bContainerAccess;
};


#define UVATLASMANAGER UVAtlasManager::getptr()
}



#endif //_ITF_UVATLAS_H_
