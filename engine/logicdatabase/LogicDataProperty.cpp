#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_LOGICDATAUTILS_H_
#include "engine/logicdatabase/logicdatautils.h"
#endif //_ITF_LOGICDATAUTILS_H_

namespace ITF
{


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataProperty::serializeWrite(ArchiveMemory& _archive)
{
    switch (type)
    {
    case eLogicDataString: 
        _archive.serialize(string.cbData);
        _archive.serializeBlock16(string.pwszData,string.cbData);
        break;

    case eLogicDataNumber:
    case eLogicDataVector2d:
    case eLogicDataVector3d:
        break;

    default:
        ITF_ASSERT(0);
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataProperty::serializeTracking(LogicDataMemoryShared& _memoryShared)
{
    switch (type)
    {
    case eLogicDataNumber:
        {
            _memoryShared.m_archive[type].serializeBlock8((u8*) &fData,sizeof(f32));
        }
        break;

    case eLogicDataString: 
        {
            _memoryShared.m_stringLenAllocation += (string.cbData+1);
        }
        break;

    case eLogicDataVector2d:
        {
            Vec2d* vec2d = (Vec2d*)(binary.pBuffer);
            //directly the buffer, not swapping
            _memoryShared.m_archive[type].serializeBlock8((u8*)vec2d,sizeof(Vec2d));

        }
        break;

    case eLogicDataVector3d:
        {
            Vec3d* vec3d = (Vec3d*)(binary.pBuffer);
            //directly the buffer, not swapping
            _memoryShared.m_archive[type].serializeBlock8((u8*)vec3d,sizeof(Vec3d));
        }
        break;

    default:
        break;
    }

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataProperty::serializeRead(ArchiveMemory& _archive,LogicDataMemoryShared& _memoryShared)
{
    switch (type)
    {
        case eLogicDataNumber:
            {
                f32* pF32 = (f32*)(_memoryShared.m_linearMemory[type] + _memoryShared.m_positionDataNumber*sizeof(f32));
                fData = *pF32;
                    
                ++_memoryShared.m_positionDataNumber;
            }
            break;

        case eLogicDataString: 
            {
                _archive.serialize(string.cbData);

                string.pwszData = _memoryShared.m_linearString+_memoryShared.m_positionLinearString;
                _memoryShared.m_positionLinearString += string.cbData+1;

                _archive.serializeBlock16(string.pwszData,string.cbData);
                string.pwszData[string.cbData] = 0;
            }
            break;

        case eLogicDataVector2d:
            {
                binary.size = sizeof(Vec2d);
                Vec2d* vec2d = (Vec2d*) (_memoryShared.m_linearMemory[type]+ _memoryShared.m_positionVect2D*sizeof(Vec2d));
                ++_memoryShared.m_positionVect2D;
                binary.pBuffer =(void*) vec2d;
            }
            break;

        case eLogicDataVector3d:
            {
                binary.size = sizeof(Vec3d);
                Vec3d* vec3d = (Vec3d*) (_memoryShared.m_linearMemory[type] + _memoryShared.m_positionVect3D*sizeof(Vec3d));
                ++_memoryShared.m_positionVect3D;
                binary.pBuffer =(void*) vec3d;
            }
            break;

        default:
            ITF_ASSERT(0);
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataProperty::destroy()
{

    switch (type)
    {
        case eLogicDataString: 
            {
                u16* ptr = (u16*)(string.pwszData);
                SF_DEL_ARRAY(ptr);
                string.pwszData = NULL;
            }
            break;

        case eLogicDataVector2d:
            {
                Vec2d* vec2d = (Vec2d*)(binary.pBuffer);
                SF_DEL(vec2d);
            }
            break;

        case eLogicDataVector3d:
            {
                Vec3d* vec3d = (Vec3d*)(binary.pBuffer);
                SF_DEL(vec3d);
            }
            break;

        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif //ITF_SUPPORT_LOGICDATABASE
