#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#define _ITF_GFXADAPTER_SHADERMANAGER_H_

#ifndef ITF_WII

namespace ITF
{
    const u32 MAX_VECTOR4CONST = 128;
    const u32 MAX_NB_CONSTANTBUFFER = 2;

    class ITF_shader
    {
    public:
        static const int maxHandle = 8;        

        ITF_shader     ( const String& _name  );
        ~ITF_shader    (   );

        void* getTechniqueSelected() {return m_TechHandle[m_selectedTech];}

        bbool loadShaderGroup (   );

        void release    (   );

        // effect adress.
        uPtr                mp_effect = 0;
        String              m_name;
		i32                 m_type = 0;

        u32                 m_selectedTech = 0;
        u32                 m_selectedPS = 0;
        u32                 m_selectedVS = 0;
        
        void*               m_TextureHandle[maxHandle]{};
        void*               m_MatrixHandle[maxHandle]{};
        void*               m_FloatHandle[16]{};
        void*               m_VectorArrayHandle[2]{};
        void*               m_IntHandle[maxHandle]{};
        void*               m_VecHandle[maxHandle]{};
        void*               m_TechHandle[16]{};
        void*               m_alphaTestRef = nullptr;

        void                setVectorArray  (   u32 _h, const GFX_Vector4* _va, u32 _count   );
        void                setVector       (   u32 _h, const GFX_Vector4* _v   );   
        void                setFloat        (   u32 _h, float _f  );  
        void                setInt          (   u32 _h, i32 _i  );  
        void                commitChange    (   );

		ITF_VECTOR<uPtr>		mp_Vs;
		ITF_VECTOR<uPtr>		mp_Ps;
	};

  
    class GFXAdapter_shaderManager
    {

        //prepare Migration
        struct shaderBuffer
        {
            u32 m_curBuffFloatPos[MAX_NB_CONSTANTBUFFER];
            u32 m_numberVector4ConstToFlush[MAX_NB_CONSTANTBUFFER];
            f32 m_bufferFloat[MAX_NB_CONSTANTBUFFER][MAX_VECTOR4CONST*4];
        };


    public:
        GFXAdapter_shaderManager        (   );
        ~GFXAdapter_shaderManager       (   );

        void loadShaderGroup            (   const String& _name );
        void createShaderGroup          (   const String& _name );

        void removeShaderGroup          (   ITF_shader* _shader );
        void destroyAll                 (   );
        ITF_shader* getShaderByIndex    (   u32 _id );
        i32 getShaderIndex              (ITF_shader * _shader);
        ITF_shader* getShaderByName     (   const String& _name );
        ITF_INLINE u32 getNumberShader  (   ) {return mp_shaderList.size();};

        ITF_INLINE  const f32* getBufferFloat(u32 _id) const {ITF_ASSERT(_id < MAX_NB_CONSTANTBUFFER); return m_shaderBuffer.m_bufferFloat[_id];};
        ITF_INLINE  const f32* getBufferFloatAt(u32 _id, u32 _n) const {ITF_ASSERT(_id < MAX_NB_CONSTANTBUFFER); return m_shaderBuffer.m_bufferFloat[_id] + _n;};
        ITF_INLINE void setBufferFloatAt(u32 _id ,u32 _n, f32 _val) {m_shaderBuffer.m_bufferFloat[_id][_n] = _val;};

        ITF_INLINE u32 getCurBuffFloatPos(u32 _id) {ITF_ASSERT(_id < MAX_NB_CONSTANTBUFFER); return m_shaderBuffer.m_curBuffFloatPos[_id];};
        ITF_INLINE void setCurBuffFloatPos(u32 _id, u32 _pos) {m_shaderBuffer.m_curBuffFloatPos[_id] = _pos;};
        ITF_INLINE void addCurBuffFloatPos(u32 _id, u32 _add) {m_shaderBuffer.m_curBuffFloatPos[_id] += _add;};
        ITF_INLINE u32 getNumberVector4ConstToFlush(u32 _id) {ITF_ASSERT(_id < MAX_NB_CONSTANTBUFFER); return m_shaderBuffer.m_numberVector4ConstToFlush[_id];};
        ITF_INLINE void addVector4ConstToFlush(u32 _id, u32 _add) {ITF_ASSERT(_id < MAX_NB_CONSTANTBUFFER); m_shaderBuffer.m_numberVector4ConstToFlush[_id] +=_add;};
        ITF_INLINE void setVector4ConstToFlush(u32 _id, u32 _pos) {ITF_ASSERT(_id < MAX_NB_CONSTANTBUFFER); m_shaderBuffer.m_numberVector4ConstToFlush[_id] = _pos;};

    private:
        void addShaderGroup            (   ITF_shader* _shader );
        /// effect adress.
        ITF_VECTOR<ITF_shader*>         mp_shaderList;

        /// buffer for shader constant.
        shaderBuffer m_shaderBuffer;
    };

} // namespace ITF

#endif // ITF_WII

#endif // _ITF_GFXADAPTER_SHADERMANAGER_H_
