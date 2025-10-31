#ifndef ITF_EDGEUNCOMPRESS_H_
#define ITF_EDGEUNCOMPRESS_H_

namespace ITF
{
    class EdgeUnCompress
    {
public:
        static void Init(struct CellSpurs * _spurs, ux _nbTasks);
        static void Destroy();
        static i32 uncompressBuffer(u8 *target, u32 target_size, const u8 *source, u32 source_size);
        static i32 copyBuffer(u8 *_target, u32 _size, const u8 *_source);
    };

} // namespace ITF

#endif // ITF_EDGEUNCOMPRESS_H_