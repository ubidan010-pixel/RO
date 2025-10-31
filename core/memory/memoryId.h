#ifndef ITF_MEMORYID_H_
#define ITF_MEMORYID_H_

#pragma once

namespace ITF
{
    class MemoryId
    {

        public:


            static const unsigned int HEAP_MEMORY      =  0;
            static const unsigned int PHYSICAL_MEMORY  =  1;

            static unsigned int poolId_XMLHandle;
            static unsigned int poolId_TiXmlDocument;

            enum ITF_ALLOCATOR_IDS
            {
                mId_Unknow              = 1,
                mId_String8             = 2,
                mId_String16            = 3,
                mId_IDServer            = 4, 
                mId_SafeArray           = 5,
                mId_Scene               = 6,
                mId_Animation           = 7,
                mId_LogicDataBase       = 8,
                mId_Temporary           = 9,
                mId_Singleton           = 10,
                mId_StringIDDatabase    = 11,
                mId_Map                 = 12,
                mId_Vector              = 13,
                mId_Phys                = 14,
                mId_Vertex              = 15,
                mId_List                = 16,
                mId_Misc                = 17,
                mId_Gameplay            = 18,
                mId_Allocator           = 19,
                mId_AudioData           = 20,
                mId_TinyXML             = 21,
                mId_UI                  = 22,
                mId_Resource            = 23,
                mId_Mesh                = 24,
                mId_Editor              = 25,
                mId_System              = 26,
                mId_Particle            = 27,
                mId_Polyline            = 28,
                mId_Frieze              = 29,
                mId_Blob                = 30,
                mId_GfxAdapter          = 31,
                mId_VectorAnimation     = 32,
                mId_SavegameManager     = 33,
                mId_Plugin              = 34,
                mId_Textures            = 35,
                mId_VertexBuffer        = 36,
                mId_IndexBuffer         = 37,
                mId_ExeSystem           = 38,
                mId_SafeArrayCache      = 39,
                
                mId_VectorPointer       = 40,
                mId_VectorObjectRef     = 41,
                mId_VectorString        = 42,
                mId_VectorInt             = 43,
                mId_VectorAnimTrackBone   = 44,
                mId_VectorAnimTrackBonePAS = 45,
                mId_VectorAnimTrackBoneZAL = 46,
                mId_VertexBufferStatic   = 47,

                mId_GameplayTemplate  = 48,
                mId_GameplayFactory  = 49,
                mId_GameplayMisc  = 50,
                mId_GameplayInputCriteria  = 51,
                mId_ParticleVertex = 52,
                mId_Actor = 53,
                mId_Serialization = 54,
                mId_WiiSystem = 55,
                mId_FileCache = 56,
                
                mId_AudioStreamBuffers = 57,
                mId_AudioEngine = 58,
                mId_Archive = 59,

                mId_Debug = 60,
                mId_OnlineTracking = 61,

                mId_VRAMPool = 62,

                mId_OnlineServices = 63,

                mId_Max

            };
    };
}

#endif //ITF_MEMORYID_H_
