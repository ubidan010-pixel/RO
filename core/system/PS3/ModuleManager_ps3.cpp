#include "precompiled_core.h"

#ifndef _ITF_MODULEMANAGER_PS3_H_
#include "core/system/PS3/ModuleManager_ps3.h"
#endif //_ITF_MODULEMANAGER_PS3_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

namespace ITF
{
    //--------------------------------------------------------------------------
    ModuleManager* ModuleManager::m_instance = NULL;

    ModuleManager::~ModuleManager()
    {
#ifndef ITF_FINAL
        dumpModulesActivities();
#endif //ITF_FINAL

        unloadAllModules();
        m_instance = NULL;
    }
    //--------------------------------------------------------------------------
    i32 ModuleManager::loadModule(u16 _module)
    {
        Module tmpModule (_module);        
        ModulesList::iterator it = std::find(_modulesLoaded.begin(), _modulesLoaded.end(), tmpModule);
        if(it != _modulesLoaded.end())
        {
            // Found 
            it->counter ++;
#ifndef ITF_FINAL
            if(m_activeLOG)
            {
                LOG("Module %s (ID: %u) not loaded (already done), count: %u", getModuleName(_module).cStr(), _module, it->counter);
            }
#endif //ITF_FINAL
            return CELL_OK;
        }
        else
        {
            i32 res = cellSysmoduleLoadModule(_module);
            if(res == CELL_OK) _modulesLoaded.push_back(tmpModule);
#ifndef ITF_FINAL
            else
            {
                LOG("Module %s loading error: %x",  getModuleName(_module).cStr(), res);
            }
            if(m_activeLOG && res == CELL_OK)
            {
                LOG("Module %s (ID: %u) loaded.", getModuleName(_module).cStr(), _module);
            }
#endif //ITF_FINAL
            return res;
        }
    }
    //--------------------------------------------------------------------------
    i32 ModuleManager::unloadModule(u16 _module)
    {
        // We use "printf" because LOG is not available during the unloading step
        Module tmpModule (_module);
        ModulesList::iterator it = std::find(_modulesLoaded.begin(), _modulesLoaded.end(), tmpModule);
        if(it != _modulesLoaded.end())
        {
            // Found
            if(it->counter == 1)
            {
                int res = cellSysmoduleUnloadModule(_module);
                if(res == CELL_OK) _modulesLoaded.erase(it);
#ifndef ITF_FINAL
                else
                {
                    printf("Module %s unloading error: %x\n",  getModuleName(_module).cStr(), res);
                }
                if(m_activeLOG && res == CELL_OK)
                {
                    printf("Module %s (ID: %u) unloaded.\n", getModuleName(_module).cStr(), _module);
                }
#endif //ITF_FINAL

                return res;
            } 
            else
            {
                it->counter --;
#ifndef ITF_FINAL
                if(m_activeLOG)
                {
                    printf("Module %s (ID: %u) not unloaded, remain: %u\n", getModuleName(_module).cStr(), _module, it->counter);
                }
#endif //ITF_FINAL
            }
            return CELL_OK;
        } else
        {
            ITF_ASSERT_CRASH(0, "Module not found in the list");
            return -1;
        }
    }
#ifndef ITF_FINAL
    //--------------------------------------------------------------------------
    void ModuleManager::dumpModulesActivities()
    {        
        printf("---------------------------------------\n");
        printf("ModuleManager: dumpModulesActivities\n");
        printf("Modules not correctly unloaded found: %u\n\n", _modulesLoaded.size());
        ModulesList::const_iterator it = _modulesLoaded.begin();
        for(; it != _modulesLoaded.end(); ++it)
        {
            printf("Module %s, ID: %u, count: %u\n", getModuleName(it->module).cStr(), it->module, it->counter);
        }
        printf("---------------------------------------\n");
    }
    //--------------------------------------------------------------------------
    String8 ModuleManager::getModuleName( u16 _module ) const
    {
        switch(_module)
        {
        case CELL_SYSMODULE_NET:            return "CELL_SYSMODULE_NET";
        case CELL_SYSMODULE_HTTP:           return "CELL_SYSMODULE_HTTP";
        case CELL_SYSMODULE_HTTP_UTIL:      return "CELL_SYSMODULE_HTTP_UTIL";
        case CELL_SYSMODULE_SSL:            return "CELL_SYSMODULE_SSL";
        case CELL_SYSMODULE_HTTPS:          return "CELL_SYSMODULE_HTTPS";
        case CELL_SYSMODULE_VDEC:           return "CELL_SYSMODULE_VDEC";
        case CELL_SYSMODULE_ADEC:           return "CELL_SYSMODULE_ADEC";
        case CELL_SYSMODULE_DMUX:           return "CELL_SYSMODULE_DMUX";
        case CELL_SYSMODULE_VPOST:          return "CELL_SYSMODULE_VPOST";
        case CELL_SYSMODULE_RTC:            return "CELL_SYSMODULE_RTC";
        case CELL_SYSMODULE_SPURS:          return "CELL_SYSMODULE_SPURS";
        case CELL_SYSMODULE_OVIS:           return "CELL_SYSMODULE_OVIS";
        case CELL_SYSMODULE_SHEAP:          return "CELL_SYSMODULE_SHEAP";
        case CELL_SYSMODULE_SYNC:           return "CELL_SYSMODULE_SYNC";
        case CELL_SYSMODULE_FS:             return "CELL_SYSMODULE_FS";
        case CELL_SYSMODULE_JPGDEC:         return "CELL_SYSMODULE_JPGDEC";
        case CELL_SYSMODULE_GCM_SYS:        return "CELL_SYSMODULE_GCM_SYS";
        case CELL_SYSMODULE_AUDIO:          return "CELL_SYSMODULE_AUDIO";
        case CELL_SYSMODULE_PAMF:           return "CELL_SYSMODULE_PAMF";
        case CELL_SYSMODULE_ATRAC3PLUS:     return "CELL_SYSMODULE_ATRAC3PLUS";
        case CELL_SYSMODULE_NETCTL:         return "CELL_SYSMODULE_NETCTL";
        case CELL_SYSMODULE_SYSUTIL:        return "CELL_SYSMODULE_SYSUTIL";
        case CELL_SYSMODULE_SYSUTIL_NP:     return "CELL_SYSMODULE_SYSUTIL_NP";
        case CELL_SYSMODULE_IO:             return "CELL_SYSMODULE_IO";
        case CELL_SYSMODULE_PNGDEC:         return "CELL_SYSMODULE_PNGDEC";
        case CELL_SYSMODULE_FONT:           return "CELL_SYSMODULE_FONT";
        case CELL_SYSMODULE_FONTFT:         return "CELL_SYSMODULE_FONTFT";
        case CELL_SYSMODULE_FREETYPE:       return "CELL_SYSMODULE_FREETYPE";
        case CELL_SYSMODULE_USBD:           return "CELL_SYSMODULE_USBD";
        case CELL_SYSMODULE_SAIL:           return "CELL_SYSMODULE_SAIL";
        case CELL_SYSMODULE_L10N:           return "CELL_SYSMODULE_L10N";
        case CELL_SYSMODULE_RESC:           return "CELL_SYSMODULE_RESC";
        case CELL_SYSMODULE_DAISY:          return "CELL_SYSMODULE_DAISY";
        case CELL_SYSMODULE_KEY2CHAR:       return "CELL_SYSMODULE_KEY2CHAR";
        case CELL_SYSMODULE_MIC:            return "CELL_SYSMODULE_MIC";
        case CELL_SYSMODULE_CAMERA:         return "CELL_SYSMODULE_CAMERA";
        case CELL_SYSMODULE_VDEC_MPEG2:     return "CELL_SYSMODULE_VDEC_MPEG2";
        case CELL_SYSMODULE_VDEC_AVC:       return "CELL_SYSMODULE_VDEC_AVC";
        case CELL_SYSMODULE_ADEC_LPCM:      return "CELL_SYSMODULE_ADEC_LPCM";
        case CELL_SYSMODULE_ADEC_AC3:       return "CELL_SYSMODULE_ADEC_AC3";
        case CELL_SYSMODULE_ADEC_ATX:       return "CELL_SYSMODULE_ADEC_ATX";
        case CELL_SYSMODULE_ADEC_AT3:       return "CELL_SYSMODULE_ADEC_AT3";
        case CELL_SYSMODULE_DMUX_PAMF:      return "CELL_SYSMODULE_DMUX_PAMF";
        case CELL_SYSMODULE_LV2DBG:         return "CELL_SYSMODULE_LV2DBG";
        case CELL_SYSMODULE_USBPSPCM:       return "CELL_SYSMODULE_USBPSPCM";
        case CELL_SYSMODULE_AVCONF_EXT:     return "CELL_SYSMODULE_AVCONF_EXT";
        case CELL_SYSMODULE_SYSUTIL_USERINFO:       return "CELL_SYSMODULE_SYSUTIL_USERINFO";
        case CELL_SYSMODULE_SYSUTIL_SAVEDATA:       return "CELL_SYSMODULE_SYSUTIL_SAVEDATA";
        case CELL_SYSMODULE_SUBDISPLAY:     return "CELL_SYSMODULE_SUBDISPLAY";
        case CELL_SYSMODULE_SYSUTIL_REC:    return "CELL_SYSMODULE_SYSUTIL_REC";
        case CELL_SYSMODULE_VIDEO_EXPORT:   return "CELL_SYSMODULE_VIDEO_EXPORT";
        case CELL_SYSMODULE_SYSUTIL_GAME_EXEC:      return "CELL_SYSMODULE_SYSUTIL_GAME_EXEC";
        case CELL_SYSMODULE_SYSUTIL_NP2:    return "CELL_SYSMODULE_SYSUTIL_NP2";
        case CELL_SYSMODULE_SYSUTIL_AP:     return "CELL_SYSMODULE_SYSUTIL_AP";
        case CELL_SYSMODULE_SYSUTIL_NP_CLANS:       return "CELL_SYSMODULE_SYSUTIL_NP_CLANS";
        case CELL_SYSMODULE_SYSUTIL_OSK_EXT:        return "CELL_SYSMODULE_SYSUTIL_OSK_EXT";
        case CELL_SYSMODULE_VDEC_DIVX:      return "CELL_SYSMODULE_VDEC_DIVX";
        case CELL_SYSMODULE_JPGENC:         return "CELL_SYSMODULE_JPGENC";
        case CELL_SYSMODULE_SYSUTIL_GAME:   return "CELL_SYSMODULE_SYSUTIL_GAME";
        case CELL_SYSMODULE_BGDL:           return "CELL_SYSMODULE_BGDL";
        case CELL_SYSMODULE_FREETYPE_TT:    return "CELL_SYSMODULE_FREETYPE_TT";
        case CELL_SYSMODULE_SYSUTIL_VIDEO_UPLOAD:   return "CELL_SYSMODULE_SYSUTIL_VIDEO_UPLOAD";
        case CELL_SYSMODULE_SYSUTIL_SYSCONF_EXT:    return "CELL_SYSMODULE_SYSUTIL_SYSCONF_EXT";
        case CELL_SYSMODULE_FIBER:          return "CELL_SYSMODULE_FIBER";
        case CELL_SYSMODULE_SYSUTIL_NP_COMMERCE2:   return "CELL_SYSMODULE_SYSUTIL_NP_COMMERCE2";
        case CELL_SYSMODULE_SYSUTIL_NP_TUS: return "CELL_SYSMODULE_SYSUTIL_NP_TUS";
        case CELL_SYSMODULE_VOICE:          return "CELL_SYSMODULE_VOICE";
        case CELL_SYSMODULE_ADEC_CELP8:     return "CELL_SYSMODULE_ADEC_CELP8";
        case CELL_SYSMODULE_CELP8ENC:       return "CELL_SYSMODULE_CELP8ENC";
        case CELL_SYSMODULE_SYSUTIL_LICENSEAREA:    return "CELL_SYSMODULE_SYSUTIL_LICENSEAREA";
        case CELL_SYSMODULE_SYSUTIL_MUSIC2:         return "CELL_SYSMODULE_SYSUTIL_MUSIC2";
        case CELL_SYSMODULE_SYSUTIL_SCREENSHOT:     return "CELL_SYSMODULE_SYSUTIL_SCREENSHOT";
        case CELL_SYSMODULE_SYSUTIL_MUSIC_DECODE:   return "CELL_SYSMODULE_SYSUTIL_MUSIC_DECODE";
        case CELL_SYSMODULE_SPURS_JQ:       return "CELL_SYSMODULE_SPURS_JQ";
        case CELL_SYSMODULE_PNGENC:         return "CELL_SYSMODULE_PNGENC";
        case CELL_SYSMODULE_SYSUTIL_MUSIC_DECODE2:  return "CELL_SYSMODULE_SYSUTIL_MUSIC_DECODE2";
        case CELL_SYSMODULE_SYNC2:          return "CELL_SYSMODULE_SYNC2";
        case CELL_SYSMODULE_SYSUTIL_NP_UTIL:        return "CELL_SYSMODULE_SYSUTIL_NP_UTIL";
        case CELL_SYSMODULE_RUDP:           return "CELL_SYSMODULE_RUDP";
        case CELL_SYSMODULE_SYSUTIL_NP_SNS: return "CELL_SYSMODULE_SYSUTIL_NP_SNS";
        case CELL_SYSMODULE_GEM:            return "CELL_SYSMODULE_GEM";
        case CELL_SYSMODULE_CELPENC:        return "CELL_SYSMODULE_CELPENC";
        case CELL_SYSMODULE_GIFDEC:         return "CELL_SYSMODULE_GIFDEC";
        case CELL_SYSMODULE_ADEC_CELP:      return "CELL_SYSMODULE_ADEC_CELP";
        case CELL_SYSMODULE_ADEC_M2BC:      return "CELL_SYSMODULE_ADEC_M2BC";
        case CELL_SYSMODULE_ADEC_M4AAC:     return "CELL_SYSMODULE_ADEC_M4AAC";
        case CELL_SYSMODULE_ADEC_MP3:       return "CELL_SYSMODULE_ADEC_MP3";
        case CELL_SYSMODULE_IMEJP:          return "CELL_SYSMODULE_IMEJP";
        case CELL_SYSMODULE_SYSUTIL_MUSIC:  return "CELL_SYSMODULE_SYSUTIL_MUSIC";
        case CELL_SYSMODULE_PHOTO_EXPORT:   return "CELL_SYSMODULE_PHOTO_EXPORT";
        case CELL_SYSMODULE_PRINT:          return "CELL_SYSMODULE_PRINT";
        case CELL_SYSMODULE_PHOTO_IMPORT:   return "CELL_SYSMODULE_PHOTO_IMPORT";
        case CELL_SYSMODULE_MUSIC_EXPORT:   return "CELL_SYSMODULE_MUSIC_EXPORT";
        case CELL_SYSMODULE_PHOTO_DECODE:   return "CELL_SYSMODULE_PHOTO_DECODE";
        case CELL_SYSMODULE_SYSUTIL_SEARCH: return "CELL_SYSMODULE_SYSUTIL_SEARCH";
        case CELL_SYSMODULE_SYSUTIL_AVCHAT2:return "CELL_SYSMODULE_SYSUTIL_AVCHAT2";
        case CELL_SYSMODULE_SAIL_REC:       return "CELL_SYSMODULE_SAIL_REC";
        case CELL_SYSMODULE_SYSUTIL_NP_TROPHY:      return "CELL_SYSMODULE_SYSUTIL_NP_TROPHY";
        default:
            String8 notFound("");
            notFound.setTextFormat("Module name not found: %u", _module);
            return notFound;
        }
    }
#endif //ITF_FINAL

    //--------------------------------------------------------------------------
    void ModuleManager::createInstance()
    {
        m_instance = new ModuleManager();
    }
    //--------------------------------------------------------------------------
    void ModuleManager::unloadAllModules()
    {
        ModulesList::const_iterator it = _modulesLoaded.begin();
        for(; it != _modulesLoaded.end(); ++it)
        {
            cellSysmoduleUnloadModule(it->module);
        }
        _modulesLoaded.clear();
    }
    //--------------------------------------------------------------------------
    void ModuleManager::destroy()
    {
        ITF_ASSERT(m_instance);
        if(m_instance)
        {
            delete m_instance;
            m_instance = NULL;
        }
    }
}//namespace ITF
