#include "precompiled_engine.h"

#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_FONTLOADER_H_
#include "engine/display/fontLoader.h"
#endif //_ITF_FONTLOADER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#if defined(ITF_CTR) || defined(ITF_WII)
#define USE_VERTEX_PCT
#endif // defined(ITF_CTR) || defined(ITF_WII)

#if defined (ITF_WINDOWS) && !defined (ITF_CONSOLE)
#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_
#endif //defined (ITF_WINDOWS) && !defined (ITF_CONSOLE)

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------------------

#define MAX_SPACES_BY_ICON      21

bool PrefetchFont( const Path& path, PrefetchFactory::Params& params )
{
    return ResourceManager::PrefetchResource( path, params, Resource::ResourceType_Font );
}

Font::Font()
{
    fontHeight = 0;
    base = 0;
    scaleW = 0;
    scaleH = 0;
    scale = 1.0f;
    globalScale = 1.0f;
    hasOutline = false;
    m_textAdjustement = bfalse;
    encoding = NONE;
    m_totalTextHeight = 0.f;
    m_totalTextHeightComputed = 0.f;
    m_yNextLine = 0.f;
    outlineThickness = 0.0f;
	m_boxTransform = bfalse;
    m_autoScale = btrue;
    m_shadowOffset = Vec2d::Zero;
    m_shadowcolor = COLOR_BLACK;
    m_is2d = bfalse;
    m_lineSpacingFactor = 1.f;
	m_textWithStyleTabNb = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------

Font::~Font()
{
    cleanFontCache();
    ITF_MAP<i32, SCharDescr*>::iterator it = chars.begin();
    while( it != chars.end() )
    {
        delete it->second;
        it++;
    }
    chars.clear();

    for( u32 n = 0; n < pages.size(); n++ )
    {
        RESOURCE_MANAGER->removeUserToResourceData(pages[n]);
    }
    pages.clear();
}
void Font::getFontPathBaseLanguage(const char *fontFile,ITF_LANGUAGE _lanugage,String8& fontFileLocal)
{
    Path fullpath(fontFile);
    String path(fullpath.getDirectory());
    String baseName;
    fullpath.getBasenameWithoutExtension(baseName);
    String suffix("");
    String ext(fullpath.getExtension());
    switch (_lanugage)
    {
        case ITF_LANGUAGE_JAPANESE:
            suffix = "_jap.";
            break;
        case ITF_LANGUAGE_SIMPLIFIEDCHINESE:
            suffix="_sch.";
            break;
        case ITF_LANGUAGE_TRADITIONALCHINESE:
            suffix="_tch.";
        default: break;
    }
    if (suffix!="")
    {
        String fontFileLocal16 = path + baseName + suffix + ext;
        fontFileLocal = fontFileLocal16.cStr();
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------

i32 Font::init(const char *fontFile)
{
    String8 fontFileLocal(fontFile);
    ITF_LANGUAGE currentLanguage = LOCALISATIONMANAGER->getCurrentLanguage();
    getFontPathBaseLanguage(fontFile,currentLanguage,fontFileLocal);
//#ifndef ITF_WII
    File* fileFont = FILEMANAGER->openFile(fontFileLocal,ITF_FILE_ATTR_READ);
    if (fileFont)
    {
        // Determine format by reading the first bytes of the file
        char str[4] = {0};

        fileFont->read(str,3);
        fileFont->seek(0,0);

		FontLoader *loader = 0;
		if( strcmp(str, "BMF") == 0 )
			loader = newAlloc(mId_Resource,FontLoaderBinaryFormat(fileFont, this, fontFileLocal.cStr()));
		else
			loader = newAlloc(mId_Resource,FontLoaderTextFormat(fileFont, this, fontFileLocal.cStr()));

        i32 r = loader->Load();
        SF_DEL(loader);
        return r;
    }

//#endif
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Internal
SCharDescr *Font::getChar(i32 id)
{
    ITF_MAP<i32, SCharDescr*>::iterator it = chars.find(id);
    if( it == chars.end() )
    {
       if (id == L'F')
       {
            static SCharDescr charDescForF;
            charDescForF.srcX = 831;
            charDescForF.srcY = 958;
            charDescForF.srcW = 39;
            charDescForF.srcH = 54;
            charDescForF.xOff = -1;
            charDescForF.yOff = 14;
            charDescForF.xAdv = 36;
            charDescForF.page = 0;
            charDescForF.chnl = 0;
            return &charDescForF;
        }
        return 0;
    }

    return it->second;
}

bbool Font::texturePageIsPhysicalReady() const
{
    for (ITF_VECTOR<ResourceID>::const_iterator iter =  pages.begin();iter!=pages.end();++iter)
    {
        const ResourceID& resID = *iter;
        Resource* pTexture = resID.getResource();
        if (!pTexture->isPhysicallyLoaded())
            return bfalse;
    }

    return btrue;
}

//---------------------------------------------------------------------------------------------------------------------------------------

// Internal
f32 Font::adjustForKerningPairs(i32 first, i32 second)
{
    SCharDescr *ch = getChar(first);
    if( ch == 0 ) return 0;
    for( u32 n = 0; n < ch->kerningPairs.size(); n += 2 )
    {
        if( ch->kerningPairs[n] == second )
            return ch->kerningPairs[n+1] * scale;
    }

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Internal
f32 Font::adjustForKerningPairsDescr(const SCharDescr *ch, i32 second)
{
    if( ch == 0 ) return 0;
    for( u32 n = 0; n < ch->kerningPairs.size(); n += 2 )
    {
        if( ch->kerningPairs[n] == second )
            return ch->kerningPairs[n+1] * scale;
    }

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------


f32 Font::getTextWidth(const String& text, i32 count)
{
    if( count <= 0 )
    {
        count = getTextLength(text);
    }

    f32 x = 0;

    for( i32 n = 0; n < count; )
    {
        i32 charId = getTextChar(text,n,&n);

        SCharDescr *ch = getChar(charId);
        if( ch == 0 ) ch = &defChar;
        x += scale * ((ch->xAdv ) + getOutlineThickness());// * scalee;

        if( n < count )
            x += adjustForKerningPairs(charId, getTextChar(text,n));
    }

    return x;
}

//---------------------------------------------------------------------------------------------------------------------------------------

f32 Font::getTextWidthMS(const String& text, i32 count, const SafeArray<CharInfo> &_charInfoTab, u32 _tabStart)
{
    if( count <= 0 )
    {
        count = getTextLength(text);
    }

    u32 tabCpt = _tabStart;
    f32 x = 0;

    for( i32 n = 0; n < count; )
    {
        const CharInfo &charInfo = (_charInfoTab)[tabCpt];
        tabCpt++;

        // Temporarily modify the scale
        f32 oldScale = scale;
        scale = charInfo.m_charSize / f32(fontHeight) * UI_TEXTMANAGER->getTextBoxScale();
        scale *= globalScale;

        i32 charId = getTextChar(text,n,&n);

        SCharDescr *ch = getChar(charId);
        if( ch == 0 ) ch = &defChar;

        x += scale * ((ch->xAdv) + getOutlineThickness());

        if( n < count )
            x += adjustForKerningPairs(charId, getTextChar(text,n));

        scale = oldScale;
    }

    return x;
}

//---------------------------------------------------------------------------------------------------------------------------------------

int Font::getTextLength(const String&text)
{
    return text.getLen();
}


//---------------------------------------------------------------------------------------------------------------------------------------

ITF_INLINE i32 Font::getTextChar(const String& text, i32 pos, i32 *nextPos)
{
    i32 ch = 0;
    u32 len;

    {
        len = 1;
        ch = text.cStr()[pos];
    }

    if( nextPos ) *nextPos = pos + len;
    return ch;
}

ITF_INLINE void Font::getNextChar(const String& text, i32 & pos)
{
    // depending on utf16 or ucs2 support, the String could be needed
    pos++;
}


//---------------------------------------------------------------------------------------------------------------------------------------

i32 Font::findTextChar(const String& text, i32 start, i32 length, i32 ch)
{
    i32 pos = start;
    i32 nextPos;
    i32 currChar = -1;
    while( pos < length )
    {
        currChar = getTextChar(text, pos, &nextPos);
        if( currChar == ch )
            return pos;
        pos = nextPos;
    }

    return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------

void Font::manageFontCache()
{
    for (i32 index=m_vFontCache.size()-1 ; index>=0 ; --index)
    {
        FontCache* pCache = m_vFontCache[index];
        if (pCache->unUsed())
        {
            SF_DEL(pCache);
            m_vFontCache.eraseNoOrder(index);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------

void Font::internalWrite(unsigned int colorOriginal,f32 x, f32 y, f32 z, bbool _isRender2D, const String& text, i32 count, u32 _mode, f32 spacing, const SafeArray<CharInfo> *_charInfoTab, i32 startTabColor)
{
    if ( count <= 0 )
    {
        return;
    }

    ITF_ASSERT(pages.size() < FONT_PAGE_MAX);

    FontCache* pCacheUsed = NULL;
    const u32 fontCacheSize = m_vFontCache.size();

    // force no 2d z;
    if (_isRender2D)
        z = 0.f;

    Vec3d pos(x,y,z);
    String internalCacheTextToCache = text;

    u32 color = colorOriginal;
    u32 crcCharInfo = _charInfoTab ? _charInfoTab->computeCRC() : 0;

    //check if you can use the font cache...
    if (fontCacheSize)
    {
        for (u32 index = 0; index < fontCacheSize; ++index)
        {
            FontCache* pCache = m_vFontCache[index];
            if (pCache->match(_isRender2D, count,crcCharInfo,colorOriginal,pos,internalCacheTextToCache, _mode))
            {
                pCacheUsed = pCache;
                pCacheUsed->usage();
                break;
            }
        }
    }

#if 0
    // HACK : find a way to be able to modify vertex buffer position using its cache when
    // the only thing that has change from the font is its position (text is the same, size of the box is the same, etc.)
    if ( !_isRender2D ) //Only for 3d
        pCacheUsed = NULL;
#endif

    u32 page = 0;
    bbool pageFirstChange = btrue;

    for ( ITF_VECTOR<ResourceID>::const_iterator iter = pages.begin(); iter!= pages.end();++iter)
    {
        const ResourceID& rid = (*iter);
        Texture* pTexture = (Texture*) rid.getResource();

        ITF_ASSERT(pTexture != NULL);
        if (pTexture == NULL || !pTexture->isPhysicallyLoaded())
            return;
    }

    // init mesh
    ITF_Mesh *pMesh[FONT_PAGE_MAX];

    // init myBuffer
    #ifdef USE_VERTEX_PCT
        VertexPCT *myBuffer[FONT_PAGE_MAX];
    #else
        VertexPCBT *myBuffer[FONT_PAGE_MAX];
    #endif
    u32 countVertices[FONT_PAGE_MAX];

    for ( u32 iPage = 0; iPage < FONT_PAGE_MAX; iPage++ )
    {
        countVertices[iPage] = 0;
        pMesh[iPage] = 0;
    }

    if (!pCacheUsed)
    {
        u32 countByPage[FONT_PAGE_MAX];

        for ( u32 iPage = 0; iPage < FONT_PAGE_MAX; iPage++ )
            countByPage[iPage] = 0;
        // count of char by page
        for ( i32 iChar = 0; iChar < count; iChar++ )
        {
            i32 charId = getTextChar(text, iChar, NULL);
            SCharDescr *ch = getChar(charId);
            if(ch)
            {
                ITF_ASSERT(ch->page < FONT_PAGE_MAX);
                if(ch->page < FONT_PAGE_MAX)
                    countByPage[ch->page]++;
            }
            else
            {
                ITF_ASSERT(defChar.page < FONT_PAGE_MAX);
                if(defChar.page < FONT_PAGE_MAX)
                    countByPage[defChar.page]++;
            }
        }

#ifdef ASSERT_ENABLED
        i32 ic = 0;
        for ( i32 iPage = 0; iPage < FONT_PAGE_MAX; iPage++ )
            ic += countByPage[iPage];
        ITF_ASSERT(ic == count);
#endif

        // new mesh allocation
        for ( u32 iPage = 0; iPage < FONT_PAGE_MAX; iPage++ )
        {
            if (iPage < pages.size() && countByPage[iPage] != 0)
            {
                pMesh[iPage] = newAlloc(mId_Mesh, ITF_Mesh());
                #ifdef USE_VERTEX_PCT
                    pMesh[iPage]->m_vtxBuffer =   GFX_ADAPTER->createVertexBuffer(countByPage[iPage] * 6, VertexFormat_PCT, sizeof(VertexPCT), bfalse, VB_T_FONT);
                #else
                    pMesh[iPage]->m_vtxBuffer =   GFX_ADAPTER->createVertexBuffer(countByPage[iPage] * 6, VertexFormat_PCBT, sizeof(VertexPCBT), bfalse, VB_T_FONT);
                #endif

                GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(pMesh[iPage]->m_vtxBuffer,(void **)&myBuffer[iPage]);
            }
            else
            {
                pMesh[iPage] = NULL;
            }
            countVertices[iPage] = 0;
        }

        if (!_isRender2D)
        {
            x -= m_boxPos.m_x;
            y -= m_boxPos.m_y;
        }

        //f32 highestSize = 0.f;
        if (_charInfoTab)
        {
            // $GB highestSize reported as not used by a warning, what was the intent?
            // highestSize = getHighestSize(*_charInfoTab, startTabColor, count);
        }
        else
        {
			if (_isRender2D)
				y = GFX_ADAPTER->getScreenHeight()- y + scale * f32(base);
			else
				y = y + scale * f32(base);
        }

        for( i32 n = 0; n < count; )
        {
            f32 oldScale = scale;
            f32 oldY = y;
            if (_charInfoTab)
            {
                color = (*_charInfoTab)[startTabColor + n].m_charColor;
                f32 newHeight = (*_charInfoTab)[startTabColor + n].m_charSize;
                scale = newHeight / f32(fontHeight) * UI_TEXTMANAGER->getTextBoxScale();
                scale *= globalScale;
                if (_isRender2D)
					y = GFX_ADAPTER->getScreenHeight()- y + scale * f32(base);
				else
					y = y + scale * f32(base);
			}

            i32 charId = getTextChar(text, n, &n);
            SCharDescr *ch = getChar(charId);
            if( ch == 0 ) ch = &defChar;

            // Map the center of the texel to the corners
            // in order to get pixel perfect mapping
            f32 u = (f32(ch->srcX)+0.5f) / scaleW;
            f32 v = (f32(ch->srcY)+0.5f) / scaleH;
            f32 u2 = u + f32(ch->srcW) / scaleW;
            f32 v2 = v + f32(ch->srcH) / scaleH;

            f32 a = scale * f32(ch->xAdv);
            f32 w = scale * f32(ch->srcW);
            f32 h = scale * f32(ch->srcH);
            f32 ox = scale * f32(ch->xOff);
            f32 oy = scale * f32(ch->yOff);

            if( (u32)ch->page != page || pageFirstChange)
            {
                pageFirstChange = bfalse;
                const ResourceID& resID = pages[ch->page];
                Texture* pTexture = (Texture*) resID.getResource();

                if (!pTexture->isPhysicallyLoaded())
                    continue;

                GFX_ADAPTER->SetTexture(0,pTexture);
                page = ch->page;
            }

            ITF_ASSERT(countByPage[page] * 6 >= countVertices[page] + 6);
#ifdef USE_VERTEX_PCT
            myBuffer[page]->setData(Vec3d(x+ox, y-oy, z),Vec2d(u, v),color);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+w+ox, y-oy, z),Vec2d(u2, v),color);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+w+ox, y-h-oy, z),Vec2d(u2, v2),color);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+ox, y-h-oy, z),Vec2d(u, v2),color);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+ox, y-oy, z),Vec2d(u, v),color);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+w+ox, y-h-oy, z),Vec2d(u2, v2),color);
            myBuffer[page]++;
            countVertices[page]+=6;
#else
            myBuffer[page]->setData(Vec3d(x+ox, y-oy, z),Vec3d(0,0,0),Vec2d(u, v),color,ch->chnl);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+w+ox, y-oy, z),Vec3d(0,0,0),Vec2d(u2, v),color,ch->chnl);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+w+ox, y-h-oy, z),Vec3d(0,0,0),Vec2d(u2, v2),color,ch->chnl);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+ox, y-h-oy, z),Vec3d(0,0,0),Vec2d(u, v2),color,ch->chnl);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+ox, y-oy, z),Vec3d(0,0,0),Vec2d(u, v),color,ch->chnl);
            myBuffer[page]++;
            myBuffer[page]->setData(Vec3d(x+w+ox, y-h-oy, z),Vec3d(0,0,0),Vec2d(u2, v2),color,ch->chnl);
            myBuffer[page]++;
            countVertices[page]+=6;
#endif
            x += a + (scale * getOutlineThickness());
            if( charId == ' ' )
                x += spacing;

            if( n < count )
                x += adjustForKerningPairsDescr(ch, getTextChar(text,n));

            scale = oldScale;
            y = oldY;
        }

        for ( u32 iPage = 0; iPage < pages.size(); iPage++ )
        {
            if (pMesh[iPage] && countByPage[iPage] != 0)
            {
                GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(pMesh[iPage]->m_vtxBuffer);
            }
        }
    }
    else
    {
        for ( u32 iPage = 0; iPage < pages.size(); iPage++ )
        {
            pMesh[iPage] = pCacheUsed->getMesh(iPage);
            countVertices[iPage] = pCacheUsed->getVerticesCount(iPage);
        }
    }

	// transformation with box.
    Matrix44 mOut;

    if (m_boxTransform)
	{
        Matrix44 translation;
		Matrix44 mRotScale;
        Matrix44 mtransform;

		M44_setMatrixRotationZ(&mRotScale, m_boxRotation);
		mRotScale.mulScale(m_boxScale);

 		f32 pox = 0.f;
        f32 poy = 0.f;

        if ( !_isRender2D )
        {
            pox = m_boxPivot.m_x;
            poy = m_boxPivot.m_y;

            Matrix44 invtrans;
            M44_setMatrixTranslation(&invtrans, -pox, -poy, -z);
            M44_mul4x4Out(&mtransform, &invtrans, &mRotScale);
            pox = m_boxPos.m_x;
            poy = m_boxPos.m_y;
        }
        else
        {
            pox = m_boxPos.m_x + m_boxPivot.m_x;
            poy = GFX_ADAPTER->getScreenHeight() - (m_boxPos.m_y + m_boxPivot.m_y);

            Matrix44 invtrans;
            M44_setMatrixTranslation(&invtrans, -pox, -poy, -z);
            M44_mul4x4Out(&mtransform, &invtrans, &mRotScale);
        }

        M44_setMatrixTranslation(&translation, pox, poy, z);
        M44_mul4x4Out(&mOut, &mtransform, &translation);

		GFX_ADAPTER->setMatrixTransform(GFX_MWORLD, (GMatrix44*)&mOut);
    }

    for ( u32 iPage = 0; iPage < pages.size(); iPage++ )
    {
        if (!countVertices[iPage])
            continue;

        const ResourceID& resID = pages[iPage];
        Texture* pTexture = (Texture*) resID.getResource();

        if (!pTexture->isPhysicallyLoaded())
            continue;

        GFX_ADAPTER->SetTexture(0,pTexture);

        if (!m_boxTransform)
        {
            if ( _isRender2D )
            {
                mOut = pMesh[iPage]->getMatrix();
            }
            else
            {
                M44_setMatrixTranslation(&mOut, m_boxPos.m_x, m_boxPos.m_y, z);
                GFX_ADAPTER->setMatrixTransform(GFX_MWORLD, (GMatrix44*)&mOut);
            }
        }

        //draw
        GFX_ADAPTER->setAlphaBlend(GFX_BLEND_ALPHA);

        if (!m_shadowOffset.IsEqual(Vec2d::Zero, MTH_EPSILON))
        {
            f32 rap = fontHeight / FONT_INITIAL_SIZE;
            Matrix44 Offset = mOut;
            Vec3d tr(mOut.fa16[12] + m_shadowOffset.m_x * (scale*3.f * rap), mOut.fa16[13] - m_shadowOffset.m_y * (scale*3.f * rap) , mOut.fa16[14]);
            Offset.setTranslation(tr);
            GFX_ADAPTER->setMatrixTransform(GFX_MWORLD, (GMatrix44*)&Offset);
            GFX_ADAPTER->setGlobalColor(m_shadowcolor);
            GFX_ADAPTER->DrawVertexBuffer(GFX_TRIANGLES,  pMesh[iPage]->m_vtxBuffer, 0,countVertices[iPage] );
        }

        GFX_ADAPTER->setGlobalColor(Color::white());
        GFX_ADAPTER->setMatrixTransform(GFX_MWORLD, (GMatrix44*)&mOut);
        GFX_ADAPTER->DrawVertexBuffer(GFX_TRIANGLES,  pMesh[iPage]->m_vtxBuffer, 0,countVertices[iPage] );
    }

    if (!pCacheUsed)
    {
        FontCache* pCache = newAlloc(mId_GfxAdapter, FontCache(pMesh, count, crcCharInfo, colorOriginal, pos, _mode, internalCacheTextToCache, countVertices));
        m_vFontCache.push_back(pCache);
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------

f32 Font::write(u32 color,f32 x, f32 y, f32 z, const String& text, i32 count, u32 mode, bbool _write )
{
    if( count <= 0 )
        count = text.getLen();

    f32 w = getTextWidth(text, count);

    if( mode == FONT_ALIGN_CENTER )
    {
        x -= w/2;
    }
    else if( mode == FONT_ALIGN_RIGHT )
    {
        x -= w;
    }

    if ( _write )
    {
        internalWrite(color,x, y, z, btrue, text, count, mode);
    }

    return w;
}

//---------------------------------------------------------------------------------------------------------------------------------------

f32 Font::writeML(u32 color,f32 x, f32 y, f32 z, const String& text, i32 count, u32 mode, bbool _write )
{
    if( count <= 0 )
        count = getTextLength(text.cStr());

    f32 maxW = 0.0f;

    // Get first line
    i32 pos = 0;
    i32 len = findTextChar(text, pos, count, '\n');
    if( len == -1 ) len = count;
    while( pos < count )
    {
        f32 cx = x;
        f32 w = getTextWidth(&text.cStr()[pos], len);

        if( mode == FONT_ALIGN_CENTER )
            cx -= w/2;
        else if( mode == FONT_ALIGN_RIGHT )
            cx -= w;

        if ( _write )
        {
            internalWrite(color,cx, y, z, btrue, &text.cStr()[pos], len, mode);
        }

        if (w > maxW)
            maxW = w;

        y += scale * f32(fontHeight);

        // Get next line
        pos += len;
        i32 ch = getTextChar(text, pos, &pos);
        if( ch == '\n' )
        {
            len = findTextChar(text, pos, count, '\n');
            if( len == -1 )
                len = count - pos;
            else
                len = len - pos;
        }
    }
    return maxW;
}

inline bbool isSpace(i32 _char)
{
    if (_char == ' ' || _char == 12288)
        return btrue;
    else
        return bfalse;
}

namespace FontPrivate
{
    static const char START_TAG_DELIMITER = '[';
    static const char TAG_NAME_SEPARATOR = ':';
    static const char END_TAG_DELIMITER = ']';

    static String BuildNamedTag(const char* tagName)
    {
        String res;
        res += START_TAG_DELIMITER;
        res += tagName;
        res += TAG_NAME_SEPARATOR;
        return res;
    }
}

void Font::writeBox(u32 color,f32 x, f32 y, f32 z, bbool _isRender2D, const Vec3d &_boxPos, const Vec3d &_boxPivot, const Vec3d &_boxScale, f32 _boxRotation, bbool _useTransBox, f32 width, const SafeArray<CharInfo> &_charInfoTab, const String& text, i32 count, u32 mode, u32 modeY, f32 height, bbool _write, bbool _cut, TextActorInfoList* _actorInfo )
{
    if( count <= 0 )
        count = getTextLength(text);

    if (!count)
        return;

    static SafeArray<TAGicon> tagicon;
    static SafeArray<TAGicon> tagActor;

    tagicon.clear();
    tagActor.clear();

	// record Box params.
	m_boxPos = _boxPos;
    m_boxScale = _boxScale;
	m_boxPivot = _boxPivot;
	m_boxRotation = _boxRotation;
	m_boxTransform = _useTransBox;
    m_textAdjustement = bfalse;
    m_is2d = _isRender2D;

	f32 currWidth = 0, wordWidth = 0;
    i32 lineS = 0, lineE = 0, wordS = 0, wordE = 0;
    i32 wordCount = 0;
    bbool softBreak = bfalse;

    String linetext;
    String word;
    bbool keepContentToNextLine = bfalse;
    f32 curPos = 0.f;
    bbool addIcon = bfalse;
    TAGicon newicon;
    bbool addActor = bfalse;
    TAGicon newActor;
    f32 actorWidth = 0.f;
    u32 actorCounter = 0;
    u32 actorPosCounter = 0;

    // icon size depends on resolution & font height (RO-11195 + RO-11198)
    // NB[LaurentCou]: take the size for the entire text as we don't know what
    // to take as the first line yet
    static const f32 s_refHighestSize =  40.0f;
    f32 highestSize = getHighestSize(_charInfoTab, 0, count);
    const f32 iconScale = highestSize / s_refHighestSize;
    const f32 iconSize = UI_TEXTMANAGER->getIconSize() * iconScale;
    const f32 iconYOffset = UI_TEXTMANAGER->getIconYOffset() * iconScale;
    const f32 iconXOffset = UI_TEXTMANAGER->getIconXOffset() * iconScale;

    for(; lineS < count;)
    {
        // Skip TAG
        if(addIcon || addActor)
        {
            lineE = wordE;
            lineS = lineE;
        }

        // Determine the extent of the line
        tagicon.clear();
        tagActor.clear();

        if ( !keepContentToNextLine )
        {
            curPos = 0.f;
            linetext = "";
        }
        else
        {
            keepContentToNextLine = bfalse;

            if ( addIcon )
            {
                tagicon.push_back(newicon);
                addIcon = bfalse;
            }

            if ( addActor )
            {
                tagActor.push_back(newActor);
                addActor = bfalse;
            }
        }

        for(;;)
        {
            // Determine the number of characters in the word
            bbool tagOpened = bfalse;
            word = "";
            //wordWidth = 0;

            while( wordE < count &&
                !isSpace( getTextChar(text, wordE) ) &&
                getTextChar(text, wordE) != '\n' )
             {
                u16 c = text.cStr()[wordE];

                if(word.getLen() == 0)
                    tagOpened = (c == FontPrivate::START_TAG_DELIMITER);

                word += c;

                // Advance the cursor to the next character
                getNextChar(text,wordE);
            }

            if(tagOpened)
            {
                static const String iconTag = FontPrivate::BuildNamedTag("icon");
                static const String actorTag = FontPrivate::BuildNamedTag("actor");
                static const u32 iconTagBeginSize = iconTag.getLen();
                static const u32 actorTagBeginSize = actorTag.getLen();
                i32 indexIconBegin, indexIconEnd;
                String8 textWithoutTags = "";

                word.strstr(iconTag.cStr(), bfalse, &indexIconBegin);
                word.strstr(FontPrivate::END_TAG_DELIMITER, bfalse, &indexIconEnd);

                if (indexIconBegin != -1 && indexIconEnd != -1 && indexIconEnd > indexIconBegin) // Tag found
                {
                    String charsAfterBraket(word.substr(indexIconEnd+1, word.getLen()- 1 - indexIconEnd));

                    const String iconName = word.substr(indexIconBegin + iconTagBeginSize, indexIconEnd - indexIconBegin - iconTagBeginSize);
                    String8 iconName8(iconName.cStr());

                    i32 skipIconIndex = -1;
                    newicon.m_isSkipIcon = UI_TEXTMANAGER->getSkipIconInfo(iconName8, skipIconIndex);
                    if (newicon.m_isSkipIcon)
                    {
                        newicon.m_isButton = bfalse;
                        newicon.m_index = skipIconIndex;
                    }
                    else
                    {
                        newicon.m_isSkipIcon = bfalse;
                    }

                    i32 menuLogoIndex = -1;
                    newicon.m_isMenuLogo = UI_TEXTMANAGER->getMenuLogoInfo(iconName8, menuLogoIndex);
                    if (newicon.m_isMenuLogo)
                    {
                        newicon.m_isButton = bfalse;
                        newicon.m_index = menuLogoIndex;
                    }
                    else
                    {
                        newicon.m_isMenuLogo = bfalse;
                    }

                    if (newicon.m_isSkipIcon || newicon.m_isMenuLogo || UI_TEXTMANAGER->getIconInfo(iconName8, newicon.m_isButton, newicon.m_index))
                    {
                        // compute space char from size of icon
                        f32 spaceW = getTextWidth(" ", 1);
                        i32 nb = ((i32)(iconSize/spaceW) - 1) + ((i32)(iconXOffset/spaceW));
                        nb = Max(0, nb);

                        // Check why this is happening (and remove the clamp when solved)
                        ITF_WARNING_CATEGORY(Engine,NULL,nb <= MAX_SPACES_BY_ICON,"Trying to insert %d spaces to display an icon, please check this",nb);
                        nb = Clamp(nb,0,MAX_SPACES_BY_ICON);

                        word ="";
                        for (i32 i=0; i< nb; i++)
                            word += " ";

                        f32 globalspacesize = (nb*spaceW) + spaceW*2;
                        wordWidth = globalspacesize - spaceW;

                        f32 shiftx = (globalspacesize - iconSize)*0.5f;
                        newicon.m_pos = Vec2d(curPos + shiftx - spaceW , 0.f);
                        addIcon = btrue;
                        wordCount++;
                    }
                    wordWidth += getTextWidth(charsAfterBraket.cStr(), charsAfterBraket.getLen());
                    word += charsAfterBraket;
                }
                else if ( _actorInfo && _actorInfo->size() > 0 )
                {
                    i32 indexActorBegin, indexActorEnd;

                    word.strstr(actorTag.cStr(), bfalse, &indexActorBegin);
                    word.strstr(FontPrivate::END_TAG_DELIMITER, bfalse, &indexActorEnd);

                    String charsAfterBraket(word.substr(indexIconEnd+1, word.getLen()- 1 - indexIconEnd));

                    if (indexActorBegin != -1 && indexActorEnd != -1 && indexActorEnd > indexActorBegin) // Tag found
                    {
                        const String actorName(word.substr(indexActorBegin + actorTagBeginSize, indexActorEnd - indexActorBegin - actorTagBeginSize));
                        const String8 actorName8(actorName.cStr());
                        u32 numActors = _actorInfo->size();

                        for ( u32 i = actorCounter; i < numActors; i++ )
                        {
                            const TextActorInfo& textActorInfo = (*_actorInfo)[i];

                            if ( textActorInfo.m_path == actorName8 )
                            {
                                actorWidth = textActorInfo.m_width;
                                f32 spaceW = getTextWidth(" ", 1);
                                i32 nb = (u32)(actorWidth/spaceW) - 1;
                                nb = Max(0, nb);

                                // Check why this is happening (and remove the clamp when solved)
                                ITF_WARNING_CATEGORY(Engine,NULL,nb <= MAX_SPACES_BY_ICON,"Trying to insert %d spaces to display an icon, please check this",nb);
                                nb = Clamp(nb,0,MAX_SPACES_BY_ICON);

                                word ="";
                                for (i32 i=0; i< nb; i++)
                                    word += " ";

                                f32 globalspacesize = (nb*spaceW) + spaceW*2;
                                wordWidth = globalspacesize - spaceW;

                                f32 shiftx = (globalspacesize - actorWidth)*0.5f;
                                newActor.m_pos = Vec2d(curPos + shiftx - spaceW + (actorWidth*0.5f), 0.f);
                                addActor = btrue;
                                actorCounter++;
                                wordCount++;
                                break;
                            }
                        }
                    }
                    wordWidth += getTextWidth(charsAfterBraket.cStr(), charsAfterBraket.getLen());
                    word += charsAfterBraket;
                }
            }
            else
            {
                // Determine the width of the word
                if( wordE > wordS )
                {
                    wordCount++;
                    wordWidth = getTextWidthMS(word.cStr(), word.getLen(), _charInfoTab, wordS);
                }
                else
                    wordWidth = 0;
            }

            const char *s = " ";
            f32 spaceWidth;

            if (wordE>0)
            {
                // SetScale
                f32 oldScale = scale;
                scale = (_charInfoTab)[wordE-1].m_charSize / f32(fontHeight) * UI_TEXTMANAGER->getTextBoxScale();
                scale *= globalScale;
                spaceWidth = getTextWidth(s, 1);
                scale = oldScale;
            }
            else
                spaceWidth = getTextWidth(s, 1);

            // Does the word fit on the line? The first word is always accepted.
            if( wordCount == 1 || currWidth + (wordCount > 1 ? spaceWidth : 0) + wordWidth <= width )
            {
                if ( addIcon )
                {
                    tagicon.push_back(newicon);
                    addIcon = bfalse;
                    // remove the last space because it s for the line return test..
                    wordWidth -= spaceWidth;
                }

                if ( addActor )
                {
                    tagActor.push_back(newActor);
                    addActor = bfalse;
                    // remove the last space because it s for the line return test..
                    wordWidth -= spaceWidth;
                }

                linetext += word;
                curPos += wordWidth;

                // Increase the line extent to the end of the word
                lineE = wordE;
                currWidth += (wordCount > 1 ? spaceWidth : 0) + wordWidth;

                 // Did we reach the end of the line?
                if( wordE == count || getTextChar(text,wordE) == '\n' )
                {
                    softBreak = false;

                    // Skip the newline character
                    if( wordE < count )
                    {
                        // Advance the cursor to the next character
                        getNextChar(text,wordE);
                    }

                    break;
                }

                // Skip the trailing space
                i32 ch = getTextChar(text,wordE);
                if( wordE < count && isSpace(ch))
                {
                    // Advance the cursor to the next character
                    getNextChar(text, wordE);
                    linetext += (u16)ch;
                    curPos += spaceWidth;
                }

                // Move to next word
                wordS = wordE;
            }
            else
            {
                softBreak = true;

                // Skip the trailing space
                i32 ch = getTextChar(text,wordE);
                if( wordE < count && isSpace(ch))
                {
                    // Advance the cursor to the next character
                    getNextChar(text,wordE);
                    word += (u16)ch;
                    wordWidth += spaceWidth;
                }

                break;
            }
        }

        // Determine the scale and the position of the next line
        f32 highestSize = 0.f;
        if (lineE - lineS)
        {
            highestSize = getHighestSize(_charInfoTab, lineS, lineE - lineS);
            //highestSize = getHighestSize(_charInfoTab, lineS, linetext.getLen());
            setHeight(highestSize);
            scale = scale * UI_TEXTMANAGER->getTextBoxScale();
            scale *= globalScale;
        }

        if( _isRender2D )
        {
            y += scale * fontHeight * m_lineSpacingFactor;
        }
        else
        {
            y -= scale * fontHeight * m_lineSpacingFactor;
        }

        m_totalTextHeight += scale * fontHeight * m_lineSpacingFactor;

        // Can I wrote the next line?
//         if (_cut && height != -1.f && m_totalTextHeight >= height)
//         {
//             m_textAdjustement = btrue;
//             break;
//         }

        // Write the line
        f32 cx = x;
        f32 cy = y;
        if ( modeY )
        {
            if( _isRender2D )
                cy = y + (height - (m_totalTextHeightComputed + (scale * fontHeight * m_lineSpacingFactor * 0.5f)))*0.5f;
            else
                cy = y - (height - (m_totalTextHeightComputed + (scale * fontHeight * m_lineSpacingFactor * 0.5f)))*0.5f;
        }

        if( mode == FONT_ALIGN_JUSTIFY )
        {
            f32 spacing = 0;
            if( softBreak )
            {
                if( wordCount > 2 )
                    spacing = (width - currWidth) / (wordCount-2);
                else
                    spacing = (width - currWidth);
            }

            if (_write )
            {
                internalWrite(color ,x, cy, z, _isRender2D, linetext.cStr(), linetext.getLen(), mode, spacing, &_charInfoTab, 0);
            }
        }
        else
        {
            if( mode == FONT_ALIGN_RIGHT )
                cx = x + width - currWidth;
            else if( mode == FONT_ALIGN_CENTER )
                cx = x + 0.5f*(width - currWidth);

            if (_write )
            {
                internalWrite(color, cx, cy, z, _isRender2D, linetext.cStr(), linetext.getLen(), mode, 0, &_charInfoTab, 0);
            }
        }

        //iconpos
        if ( tagicon.size() && _write)
        {
            Texture* buttonTexture = UI_TEXTMANAGER->getButtonTexture();
            Texture* gpeTexture = UI_TEXTMANAGER->getGpeTexture();
            Texture* skipIconsTexture = UI_TEXTMANAGER->getSkipIconsTexture();
            Texture* menuLogosTexture = UI_TEXTMANAGER->getMenuLogosTexture();
            UVdata uvData;

            for (u32 i = 0; i< tagicon.size(); i++)
            {
                f32 xp = cx + tagicon[i].m_pos.m_x;
                f32 yp = cy - iconSize + iconYOffset;

                Texture* texture;
                if (tagicon[i].m_isSkipIcon)
                    texture = skipIconsTexture;
                else if (tagicon[i].m_isMenuLogo)
                    texture = menuLogosTexture;
                else if (tagicon[i].m_isButton)
                    texture = buttonTexture;
                else
                    texture = gpeTexture;

                if (texture && texture->getUVAtlas())
                    uvData = texture->getUVAtlas()->getUVDatabyIndex(tagicon[i].m_index);

                f32 iconWidth = iconSize;
                f32 iconHeight = iconSize;
                f32 xOffset = 0.f;
                f32 yOffset = 0.f;

                if (tagicon[i].m_isMenuLogo && texture)
                {
                    const f32 logoScale = UI_TEXTMANAGER->getMenuLogoScale();
                    const f32 aspectRatio = (f32)texture->getSizeX() / (f32)texture->getSizeY();
                    iconWidth = iconSize * aspectRatio * logoScale;
                    iconHeight = iconSize * logoScale;

                    xOffset = (iconSize - iconWidth) * 0.5f;
                    yOffset = (iconSize - iconHeight) * 0.5f;
                }

                if (texture)
                    GFX_ADAPTER->drawQuad2D(xp + xOffset, yp + yOffset, iconWidth, iconHeight, 0.f, 0xffffffff, texture, &uvData.getUV0(), &uvData.getUV1());
            }
            prepareEffect();
        }

        if ( tagActor.size() && _write)
        {
            for (u32 i = 0; i< tagActor.size(); i++)
            {
                Vec2d pos(cx + tagActor[i].m_pos.m_x, cy);

                if ( m_boxTransform)
                {
                    pos.m_x = m_boxPos.m_x + (( pos.m_x - m_boxPos.m_x) * m_boxScale.m_x);
                    pos.m_y = m_boxPos.m_y + (( pos.m_y - m_boxPos.m_y) * m_boxScale.m_y);
                }

                (*_actorInfo)[actorPosCounter].m_pos.m_x = pos.m_x;
                (*_actorInfo)[actorPosCounter].m_pos.m_y = pos.m_y;

                actorPosCounter++;
            }
        }

        if( softBreak )
        {
            // We've already counted the first word on the next line
            currWidth = wordWidth;
            wordCount = 1;
            curPos = wordWidth;
            keepContentToNextLine = btrue;
            linetext = word;

            f32 spacew = getTextWidth(" ", 1);

            if (addIcon)
            {
                // skip the TAG:
                linetext+=" ";
                curPos += spacew;

                // compute Icon position a the beginning line.
                f32 globalspacesize = (linetext.getLen()*spacew);
                f32 shiftx = (globalspacesize - iconSize)*0.5f;
                newicon.m_pos.m_x = shiftx;
            }

            if ( addActor )
            {
                // skip the TAG:
                linetext+=" ";
                curPos += spacew;

                // compute Icon position a the beginning line.
                f32 globalspacesize = (linetext.getLen()*spacew);
                f32 shiftx = (globalspacesize - actorWidth)*0.5f;
                newActor.m_pos.m_x = shiftx - spacew + (actorWidth*0.5f);
            }

            // Skip the trailing space
            if( lineE < count && isSpace(getTextChar(text,lineE)))
            {
                // Advance the cursor to the next character
                getNextChar(text,lineE);
            }
        }
        else
        {
            // Skip the line break
            if( lineE < count && getTextChar(text,lineE) == '\n' )
                // Advance the cursor to the next character
                getNextChar(text,lineE);

            currWidth = 0;
            wordCount = 0;
            word = "";
            linetext.clear();
        }

        // Move to next line
        lineS = lineE;
        wordS = wordE;

        m_yNextLine = y;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void Font::writeBoxWithStyle(f32 x, f32 y, f32 z, bbool _isRender2D, const Vec3d &_boxPos, const Vec3d &_boxPivot, const Vec3d &_boxScale, f32 _boxRotation, bbool _useTransBox, f32 width, f32 height, const Vec2d& _shadowOffset, u32 _shadowColor,  i32 count, u32 mode, u32 modeY, const ITF_VECTOR<TextInfo> &_textTab, bbool isUseScale, u32 _textTabStart, u32 _textTabStop, bbool _write )
{
    u32 textTabSize = _textTab.size();
    if (textTabSize > 0)
    {
        ITF_ASSERT(_textTabStart < textTabSize);

        m_is2d = _isRender2D;

        if (_textTabStop != U32_INVALID)
        {
            ITF_ASSERT(_textTabStop<=_textTab.size());
            textTabSize = _textTabStop;
        }

        getTextWithStyleTab(_textTab, _textTabStart, textTabSize);
        u32 coloredTextSize = m_textWithStyleTabNb;
        m_yNextLine = y;

        //simulation for compute height if Y center text:
        if (modeY)
        {
            m_totalTextHeight = 0.f;
            m_totalTextHeightComputed = 0.f;
            for (u32 cpt = 0; cpt < coloredTextSize; cpt++)
            {
                const String &blockText = m_textWithStyleTab[cpt].m_text;
                const SafeArray<CharInfo> &charInfoTab = m_textWithStyleTab[cpt].m_charInfoTab;
                TextActorInfoList* actorInfoList = NULL;

                if ( cpt < _textTab.size() )
                {
                    actorInfoList = const_cast<TextActorInfoList*>(&_textTab[cpt].m_actors);
                }

                writeBox(0, x, m_yNextLine, z, _isRender2D, _boxPos, _boxPivot, _boxScale, _boxRotation, _useTransBox, width, charInfoTab, blockText, getTextLength(blockText), mode, modeY, height, bfalse, btrue, actorInfoList);
            }
            m_totalTextHeightComputed = m_totalTextHeight;
        }

        //if (isUseScale)
        //{
        //    // Reduce the height of the font while it's necessary
        //    m_textAdjustement = btrue;
        //    while (m_textAdjustement)
        //    {
        //        m_totalTextHeight = 0.f;
        //        for (u32 cpt = 0; cpt < coloredTextSize; cpt++)
        //        {
        //            const char * blockText = m_textWithStyleTab[cpt].m_text.cStr();
        //            SafeArray<CharInfo> charInfoTab = m_textWithStyleTab[cpt].m_charInfoTab;
        //            writeBox(0,x,m_yNextLine,z, _isRender2D, _boxPos, _boxPivot, _boxScale, _boxRotation, _useTransBox, width, blockText,getTextLength(blockText),mode,height,bfalse,btrue,&charInfoTab);
        //        }
        //        if (m_textAdjustement)
        //        {
        //            globalScale -= 0.05f;
        //            scale *= globalScale;
        //        }
        //    }
        //}
        //else
        globalScale = 1.f;

        m_totalTextHeight = 0.f;
        m_yNextLine = y;

        m_shadowOffset = _shadowOffset;
        m_shadowcolor = Color(m_shadowcolor);

        for (u32 cpt = 0; cpt < coloredTextSize; cpt++)
        {
            const String& blockText = m_textWithStyleTab[cpt].m_text;
            const SafeArray<CharInfo>& charInfoTab = m_textWithStyleTab[cpt].m_charInfoTab;

            TextActorInfoList* actorInfoList = NULL;

            if ( cpt < _textTab.size() )
            {
                actorInfoList = const_cast<TextActorInfoList*>(&_textTab[cpt].m_actors);
            }

            writeBox(0, x, m_yNextLine, z, _isRender2D, _boxPos, _boxPivot, _boxScale, _boxRotation, _useTransBox, width, charInfoTab, blockText, getTextLength(blockText), mode, modeY, height, _write, btrue, actorInfoList);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void Font::getTextWithStyleTab (const ITF_VECTOR<TextInfo> &_textTab, u32 _textTabStart, u32 _textTabStop)
{
	m_textWithStyleTabNb = 0;
	u32 textTabSize = _textTabStop;
    for (u32 cpt = _textTabStart; cpt < textTabSize; cpt++)
    {
		m_textWithStyleTabNb++;
		if (m_textWithStyleTabNb > m_textWithStyleTab.size())
			m_textWithStyleTab.resize(m_textWithStyleTabNb);
        TextWithStyle &textWithStyle = m_textWithStyleTab[m_textWithStyleTabNb-1];
        textWithStyle.m_text = _textTab[cpt].m_text;
		textWithStyle.m_charInfoTab.clear();

        // Determine the number of characters in the block and set its color and its size
        i32 textLen = textWithStyle.m_text.getLen();
        i32 nbChar = 0;
        while (nbChar < textLen)
        {
            // Advance the cursor to the next character
            getNextChar(textWithStyle.m_text,nbChar);
            CharInfo charInfo;
            charInfo.m_charColor = _textTab[cpt].m_color;
            charInfo.m_charSize = _textTab[cpt].m_size;
            if (m_is2d && m_autoScale)
                charInfo.m_charSize *= getScreenRatio();
            textWithStyle.m_charInfoTab.push_back(charInfo);
        }

        // The next element in textTab is the same block
        while ((cpt + 1) < textTabSize && _textTab[cpt].m_friendly == _textTab[cpt+1].m_friendly)
        {
            const String &textC = _textTab[cpt+1].m_text;
            textWithStyle.m_text += textC;

            // Determine the number of characters in the block and set its color and its size
            textLen = textC.getLen();
            nbChar = 0;
            while (nbChar < textLen)
            {
                // Advance the cursor to the next character
                getNextChar(textC,nbChar);

                CharInfo charInfo;
                charInfo.m_charColor = _textTab[cpt+1].m_color;
                charInfo.m_charSize = _textTab[cpt+1].m_size;
                if (m_is2d && m_autoScale)
                    charInfo.m_charSize *= getScreenRatio();
                textWithStyle.m_charInfoTab.push_back(charInfo);
            }
            cpt++;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
f32 Font::getHighestSize(const SafeArray<CharInfo> &_charInfoTab, u32 _startTabColor, u32 _count)
{
    ITF_ASSERT(_startTabColor + _count <= (_charInfoTab).size());
    f32 highestSize = 0.f;
    for (u32 i = 0; i < _count; i++)
    {
        f32 currentSize = (_charInfoTab)[_startTabColor + i].m_charSize;
        if (highestSize < currentSize)
            highestSize = currentSize;
    }
    return highestSize;
}

///////////////////////////////////////////////////////////////////////////////////////////
/*
#ifndef ITF_FINAL
    void Font::drawDebug()
    {
        if (pages.size())
        {
			const ResourceID& resID = pages[0];
		    Texture* pTexture = (Texture*) resID.getResource();

            if (pTexture->isPhysicallyLoaded())
            {
                GFX_ADAPTER->init2DGuiRender();
                //GFX_ADAPTER->SetTexture(0,pages[0]);
				//GFX_ADAPTER->drawQuad2D(0,0, pTexture->m_sizeX, pTexture->m_sizeY, 0.f, COLOR_WHITE,pTexture);


			//	prepareEffect();
			//	                GFX_ADAPTER->SetTexture(0,pTexture);
				//write(0xffff0000,100, 40, 0, "0", 0, FONT_ALIGN_LEFT);

                GFX_ADAPTER->drawText(this,0xffff0000,100, 40, 0, "0", 0, FONT_ALIGN_LEFT);
				//Write(0xff00ff00,200, 1050+500+GetHeight(), 0, "r0123456789abcdefghijklmnopqrstuvwyxz", 0, FONT_ALIGN_LEFT);
                //Write(0xff00ffff,100,-200, 0, "-200", 0, FONT_ALIGN_LEFT);
                GFX_ADAPTER->drawText(this,0xffffff00,100, 240, 0, "200", 0, FONT_ALIGN_LEFT);
                GFX_ADAPTER->restoreDefaultShader();
            }
        }
    }
#endif // ITF_FINAL
*/
void Font::prepareEffect()
{
    GFX_ADAPTER->setShaderForFont(hasOutline);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Font::cleanFontCache()
{
    for (u32 index = 0;index<m_vFontCache.size();++index)
    {
        delete (m_vFontCache[index]);
    }

    m_vFontCache.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#if defined(ITF_SUPPORT_EDITOR)
void Font::dependenciesFile(const String& _filename,DepCollection& _collection)
{
    Font* pFont = new Font();
    //TODO switch to u16..

    StringConverter charName(_filename);

    pFont->init(charName.getChar());

    PathString_t resFilename;

    for ( ITF_VECTOR<ResourceID>::const_iterator iter = pFont->pages.begin();iter!=pFont->pages.end();++iter)
    {
        const ResourceID& rid = (*iter);
        rid.getPath()->getString(resFilename);
        _collection.add(resFilename);
    }
    Path fullpath(_filename);

    String path(fullpath.getDirectory());

    String baseName;
    fullpath.getBasenameWithoutExtension(baseName);

    String langaugeSuffixes[] ={"_jap","_tch","_sch"};
    for (String& langaugeSuffix : langaugeSuffixes)
    {
        if (baseName.strstr(langaugeSuffix.cStr()) == NULL)
        {
            String ext(fullpath.getExtension());

            String fontFileLocal16 = path + baseName + langaugeSuffix +"." + ext;
            _collection.add(fontFileLocal16);
        }
    }

    SF_DEL(pFont);
}
#endif // defined(ITF_SUPPORT_EDITOR)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


FontCache::FontCache(ITF_Mesh *_pMesh[FONT_PAGE_MAX],i32 _count,u32 _crcColor,u32 _color,const Vec3d& _position, u32 _mode, const String& _text,u32 _countVertices[FONT_PAGE_MAX]):
                     /*m_pMesh(_pMesh),*/m_crcColor(_crcColor),m_count( _count),m_color(_color),m_position(_position),m_text(_text),/*m_countVertices(_countVertices)*/
                     m_timeLife(FONTCACHE_TIMELIFE),m_mode(_mode)
{
    for (u32 i = 0; i < FONT_PAGE_MAX; i++)
    {
        m_pMesh[i] = _pMesh[i];
        m_countVertices[i] = _countVertices[i];
    }


}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool FontCache::match(bbool _2d, i32 _count,u32 _crcColor,u32 _color,const Vec3d& _position,const String& _text, u32 _mode){

    if (_2d)
    {
    if (m_position.IsEqual(_position, 0.09f) &&
        m_text     == _text     &&
        m_color    == _color    &&
        m_count    == _count    &&
        m_crcColor == _crcColor)

        {
            return btrue;
        }
    }
    else
    {
    if (
        m_text     == _text     &&
        m_mode     == _mode     &&
        m_color    == _color    &&
        m_count    == _count    &&
        m_crcColor == _crcColor)

        {
            return btrue;
        }
    }

    return bfalse;
}

FontCache::~FontCache()
{
    for ( u32 iPage = 0; iPage < FONT_PAGE_MAX; iPage++ )
    {
        if (m_pMesh[iPage])
        {
            GFX_ADAPTER->removeVertexBuffer(m_pMesh[iPage]->m_vtxBuffer);
            SF_DEL(m_pMesh[iPage]);
        }
    }

}


}
