#include "precompiled_engine.h"

#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_FONTLOADER_H_
#include "engine/display/fontLoader.h"
#endif //_ITF_FONTLOADER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{

    //=============================================================================
    // CFontLoader
    //
    // This is the base class for all loader classes. This is the only class
    // that has access to and knows how to set the CFont members.
    //=============================================================================

    FontLoader::FontLoader(File *f, Font *font, const char *fontFile)
    {
        this->m_File= f;
        this->font = font;
        this->fontFile = fontFile;

        outlineThickness = 0;
    }

    void FontLoader::LoadPage(i32 ID, const char *pageFile, const char *_relativeFontFile)
    {
        String relativeDir = _relativeFontFile;
        i32 reduceIndex = 0;
        if(relativeDir.strstr('/', btrue, &reduceIndex))
            relativeDir.truncate(reduceIndex+1);

		const ResourceID& idResource = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_Texture, relativeDir+pageFile);
		RESOURCE_MANAGER->queryAddUserToResourcePhysicalData(idResource);
        font->pages[ID] = idResource;
   }

    void FontLoader::SetFontInfo(i32 outlineThickness)
    {
        this->outlineThickness = outlineThickness;
    }

    void FontLoader::SetCommonInfo(i32 fontHeight, i32 base, i32 scaleW, i32 scaleH, i32 pages, bbool isPacked)
    {
       
        font->fontHeight = fontHeight;
        font->base = base;
        font->scaleW = scaleW;
        font->scaleH = scaleH;
        font->pages.resize(pages);

        font->outlineThickness = (f32)outlineThickness;

        //for( i32 n = 0; n < pages; n++ )
        //    font->pages[n] = 0;

        if( isPacked && outlineThickness )
            font->hasOutline = true;
    }

    void FontLoader::AddChar(i32 ID, i32 x, i32 y, i32 w, i32 h, i32 xoffset, i32 yoffset, i32 xadvance, i32 page, i32 chnl)
    {
         
         
        // Convert to a 4 element vector
        // TODO: Does this depend on hardware? It probably does
        if     ( chnl == 1 ) chnl = 0x00010000;  // Blue channel
        else if( chnl == 2 ) chnl = 0x00000100;  // Green channel
        else if( chnl == 4 ) chnl = 0x00000001;  // Red channel
        else if( chnl == 8 ) chnl = 0x01000000;  // Alpha channel
        else chnl = 0;

        if( ID >= 0 )
        {
            SCharDescr *ch = newAlloc(mId_Resource,SCharDescr);
            ch->srcX = (short)x;
            ch->srcY = (short)y;
            ch->srcW = (short)w;
            ch->srcH = (short)h;
            ch->xOff = (short)xoffset;
            ch->yOff = (short)yoffset;
            ch->xAdv = (short)xadvance;
            ch->page = (short)page;
            ch->chnl = (u32)chnl;

            font->chars.insert(ITF_MAP<i32, SCharDescr*>::value_type(ID, ch));
        }

        if( ID == -1 )
        {
            font->defChar.srcX = (short)x;
            font->defChar.srcY = (short)y;
            font->defChar.srcW = (short)w;
            font->defChar.srcH = (short)h;
            font->defChar.xOff = (short)xoffset;
            font->defChar.yOff = (short)yoffset;
            font->defChar.xAdv = (short)xadvance;
            font->defChar.page = (short)page;
            font->defChar.chnl = chnl;
        }
    }

    void FontLoader::AddKerningPair(i32 first, i32 second, i32 amount)
    { 
    
        if( first >= 0 && first < 256 && font->chars[first] )
        {
            font->chars[first]->kerningPairs.push_back(second);
            font->chars[first]->kerningPairs.push_back(amount);
        }
    }

    //=============================================================================
    // CFontLoaderTextFormat
    //
    // This class implements the logic for loading a BMFont file in text format
    //=============================================================================

    FontLoaderTextFormat::FontLoaderTextFormat(File *f, Font *font, const char *fontFile) : FontLoader(f, font, fontFile)
    {
    }

    i32 FontLoaderTextFormat::Load()
    {
        
        const u32 buffer_size = static_cast<i32>(m_File->getLength());
        char *pBufferFile = newAlloc(mId_Temporary,char[buffer_size+1]);
        
        m_File->read(pBufferFile,buffer_size);
        pBufferFile[buffer_size] = 0;

        FILEMANAGER->closeFile(m_File);

        char* pRead = pBufferFile;

        String8 line;

        while (*pRead)
        {
            line = "";
			
           // line.reserve(256);

            while (*pRead)
            {
                char ch = *pRead;
                pRead++;
                if( ch != '\n' ) 
                    line += ch; 
                else
                    break;
            }


            int pos = SkipWhiteSpace(line, 0);

            // Read token
            int pos2 = FindEndOfToken(line, pos);
			
            String8 token = line.substr(pos, pos2-pos);

            // Interpret line
            if( token == "info" )
                InterpretInfo(line, pos2);
            else if( token == "common" )
                InterpretCommon(line, pos2);
            else if( token == "char" )
                InterpretChar(line, pos2);
            else if( token == "kerning" )
                InterpretKerning(line, pos2);
            else if( token == "page" )
                InterpretPage(line, pos2, fontFile);
           
        }
        // Success
        SF_DEL_ARRAY(pBufferFile);
        return 1;
    }

    i32 FontLoaderTextFormat::SkipWhiteSpace(String8 &str, i32 start)
    {
        u32 n = start;
		const u32 len = str.getLen(); 
        while( n < len)
        {
			char ch = str.cStr()[n];
            if( ch != ' ' && 
                ch != '\t' && 
                ch != '\r' && 
                ch != '\n' )
                break;

            ++n;
        }

        return n;
    }

    i32 FontLoaderTextFormat::FindEndOfToken(String8 &str, i32 start)
    {
        u32 n = start;
		const u32 len = str.getLen(); 
        if( n<len && str.cStr()[n] == '"' )
        {
            n++;
            while( n < len )
            {
                char ch = str.cStr()[n];
                if( ch == '"' )
                {
                    // Include the last quote char in the token
                    ++n;
                    break;
                }
                ++n;
            }
        }
        else
        {
            while( n < len )
            {
                char ch = str.cStr()[n];
                if( ch == ' ' ||
                    ch == '\t' ||
                    ch == '\r' ||
                    ch == '\n' ||
                    ch == '=' )
                    break;

                ++n;
            }
        }

        return n;
    }

    void FontLoaderTextFormat::InterpretKerning(String8 &str, int start)
    {
        // Read the attributes
        i32 first = 0;
        i32 second = 0;
        i32 amount = 0;
		const u32 len = str.getLen(); 
        i32 pos, pos2 = start;
        for( ;; )
        {
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);

			String8 token = str.substr(pos, pos2-pos);

            pos = SkipWhiteSpace(str, pos2);
            if( pos == (i32)len || str.cStr()[pos] != '=' ) break;

            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);

            String8 value =str.substr(pos, pos2-pos);

            if( token == "first" )
                first = strtol(value.cStr(), 0, 10);
            else if( token == "second" )
                second = strtol(value.cStr(), 0, 10);
            else if( token == "amount" )
                amount = strtol(value.cStr(), 0, 10);

            if( pos == (i32)len ) break;
        }

        // Store the attributes
        AddKerningPair(first, second, amount);
    }

    void FontLoaderTextFormat::InterpretChar(String8 &str, i32 start)
    {
        // Read all attributes
        i32 ID = 0;
        i32 x = 0;
        i32 y = 0;
        i32 width = 0;
        i32 height = 0;
        i32 xoffset = 0;
        i32 yoffset = 0;
        i32 xadvance = 0;
        i32 page = 0;
        i32 chnl = 0;

        i32 pos, pos2 = start;
		const u32 len = str.getLen(); 
        for( ;; )
        {
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);

            String8 token = str.substr(pos, pos2-pos);

            pos = SkipWhiteSpace(str, pos2);
			if( pos == (i32)len || str.cStr()[pos] != '=' ) break;

            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);

            String8 value = str.substr(pos, pos2-pos);

            if( token == "id" )
				ID = strtol(value.cStr(), 0, 10);
            else if( token == "x" )
                x = strtol(value.cStr(), 0, 10);
            else if( token == "y" )
                y = strtol(value.cStr(), 0, 10);
            else if( token == "width" )
                width = strtol(value.cStr(), 0, 10);
            else if( token == "height" )
                height = strtol(value.cStr(), 0, 10);
            else if( token == "xoffset" )
                xoffset = strtol(value.cStr(), 0, 10);
            else if( token == "yoffset" )
                yoffset = strtol(value.cStr(), 0, 10);
            else if( token == "xadvance" )
                xadvance = strtol(value.cStr(), 0, 10);
            else if( token == "page" )
                page = strtol(value.cStr(), 0, 10);
            else if( token == "chnl" )
                chnl = strtol(value.cStr(), 0, 10);

			if( pos == (i32)len ) break;
        }

        // Store the attributes
        AddChar(ID, x, y, width, height, xoffset, yoffset, xadvance, page, chnl);
    }

	void FontLoaderTextFormat::InterpretCommon(String8 &str, int start)
    {
        int fontHeight = 0;
        int base = 0;
        int scaleW = 0;
        int scaleH = 0;
        int pages = 0;
        int packed = 0;

        // Read all attributes
        int pos, pos2 = start;
		const u32 len = str.getLen();

        for( ;; )
        {
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);

            String8 token = str.substr(pos, pos2-pos);

            pos = SkipWhiteSpace(str, pos2);
            if( pos == (i32)len || str.cStr()[pos] != '=' ) break;

            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);

            String8 value =  str.substr(pos, pos2-pos);

            if( token == "lineHeight" )
				fontHeight = (short)strtol(value.cStr(), 0, 10);
            else if( token == "base" )
                base = (short)strtol(value.cStr(), 0, 10);
            else if( token == "scaleW" )
                scaleW = (short)strtol(value.cStr(), 0, 10);
            else if( token == "scaleH" )
                scaleH = (short)strtol(value.cStr(), 0, 10);
            else if( token == "pages" )
                pages = strtol(value.cStr(), 0, 10);
            else if( token == "packed" )
                packed = strtol(value.cStr(), 0, 10);

            if( pos == (i32)len ) break;
        }

        SetCommonInfo(fontHeight, base, scaleW, scaleH, pages, packed ? true : false);
    }

    void FontLoaderTextFormat::InterpretInfo(String8 &str, int start)
    {
        int outlineThickness = 0;

        // Read all attributes
        int pos, pos2 = start;
		const u32 len = str.getLen(); 
        for( ;; )
        {
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);

			String8 token =  str.substr(pos, pos2-pos);

            pos = SkipWhiteSpace(str, pos2);
            if( pos == (i32)len || str.cStr()[pos] != '=' ) break;

            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);

			String8 value =  str.substr(pos, pos2-pos);

            if( token == "outline" )
                outlineThickness = (short)strtol(value.cStr(), 0, 10);

            if( pos == (i32)len ) break;
        }

        SetFontInfo(outlineThickness);
    }

    void FontLoaderTextFormat::InterpretPage(String8 &str, int start, const char *fontFile)
    {
        int ID = -1;
        String8 file;

        // Read all attributes
        int pos, pos2 = start;
		const u32 len = str.getLen(); 
        for( ;; )
        {
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);

            String8 token =  str.substr(pos, pos2-pos);

            pos = SkipWhiteSpace(str, pos2);
            if( pos == (i32)len || str.cStr()[pos] != '=' ) break;

            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);

            String8 value = str.substr(pos, pos2-pos);

            if( token == "id" )
                ID = strtol(value.cStr(), 0, 10);
            else if( token == "file" )
				file = value.substr(1, value.getLen()-2);

            if(ID != -1 && !file.isEmpty())
            {
                LoadPage(ID, file.cStr(), fontFile);
                ID = -1;
                file = String8::emptyString;
            }


            if( pos == (i32)len ) break;
        }

    }

    //=============================================================================
    // FontLoaderBinaryFormat
    //
    // This class implements the logic for loading a BMFont file in binary format
    //=============================================================================

    FontLoaderBinaryFormat::FontLoaderBinaryFormat(File *_file, Font *font, const char *fontFile) : FontLoader(_file, font, fontFile)
    {
    }

    int FontLoaderBinaryFormat::Load()
    {
        // Read and validate the tag. It should be 66, 77, 70, 2, 
        // or 'BMF' and 2 where the number is the file version.
        /*
        char magicString[4];
        fread(magicString, 4, 1, f);
        if( strncmp(magicString, "BMF\003", 4) != 0 )
        {
            LOG(("Unrecognized format for '%s'", fontFile));
            fclose(f);
            return -1;
        }

        // Read each block
        char blockType;
        int blockSize;
        while( fread(&blockType, 1, 1, f) )
        {
            // Read the blockSize
            fread(&blockSize, 4, 1, f);

            switch( blockType )
            {
            case 1: // info
                ReadInfoBlock(blockSize);
                break;
            case 2: // common
                ReadCommonBlock(blockSize);
                break;
            case 3: // pages
                ReadPagesBlock(blockSize);
                break;
            case 4: // chars
                ReadCharsBlock(blockSize);
                break;
            case 5: // kerning pairs
                ReadKerningPairsBlock(blockSize);
                break;
            default:
                LOG(("Unexpected block type (%d)", blockType));
                fclose(f);
                return -1;
            }
        }

        fclose(f);
*/
        // Success
        return 0;
    }

    void FontLoaderBinaryFormat::ReadInfoBlock(int size)
    {
#pragma pack(push)
#pragma pack(1)
        struct infoBlock
        {
            u16 fontSize;
            u8 reserved:4;
            u8 bold    :1;
            u8 italic  :1;
            u8 unicode :1;
            u8 smooth  :1;
            u8 charSet;
            u8 stretchH;
            u8 aa;
            u8 paddingUp;
            u8 paddingRight;
            u8 paddingDown;
            u8 paddingLeft;
            u8 spacingHoriz;
            u8 spacingVert;
            u8 outline;         // Added with version 2
            char fontName[1];
        };
#pragma pack(pop)

        ITF_ASSERT(0);
        /*
        char *buffer = new char[size];
        fread(buffer, size, 1, f);

        // We're only interested in the outline thickness
        infoBlock *blk = (infoBlock*)buffer;
        SetFontInfo(blk->outline);

        delete[] buffer;*/
    }

    void FontLoaderBinaryFormat::ReadCommonBlock(int size)
    {
#pragma pack(push)
#pragma pack(1)
        struct commonBlock
        {
            u16 lineHeight;
            u16 base;
            u16 scaleW;
            u16 scaleH;
            u16 pages;
            u8 packed  :1;
            u8 reserved:7;
            u8 alphaChnl;
            u8 redChnl;
            u8 greenChnl;
            u8 blueChnl;
        }; 
#pragma pack(pop)
        ITF_ASSERT(0);
        /*
        char *buffer = new char[size];
        fread(buffer, size, 1, f);

        commonBlock *blk = (commonBlock*)buffer;

        SetCommonInfo(blk->lineHeight, blk->base, blk->scaleW, blk->scaleH, blk->pages, blk->packed ? true : false);

        delete[] buffer;*/
    }

    void FontLoaderBinaryFormat::ReadPagesBlock(int size)
    { ITF_ASSERT(0);
    /*
#pragma pack(push)
#pragma pack(1)
        struct pagesBlock
        {
            char pageNames[1];
        };
#pragma pack(pop)

        char *buffer = new char[size];
        fread(buffer, size, 1, f);

        pagesBlock *blk = (pagesBlock*)buffer;

        for( int ID = 0, pos = 0; pos < size; ID++ )
        {
            LoadPage(ID, &blk->pageNames[pos], fontFile);
            pos += 1 + (int)strlen(&blk->pageNames[pos]);
        }

        delete[] buffer;*/
    }

    void FontLoaderBinaryFormat::ReadCharsBlock(int size)
    { ITF_ASSERT(0);
    /*
#pragma pack(push)
#pragma pack(1)
        struct charsBlock
        {
            struct charInfo
            {
                DWORD ID;
                WORD  x;
                WORD  y;
                WORD  width;
                WORD  height;
                short xoffset;
                short yoffset;
                short xadvance;
                BYTE  page;
                BYTE  chnl;
            } chars[1];
        };
#pragma pack(pop)

        char *buffer = new char[size];
        fread(buffer, size, 1, f);

        charsBlock *blk = (charsBlock*)buffer;

        for( int n = 0; int(n*sizeof(charsBlock::charInfo)) < size; n++ )
        {
            AddChar(blk->chars[n].ID,
                blk->chars[n].x,
                blk->chars[n].y,
                blk->chars[n].width,
                blk->chars[n].height,
                blk->chars[n].xoffset,
                blk->chars[n].yoffset,
                blk->chars[n].xadvance,
                blk->chars[n].page,
                blk->chars[n].chnl);
        }

        delete[] buffer;*/
    }

    void FontLoaderBinaryFormat::ReadKerningPairsBlock(int size)
    { ITF_ASSERT(0);
    /*
#pragma pack(push)
#pragma pack(1)
        struct kerningPairsBlock
        {
            struct kerningPair
            {
                DWORD first;
                DWORD second;
                short amount;
            } kerningPairs[1];
        };
#pragma pack(pop)

        char *buffer = new char[size];
        fread(buffer, size, 1, f);

        kerningPairsBlock *blk = (kerningPairsBlock*)buffer;

        for( int n = 0; int(n*sizeof(kerningPairsBlock::kerningPair)) < size; n++ )
        {
            AddKerningPair(blk->kerningPairs[n].first,
                blk->kerningPairs[n].second,
                blk->kerningPairs[n].amount);
        }

        delete[] buffer;*/
    }

}
