/* SIE CONFIDENTIAL
PlayStation(R)5 Programmer Tool Runtime Library Release 10.00.00.48-00.00.00.0.1
* Copyright (C) 2018 Sony Interactive Entertainment Inc.
* 
*/

#if !defined(LIBSCESHADER_INLINE)
#define LIBSCESHADER_INLINE 1
#endif

#ifndef __LIBSCESHADER_H_
#define __LIBSCESHADER_H_

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif	// def __cplusplus

/* The usual compatibility preprocessor shenanigans */
#if !defined(DOXYGEN_IGNORE)
#if defined(LIBSCESHADER_INLINE) && LIBSCESHADER_INLINE
#	define SCE_SHADER_INTERFACE inline
#else
#	if defined(__ORBIS__) || defined(__PROSPERO__) || defined(__linux__) || defined(SCE_WAVE_STATIC)
#		define SCE_SHADER_INTERFACE
#	else
#		ifdef SCE_SHADER_EXPORTS
#			define SCE_SHADER_INTERFACE	__declspec( dllexport )
#		else
#			define SCE_SHADER_INTERFACE	__declspec( dllimport )
#		endif
#	endif /* __ORBIS__ / __PROSPERO__ / __linux__ / SCE_WAVE_STATIC */
#endif
#endif /* DOXYGEN_IGNORE */


///////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////

/**	@brief Provides the entry point for the shader to introspect

	@ingroup shader_reflection
 */
typedef struct _SceShaderBinaryHandle const * SceShaderBinaryHandle;

/**	@brief Wraps opaque pointer
	@ingroup shader_reflection
 */
typedef const void * SceShaderOpaque;
/**	@brief Represents the basic shader profile types

	@ingroup shader_reflection
 */
typedef enum SceShaderProfile
{
	SceShaderProfileVertex,			///< Vertex shader.
	SceShaderProfilePixel,			///< Pixel shader.
	SceShaderProfileCompute,		///< Compute shader.
	SceShaderProfileGeometry,		///< Geometry shader.
	SceShaderProfileHull,			///< Hull shader.
	SceShaderProfileDomain,			///< Domain shader.
	SceShaderProfilePrimitive,		///< Primitive shader.
	SceShaderProfilePrimitiveTess,	///< Primitive shader with tessellation input.
	SceShaderProfileFunction,	 	///< Function shader.
	SceShaderProfileMesh	 		///< Mesh shader.
} SceShaderProfile;

/**	@brief Represents the vertex shader variants.

	@ingroup shader_reflection
 */
typedef enum SceShaderVertexVariant
{
	SceShaderVertexVariantVertex,	///< Vertex shader.
	SceShaderVertexVariantExport,	///< Vertex shader as an export shader.
	SceShaderVertexVariantLocal,	///< Vertex shader as a local shader.
} SceShaderVertexVariant;

/**	@brief Represents the domain shader variants.

	@ingroup shader_reflection
 */
typedef enum SceShaderDomainVariant
{
	SceShaderDomainVariantVertex,	///< Domain shader as a vertex shader.
	SceShaderDomainVariantExport,	///< Domain shader as an export shader.
} SceShaderDomainVariant;

/**	@brief Represents the patch types for domain and hull shaders.

	@ingroup shader_reflection
 */
typedef enum SceShaderPatchType
{
	SceShaderPatchTypeIsoline,		///< Isoline patch.
	SceShaderPatchTypeTri,			///< Triangle patch.
	SceShaderPatchTypeQuad			///< Quad patch.
} SceShaderPatchType;

/**	@brief Represents the topology types for hull shaders.

	@ingroup shader_reflection
 */
typedef enum SceShaderTopologyType
{	
	SceShaderTopologyTypePoint,		///< Point topology.
	SceShaderTopologyTypeLine,		///< Line topology.
	SceShaderTopologyTypeCwtri,		///< Clockwise triangle topology.
	SceShaderTopologyTypeCcwtri		///< Counter-clockwise triangle topology.
} SceShaderTopologyType;

/**	@brief Represents the tessellation partitioning types.

	@ingroup shader_reflection
 */
typedef enum SceShaderPartitioningType
{
	SceShaderPartitioningTypeInteger,		///< Integer spacing mode.
	SceShaderPartitioningTypePowerOf2,		///< Power-of-2 spacing mode.
	SceShaderPartitioningTypeOddFactorial,	///< Fractional-odd spacing mode.
	SceShaderPartitioningTypeEvenFactorial, ///< Fractional-even spacing mode
} SceShaderPartitioningType;

/**	@brief Represents the input and output primitive types for geometry shaders.

	@ingroup shader_reflection
 */
typedef enum SceShaderGsPrimType
{
	SceShaderGsPrimTypeTri,			///< Triangle.
	SceShaderGsPrimTypeLine,		///< Line.
	SceShaderGsPrimTypePoint,		///< Point.
	SceShaderGsPrimTypeAdjTri,		///< Triangle with adjacency.
	SceShaderGsPrimTypeAdjLine,		///< Line with adjacency.
} SceShaderGsPrimType;

/**	@brief Represents export color formats.

	@ingroup shader_reflection
 */
typedef enum SceShaderExportColorFmt
{
	SceShaderExportColorFmt_ZERO,			///< No export done.
	SceShaderExportColorFmt_32_R,			///< Red component. FP32 or SINT32 or UINT32.
	SceShaderExportColorFmt_32_GR,			///< Green and red components. FP32 or SINT32 or UINT32.
	SceShaderExportColorFmt_32_AR,			///< Alpha and red components. FP32 or SINT32 or UINT32.
	SceShaderExportColorFmt_FP16_ABGR,		///< All components (red, green, blue, alpha). FP16.
	SceShaderExportColorFmt_UNORM16_ABGR,	///< All components (red, green, blue, alpha). UNORM16
	SceShaderExportColorFmt_SNORM16_ABGR,	///< All components (red, green, blue, alpha). SNORM16.
	SceShaderExportColorFmt_UINT16_ABGR,	///< All components (red, green, blue, alpha). UINT16.
	SceShaderExportColorFmt_SINT16_ABGR,	///< All components (red, green, blue, alpha). SINT16.
	SceShaderExportColorFmt_32_ABGR			///< All components (red, green, blue, alpha). FP32 or SINT32 or UINT32.
} SceShaderExportColorFmt;

/**	@brief Represents the interpolation classes for pixel shader inputs.

	@ingroup shader_reflection
 */
typedef enum SceShaderInterpolationClass
{
	SceShaderInterpolationClass_PerspectiveCenter,		///< Perspective at pixel center.
	SceShaderInterpolationClass_PerspectiveSample,		///< Perspective at sample position.
	SceShaderInterpolationClass_PerspectiveCentroid,	///< Perspective at pixel centroid.
	SceShaderInterpolationClass_LinearCenter,			///< Linear at pixel center.
	SceShaderInterpolationClass_LinearSample,			///< Linear at sample position.
	SceShaderInterpolationClass_LinearCentroid,			///< Linear at pixel centroid.
	SceShaderInterpolationClass_Flat,					///< No interpolation.
	SceShaderInterpolationClass_Custom,					///< Custom interpolation.
	SceShaderInterpolationClass_Unknown = -1			///< Information missing from metadata
} SceShaderInterpolationClass;

/**	@brief Represents the compression modes for pixel shader inputs.

	@ingroup shader_reflection
 */
typedef enum SceShaderCompressionMode
{
	SceShaderCompressionMode_None,			///< No compression.
	SceShaderCompressionMode_Fp16,			///< FP16 precision.
	SceShaderCompressionMode_UNorm16,		///< Unsigned 16-bit integer mapped to [0..1].
	SceShaderCompressionMode_SNorm16,		///< Signed 16-bit integer mapped to [-1..1].
	SceShaderCompressionMode_UInt16,		///< Unsigned 16-bit integer
	SceShaderCompressionMode_Int16,			///< Signed 16-bit integer
	SceShaderCompressionMode_UNorm8,		///< Unsigned 8-bit integer mapped to [0..1].
	SceShaderCompressionMode_SNorm8,		///< Signed 8-bit integer mapped to [-1..1].
	SceShaderCompressionMode_UInt8,			///< Unsigned 8-bit integer
	SceShaderCompressionMode_Unknown = -1	///< Information missing from metadata
} SceShaderCompressionMode;

/**	@brief Represents the Z order testing strategies used by pixel shaders.

	@ingroup shader_reflection
 */
typedef enum SceShaderZOrder
{
	SceShaderZOrder_LateZ,					///< Z test and write after pixel shader run
	SceShaderZOrder_EarlyZThenLateZ,		///< Z test and write before pixel shader run, fallback to LateZ
	SceShaderZOrder_ReZ,					///< Z test before and Z test and write after pixel shader run
	SceShaderZOrder_EarlyZThenReZ			///< Z test and write before pixel shader run, fallback to ReZ
} SceShaderZOrder;

/**	@brief Represents the conservative Z modes used by pixel shaders.

	@ingroup shader_reflection
 */
typedef enum SceShaderConservativeZMode
{
	SceShaderConservativeZMode_AnyZ,			///< The pixel shader can export any Z value.
	SceShaderConservativeZMode_LessThanZ,		///< The pixel shader can export Z values that are less than or the same as the input Z value.
	SceShaderConservativeZMode_GreaterThanZ,	///< The pixel shader can export Z values that are greater than or the same as the input Z value.
} SceShaderConservativeZMode;

/**	@brief Represents the shader resource types.

	@ingroup shader_reflection
 */
typedef enum SceShaderResourceClass
{
	SceShaderSrt,					///< SRT data.
	SceShaderCb,					///< Constant buffer.
	SceShaderUav,					///< Writeable resource.
	SceShaderSrv,					///< Read-only resource.
	SceShaderInputAttribute,		///< Input attribute.
	SceShaderOutputAttribute,		///< Output attribute.
	SceShaderSamplerState,			///< Sample state.
	SceShaderStreamOut,				///< Stream out.
	SceShaderGlobalSrt,				///< Global SRT data.
	SceShaderUnknownResource		///< Unknown resource.
} SceShaderResourceClass;

/**	@brief Represents the shader resource types available in PSSL.

	@ingroup shader_reflection
 */
typedef enum SceShaderTypeClass
{
	SceShaderVectorType,			///< Built-in vector type.
	SceShaderMatrixType,			///< Built-in matrix type.
	SceShaderArrayType,				///< Array type.
	SceShaderStructType,			///< Structure type.
	SceShaderPointerType,			///< Pointer type.
	SceShaderSamplerStateType,		///< Sampler state type.
	SceShaderTextureType,			///< Built-in texture type.
	SceShaderBufferType,			///< Built-in buffer type.
	SceShaderFunctionShaderType,	///< Built-in function shader type.
	SceShaderUnknownType			///< Unknown type.
} SceShaderTypeClass;

/**	@brief Represents the built-in numeric types available in PSSL.

	@ingroup shader_reflection
 */
typedef enum SceShaderNumericClass
{
	SceShaderFloat16,				///< Half float. Maps to PSSL <c>half</c>.
	SceShaderFloat32,				///< 32-bit float. Maps to PSSL <c>float</c>.
	SceShaderFloat64,				///< 64-bit float. Maps to PSSL <c>double</c>.
	SceShaderInt8,					///< 8-bit signed integer. Maps to PSSL <c>char</c>.
	SceShaderInt16,					///< 16-bit signed integer. Maps to PSSL <c>short</c>.
	SceShaderInt32,					///< 32-bit signed integer. Maps to PSSL <c>int</c>.
	SceShaderInt64,					///< 64-bit signed integer. Maps to PSSL <c>long</c>.
	SceShaderUint8,					///< 8-bit unsigned integer. Maps to PSSL <c>uchar</c>.
	SceShaderUint16,				///< 16-bit unsigned integer. Maps to PSSL <c>ushort</c>.
	SceShaderUint32,				///< 32-bit unsigned integer. Maps to PSSL <c>uint</c>.
	SceShaderUint64,				///< 64-bit unsigned integer. Maps to PSSL <c>ulong</c>.
	SceShaderBool,					///< Boolean. Maps to PSSL <c>bool</c>.
} SceShaderNumericClass;

/**	@brief Represents the PSSL types for buffers and textures.

	@ingroup shader_reflection
 */
typedef enum SceShaderBufferClass
{
	SceShaderDataBuffer,			///< Maps to PSSL <c>DataBuffer\<type\></c>
	SceShaderRegularBuffer,			///< Maps to PSSL <c>RegularBuffer\<type\></c>
	SceShaderByteBuffer,			///< Maps to PSSL <c>ByteBuffer</c>
	SceShaderAppendBuffer,			///< Maps to PSSL <c>AppendRegularBuffer\<type\></c>
	SceShaderConsumeBuffer,			///< Maps to PSSL <c>ConsumeRegularBuffer\<type\></c>
	SceShaderConstantBuffer,		///< Maps to PSSL <c>ConstantBuffer</c>
	SceShaderTexture1d,				///< Maps to PSSL <c>Texture1D\<type\></c>
	SceShaderTexture2d,				///< Maps to PSSL <c>Texture2D\<type\></c>
	SceShaderTexture3d,				///< Maps to PSSL <c>Texture3D\<type\></c>
	SceShaderTexture1dR128,			///< Maps to PSSL <c>Texture1D_R128\<type\></c>
	SceShaderTexture2dR128,			///< Maps to PSSL <c>Texture2D_R128\<type\></c>
	SceShaderTextureCube,			///< Maps to PSSL <c>TextureCube\<type\></c>
	SceShaderTexture1dArray,		///< Maps to PSSL <c>Texture1D_Array\<type\></c>
	SceShaderTexture2dArray,		///< Maps to PSSL <c>Texture2D_Array\<type\></c>
	SceShaderTextureCubeArray,		///< Maps to PSSL <c>TextureCube_Array\<type\></c>
	SceShaderMsTexture2d,			///< Maps to PSSL <c>MS_Texture2D\<type\></c>
	SceShaderMsTexture2dArray,		///< Maps to PSSL <c>MS_Texture2D_Array\<type\></c>
	SceShaderMsTexture2dR128		///< Maps to PSSL <c>MS_Texture2D_R128\<type\></c>
} SceShaderBufferClass;

/**	@brief Represents the PSSL system semantics.

	@ingroup shader_reflection
 */
typedef enum SceShaderSemantic
{
	SceShaderSemanticEdgeTessFactor,			///< Amount of tessellation within each edge of a patch surface. Maps to PSSL <c>S_EDGE_TESS_FACTOR</c>.
	SceShaderSemanticInsideTessFactor,			///< Amount of tessellation within a patch surface. Maps to PSSL <c>S_INSIDE_TESS_FACTOR</c>.
	SceShaderSemanticDomainLocation,			///< The current hull domain point being evaluated. Maps to PSSL <c>S_DOMAIN_LOCATION</c>.
	SceShaderSemanticPrimitiveId,				///< Index of the contributing primitive. Maps to PSSL <c>S_PRIMITIVE_ID</c>.
	SceShaderSemanticDispatchThreadId,			///< Global thread ID. Maps to PSSL <c>S_DISPATCH_THREAD_ID</c>.
	SceShaderSemanticGroupId,					///< Group ID within the dispatch. Maps to PSSL <c>S_GROUP_ID</c>.
	SceShaderSemanticGroupIndex,				///< Computed flat index of a thread within a group. Maps to PSSL <c>S_GROUP_INDEX</c>.
	SceShaderSemanticGroupThreadId,				///< Offset of a thread within a group, per group dimension. Maps to PSSL <c>S_GROUP_THREAD_ID</c>.
	SceShaderSemanticOutputControlPointId,		///< Current control point index. Maps to PSSL <c>S_OUTPUT_CONTROL_POINT_ID</c>.
	SceShaderSemanticVertexId,					///< Index of the current vertex. Maps to PSSL <c>S_VERTEX_ID</c>.
	SceShaderSemanticInstanceId,				///< Current instance index. Maps to PSSL <c>S_INSTANCE_ID</c>.
	SceShaderSemanticVertexOffsetId,			///< Offset applied to the index of the current vertex when using indirect draw. Maps to PSSL <c>S_VERTEX_OFFSET_ID</c>.
	SceShaderSemanticInstanceOffsetId,			///< Offset applied to the instance index when using indirect draw. Maps to PSSL <c>S_INSTANCE_OFFSET_ID</c>.
	SceShaderSemanticPosition,					///< Vertex position output, pixel shader input as screen position. Maps to PSSL <c>S_POSITION</c>.
	SceShaderSemanticFrontFace,					///< Indication of a front-facing primitive. Maps to PSSL <c>S_FRONT_FACE</c>.
	SceShaderSemanticCoverage,					///< Output coverage. Maps to PSSL <c>S_COVERAGE[n]</c>.
	SceShaderSemanticSampleIndex,				///< Index of the sample. Maps to PSSL <c>S_SAMPLE_INDEX</c>.
	SceShaderSemanticRenderTargetIndex,			///< Render target array index. Maps to PSSL <c>S_RENDER_TARGET_INDEX</c>.
	SceShaderSemanticViewportIndex,				///< Viewport array index. Maps to PSSL <c>S_VIEWPORT_INDEX</c>.
	SceShaderSemanticPointCoord,				///< Coordinate for point sprite. Maps to PSSL <c>S_POINT_COORD</c>.
	SceShaderSemanticTargetOutput,				///< Output color. Maps to PSSL <c>S_TARGET_OUTPUT[n]</c>.
	SceShaderSemanticDepthOutput,				///< Output depth. Maps to PSSL <c>S_DEPTH_OUTPUT</c>.
	SceShaderSemanticDepthLeOutput,				///< Conservative depth output (less than or equal to). Maps to PSSL <c>S_DEPTH_LE_OUTPUT</c>.
	SceShaderSemanticDepthGeOutput,				///< Conservative depth output (greater than or equal to). Maps to PSSL <c>S_DEPTH_GE_OUTPUT</c>.
	SceShaderSemanticDepthStencilValue,			///< Unsigned int value to apply to the stencil test. Maps to PSSL <c>S_STENCIL_VALUE</c>.
	SceShaderSemanticDepthStencilOp,			///< Unsigned int value to apply to the stencil operation. Maps to PSSL <c>S_STENCIL_OP</c>.
	SceShaderSemanticClipDistance,				///< Clip distance. Maps to PSSL <c>S_CLIP_DISTANCE[n]</c>.
	SceShaderSemanticCullDistance,				///< Cull distance. Maps to PSSL <c>S_CULL_DISTANCE[n]</c>.
	SceShaderSemanticObjPrimId,					///< Object primitive index. Maps to PSSL <c>S_OBJPRIM_ID</c>.
	SceShaderSemanticPointSize,					///< Size for point sprite. Maps to PSSL <c>S_POINT_SIZE</c>.
	SceShaderSemanticGsInstanceId,				///< Geometry shader instance ID. Maps to PSSL <c>S_GSINSTANCE_ID</c>.
	SceShaderSemanticStereoPosition,			///< Stereo position. Maps to PSSL <c>S_STEREO_POSITION</c>.
	SceShaderSemanticEyeIndex,					///< Eye index. Maps to PSSL <c>S_EYE_INDEX</c>.
	SceShaderSemanticDrawId,					///< Draw index. Maps to PSSL <c>S_DRAW_ID</c>.
	SceShaderSemanticTargetOutputMask,			///< Output color mask. Maps to PSSL <c>S_TARGET_OUTPUT_MASK[n]</c>.
	SceShaderSemanticNggPrimitiveCount,			///< Primitive count in this wave. Maps to PSSL <c>S_NGG_PRIMITIVE_COUNT</c>.
	SceShaderSemanticNggVertexCount,			///< Vertex count in this wave. Maps to PSSL <c>S_NGG_VERTEX_COUNT</c>.
	SceShaderSemanticNggGroupVertexIndex,		///< Subgroup local vertex index. Maps to PSSL <c>S_NGG_GROUP_VERTEX_INDEX</c>.
	SceShaderSemanticNggGroupPrimitiveCount,	///< Primitive count in this subgroup. Maps to PSSL <c>S_NGG_GROUP_PRIMITIVE_COUNT</c>.
	SceShaderSemanticNggGroupVertexCount,		///< Vertex count in this subgroup. Maps to PSSL <c>S_NGG_GROUP_VERTEX_COUNT</c>.
	SceShaderSemanticNggWaveIndex,				///< The wave index within the subgroup. Maps to PSSL <c>S_NGG_WAVE_INDEX</c>.
	SceShaderSemanticNggGroupSize,				///< The size of the subgroup in waves. Maps to PSSL <c>S_NGG_GROUP_SIZE</c>.
	SceShaderSemanticNggPrimPayload,			///< The primitive ID payload. Maps to PSSL <c>S_NGG_PRIM_PAYLOAD</c>.
	SceShaderSemanticNggOutputPrim,				///< The output primitive. Maps to PSSL <c>S_NGG_OUTPUT_PRIMITIVE</c>.
	SceShaderSemanticNggOutputPrimPayload,		///< The output primitive ID payload. Maps to PSSL <c>S_NGG_OUTPUT_PRIM_PAYLOAD</c>.
	SceShaderSemanticNggOutputVertCount,		///< The total number of output vertices. Maps to PSSL <c>S_NGG_OUTPUT_VERTEX_COUNT</c>.
	SceShaderSemanticNggOutputPrimCount,		///< The total number of output primitives. Maps to PSSL <c>S_NGG_OUTPUT_PRIMITIVE_COUNT</c>.
	SceShaderSemanticShadingRate,				///< Maps to PSSL <c>S_SHADING_RATE</c>.
	SceShaderSemanticUser = 0xff				///< User-defined semantic
} SceShaderSemantic;

/** @brief Represents the shader wave modes */
typedef enum SceShaderWaveMode
{
	SceShaderWaveModeWave32,					///< <c>wave32</c>
	SceShaderWaveModeWave64						///< <c>wave64</c>
} SceShaderWaveMode;

/** @brief Represents a Boolean value that can have an unknown state */
typedef enum SceShaderTriBool
{
	SceShaderTriBoolFalse,					///< False
	SceShaderTriBoolTrue,					///< True
	SceShaderTriBoolUnknown,				///< Unknown
} SceShaderTriBool;

/**	@brief Represents the available compilation targets. */
typedef enum SceShaderCompileTarget
{
	SceShaderCompileTargetBase = 0xd,		///< Base compile target
	SceShaderCompileTargetCommon = 0xe,		///< Common compile target
	SceShaderCompileTargetTrinity = 0xf,	///< Trinity compile target
	SceShaderCompileTargetUnknown = 0	///< Undefined compile target
} SceShaderCompileTarget;

/**	@brief Represents the type of encoding used for a PSSL buffer.

	@ingroup shader_reflection
 */
typedef enum SceShaderBufferEncoding
{
	SceShaderBufferEncodingVSharp,			///< Buffer is encoded as a native V#
	SceShaderBufferEncodingPtr64,			///< Buffer is encoded as a 64-bit pointer
	SceShaderBufferEncodingSizePtr64,		///< Buffer is encoded as a 64-bit SizedPtr
} SceShaderBufferEncoding;

/**	@brief Creates a handle to a binary file in memory.

	@param[in] data
		Pointer to the shader file data.
	
	@return
		A handle to the shader.

	@ingroup shader_reflection
 */
SCE_SHADER_INTERFACE SceShaderBinaryHandle sceShaderGetBinaryHandle(void const *data);


///////////////////////////////////////////////////////////////////////////////
// Microcode
///////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the size of the shader program in bytes.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		Size of the program in bytes.

	@ingroup shader_reflection
*/

SCE_SHADER_INTERFACE uint64_t sceShaderGetProgramSize(SceShaderBinaryHandle shaderBinary);

/**	@brief	Gets a pointer to the shader program.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		A pointer to the program.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderOpaque sceShaderGetProgram(SceShaderBinaryHandle shaderBinary);


///////////////////////////////////////////////////////////////////////////////
// Agc Header
///////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the header size of a program.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		The size of the program header in bytes.

	@ingroup shader_reflection
*/

SCE_SHADER_INTERFACE uint64_t sceShaderGetProgramHeaderSize(SceShaderBinaryHandle shaderBinary);

/**	@brief	Gets a pointer to the header.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		A pointer to the header.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderOpaque sceShaderGetProgramHeader(SceShaderBinaryHandle shaderBinary);


///////////////////////////////////////////////////////////////////////////////
// Metadata
///////////////////////////////////////////////////////////////////////////////

/**	@brief	Represents a handle to the <c>.metadata</c> section in the shader binary file.

	@ingroup shader_reflection
*/
typedef struct _SceShaderMetadataSectionHandle const * SceShaderMetadataSectionHandle;

/**	@brief	Represents the entry point for all reflection nodes in the shader

	@ingroup shader_reflection
*/
typedef struct _SceShaderResourceListHandle const * SceShaderResourceListHandle;

/**	@brief	Represents a generic resource in the program.

	This is the basic node to inspect.

	@ingroup shader_reflection
*/
typedef struct _SceShaderResourceHandle const * SceShaderResourceHandle;

/**	@brief	Represents the entry point for all annotation nodes in a shader

	@ingroup shader_reflection
*/
typedef struct _SceShaderAnnotationListHandle const* SceShaderAnnotationListHandle;

/**	@brief	Represents a generic annotation in the program.

	This is the basic node to inspect.

	@ingroup shader_reflection
*/
typedef struct _SceShaderAnnotationHandle const* SceShaderAnnotationHandle;

/**	@brief	Represents a type in the reflection data.

	This is used for structures, arrays and pointers.

	@ingroup shader_reflection
*/
typedef struct _SceShaderTypeHandle const * SceShaderTypeHandle;

/**	@brief	Represents a member of a structure.

	@ingroup shader_reflection
*/
typedef struct _SceShaderMemberHandle const * SceShaderMemberHandle;

/**	@brief	Gets a handle to the metadata section.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		A handle to the metadata section.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderMetadataSectionHandle
sceShaderGetMetadataSection(
	SceShaderBinaryHandle shaderBinary);

/**	@brief	Gets the shader binary hash.

	@param[in] metadata
		A handle to the metadata section.

	@return
		The shader binary hash.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetBinaryHash(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the source hash for the shader.

	@param[in] metadata
		A handle to the metadata section.

	@return
		The shader source hash.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetSourceHash(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the compiler version in integer format.

	@param[in] metadata
		A handle to the metadata section.

	@return
		The compiler version, represented as an integer.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetCompilerVersion(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the compiler version in string format.

	@param[in] metadata
	A handle to the metadata section.

	@return
	The compiler version, represented as a string.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetCompilerVersionString(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the shader profile.

	@param[in] metadata
		A handle to the metadata section.

	@return
		The shader profile.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderProfile sceShaderGetProfile(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether the shader uses SRT in any form.

	@param[in] metadata
		A handle to the metadata section.

	@return
		Returns true if the shader uses SRT, false if it does not.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderUsesSrt(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the shader wave mode.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the wave mode for the shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderWaveMode sceShaderGetWaveMode(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the VGPR count for a shader.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the VGPR count for the shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetVgprCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the SGPR count for a shader.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the SGPR count for the shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetSgprCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the shared VGPR count for a shader.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the shared VGPR count for the shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetSharedVgprCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the shader scratch size in DWORDs.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the shader scratch size, in DWORDs.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetScratchSizeInDwords(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the shader LDS size in bytes.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the shader LDS size, in bytes.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetLdsSizeInBytes(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the shader GDS size in bytes.

	@param[in] metadata
	A handle to the metadata section.

	@return
	Returns the shader GDS size, in bytes.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetGdsSizeInBytes(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether the shader computes derivatives.

	@param[in] metadata
		A handle to the metadata section.

	@return
		Returns <c>true</c> if the shader computes derivatives, <c>false</c> otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderUsesDerivatives(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether the shader was compiled with development features enabled.

	@param[in] metadata
		A handle to the metadata section.

	@return
		@retval SceShaderTriBoolTrue	The shader was compiled with development features enabled.
		@retval SceShaderTriBoolFalse	The shader was compiled without development features enabled.
		@retval SceShaderTriBoolUnknown	The shader binary is too old to contain this information.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTriBool sceShaderHasDevelopmentFeatures(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether the shader was compiled with realtypes enabled.

	@param[in] metadata
		A handle to the metadata section.

	@return
		@retval SceShaderTriBoolTrue	The shader was compiled with realtypes enabled.
		@retval SceShaderTriBoolFalse	The shader was compiled without realtypes enabled.
		@retval SceShaderTriBoolUnknown	The shader binary is too old to contain this information.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTriBool sceShaderHasRealTypes(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Returns the compile target that was used to compile this shader.

	@param[in] metadata
		A handle to the metadata section.

	@return
		The compile target that was used to compile this shader, or unknown if
		the binary file is too old to contain this information.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileTarget sceShaderGetCompileTarget(
	SceShaderMetadataSectionHandle metadata);

//////////////////////////////////////////////////////////////////////////////
// Compute shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the number of threads in the X direction for a compute shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a compute shader.

	@return
		The number of threads in the X direction.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetComputeNumThreadX(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of threads in the Y direction for a compute shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a compute shader.

	@return
		The number of threads in the Y direction.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetComputeNumThreadY(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of threads in the Z direction for a compute shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a compute shader.

	@return
		The number of threads in the Z direction.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetComputeNumThreadZ(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the <c>use_thread_dimension</c> flag for a compute shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must
		belong to a compute shader.

	@return
		Returns <c>true</c> if the shader was tagged with <c>[use_thread_dimension]</c>,
		<c>false</c> otherwise. If <c>true</c>, the dispatch dimension is specified
		as a number of lanes. If <c>false</c>, the dispatch dimension is specified as a number of thread groups.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderGetUseThreadDimension(
	SceShaderMetadataSectionHandle metadata);

//////////////////////////////////////////////////////////////////////////////
// Vertex shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the shader variant for a vertex shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a vertex shader.

	@return
		The vertex shader variant.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderVertexVariant sceShaderGetVertexVariant(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether a shader uses the <c>[OBJPRIMID]</c> attribute.

	@param[in] metadata
		A handle to the metadata section. The metadata section must
		belong to a vertex shader.

	@return
		Returns <c>true</c> if the shader uses <c>[OBJPRIMID]</c>, <c>false</c> otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderHasObjPrimId(
	SceShaderMetadataSectionHandle metadata);

//////////////////////////////////////////////////////////////////////////////
// Pixel shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Checks whether a shader can discard pixels.

	@param[in] metadata
	A handle to the metadata section. The metadata section must
	belong to a pixel shader.
	
	@return
	Returns true if the shader can discard pixels, false otherwise.
	
	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderHasDiscard(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether a shader exports depth information.

	@param[in] metadata
	A handle to the metadata section. The metadata section must
	belong to a pixel shader.
	
	@return
	Returns true if the shader exports depth, false otherwise.
	
	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderHasZWrite(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Checks whether a shader uses primitive ordered pixel shading (POPS).

	@param[in] metadata
	A handle to the metadata section. The metadata section must
	belong to a pixel shader.

	@return
	Returns true if the shader uses POPS, false otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderHasPops(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Returns the pixel export color format for the given export target.

	@param[in] metadata
	A handle to the metadata section. The metadata section must
	belong to a pixel shader.

	@param[in] index
	The index of the export target.

	@return
	Format mask.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderExportColorFmt sceShaderGetExportColorFmt(
	SceShaderMetadataSectionHandle metadata,
	uint32_t index);

/**	@brief	Gets the Z order testing strategy used by a pixel shader.

	@param[in] metadata
	A handle to the metadata section. The metadata section must
	belong to a pixel shader.

	@return
	Returns the Z order testing strategy used by the pixel shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderZOrder sceShaderGetZOrder(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Returns the conservative Z mode used by a pixel shader.

	@param[in] metadata
	A handle to the metadata section. The metadata section must
	belong to a pixel shader.

	@return
	Returns the conservative Z mode used by the pixel shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderConservativeZMode sceShaderGetConservativeZMode(
	SceShaderMetadataSectionHandle metadata);

//////////////////////////////////////////////////////////////////////////////
// Domain shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the shader variant for a domain shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a domain shader.

	@return
		The domain shader variant.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderDomainVariant sceShaderGetDomainVariant(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the patch type of a domain shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a domain shader.

	@return
		The shader patch type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderPatchType sceShaderGetDomainPatchType(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of control points consumed by a domain shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a domain shader.

	@return
		The number of control points per domain.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetDomainInputControlPointCount(
	SceShaderMetadataSectionHandle metadata);


//////////////////////////////////////////////////////////////////////////////
// Hull shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the patch type of a hull shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The patch type of the hull shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderPatchType sceShaderGetHullPatchType(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the topology type of a hull shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The topology type of the hull shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTopologyType sceShaderGetHullTopologyType(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the partitioning type of a hull shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The partitioning type of the hull shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderPartitioningType sceShaderGetHullPartitionType(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of input control points for a hull shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The number of input control points.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetHullInputControlPointCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of output control points for a hull shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The number of output control points.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetHullOutputControlPointCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the patch size of a hull shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The hull shader patch size.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetHullPatchSize(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the maximum tessellation level for hull shader patches.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a hull shader.

	@return
		The maximum tessellation level for the hull shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
float sceShaderGetHullMaxTessLevel(
	SceShaderMetadataSectionHandle metadata);


//////////////////////////////////////////////////////////////////////////////
// Geometry shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the number of instances in a geometry shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a geometry shader.

	@return
		The number of instances in the geometry shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetGsInstanceCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the maximum number of output vertices for a geometry shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a geometry shader.

	@return
		The maximum number of output vertices in the geometry shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetGsMaxVertexCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the input primitive type of a geometry shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a geometry shader.

	@return
		The type of the input primitives.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderGsPrimType sceShaderGetGsInputPrimType(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the output primitive type of a geometry shader.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a geometry shader.

	@return
		The type of the output primitives.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderGsPrimType sceShaderGetGsOutputPrimType(
	SceShaderMetadataSectionHandle metadata);


//////////////////////////////////////////////////////////////////////////////
// Function shader only metadata
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the first VGPR used in the function shader ABI.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The first VGPR used in the function shader ABI.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiVgprStart(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the first SGPR used in the function shader ABI.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The first SGPR used in the function shader ABI.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiSgprStart(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of VGPRs used in the function shader ABI.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The number of VGPRs used in the function shader ABI.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiVgprCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of SGPRs used in the function shader ABI.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The number of SGPRs used in the function shader ABI.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiSgprCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the scratch size used in the function shader ABI, in DWORDs.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The scratch size used in the function shader ABI, in DWORDs.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiScratchSizeInDwords(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the  size of the LDS used in the function shader ABI, in DWORDs.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The size of the LDS used in the function shader ABI, in DWORDs.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiLdsSizeInDwords(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of input/output VGPRs used in the function shader
			ABI.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The number of input/output VGPRs used in the function shader ABI.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiVgprInOutCount(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the number of input-only VGPRs used in the function shader
			ABI.

	@param[in] metadata
		A handle to the metadata section. The metadata section must 
		belong to a function shader.

	@return
		The number of input-only VGPRs used in the function shader ABI.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetFunctionAbiVgprInCount(
	SceShaderMetadataSectionHandle metadata);

//////////////////////////////////////////////////////////////////////////////
// Complete reflection interface
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets a handle to the resource list.

	@param[in] metadata
		A handle to the metadata section.

	@return
		A handle to the list of resources.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderResourceListHandle sceShaderGetResourceList(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets the first resource handle in a list of resource handles.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resourcelist
		A handle to the list of resources.

	@return
		A handle to the first resource in the list

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderResourceHandle sceShaderGetFirstResource(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceListHandle resourcelist);

/**	@brief	Gets the next resource in a list.

	A handle to the current resource is specified as an input parameter. If the current 
	resource is the last in the list, this function returns null.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to the current resource in the list.

	@return
		A handle to the next resource in the list. Null if 
		the current resource is the last resource in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderResourceHandle sceShaderGetNextResource(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the resource class for the supplied resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The resource class.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderResourceClass sceShaderGetResourceClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the resource name from a resource handle.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The resource name.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetResourceName(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the API slot for a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The API slot for the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetResourceApiSlot(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the underlying type for a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The PSSL type of the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTypeHandle sceShaderGetResourceType(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the shader semantic name for a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The name of the shader semantic for the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetSemanticName(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the semantic index of a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The semantic index of the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetSemanticIndex(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the interpolation class of a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource, which must be an input to a pixel shader.

	@return
		Returns the interpolation class of the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderInterpolationClass sceShaderGetInterpolationClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the compression mode of a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource, which must be an input to a pixel shader.

	@return
		Returns the compression mode.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompressionMode sceShaderGetCompressionMode(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the offset of a pixel input or vertex output resource in the parameter space, in bytes.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource which must be a pixel input or
		a vertex output with a user semantic.

	@return
		Returns the offset of the resource in bytes, or -1 if the resource
		is not allocated in the parameter cache.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int sceShaderGetParameterSpaceOffset(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

/**	@brief	Gets the shader semantic class of a resource.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to a resource.

	@return
		The shader semantic class of the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderSemantic sceShaderGetSemanticClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resource);

//////////////////////////////////////////////////////////////////////////////
// Annotation interface
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets a handle to the annotation list.

	@param[in] metadata
		A handle to the metadata section.

	@return
		Returns a handle to the list of annotations for the shader. Returns null if the shader has no annotations.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderAnnotationListHandle sceShaderGetAnnotationList(
	SceShaderMetadataSectionHandle metadata);

/**	@brief	Gets a handle to the first annotation in a list.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] annotationlist
		A handle to the list of annotations.

	@return
		Returns a handle to the first annotation in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderAnnotationHandle sceShaderGetFirstAnnotation(
	SceShaderMetadataSectionHandle metadata,
	SceShaderAnnotationListHandle annotationlist);

/**	@brief	Gets a handle to the next annotation in a list.

	A handle to the current annotation is specified as an input parameter. If the current
	annotation is the last in the list, this function returns null.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to the current annotation in the list.

	@return
		Returns a handle to the next annotation in the list. Returns null if
		the current annotation is the last in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderAnnotationHandle sceShaderGetNextAnnotation(
	SceShaderMetadataSectionHandle metadata,
	SceShaderAnnotationHandle resource);

/**	@brief	Gets the annotation value.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] resource
		A handle to the annotation.

	@return
		Returns the value of the specified annotation.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const* sceShaderGetAnnotationValue(
	SceShaderMetadataSectionHandle metadata,
	SceShaderAnnotationHandle resource);

//////////////////////////////////////////////////////////////////////////////
// Types
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the class of a specified type.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type.

	@return
		The class of the specified type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTypeClass sceShaderGetTypeClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the name of a specified type.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type.

	@return
		The name of the specified type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetTypeName(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);


//////////////////////////////////////////////////////////////////////////////
// Numeric types (scalars, vectors, and matrices)
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the class of a numeric type.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a numeric type.

	@return
		The class of the numeric type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderNumericClass sceShaderGetNumericClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the number of columns in a matrix type or the number of coefficients in a vector type.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a matrix or vector type.

	@return
		The number of columns in the matrix type or the number of coefficients in a vector type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetColumns(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the number of rows in a matrix type.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a matrix type.

	@return
		The number of rows in the matrix type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetRows(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Checks whether a matrix is row major.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a matrix type.

	@return
		Returns true if the matrix type is row major, false if column major.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderIsRowMajor(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);


//////////////////////////////////////////////////////////////////////////////
// Array types
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the number of elements in an array.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be an array.

	@return
		The number of elements in the array.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetArraySize(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the element type of an array.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be an array.

	@return
		The type of the elements in the array.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTypeHandle sceShaderGetArrayElement(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);


//////////////////////////////////////////////////////////////////////////////
// Pointer types
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the type of the element pointed to by the input type.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a pointer type.

	@return
		The type of the element pointed to by type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTypeHandle sceShaderGetPointerElement(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);


//////////////////////////////////////////////////////////////////////////////
// Structure types
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the size of a structure, in bytes.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a <c>struct</c> type.

	@return
		The size of the structure, in bytes.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetStructSize(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the first member of a structure.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL type. Must be a structure type.

	@return
		A handle to the first member of the structure.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderMemberHandle sceShaderGetFirstMember(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the next member in a structure.

	A handle to the current member is specified as an input parameter. 
	If the current member is the last in the structure, 
	this function returns null.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to the current member in the structure.

	@return
		A handle to the next member in the structure.
		Null if the parameter member is the last member.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderMemberHandle sceShaderGetNextMember(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Checks whether a member of a structure is used in the shader.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		Returns true if the member is used, false otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderIsMemberUsed(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the name of a member of a structure.

	For example, for variable <c>int a;</c>, the name is <c>a</c>.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The name of the member.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetMemberName(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the member offset within the structure, in bytes.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The member offset within the structure, in bytes.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetMemberOffset(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the size of a member of a structure, in bytes.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The size of the member, in bytes.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetMemberSize(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the type of a member of a structure.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The PSSL type of the structure member.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTypeHandle sceShaderGetMemberType(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Checks whether the member was created from a resource specified in an <c>SrtSignature</c>.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		True if the member was a resource specified in an <c>SrtSignature</c>.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderIsSrtSignatureResource(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the index of the original API slot for a resource created from an <c>SrtSignature</c>.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The index of the original API slot.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetSrtSignatureResourceSlotIndex(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the index of the original space keyword for a resource created from an <c>SrtSignature</c>.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The index of the original space keyword.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
int32_t sceShaderGetSrtSignatureResourceSpaceIndex(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the original name of a resource created from an <c>SrtSignature</c>.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The original resource name.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetSrtSignatureResourceName(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the original class for a resource created from an <c>SrtSignature</c>.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The original resource class.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderResourceClass sceShaderGetSrtSignatureResourceClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

//////////////////////////////////////////////////////////////////////////////
// Default values
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the default value of a member of a structure.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		A pointer to the default value of the member.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderOpaque sceShaderGetDefaultValue(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);

/**	@brief	Gets the default size of a member of a structure.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] member
		A handle to a structure member.

	@return
		The default size of the structure member.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint64_t sceShaderGetDefaultValueSize(
	SceShaderMetadataSectionHandle metadata,
	SceShaderMemberHandle member);


//////////////////////////////////////////////////////////////////////////////
// Textures and buffers
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the class of the given buffer or texture object.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL buffer or texture type.

	@return
		The class of the buffer or texture type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderBufferClass sceShaderGetBufferClass(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Checks whether a buffer or texture is writeable.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL buffer or texture type.

	@return
		Returns true if the buffer or texture is writeable, false if it is read-only.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderIsRwBuffer(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the type of the elements in a buffer.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL buffer or texture type.

	@return
		Returns the type of the buffer elements.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderTypeHandle sceShaderGetBufferElementType(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	Gets the encoding of the given buffer.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL buffer.

	@return
		The encoding used for the buffer.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderBufferEncoding sceShaderGetBufferEncoding(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	When encoding is SizePtr64, gets the address shift of the given buffer.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL buffer.

	@return
		The address shift.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetBufferAddrShift(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

/**	@brief	When encoding is SizePtr64, gets the size shift of the given buffer.

	@param[in] metadata
		A handle to the metadata section.

	@param[in] type
		A handle to a PSSL buffer.

	@return
		The size shift.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetBufferSizeShift(
	SceShaderMetadataSectionHandle metadata,
	SceShaderTypeHandle type);

///////////////////////////////////////////////////////////////////////////////
// Agcl - Shader library functions
///////////////////////////////////////////////////////////////////////////////
// All routines to access shader files inside a shader library
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Represents a handle to a shader library
	file.

	@ingroup shader_reflection
*/
typedef struct _SceShaderLibraryBinaryHandle const* SceShaderLibraryBinaryHandle;

/**	@brief Creates a handle to a library file in memory.

	@param[in] data
		Pointer to the library file data.

	@return
		Returns a handle to the library.

	@ingroup shader_reflection
 */
SCE_SHADER_INTERFACE 
SceShaderLibraryBinaryHandle sceShaderGetLibraryBinaryHandle(void const* data);

/**	@brief	Gets the number of shaders in a library.

	@param[in] libraryHandle
		A handle to the library file.

	@return
		Returns the number of shaders in the library.

	@ingroup shader_reflection
*/

SCE_SHADER_INTERFACE 
uint64_t sceShaderGetLibraryShaderCount(
	SceShaderLibraryBinaryHandle libraryHandle);

/**	@brief	Gets the name of a shader in a library.

	@param[in] libraryHandle
	A handle to the library file.

	@param[in] index
	The index of the shader.

	@return
	Returns the name of the shader as a null terminated string.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const *sceShaderGetLibraryShaderName(
	SceShaderLibraryBinaryHandle libraryHandle,
	uint32_t index);

/**	@brief	Gets the data of a shader in a library.

	@param[in] libraryHandle
	A handle to the library file.

	@param[in] index
	The index of the shader.

	@return
	Returns the shader data. This pointer can be 
	used as an input parameter to sceShaderGetBinaryHandle().

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
void const* sceShaderGetLibraryShaderData(
	SceShaderLibraryBinaryHandle libraryHandle,
	uint32_t index);

/**	@brief	Gets the size of a shader in a library.

	@param[in] libraryHandle
	A handle to the library file.

	@param[in] index
	The index of the shader.

	@return
	Returns the size of the data pointed to by the return value of
	sceShaderGetLibraryShaderData().

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetLibraryShaderSize(
	SceShaderLibraryBinaryHandle libraryHandle,
	uint32_t index);

//////////////////////////////////////////////////////////////////////////////
// Compiler Options
//////////////////////////////////////////////////////////////////////////////
// All routines to get every option used by the compiler to compile this
// shader. The compiler options are only present in the shader binary file if -g 
// has been used to compile it. In such a case, a section called <c>.options</c>
// is present and can be parsed by the following routines
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Represents a handle to the <c>.options</c> section in the shader binary 
	file.

	This section is present only when <c>-g</c> was used to compile the shader
	binary file.

	@ingroup shader_reflection
*/
typedef struct _SceShaderCompileOptionSectionHandle const * SceShaderCompileOptionSectionHandle;

/**	@brief	Represents a list of compiler options.

	Use:
	- sceShaderGetFirstCompileOption() to get the first option in the list.
	- sceShaderGetNextCompileOption() to get the next option in the list.
	  
	@see SceShaderCompileOptionListHandle

	@ingroup shader_reflection
*/
typedef struct _SceShaderCompileOptionListHandle const * SceShaderCompileOptionListHandle;

/**	@brief	Represents a single compiler option. Each option has a name and a value.

	Use:
	- sceShaderGetCompileOptionName() to get the option name
	- sceShaderGetCompileOptionValue() to get the option value

	@ingroup shader_reflection
*/
typedef struct _SceShaderCompileOptionHandle const * SceShaderCompileOptionHandle;

/**	@brief	Gets the size in bytes of the <c>.options</c> section in the shader
	binary file.

	Returns 0 if the section is not present in the shader binary file.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		Returns the size of the compile option section in the shader binary file.
		Returns 0 if the shader binary file
		does not contain an <c>.options</c> section.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint64_t sceShaderGetCompileOptionSectionSize(
	SceShaderBinaryHandle shaderBinary);

/**	@brief	Decodes and returns a handle to the <c>.options</c> section in the
	shader binary file.

	Returns null if the section is not present.

	@param[in] dst
		A buffer that the decoded section is written to.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		Returns a handle to the compile option section in the shader binary file.
		Returns null if the shader binary file
		does not contain an <c>.options</c> section.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileOptionSectionHandle sceShaderDecodeCompileOptionSection(
	void *dst,
	SceShaderBinaryHandle shaderBinary);

/**	@brief	Gets the top-level list of options used by the compiler.

	When using normal compilation, only one list of options exists. Pipeline
	compilation creates a two-level hierarchy of options where
	the second level contains the options used for each stage.

	@param[in] shaderBinary
		A handle to the shader file.

	@param[in] compileOptionSection
		A handle to the compiler options section in the shader binary file.

	@return
		A handle to the list of compiler options.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileOptionListHandle sceShaderGetCompileOptionList(
	SceShaderBinaryHandle shaderBinary,
	SceShaderCompileOptionSectionHandle compileOptionSection);

/**	@brief	Gets the first compiler option in a list.

	To get the next one, use sceShaderGetNextCompileOption().

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOptionList
		A handle to a list of compiler options.

	@return
		A handle to the first compiler option in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileOptionHandle sceShaderGetFirstCompileOption(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionListHandle compileOptionList);

/**	@brief	Gets the next compiler option in a list.

	A handle to the current compiler option is specified as an input 
	parameter. If the current compiler option is the last in the list, 
	this function returns null.

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOption
		A handle to the current compiler option.

	@return
		A handle to the next compiler option in the list. Null if 
		the current compiler option is the last option in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileOptionHandle sceShaderGetNextCompileOption(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionHandle compileOption);

/**	@brief	Gets the name of a compiler option.

	For example, for the option O3, the name is <c>optimizationlevel</c>.

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOption
		A handle to a compiler option.

	@return
		The name of the compiler option.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetCompileOptionName(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionHandle compileOption);

/**	@brief	Gets the value set for a compiler option.

	For example, for the option O3, the value is <c>3</c>.

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOption
		A handle to a compiler option.

	@return
		The string formatted value of the compiler option.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetCompileOptionValue(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionHandle compileOption);

/**	@brief	Gets the compiler options for the first stage in a pipeline.

	If the compiler option <c>pipeline</c> is specified, a two-level 
	hierarchy compiler options is created. The second level 
	contains a list of compiler options for each pipeline stage.

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOption
		A handle to the compiler option.

	@return
		The list of options used to compile the first pipeline stage.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileOptionListHandle sceShaderGetFirstPipelineStage(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionHandle compileOption);

/**	@brief	Gets the list of options used for the next stage in the pipeline.

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOptionList
		A handle to the list of compiler options to traverse.

	@return
		The list of options used to compile the next pipeline stage. It is null
		if the list of options passed as parameter is the last in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderCompileOptionListHandle sceShaderGetNextPipelineStage(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionListHandle compileOptionList);

/**	@brief	Gets the shader stage name for the given list of compiler options.

	This is for use with pipeline shader compilation. When shaders are compiled as
	part of a pipeline, a two-level compiler option list is created. The top-level 
	list represents the pipeline as a whole. It does not have an associated shader 
	stage name. The second level of compiler options contains a list of options 
	for each stage in the pipeline.

	@param[in] compileOptionSection
		A handle to the compiler option section in the shader binary file.

	@param[in] compileOptionList
		A handle to the list of compiler options to traverse.

	@return
		Pipeline stage name.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetPipelineStageName(
	SceShaderCompileOptionSectionHandle compileOptionSection,
	SceShaderCompileOptionListHandle compileOptionList);

/**	@brief	Gets a null-terminated string that contains the command line
			arguments used to compile this shader.

	The string only contains command line arguments that are not set to their
	default values. The compiler stores the command line arguments in the .agsd
	file. This function returns a null pointer if the .agsd file is
	too old or if the shader was not compiled with debug information.

	@param[in] shaderBinary
		A handle to the shader binary.

	@return
		A string containing the command line arguments, or null.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const* sceShaderGetCmdLineArgs(
	SceShaderBinaryHandle shaderBinary);

//////////////////////////////////////////////////////////////////////////////
// Source Files
//////////////////////////////////////////////////////////////////////////////
// All routines to get every source file needed to compile the shader
// The source files are only present in the shader binary file if -g has been
// used to compile it. In such a case, a section called <c>.sources</c> is
// present and can be parsed by the following routines
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Represents a handle to the <c>.sources</c> section in the shader
	binary file.

	This section is present only when <c>-g</c> was used to compile the shader 
	binary file.

	@ingroup shader_reflection
*/
typedef struct _SceSourceFileSectionHandle const * SceShaderSourceFileSectionHandle;

/**	@brief	Represents a list of source files.

	Use:
	- sceShaderGetFirstSourceFile() to get the first source file in the list.
	- sceShaderGetNextSourceFile() to get the next source file in the list.

	@ingroup shader_reflection
*/
typedef struct _SceSourceFileListHandle const * SceShaderSourceFileListHandle;

/**	@brief	Represents a single source file.

	Use:
	- sceShaderGetSourceFileUserName() to get the name of a file as given by the
	  user.
	- sceShaderGetSourceFileAbsolutePath() to get the absolute path to a file.
	- sceShaderGetSourceFileContent() to get the actual content of a file.
	- sceShaderGetSourceFileDuplicateId() to get the ID of the file that actually owns
	  the content pointed to by this file.
	- sceShaderIsSourceFilePrimary() to check if a file is a primary file.
	- sceShaderHasSourceFileContent() to check if a file has actual content. This function
	  returns <c>true</c> if sceShaderGetSourceFileContent() returns a non-null pointer.

	@ingroup shader_reflection
*/
typedef struct _SceShaderSourceFileHandle const * SceShaderSourceFileHandle;

/**	@brief	Gets the size in bytes of the <c>.sources</c> section in the shader
	binary file.

	Returns 0 if the section is not present in the shader binary file.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		Returns the size of the source file section in the shader binary file.
		Returns 0 if the shader binary file
		does not contain a <c>.sources</c> section.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint64_t sceShaderGetSourceFileSectionSize(
	SceShaderBinaryHandle shaderBinary);

/**	@brief	Decodes and returns a handle to the <c>.sources</c> section in the
	shader binary file.

	Returns null if the section is not present.

	@param[in] dst
		A buffer that the decoded section is written to.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		Returns a handle to the source file section in the shader binary file.
		Returns null if the shader binary file
		does not contain a <c>.sources</c> section.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderSourceFileSectionHandle sceShaderDecodeSourceFileSection(
	void *dst,
	SceShaderBinaryHandle shaderBinary);

/**	@brief	Gets a list of the source files used by the compiler.

	@param[in] shaderBinary
		A handle to the shader file.

	@param[in] sourceFileSection
		A handle to the source file section.

	@return
		A handle to the list of source files.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderSourceFileListHandle sceShaderGetSourceFileList(
	SceShaderBinaryHandle shaderBinary,
	SceShaderSourceFileSectionHandle sourceFileSection);

/**	@brief	Gets the first source file handle in a list of source files.

	Use sceShaderGetNextSourceFile() to iterate over the list.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFileList
		A list of source file handles.

	@return
		The first source file handle in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderSourceFileHandle sceShaderGetFirstSourceFile(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileListHandle sourceFileList);

/**	@brief	Gets the next source file in a list.

	A handle to the current source file is specified as an input parameter. 
	If the current source file is the last in the list, this function returns null.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to the current file in the list.

	@return
		A handle to the next source file in the list. 
		Null if the current file is the last file in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
SceShaderSourceFileHandle sceShaderGetNextSourceFile(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Gets the name of a file as given by the user when compiling the shader.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file in the source file list.

	@return
		The name of the file as given by the user when compiling the shader.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetSourceFileUserName(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Gets the absolute path for a file.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file.

	@return
		The absolute path of the source file.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetSourceFileAbsolutePath(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Gets the actual content of a file, if any content exists.

	If none, returns null.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file.

	@return
		The content of the file, if any exists. Null if no content exists.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetSourceFileContent(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Gets the ID of a source file.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file.

	@return
		The ID of the source file.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetSourceFileId(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Gets the ID of the file that the specified file is duplicated from.

	Returns 0 if not a duplicate file.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file.

	@return
		For duplicate files, returns the ID of the file it is duplicated from. Returns 
		0 if this is not a duplicate file.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
uint32_t sceShaderGetSourceFileDuplicateId(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Checks whether a source file is primary.
 
	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file.

	@return
		Returns true if the specified source file is a primary file, false otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderIsSourceFilePrimary(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);

/**	@brief	Checks whether a source file has any content.

	If so, sceShaderGetSourceFileContent() returns a non-null value.

	@param[in] sourceFileSection
		A handle to the source file section.

	@param[in] sourceFile
		A handle to a source file.

	@return
		Returns true if the file has any content, false otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
bool sceShaderHasSourceFileContent(
	SceShaderSourceFileSectionHandle sourceFileSection,
	SceShaderSourceFileHandle sourceFile);


//////////////////////////////////////////////////////////////////////////////
// Control flow graph
//////////////////////////////////////////////////////////////////////////////
// We encode complete control flow graph information (including basic blocks and
// functions when [noinline] is used).
//////////////////////////////////////////////////////////////////////////////

/**	@brief	Represents a handle to the <c>.cfg</c> section of the shader binary file.

	@ingroup shader_reflection
*/
typedef struct _SceShaderCfgSectionHandle const * SceShaderCfgSectionHandle;

/**	@brief	Represents the entry point for all program nodes in the shader.

	@ingroup shader_reflection
*/
typedef struct _SceShaderProgramListHandle const * SceShaderProgramListHandle;

/**	@brief	Represents one program in the shader.

	@ingroup shader_reflection
*/
typedef struct _SceShaderProgramHandle const * SceShaderProgramHandle;

/**	@brief	Represents a list of basic blocks.

	@ingroup shader_reflection
*/
typedef struct _SceShaderBasicBlockListHandle const * SceShaderBasicBlockListHandle;

/**	@brief	Represents one basic block.

	@ingroup shader_reflection
*/
typedef struct _SceShaderBasicBlockHandle const * SceShaderBasicBlockHandle;

/**	@brief	Represents a reference to one basic block.

	@ingroup shader_reflection
*/
typedef struct _SceShaderBasicBlockRefHandle const * SceShaderBasicBlockRefHandle;

/**	@brief	Represents a list of basic block references.

	@ingroup shader_reflection
*/
typedef struct _SceShaderBasicBlockRefListHandle const * SceShaderBasicBlockRefListHandle;

/**	@brief	Represents a list of functions.

	@ingroup shader_reflection
*/
typedef struct _SceShaderFunctionListHandle const * SceShaderFunctionListHandle;

/**	@brief	Represents one function.

	@ingroup shader_reflection
*/
typedef struct _SceShaderFunctionHandle const * SceShaderFunctionHandle;

/**	@brief	Represents an encoded range of unknown size

	Used by <c>movrel</c> instructions.

	@ingroup shader_reflection
*/
typedef struct _SceShaderOperandRange
{
	uint32_t m_offset;			///< Offset
	uint16_t m_operandIndex;	///< Operand index
	uint16_t m_rangeSize;		///< Range Size
} SceShaderOperandRange;

/**	@brief	Gets a handle to the .cfg section.

	@param[in] shaderBinary
		A handle to the shader file.

	@return
		A handle to the .cfg section in the shader binary file.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderCfgSectionHandle
sceShaderGetCfgSection(
	SceShaderBinaryHandle shaderBinary);

/**	@brief	Gets the list of programs from the .cfg section.

	@param[in] cfgSection
		A handle to the .cfg section.

	@return
		The list of programs in the .cfg section.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderProgramListHandle
sceShaderGetProgramList(
	SceShaderCfgSectionHandle cfgSection);

/**	@brief	Gets the first program in a list of programs.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] programList
		A list of programs. Usually the list of programs in the .cfg section.

	@return
		A handle to the first program in the program list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderProgramHandle
sceShaderGetFirstProgram(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderProgramListHandle programList);

/**	@brief	Gets the number of operand ranges to handle in the program.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] program
		A handle to the program.

	@return
		The number of operand ranges to handle in the program.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE uint32_t
sceShaderGetRangeNumber(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderProgramHandle program);

/**	@brief	Gets the operand ranges to handle in the program.

	Returns null if there is no range.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] program
		A handle to the program.

	@return
		An array of operand ranges, or null if there is no range.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderOperandRange const *
sceShaderGetRangeList(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderProgramHandle program);

/**	@brief	Gets the next program in a list.

	A handle to the current program is specified as an input parameter. If the current 
	program is the last in the list, this function returns null.
	
	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] program
		A handle to the current program.

	@return
		A handle to the next program in the .cfg section. Null if the current 
		program is the last in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderProgramHandle
sceShaderGetNextProgram(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderProgramHandle program);

/**	@brief	Gets a list of the basic blocks in a program.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] program
		A handle to a program.

	@return
		A list of the basic blocks in the program.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockListHandle
sceShaderGetBasicBlockList(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderProgramHandle program);

/**	@brief	Gets the first block in a list of basic blocks.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlockList
		A list of basic blocks.

	@return
		The first basic block in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockHandle
sceShaderGetFirstBasicBlock(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockListHandle basicBlockList);

/**	@brief	Gets the next basic block in the list.

	A handle to the current basic block is specified as an input parameter. If the 
	current basic block is the last in the list, this function returns null.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		A handle to the next basic block in the list. Null if 
		the current basic block is the last in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockHandle
sceShaderGetNextBasicBlock(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Gets the list of successors in the SGPR control flow for the given basic block.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		The successors of the basic block as a list of basic block references.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockRefListHandle
sceShaderGetBasicBlockSuccessorList(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Gets the list of successors in the VGPR control flow for the given basic block.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		The successors of the basic block as a list of basic block references.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockRefListHandle
sceShaderGetBasicBlockVgprSuccessorList(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Gets the first reference in a list of basic block references.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlockRefList
		A list of basic block references.

	@return
		The first basic block reference in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockRefHandle
sceShaderGetFirstBasicBlockRef(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockRefListHandle basicBlockRefList);

/**	@brief	Gets the next basic block reference in a list.

	Returns null if the current block is the last block in the list.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlockRef
		A handle to the current block in the list.

	@return
		The next block in the list. Returns null if the current block 
		is the last in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockRefHandle
sceShaderGetNextBasicBlockRef(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockRefHandle basicBlockRef);

/**	@brief	Gets the basic block pointed to by a block reference.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlockRef
		A handle to a basic block reference.

	@return
		A handle to the basic block indicated by the reference.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockHandle
sceShaderGetBasicBlock(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockRefHandle basicBlockRef);

/**	@brief	Gets the start offset for a basic block, in bytes.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		The offset, in bytes, for the start of the basic block.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE uint32_t
sceShaderGetBasicBlockStartOffset(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Gets the end offset for a basic block, in bytes.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to the basic block.

	@return
		The offset, in bytes, for the end of the basic block.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE uint32_t
sceShaderGetBasicBlockEndOffset(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Gets the nesting level of a basic block.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		The nesting level of the basic block.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE uint32_t
sceShaderGetBasicBlockNestingLevel(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Checks whether a basic block is on a uniform control path.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		Returns true if the block is on a uniform control path, false otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE bool
sceShaderIsBasicBlockUniform(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Checks whether a basic block ignores the exec mask for writes.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		Returns <c>true</c> if the block ignores exec mask for writes, <c>false</c> otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE bool
sceShaderBasicBlockOverrideExecForWrites(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Checks whether a basic block ignores the exec mask for reads.
	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] basicBlock
		A handle to a basic block.

	@return
		Returns <c>true</c> if the block ignores exec mask for reads, <c>false</c> otherwise.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE bool
sceShaderBasicBlockOverrideExecForReads(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderBasicBlockHandle basicBlock);

/**	@brief	Gets a list of the functions in a program.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] program
		A handle to a program.

	@return
		A list of handles to the functions in the program.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderFunctionListHandle
sceShaderGetFunctionList(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderProgramHandle program);

/**	@brief	Gets the first function in a list.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] functionList
		A list of the functions in a program.

	@return
		A handle to the first function in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderFunctionHandle
sceShaderGetFirstFunction(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderFunctionListHandle functionList);

/**	@brief	Gets the next function in a list.

	A handle to the current function is specified as an input parameter. If the current 
	function is the last in the list, returns null.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] function
		A handle to the current function in the list.

	@return
		A handle to the next function in the list. Null if 
		the current function is the last function in the list.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderFunctionHandle
sceShaderGetNextFunction(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderFunctionHandle function);

/**	@brief	Gets the first basic block of a function.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] function
		A handle to a function.

	@return
		A handle to the first basic block of the function.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockHandle
sceShaderGetFunctionFirstBlock(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderFunctionHandle function);

/**	@brief	Gets the last basic block of a function.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] function
		A handle to a function.

	@return
		A handle to the last basic block of the function.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE SceShaderBasicBlockHandle
sceShaderGetFunctionLastBlock(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderFunctionHandle function);

/**	@brief	Gets the mangled function name.

	@param[in] cfgSection
		A handle to the .cfg section.

	@param[in] function
		A handle to a function.

	@return
		The function name.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE
char const * sceShaderGetFunctionName(
	SceShaderCfgSectionHandle cfgSection,
	SceShaderFunctionHandle function);


///////////////////////////////////////////////////////////////////////////////
// Stringification utility functions
///////////////////////////////////////////////////////////////////////////////
// Enum to string conversions
///////////////////////////////////////////////////////////////////////////////

/**	@brief	Gets the name of a shader profile.

	@param[in] profile
		A shader profile.

	@return
		The name of the shader profile.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetProfileName(SceShaderProfile profile);

/**	@brief	Gets the name of a vertex shader variant.

	@param[in] variant
		A vertex shader variant.

	@return
		The name of the vertex shader variant.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetVertexVariantName(SceShaderVertexVariant variant);

/**	@brief	Gets the name of a domain shader variant.

	@param[in] variant
		A domain shader variant.

	@return
		The name of the domain shader variant.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetDomainVariantName(SceShaderDomainVariant variant);

/**	@brief	Gets the name of a geometry shader primitive type.

	@param[in] type
		A geometry shader primitive type.

	@return
		The name of the geometry shader primitive type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetGsPrimTypeName(SceShaderGsPrimType type);

/**	@brief	Gets the name of a patch type.

	@param[in] type
		A patch type.

	@return
		The name of the patch type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetPatchTypeName(SceShaderPatchType type);

/**	@brief	Gets the name of a topology type.

	@param[in] type
		A topology type.

	@return
		The name of the topology type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetTopologyTypeName(SceShaderTopologyType type);

/**	@brief	Gets the name of a partitioning type.

	@param[in] type
		A partitioning type.

	@return
		The name of the partitioning type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetPartitionTypeName(SceShaderPartitioningType type);

/**	@brief	Gets the name of a PSSL numeric type.

	@param[in] klass
		A numeric class.

	@return
		The name of the PSSL numeric type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetNumericClassName(SceShaderNumericClass klass);

/**	@brief	Gets the name of a buffer type.

	@param[in] klass
		A buffer class.

	@return
		The name of the buffer type.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetBufferClassName(SceShaderBufferClass klass);

/**	@brief	Gets the resource name for a resource class.

	@param[in] klass
		A resource class.

	@return
		The name of the resource.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetResourceClassName(SceShaderResourceClass klass);

/**	@brief	Gets the name of a shader semantic.

	@param[in] semantic
		A shader semantic.

	@return
		The name of the shader semantic.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const *sceShaderGetSemanticClassName(SceShaderSemantic semantic);

/**	@brief	Gets the name of an export color format.

	@param[in] format
		The export color format.

	@return
		The name of the export color format.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const* sceShaderGetExportColorFmtName(SceShaderExportColorFmt format);

/**	@brief	Gets the name of an interpolation class.

	@param[in] interpolationClass
		The interpolation class.

	@return
		Returns the name of the interpolation class.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const* sceShaderGetInterpolationClassName(SceShaderInterpolationClass interpolationClass);

/**	@brief	Gets the name of a compression mode.

	@param[in] mode
		The compression mode.

	@return
		Returns the name of the compression mode.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const* sceShaderGetCompressionModeName(SceShaderCompressionMode mode);

/**	@brief	Gets the name of a Z order testing strategy.

	@param[in] zorder
		The Z order testing strategy.

	@return
		Returns the name of the Z order testing strategy.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const* sceShaderGetZOrderName(SceShaderZOrder zorder);

/**	@brief	Gets the name of a conservative Z mode.

	@param[in] mode
		The conservative Z mode.

	@return
		Returns the name of the conservative Z mode.

	@ingroup shader_reflection
*/
SCE_SHADER_INTERFACE char const* sceShaderGetConservativeZModeName(SceShaderConservativeZMode mode);

#ifdef	__cplusplus
}
#endif	/* __cplusplus */

#endif


// ------------------------------------------------------------------------------------------------/
/// -- implementation --
// ------------------------------------------------------------------------------------------------/

#ifndef __LIB_SCE_SHADER_IMPL__
#define __LIB_SCE_SHADER_IMPL__
#if !defined(SCE_SL_INLINE)
#define SCE_SL_INLINE
#endif

#define SCE_SHADER_EXPORTS
#include <cstddef>
#include <cstring>
#include <cstdint>


#include <stdint.h>

namespace sce
{
namespace sl
{

// ------------------------------------------------------------------------------------------------/
/// SL
#if !defined(DOXYGEN_IGNORE)
#if defined(SCE_SL_INLINE)
#	define SCE_SL_INTERFACE inline
#else
#	define SCE_SL_INTERFACE
#endif /* SCE_SL_INLINE */
#endif /* DOXYGEN_IGNORE */


// ------------------------------------------------------------------------------------------------/
/// attribute form
enum class AttrForm
{
	kEof = 0,
	kULeb,							//< unsigned integer
	kSLeb,							//< signed integer
	kString,						//< offset to 0-terminated string
	kNodeRef,						//< offset to another node
	kBool,							//< true on 'presence'
	kDataBlock,						//< offset/size to generic binary data
	kFp32,							//< 32 bit floating point
	kFormCount
};
static_assert(int(AttrForm::kFormCount) <= 16, "too many forms");

// ------------------------------------------------------------------------------------------------/
/// default 4cc ("sl00")
auto constexpr kDefault4cc = 0x30306C73;

// ------------------------------------------------------------------------------------------------/
/// usefull typedefs 
using NodeHandle	= uint32_t;		//< an offset inside the binary blob
using NodeTag		= uint32_t;		//< a node tag is domain specific
using AttrTag		= uint32_t;		//< attribute tag is domain specific
using AttrHandle	= uint64_t;		//< a double offset (point to abbreviation and to data)

// ------------------------------------------------------------------------------------------------/
/// header access
SCE_SL_INTERFACE uint32_t GetLibraryFourCC(void const *base);
SCE_SL_INTERFACE uint32_t GetLibrarySize(void const *base);
SCE_SL_INTERFACE NodeHandle GetRootNode(void const *base);

// ------------------------------------------------------------------------------------------------/
/// node traversal
SCE_SL_INTERFACE NodeTag GetNodeTag(void const *base, NodeHandle node);
SCE_SL_INTERFACE NodeHandle GetFirstChildNode(void const *base, NodeHandle node);
SCE_SL_INTERFACE NodeHandle GetSiblingNode(void const *base, NodeHandle node);

// ------------------------------------------------------------------------------------------------/
/// attribute traversal
SCE_SL_INTERFACE AttrHandle GetFirstAttribute(void const *base, NodeHandle node);
SCE_SL_INTERFACE AttrHandle GetNextAttribute(void const *base, AttrHandle attr);

// ------------------------------------------------------------------------------------------------/
/// attribute data decoding
SCE_SL_INTERFACE AttrTag GetAttributeTag(void const *base, AttrHandle attr);
SCE_SL_INTERFACE AttrForm GetAttributeForm(void const *base, AttrHandle attr);

SCE_SL_INTERFACE uint64_t GetUint64Attribute(void const *base, AttrHandle attr);
SCE_SL_INTERFACE int64_t GetInt64Attribute(void const *base, AttrHandle attr);
SCE_SL_INTERFACE float GetFloatAttribute(void const *base, AttrHandle attr);
SCE_SL_INTERFACE char const *GetStringAttribute(void const *base, AttrHandle attr);
SCE_SL_INTERFACE NodeHandle GetNodeRefAttribute(void const *base, AttrHandle attr);
SCE_SL_INTERFACE void const *GetDataBlockAttribute(void const *base, AttrHandle attr);
SCE_SL_INTERFACE uint64_t GetDataBlockAttributeSize(void const *base, AttrHandle attr);

// ------------------------------------------------------------------------------------------------/
/// utils
SCE_SL_INTERFACE NodeHandle GetFirstChildNodeWithTag(void const *base, NodeHandle node, NodeTag nodeTag);
SCE_SL_INTERFACE NodeHandle GetSiblingNodeWithTag(void const *base, NodeHandle node, NodeTag nodeTag);
SCE_SL_INTERFACE AttrHandle GetFirstAttributeWithTag(void const *base, NodeHandle node, AttrTag attrTag);
SCE_SL_INTERFACE AttrHandle GetNextAttributeWithTag(void const *base, AttrHandle attr, AttrTag attrTag);
SCE_SL_INTERFACE bool ValidateShaderBinary(void const* data, uint64_t sizeInBytes, uint64_t metadataOffestInBytes, uint64_t footerOffsetinBytes, uint32_t expectedMetadataByteSize, uint32_t expectedCrc);
SCE_SL_INTERFACE uint32_t Crc32(const void* data, uint64_t length);

}
}



#include <stdint.h>

namespace sce
{
namespace sl
{

// ------------------------------------------------------------------------------------------------/
/// node tags
namespace NodeTags
{
#	define SL_NODE(NAME,VALUE)uint32_t const k ## NAME = VALUE;

/// SL_NODE(NAME, VALUE)


SL_NODE(ShaderLibrary,				0x01)
SL_NODE(Shader,						0x02)
SL_NODE(Reflection,					0x03)
SL_NODE(InputAttribute,				0x04)
SL_NODE(OutputAttribute,			0x05)
SL_NODE(Srt,						0x06)
SL_NODE(ConstantBuffer,				0x07)
SL_NODE(SamplerState,				0x08)
SL_NODE(Uav,						0x09)
SL_NODE(StreamOut,					0x0a)
//SL_NODE(Buffers,					0x0b)
//SL_NODE(Element,					0x10)
SL_NODE(PointerType,				0x11)
SL_NODE(ArrayType,					0x12)
SL_NODE(VectorType,					0x13)
SL_NODE(MatrixType,					0x14)
SL_NODE(StructType,					0x15)
SL_NODE(SamplerStateType,			0x16)
SL_NODE(TextureType,				0x17)
SL_NODE(BufferType,					0x18)
SL_NODE(StructMember,				0x19)
SL_NODE(Srv,						0x1a)
SL_NODE(Types,						0x1b)
SL_NODE(SourceManager,				0x1c)
SL_NODE(SourceFile,					0x1d)
SL_NODE(CompileOptionList,			0x1e)
SL_NODE(CompileOption,				0x1f)
SL_NODE(Cfg,						0x20)
SL_NODE(CfgProgram,					0x21)
SL_NODE(CfgBasicBlockList,			0x22)
SL_NODE(CfgFunctionList,			0x23)
SL_NODE(CfgFunction,				0x24)
SL_NODE(CfgBasicBlock,				0x25)
SL_NODE(CfgBasicBlockRef,			0x26)
SL_NODE(CfgBasicBlockSuccList,		0x27)
SL_NODE(CfgBasicBlockVgprSuccList,	0x28)
SL_NODE(ShaderFooter,				0x29)
SL_NODE(FunctionShaderType,			0x2a)
SL_NODE(Libraries,					0x2b)
SL_NODE(Library,					0x2c)
SL_NODE(ResourceDescriptorType,		0x2d)
SL_NODE(AnnotationList,				0x2e)
SL_NODE(Annotation,					0x2f)
SL_NODE(GlobalSrt,					0x30)
SL_NODE(RazorInstAnnotationList,	0x31)
SL_NODE(RazorInstAnnotation,		0x32)
SL_NODE(WaveInstAnnotationList,		0x33)
SL_NODE(WaveInstAnnotation,			0x34)

#	undef SL_NODE
};

// ------------------------------------------------------------------------------------------------/
/// attribute tags
namespace AttribTags
{
#	define SL_ATTR(NAME,VALUE)uint32_t const k ## NAME = VALUE;

/// SL_ATTR(NAME, VALUE)
SL_ATTR(Microcode,				0x01)
SL_ATTR(SourceHash,				0x02)
SL_ATTR(BinaryHash,				0x03)
SL_ATTR(Profile,				0x04)
SL_ATTR(CompilerVersion,		0x05)
SL_ATTR(Version,				0x06)
SL_ATTR(CompilerType,			0x07)
SL_ATTR(UsesSrt,				0x08)
SL_ATTR(ShaderVariant,			0x09)
SL_ATTR(NumThreadX,				0x0a)
SL_ATTR(NumThreadY,				0x0b)
SL_ATTR(NumThreadZ,				0x0c)
SL_ATTR(InstanceCount,			0x0d)
SL_ATTR(MaxVertexCount,			0x0e)
SL_ATTR(InputType,				0x0f)
SL_ATTR(OutputType,				0x10)
SL_ATTR(PatchSize,				0x11)
SL_ATTR(PatchType,				0x12)
SL_ATTR(InputCtrlPointCount,	0x13)
SL_ATTR(OutputTopologyType,		0x14)
SL_ATTR(PartitionType,			0x15)
SL_ATTR(OutputCtrlPointCount,	0x16)
SL_ATTR(MaxTessFactor,			0x17)
SL_ATTR(NumericType,			0x18)
SL_ATTR(SemanticId,				0x19)
SL_ATTR(SemanticIndex,			0x1a)
SL_ATTR(ResourceIndex,			0x1b)
SL_ATTR(InterpolationType,		0x1c)
SL_ATTR(StreamIndex,			0x1d)
SL_ATTR(Name,					0x1e)
SL_ATTR(SemanticName,			0x1f)
SL_ATTR(Stride,					0x20)
SL_ATTR(Offset,					0x21)
SL_ATTR(Slot,					0x22)
SL_ATTR(Components,				0x23)
SL_ATTR(TypeRef,				0x24)
SL_ATTR(Size,					0x25)
SL_ATTR(DefaultValue,			0x26)
SL_ATTR(IsUsed,					0x27)
SL_ATTR(IsRw,					0x28)
SL_ATTR(Class,					0x29)
SL_ATTR(Rows,					0x2a)
SL_ATTR(Columns,				0x2b)
SL_ATTR(RowMajor,				0x2c)
SL_ATTR(UserFilename,			0x2d)
SL_ATTR(AbsoluteFilename,		0x2e)
SL_ATTR(IsPrimaryfile,			0x2f)
SL_ATTR(FileId,					0x30)
SL_ATTR(DupFileId,				0x31)
SL_ATTR(Content,				0x32)
SL_ATTR(CompileOptionName,		0x33)
SL_ATTR(CompileOptionValue,		0x34)
SL_ATTR(ShaderStageName,		0x35)
SL_ATTR(CfgBasicBlockStartOffset,			0x36)
SL_ATTR(CfgBasicBlockEndOffset,				0x37)
SL_ATTR(CfgBasicBlockRegionNesting,			0x38)
SL_ATTR(CfgBasicBlockUniform,				0x39)
SL_ATTR(CfgBasicBlockOverrideExecForRead,	0x3a)
SL_ATTR(CfgBasicBlockOverrideExecForWrite,	0x3b)
SL_ATTR(CfgBasicBlockRef,					0x3c)
SL_ATTR(CfgFunctionFirstBlock,				0x3d)
SL_ATTR(CfgFunctionLastBlock,				0x3e)
SL_ATTR(CfgRange,							0x3f)
SL_ATTR(InputLayout,						0x40)
SL_ATTR(UsageMask,							0x41)
SL_ATTR(WaveMode,				0x42)
SL_ATTR(InputCtrlPointStride,	0x43)
SL_ATTR(OutputCtrlPointStride,	0x44)
SL_ATTR(PatchConstantStride,	0x45)
SL_ATTR(VertexOutputStride,		0x46)
SL_ATTR(Date,					0x47)
SL_ATTR(HasDiscard,				0x48)
SL_ATTR(HasDepthWrite,			0x49)
SL_ATTR(CompilerVersionString,	0x4a)
SL_ATTR(HardwareRevision,		0x4b)
SL_ATTR(BuildVersion,			0x4c)
SL_ATTR(BuildNumber,			0x4d)
SL_ATTR(ShaderType,				0x4e)
SL_ATTR(StereoModeX,			0x4f)
SL_ATTR(VgprCount,				0x50)
SL_ATTR(SgprCount,				0x51)
SL_ATTR(SharedVgprCount,		0x52)
SL_ATTR(ScratchSizeInDwords,	0x53)
SL_ATTR(LdsSizeInBytes,			0x54)
SL_ATTR(AbiVgprStart,			0x55)
SL_ATTR(AbiVgprCount,			0x56)
SL_ATTR(AbiSgprStart,			0x57)
SL_ATTR(AbiSgprCount,			0x58)
SL_ATTR(AbiScratchSizeInDwords,	0x59)
SL_ATTR(AbiLdsSizeInDwords,		0x60)
SL_ATTR(AbiVgprInOutCount,		0x61)
SL_ATTR(AbiVgprInCount,			0x62)
SL_ATTR(ExportFormat,			0x63)
SL_ATTR(OriginalSlot,			0x64)
SL_ATTR(OriginalName,			0x65)
SL_ATTR(OriginalResourceType,	0x66)
SL_ATTR(StaticVertexLayout,		0x67)
SL_ATTR(GdsSizeInBytes,			0x68)
SL_ATTR(StaticVertexLayoutUsageMask,	0x69)
SL_ATTR(InterpolationClass,		0x6a)
SL_ATTR(CompressionMode,		0x6b)
SL_ATTR(ZOrder,					0x6c)
SL_ATTR(ConservativeZExport,	0x6d)
SL_ATTR(HasPops,				0x6e)
SL_ATTR(CompactCfg,				0x6f)
SL_ATTR(CfgExRange,				0x70)
SL_ATTR(CfgAbiInfo,				0x71)
SL_ATTR(CfgBasicBlockSubIndex,	0x72)
SL_ATTR(DebugShader,			0x73)
SL_ATTR(ScratchSize,			0x74)
SL_ATTR(HasObjPrimId,			0x75)
SL_ATTR(IndexExtractionMask,	0x76)
SL_ATTR(IsBvh,					0x77)
SL_ATTR(RequireBvhTraversal,	0x78)
SL_ATTR(ShaderFeatureFlags,		0x79)
SL_ATTR(ShaderFeatureFlagsBitCount,		0x7a)
SL_ATTR(OriginalSpace,			0x7b)
SL_ATTR(SystemFunctionShaderCallsUsageMask, 0x7c)
SL_ATTR(AgcMemRelocationData,   0x7d)
SL_ATTR(HasDerivatives,         0x7e)
SL_ATTR(IsTranscoded,           0x7f)
SL_ATTR(TranscodeOriginalBinaryHash,      0x80)
SL_ATTR(TranscodeOriginalSourceHash,      0x81)
SL_ATTR(TranscoderVersionNumber,      0x82)
SL_ATTR(TranscoderBuildNumber,      0x83)
SL_ATTR(HasUseThreadDimension,  0x84)
SL_ATTR(BuildRevision,          0x85)
SL_ATTR(TranscoderBuildRevision,0x86)
SL_ATTR(IsDevelShader,          0x87)
SL_ATTR(IsRealType,             0x88)
SL_ATTR(CompileTarget,          0x89)
SL_ATTR(Encoding,               0x8a)
SL_ATTR(AddrShift,              0x8b)
SL_ATTR(SizeShift,              0x8c)
SL_ATTR(PointedSize,            0x8d)
SL_ATTR(AbiFlags,			    0x8e)
SL_ATTR(AbiRtAbiMask,			0x8f)
#	undef SL_ATTR
};

// ------------------------------------------------------------------------------------------------/
/// get a name for a tag/attribute or null
SCE_SL_INTERFACE char const *GetNodeTagName(uint32_t nodeTagId);
SCE_SL_INTERFACE char const *GetAttrTagName(uint32_t attrTagId);

// ------------------------------------------------------------------------------------------------/
/// get a tag from a name (-1 on error)
SCE_SL_INTERFACE uint32_t GetNodeTagFromName(char const *);
SCE_SL_INTERFACE uint32_t  GetAttrTagFromName(char const *);

// ------------------------------------------------------------------------------------------------/
/// get a name for a form id
SCE_SL_INTERFACE char const *GetFormName(AttrForm form);
SCE_SL_INTERFACE AttrForm GetFormFromName(char const *);

}
}



// ------------------------------------------------------------------------------------------------/
/// hide details into a special namespace
namespace libsceshader_details
{

// ------------------------------------------------------------------------------------------------/
/// put elf defines into the namespace



// ------------------------------------------------------------------------------------------------/
/// NOTE: do not include any files here

// ------------------------------------------------------------------------------------------------/
/// Elf identification
enum ElfFileId
{
	EI_MAG0 = 0,				// '\x7f'
	EI_MAG1 = 1,				// 'E'
	EI_MAG2 = 2,				// 'L'
	EI_MAG3 = 3,				// 'F'	
	EI_CLASS = 4,				// File class			table3
	EI_DATA = 5,				// Data encoding		table4
	EI_VERSION = 6,				// File version
	EI_OSABI = 7,				// OS / ABI identification
	EI_ABIVERSION = 8,			// ABI version
};

// ------------------------------------------------------------------------------------------------/
/// Elf file type
enum ElfType
{
	ET_NONE = 0,				// No file type
	ET_REL = 1,					// Relocatable object file
	ET_EXEC = 2,				// Executable file
	ET_DYN = 3,					// Shared object file
	ET_CORE = 4,				// Core file
	ET_LOOS = 0xFE00,			// Environment - specific use
	ET_HIOS = 0xFEFF,
	ET_LOPROC = 0xFF00,			// Processor - specific use
	ET_HIPROC = 0xFFFF
};

// ------------------------------------------------------------------------------------------------/
/// Elf program header type
enum ElfPHdrType
{
	PT_NULL = 0,				// Unused entry
	PT_LOAD = 1,				// Loadable segment
	PT_DYNAMIC = 2,				// Dynamic linking tables
	PT_INTERP = 3,				// Program interpreter path name
	PT_NOTE = 4,				// Note sections
	PT_SHLIB = 5,				// Reserved
	PT_PHDR = 6,				// Program header table
	PT_LOOS = 0x60000000,		// Environment - specific use
	PT_HIOS = 0x6FFFFFFF,
	PT_LOPROC = 0x70000000,		// Processor - specific use
	PT_HIPROC = 0x7FFFFFFF,
};

// ------------------------------------------------------------------------------------------------/
/// Elf program header flags
enum ElfPHdrFlag
{
	PF_X = 0x1,					// Execute permission
	PF_W = 0x2,					// Write permission
	PF_R = 0x4,					// Read permission
	PF_MASKOS = 0x00FF0000,		// These flag bits are reserved for environment - specific use
	PF_MASKPROC = 0xFF000000	// These flag bits are reserved for processor - specific use
};

// ------------------------------------------------------------------------------------------------/
/// symbol binding
enum ElfSymbolBinding
{
	STB_LOCAL = 0,				// Not visible outside the object file
	STB_GLOBAL = 1,				// Global symbol, visible to all object files
	STB_WEAK = 2,				// Global scope, but with lower precedence than global symbols
	STB_LOOS = 10,				// Environment - specific use
	STB_HIOS = 12,
	STB_LOPROC = 13,			// Processor - specific use
	STB_HIPROC = 15
};

// ------------------------------------------------------------------------------------------------/
/// symbol type
enum ElfSymbolType
{
	STT_NOTYPE = 0,				// No type specified (e.g., an absolute symbol)
	STT_OBJECT = 1,				// Data object
	STT_FUNC = 2,				// Function entry point
	STT_SECTION = 3,			// Symbol is associated with a section
	STT_FILE = 4,				// Source file associated with the object file
	STT_LOOS = 10,				// Environment - specific use STT_HIOS 12
	STT_LOPROC = 13,			// Processor - specific use
	STT_HIPROC = 15
};

// ------------------------------------------------------------------------------------------------/
/// Elf section types
enum ElfSHdrType
{
	SHT_NULL = 0,				// Marks an unused section header
	SHT_PROGBITS = 1,			// Contains information defined by the program
	SHT_SYMTAB = 2,				// Contains a linker symbol table
	SHT_STRTAB = 3,				// Contains a string table
	SHT_RELA = 4,				// Contains Rela type relocation entries
	SHT_HASH = 5,				// Contains a symbol hash table
	SHT_DYNAMIC = 6,			// Contains dynamic linking tables
	SHT_NOTE = 7,				// Contains note information
	SHT_NOBITS = 8,				// Contains uninitialized space; does not occupy any space in the file
	SHT_REL = 9,				// Contains Rel type relocation entries
	SHT_SHLIB = 10,				// Reserved
	SHT_DYNSYM = 11,			// Contains a dynamic loader symbol	table
	SHT_LOOS = 0x60000000,		// Environment - specific use
	SHT_HIOS = 0x6FFFFFFF,
	SHT_LOPROC = 0x70000000,	// Processor - specific use
	SHT_HIPROC = 0x7FFFFFFF,

	// specifics
	SHT_GNMX = SHT_LOOS + 1,
	SHT_REFLECTION = SHT_LOOS + 2
};

// ------------------------------------------------------------------------------------------------/
/// Elf section types
enum ElfSHdrFlag
{
	SHF_WRITE = 0x1,			// Section contains writable data
	SHF_ALLOC = 0x2,			// Section is allocated in memory image of program
	SHF_EXECINSTR = 0x4,		// Section contains executable instructions
	SHF_COMPRESSED = 0x800,		// Section with compressed data
	SHF_MASKOS = 0x0F000000,	// Environment - specific use
	SHF_MASKPROC = 0xF0000000	// Processor - specific use
};

// ------------------------------------------------------------------------------------------------/
/// Elf symbol visibility
enum ElfSymbolVisibility : uint8_t
{
	STV_DEFAULT = 0x0,			// Exported
	STV_INTERNAL = 0x1,			// Reserved
	STV_HIDDEN = 0x2,			// Not exported
	STV_PROTECTED = 0x3			// Reserved
};

// ------------------------------------------------------------------------------------------------/
/// Elf header
struct Elf64_Ehdr
{
	uint8_t  e_ident[16];		// ELF identification
	uint16_t e_type;			// Object file type
	uint16_t e_machine;			// Machine type
	uint32_t e_version;			// Object file version
	uint64_t e_entry;			// Entry point address
	uint64_t e_phoff;			// Program header offset
	uint64_t e_shoff;			// Section header offset
	uint32_t e_flags;			// Processor-specific flags
	uint16_t e_ehsize;			// ELF header size
	uint16_t e_phentsize;		// Size of program header entry
	uint16_t e_phnum;			// Number of program header entries
	uint16_t e_shentsize;		// Size of section header entry
	uint16_t e_shnum;			// Number of section header entries
	uint16_t e_shstrndx;		// Section name string table index
};

// ------------------------------------------------------------------------------------------------/
/// Elf section header
struct Elf64_Shdr
{
	uint32_t sh_name;			// Section name
	uint32_t sh_type;			// Section type
	uint64_t sh_flags;			// Section attributes
	uint64_t sh_addr;			// Virtual address in memory
	uint64_t sh_offset;			// Offset in file
	uint64_t sh_size;			// Size of section
	uint32_t sh_link;			// Link to other section
	uint32_t sh_info;			// Miscellaneous information
	uint64_t sh_addralign;		// Address alignment boundary
	uint64_t sh_entsize;		// Size of entries, if section has table
};

// ------------------------------------------------------------------------------------------------/
/// Elf program header
struct Elf64_Phdr
{
	uint32_t p_type;			// Type of segment
	uint32_t p_flags;			// Segment attributes
	uint64_t p_offset;			// Offset in file
	uint64_t p_vaddr;			// Virtual address in memory
	uint64_t p_paddr;			// Reserved
	uint64_t p_filesz;			// Size of segment in file
	uint64_t p_memsz;			// Size of segment in memory
	uint64_t p_align;			// Alignment of segment
};

// ------------------------------------------------------------------------------------------------/
/// Entry of the symbol table
struct Elf64_Sym
{
	uint32_t st_name;			// Symbol name
	unsigned char st_info;		// Type and Binding attributes
	unsigned char st_other;		// Visibility
	uint16_t st_shndx;			// Section table index
	uint64_t st_value;			// Symbol value
	uint64_t st_size;			// Size of object (e.g., common)
};

// ------------------------------------------------------------------------------------------------/
/// Comperessed data header
struct Elf64_Chdr 
{
	uint32_t ch_type;			// compression type
	uint32_t ch_reserved;
	uint64_t ch_size;			// size of uncompressed data
	uint64_t ch_addralign;		// alignment of uncompressed data
};

// ------------------------------------------------------------------------------------------------/
/// section names
namespace ElfSections
{
	static char const * const kShaderText = ".shader_text";
	static char const * const kShaderHeader = ".shader_header";
	static char const * const kShaderIR = ".shader_ir";
	static char const * const kMetadata = ".metadata";
	static char const * const kSources = ".sources";
	static char const * const kOptions = ".options";
	static char const * const kLibraries = ".libraries";
	static char const * const kCfg = ".cfg";
	static char const * const kCmdArgs = ".cmdargs";
	static char const * const kTokens = ".tokens";
	static char const * const kStrings = ".strtab";
	static char const * const kSectionNames = ".shstrtab";
	static char const * const kSymbolTable = ".symtab";

	// dwarf sections
	static char const * const kDebugInfo = ".debug_info";
	static char const * const kDebugAbbrev = ".debug_abbrev";
	static char const * const kDebugStr = ".debug_str";
	static char const * const kDebugLine = ".debug_line";
	static char const * const kDebugLoc = ".debug_loc";
	static char const * const kDebugRanges = ".debug_ranges";
	static char const * const kDebugLocLists = ".debug_loclists";
	static char const * const kDebugRngLists = ".debug_rnglists";
}

// ------------------------------------------------------------------------------------------------/
///
struct ElfSection
{
	void const *m_data;
	uint64_t m_size;
	bool m_compressed;
};

// ------------------------------------------------------------------------------------------------/
///
inline ElfSection FindElfSection(void const *_data, char const *name)
{
	auto const data = reinterpret_cast<uint8_t const *>(_data);
	auto const elf64 = reinterpret_cast<Elf64_Ehdr const *>(data);
	if (0 == elf64->e_shstrndx)
		return { nullptr, 0, false };
	auto const stringSection = reinterpret_cast<Elf64_Shdr const *>(
		data +
		elf64->e_shoff +
		uint64_t(elf64->e_shstrndx) * uint64_t(elf64->e_shentsize));
	for (auto s = 0u; s < elf64->e_shnum; ++s)
	{
		auto const section = reinterpret_cast<Elf64_Shdr const *>(
			data +
			elf64->e_shoff +
			uint64_t(s) * uint64_t(elf64->e_shentsize));
		auto const sectionName = reinterpret_cast<char const *>(
			data +
			stringSection->sh_offset +
			section->sh_name);
		if (0 == strcmp(sectionName, name))
			return { 
				data + section->sh_offset, 
				section->sh_size,
				0 != (section->sh_flags & SHF_COMPRESSED)
			};
	}
	return { nullptr, 0, false };
}

// ------------------------------------------------------------------------------------------------/
///
inline uint64_t GetDecodedSectionSize(ElfSection const &section)
{
	if (section.m_compressed)
		return reinterpret_cast<Elf64_Chdr const *>(section.m_data)->ch_size;
	else
		return section.m_size;
}



// ------------------------------------------------------------------------------------------------/
///
inline uint64_t MiniInflate(uint8_t *dst, uint8_t const *src, uint64_t /*len*/)
{
	// huffman table
	struct Huffman {
		int16_t const *counts;
		int16_t const *symbols;
	};
	
	// precomputed constants
	static constexpr auto kNumBits = 15u, kLCodes = 286u, kDCode = 30u;
	static constexpr int16_t fixed_lencnt[] = { 0,0,0,0,0,0,0,24,152,112,0,0,0,0,0,0 };
	static constexpr int16_t fixed_distcnt[] = { 0,0,0,0,0,30,0,0,0,0,0,0,0,0,0,0 };
	static constexpr int16_t fixed_lensyms[] = {
		256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274,
		275, 276, 277, 278, 279, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
		44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
		68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
		92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
		113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
		132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 280, 281, 282, 283, 284, 285, 286,
		287, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161,
		162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
		181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
		200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218,
		219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237,
		238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };
	static constexpr int16_t fixed_distsyms[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		26, 27, 28, 29 };
	static constexpr int16_t lens[] = {
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131,
		163, 195, 227, 258 };
	static constexpr int16_t lext[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
	static constexpr int16_t dists[] = {
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049,
		3073, 4097, 6145, 8193, 12289, 16385, 24577 };
	static constexpr int16_t dext[] = {
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,	7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
	static constexpr int16_t order[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	static constexpr Huffman fixed_lentable = { &fixed_lencnt[0], &fixed_lensyms[0] };
	static constexpr Huffman fixed_disttable = { &fixed_distcnt[0], &fixed_distsyms[0] };

	// huffman tables for the dynamic case
	int16_t dynamic_lencnt[kNumBits + 1], dynamic_lensyms[kLCodes];
	int16_t dynamic_distcnt[kNumBits + 1], dynamic_distsyms[kDCode];
	Huffman dynamic_lentable = { &dynamic_lencnt[0], &dynamic_lensyms[0] };
	Huffman dynamic_disttable = { &dynamic_distcnt[0], &dynamic_distsyms[0] };

	// extract bits
	uint64_t index = 0, count = 0;
	int bitcnt = 0, bitbuf = 0;
	auto const bits = [&](int c) {
		auto v = bitbuf;
		for (; bitcnt < c; bitcnt += 8)
			v |= int(src[index++]) << bitcnt;
		bitbuf = v >> c;
		bitcnt -= c;
		return v & ((1 << c) - 1);
	};

	// decode one symbol
	auto const decodeSymbol = [&] (Huffman const &h) {
		auto code = 0, f = 0, i = 0;
		for (auto l = 1u; l <= kNumBits; l++, code <<= 1)
		{
			code |= bits(1);
			auto c = h.counts[l];
			if (code - c < f)
				return h.symbols[i + (code - f)];
			i = i + c;
			f = (f + c) << 1;
		}
		return int16_t(0);
	};

	// decode one block
	auto const decodeBlock = [&](Huffman const &lh, Huffman const &dh) {
		auto symbol = 0;
		do {
			symbol = decodeSymbol(lh);
			if (symbol < 256)
				dst[count++] = uint8_t(symbol);
			else if (symbol > 256) 
			{
				auto len = lens[symbol - 257] + (int)bits(lext[symbol - 257]);
				symbol = decodeSymbol(dh);
				auto dist = (uint32_t)dists[symbol] + (uint32_t)bits(dext[symbol]);
				for (; len; --len, ++count)
					dst[count] = dst[count - dist];
			}
		} while (symbol != 256);
	};

	// build the huffmap table
	auto const buildHuffman = [](int16_t *counts, int16_t *symbols, const int16_t *length, int n) {
		for (auto l = 0u; l <= kNumBits; l++)
			counts[l] = 0;
		for (auto s = 0; s < n; s++)
			counts[length[s]]++;
		int16_t offs[kNumBits + 1];
		offs[1] = 0;
		for (auto l = 1u; l < kNumBits; l++)
			offs[l + 1] = offs[l] + counts[l];
		for (auto s = 0; s < n; s++)
			if (length[s] != 0)
				symbols[offs[length[s]]++] = int16_t(s);
	};

	// decode the huffman table for dynamic blocks
	auto const decodeDynamicTable = [&]()
	{
		// The Huffman trees themselves are compressed using Huffman encoding, build
		// the table to decode them
		auto const nlen = bits(5) + 257;
		auto const ndist = bits(5) + 1;
		auto const ncode = bits(4) + 4;
		int16_t lengths[kLCodes + kDCode];
		for (auto i = 0; i < ncode; i++)
			lengths[order[i]] = (int16_t)bits(3);
		for (auto i = ncode; i < 19; i++)
			lengths[order[i]] = 0;
		buildHuffman(dynamic_lencnt, dynamic_lensyms, lengths, 19);

		// Now decode and build the actual tables
		for (auto index = 0; index < nlen + ndist;) 
		{
			int symbol = decodeSymbol(dynamic_lentable);
			if (symbol < 16)
				lengths[index++] = (int16_t)symbol;
			else 
			{
				auto len = 0;
				if (symbol == 16) 
				{
					if (index > 0)
						len = lengths[index - 1];
					symbol = 3 + bits(2);
				}
				else if (symbol == 17)
					symbol = 3 + bits(3);
				else
					symbol = 11 + bits(7);
				while (symbol--)
					lengths[index++] = (int16_t)len;
			}
		}
		buildHuffman(dynamic_lencnt, dynamic_lensyms, lengths, nlen);
		buildHuffman(dynamic_distcnt, dynamic_distsyms, lengths + nlen, ndist);
	};

	// check and skip header
	if (src[0] == 0x1f && src[1] == 0x8b)
		index = 10;	// skip gzip header (todo: header may be more complex)
	else if ((src[0] & 0xf) == 0x8 && (256 * src[0] + src[1]) % 31 == 0)
		index = 2; // skip zlib header
	// loop over the input
	auto done = 0, l = 0;
	do {
		done = bits(1);
		auto const type = bits(2);
		switch (type)
		{
		case 0:
			bitcnt = bitbuf = 0;
			l = bits(16);
			index += 2;
			while (l--)
				dst[count++] = src[index++];
			break;
		case 1:	
			decodeBlock(fixed_lentable, fixed_disttable);
			break;
		case 2:	
			decodeDynamicTable();
			decodeBlock(dynamic_lentable, dynamic_disttable);
			break;
		default: return 0;
		}
	} while (!done);
	return count;
}

// ------------------------------------------------------------------------------------------------/
/// put agsl structures into the namespace



// ------------------------------------------------------------------------------------------------/
/// NOTE: do not include any files here

// ------------------------------------------------------------------------------------------------/
/// The AGSL header
struct AgslHeader
{
	static constexpr uint32_t kMagic = 'agsl';
	static constexpr uint32_t kVersion = 1;

	uint32_t m_magic;						// this is 'agsl'
	uint32_t m_version;						// version of the file
	uint32_t m_numShaders;					// cached number of shaders in the agsl
	uint32_t m_pad[5];						// make this a 32-byte structure for future expansion
};

// ------------------------------------------------------------------------------------------------/
/// AGSL shader entry
struct AgslShaderEntry
{
	uint64_t m_shaderOffset = 0;
	uint64_t m_nameOffset = 0;
	uint64_t m_size = 0;
	uint64_t m_pad = 0;
};


// ------------------------------------------------------------------------------------------------/
///
template <typename T>
static inline T To(uint32_t v)
{
	return reinterpret_cast<T>(ptrdiff_t(v));
}

// ------------------------------------------------------------------------------------------------/
///
template <typename T>
static inline uint32_t From(T v)
{
	return (uint32_t)reinterpret_cast<ptrdiff_t>(v);
}

// ------------------------------------------------------------------------------------------------/
///
static inline bool VerifyElfHeader(void const *data)
{
	// check against null pointers
	if (nullptr == data)
		return false;
	// verify that we have an elf64 file
	auto const elf64 = reinterpret_cast<Elf64_Ehdr const *>(data);
	if (elf64->e_ident[EI_MAG0] != 0x7f ||
		elf64->e_ident[EI_MAG1] != 'E' ||
		elf64->e_ident[EI_MAG2] != 'L' ||
		elf64->e_ident[EI_MAG3] != 'F')
		return false;
	// TODO: check few more bits ?
	return true;
}

// ------------------------------------------------------------------------------------------------/
///
static inline bool VerifyAgslHeader(void const* data)
{
	// check against null pointers
	if (nullptr == data)
		return false;
	// verify that we have an agsl file
	auto const agsl = reinterpret_cast<AgslHeader const*>(data);
	if (agsl->m_magic != AgslHeader::kMagic)
		return false;
	return true;
}

// ------------------------------------------------------------------------------------------------/
///
static inline AgslShaderEntry const *GetLibraryShaderEntry(void const* data, uint32_t index)
{
	auto const agsl = reinterpret_cast<AgslHeader const*>(data);
	auto const entries = reinterpret_cast<AgslShaderEntry const*>(agsl + 1);
	return &entries[index];
}

// ------------------------------------------------------------------------------------------------/
///
static inline bool VerifyHeader(void const *data)
{
	if (nullptr == data)
		return false;
	// TODO: check few more bits ?
	return true;
}

// ------------------------------------------------------------------------------------------------/
///
static inline uint64_t GetRangeCount(void const *base, sce::sl::AttrHandle attr)
{
	using namespace sce::sl;
	return GetDataBlockAttributeSize(base, attr) / sizeof(SceShaderOperandRange);
}

// ------------------------------------------------------------------------------------------------/
///
static inline bool DecodedSection(void *dst, ElfSection const &section)
{
	if (section.m_compressed)
	{
		if (0 == MiniInflate((uint8_t*)dst, (uint8_t const *)section.m_data + sizeof(Elf64_Chdr), section.m_size))
			return false;
	}
	else
		memcpy(dst, section.m_data, section.m_size);
	return true;
}

} // libsceshader_details

// ------------------------------------------------------------------------------------------------/
/// generically handle multiple attributes
#define GET_SIMPLE_ATTR(NAME, SRC0, SRC1, ATTR_TAG, ATTR_TYPE, ATTR_FUNC, DEFAULT_VAL)			\
	SCE_SHADER_INTERFACE ATTR_TYPE NAME(SRC0 binHandle, SRC1 shaderHandle)						\
	{																							\
		using namespace sce::sl;																\
		namespace d = libsceshader_details;														\
		if (!d::VerifyHeader(binHandle) || nullptr == shaderHandle)								\
			return static_cast<ATTR_TYPE>(DEFAULT_VAL);											\
		auto const attr = GetFirstAttributeWithTag(												\
			binHandle, d::From(shaderHandle), ATTR_TAG);										\
		return 0 == attr ?																		\
			static_cast<ATTR_TYPE>(DEFAULT_VAL) :												\
			static_cast<ATTR_TYPE>(ATTR_FUNC(binHandle, attr));									\
	}

// ------------------------------------------------------------------------------------------------/
/// generically handle multiple attributes
#define GET_SIMPLE_ATTR_ROOT(NAME, HANDLE, ATTR_TAG, ATTR_TYPE, ATTR_FUNC, DEFAULT_VAL)			\
	SCE_SHADER_INTERFACE ATTR_TYPE NAME(HANDLE metadata)										\
	{																							\
		using namespace sce::sl;																\
		namespace d = libsceshader_details;														\
		if (!d::VerifyHeader(metadata))															\
			return static_cast<ATTR_TYPE>(DEFAULT_VAL);											\
		auto const attr = GetFirstAttributeWithTag(												\
			metadata, GetFirstChildNodeWithTag(metadata, GetRootNode(metadata), NodeTags::kShader), ATTR_TAG);\
		return 0 == attr ?																		\
			static_cast<ATTR_TYPE>(DEFAULT_VAL) :												\
			static_cast<ATTR_TYPE>(ATTR_FUNC(metadata, attr));									\
	}


// ------------------------------------------------------------------------------------------------/
/// handle pointer attributes
#define GET_NODEREF_ATTR(NAME, SRC0, SRC1, ATTR_TAG, ATTR_TYPE, ATTR_FUNC, DEFAULT_VAL)			\
	SCE_SHADER_INTERFACE ATTR_TYPE NAME(														\
		SRC0 binHandle, SRC1 handle)															\
	{																							\
		using namespace sce::sl;																\
		namespace d = libsceshader_details;														\
		if (!d::VerifyHeader(binHandle) || nullptr == handle)									\
			return static_cast<ATTR_TYPE>(DEFAULT_VAL);											\
		auto const attr = GetFirstAttributeWithTag(												\
			binHandle, d::From(handle), ATTR_TAG);												\
		return 0 == attr ?																		\
			static_cast<ATTR_TYPE>(DEFAULT_VAL) :												\
			reinterpret_cast<ATTR_TYPE>((uint64_t)ATTR_FUNC(binHandle, attr));					\
	}

// ------------------------------------------------------------------------------------------------/
/// generically handle bool attributes
#define GET_BOOL_ATTR_ROOT(NAME, HANDLE, ATTR_TAG)												\
	SCE_SHADER_INTERFACE bool NAME(HANDLE metadata)												\
	{																							\
		using namespace sce::sl;																\
		namespace d = libsceshader_details;														\
		if (!d::VerifyHeader(metadata))															\
			return false;																		\
		return 0 != GetFirstAttributeWithTag(													\
			metadata, GetFirstChildNodeWithTag(metadata, GetRootNode(metadata), NodeTags::kShader), ATTR_TAG);\
	}

// ------------------------------------------------------------------------------------------------/
/// generically handle bool attributes
#define GET_BOOL_ATTR(NAME, SRC0, SRC1, ATTR_TAG)												\
	SCE_SHADER_INTERFACE bool NAME(																\
		SRC0 binHandle, SRC1 shaderHandle)														\
	{																							\
		using namespace sce::sl;																\
		namespace d = libsceshader_details;														\
		if (!d::VerifyHeader(binHandle) || nullptr == shaderHandle)								\
			return false;																		\
		return 0 != GetFirstAttributeWithTag(													\
			binHandle, d::From(shaderHandle), ATTR_TAG);										\
	}

// ------------------------------------------------------------------------------------------------/
/// generically handle a tribool attribute
#define GET_SIMPLE_TRIBOOL_ATTR_ROOT(NAME, HANDLE, ATTR_TAG)                                    \
	SCE_SHADER_INTERFACE SceShaderTriBool NAME(HANDLE metadata)                                 \
	{                                                                                           \
		using namespace sce::sl;                                                                \
		namespace d = libsceshader_details;                                                     \
		if (!d::VerifyHeader(metadata))                                                         \
			return SceShaderTriBoolUnknown;                                                     \
		auto const attr = GetFirstAttributeWithTag(                                             \
			metadata, GetFirstChildNodeWithTag(metadata, GetRootNode(metadata), NodeTags::kShader), ATTR_TAG);\
		return 0 == attr ?                                                                      \
			SceShaderTriBoolUnknown :                                                           \
			0 == GetUint64Attribute(metadata, attr) ? SceShaderTriBoolFalse : SceShaderTriBoolTrue;\
	}

// ------------------------------------------------------------------------------------------------/
/// get first child
#define GET_FIRST_CHILD(NAME, RET, SRC0, SRC1)								\
	SCE_SHADER_INTERFACE RET NAME(											\
		SRC0 binHandle, SRC1 handle)										\
	{																		\
		using namespace sce::sl;											\
		namespace d = libsceshader_details;									\
		if (!d::VerifyHeader(binHandle) || nullptr == handle)				\
			return nullptr;													\
		return d::To<RET>(GetFirstChildNode(binHandle, d::From(handle)));	\
	}

// ------------------------------------------------------------------------------------------------/
/// get next child
#define GET_NEXT_CHILD(NAME, RET, SRC0, SRC1)								\
	SCE_SHADER_INTERFACE RET NAME(SRC0 binHandle, SRC1 handle)				\
	{																		\
		using namespace sce::sl;											\
		namespace d = libsceshader_details;									\
		if (!d::VerifyHeader(binHandle) || nullptr == handle)				\
			return nullptr;													\
		return d::To<RET>(GetSiblingNode(binHandle, d::From(handle)));		\
	}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderBinaryHandle sceShaderGetBinaryHandle(void const *data)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(data))
		return nullptr;
	return reinterpret_cast<SceShaderBinaryHandle>(data);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE uint64_t sceShaderGetProgramSize(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const text = d::FindElfSection(binHandle, d::ElfSections::kShaderText);
	return text.m_size;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE void const * sceShaderGetProgram(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const text = d::FindElfSection(binHandle, d::ElfSections::kShaderText);
	return text.m_data;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
SceShaderLibraryBinaryHandle sceShaderGetLibraryBinaryHandle(void const* data)
{
	namespace d = libsceshader_details;
	if (!d::VerifyAgslHeader(data))
		return 0;
	return (SceShaderLibraryBinaryHandle)data;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
uint64_t sceShaderGetLibraryShaderCount(
	SceShaderLibraryBinaryHandle libraryHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyAgslHeader(libraryHandle))
		return 0;
	auto const agsl = reinterpret_cast<d::AgslHeader const*>(libraryHandle);
	return agsl->m_numShaders;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
char const* sceShaderGetLibraryShaderName(
	SceShaderLibraryBinaryHandle libraryHandle,
	uint32_t index)
{
	namespace d = libsceshader_details;
	if (!d::VerifyAgslHeader(libraryHandle))
		return nullptr;
	auto const e = d::GetLibraryShaderEntry(libraryHandle, index);
	return reinterpret_cast<char const*>((char const*)libraryHandle + e->m_nameOffset);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
void const* sceShaderGetLibraryShaderData(
	SceShaderLibraryBinaryHandle libraryHandle,
	uint32_t index)
{
	namespace d = libsceshader_details;
	if (!d::VerifyAgslHeader(libraryHandle))
		return nullptr;
	auto const e = d::GetLibraryShaderEntry(libraryHandle, index);
	return reinterpret_cast<void const*>((char const*)libraryHandle + e->m_shaderOffset);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
uint32_t sceShaderGetLibraryShaderSize(
	SceShaderLibraryBinaryHandle libraryHandle,
	uint32_t index)
{
	namespace d = libsceshader_details;
	if (!d::VerifyAgslHeader(libraryHandle))
		return 0;
	auto const e = d::GetLibraryShaderEntry(libraryHandle, index);
	return uint32_t(e->m_size);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
uint64_t sceShaderGetCompileOptionSectionSize(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const options = d::FindElfSection(binHandle, d::ElfSections::kOptions);
	if (0 == options.m_data)
		return 0;
	return d::GetDecodedSectionSize(options);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
SceShaderCompileOptionSectionHandle sceShaderDecodeCompileOptionSection(
	void *dst,
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const options = d::FindElfSection(binHandle, d::ElfSections::kOptions);
	if (0 == options.m_data)
		return 0;
	if (!d::DecodedSection(dst, options))
		return 0;
	else
		return reinterpret_cast<SceShaderCompileOptionSectionHandle>(dst);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderCompileOptionListHandle sceShaderGetCompileOptionList(
	SceShaderBinaryHandle binHandle,
	SceShaderCompileOptionSectionHandle sectionHandle)
{
	using namespace sce::sl;
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return nullptr;
	return d::To<SceShaderCompileOptionListHandle>(GetRootNode(sectionHandle));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstCompileOption,
	SceShaderCompileOptionHandle,
	SceShaderCompileOptionSectionHandle,
	SceShaderCompileOptionListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextCompileOption,
	SceShaderCompileOptionHandle,
	SceShaderCompileOptionSectionHandle,
	SceShaderCompileOptionHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetCompileOptionName,
	SceShaderCompileOptionSectionHandle,
	SceShaderCompileOptionHandle,
	AttribTags::kCompileOptionName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetCompileOptionValue,
	SceShaderCompileOptionSectionHandle,
	SceShaderCompileOptionHandle,
	AttribTags::kCompileOptionValue,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
SceShaderCompileOptionListHandle sceShaderGetFirstPipelineStage(
	SceShaderCompileOptionSectionHandle binHandle,
	SceShaderCompileOptionHandle optionHandle)
{
	using namespace sce::sl;
	namespace d = libsceshader_details;
	if (!d::VerifyHeader(binHandle))
		return nullptr;
	return d::To<SceShaderCompileOptionListHandle>(GetFirstChildNodeWithTag(
		binHandle,
		d::From(optionHandle),
		NodeTags::kCompileOptionList));
}

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextPipelineStage,
	SceShaderCompileOptionListHandle,
	SceShaderCompileOptionSectionHandle,
	SceShaderCompileOptionListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(sceShaderGetPipelineStageName,
	SceShaderCompileOptionSectionHandle,
	SceShaderCompileOptionListHandle,
	AttribTags::kShaderStageName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
char const* sceShaderGetCmdLineArgs(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const sources = d::FindElfSection(binHandle, d::ElfSections::kCmdArgs);
	return reinterpret_cast<char const *>(sources.m_data);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
uint64_t sceShaderGetSourceFileSectionSize(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const sources = d::FindElfSection(binHandle, d::ElfSections::kSources);
	if (0 == sources.m_data)
		return 0;
	return d::GetDecodedSectionSize(sources);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
SceShaderSourceFileSectionHandle sceShaderDecodeSourceFileSection(
	void *dst,
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const sources = d::FindElfSection(binHandle, d::ElfSections::kSources);
	if (0 == sources.m_data)
		return 0;
	if (!d::DecodedSection(dst, sources))
		return 0;
	else
		return reinterpret_cast<SceShaderSourceFileSectionHandle>(dst);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE
SceShaderSourceFileListHandle sceShaderGetSourceFileList(
	SceShaderBinaryHandle binHandle,
	SceShaderSourceFileSectionHandle sectionHandle)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(binHandle))
		return nullptr;
	return d::To<SceShaderSourceFileListHandle>(GetRootNode(sectionHandle));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstSourceFile,
	SceShaderSourceFileHandle,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextSourceFile,
	SceShaderSourceFileHandle,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSourceFileUserName,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle,
	AttribTags::kUserFilename,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSourceFileAbsolutePath,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle,
	AttribTags::kAbsoluteFilename,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSourceFileContent,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle,
	AttribTags::kContent,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSourceFileId,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle,
	AttribTags::kFileId,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSourceFileDuplicateId,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle,
	AttribTags::kDupFileId,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderIsSourceFilePrimary,
	SceShaderSourceFileSectionHandle,
	SceShaderSourceFileHandle,
	AttribTags::kIsPrimaryfile)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE bool sceShaderHasSourceFileContent(
	SceShaderSourceFileSectionHandle sectionHandle,
	SceShaderSourceFileHandle fileHandle)
{
	return nullptr != sceShaderGetSourceFileContent(sectionHandle, fileHandle);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE uint64_t sceShaderGetProgramHeaderSize(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const text = d::FindElfSection(binHandle, d::ElfSections::kShaderHeader);
	return text.m_size;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE void const * sceShaderGetProgramHeader(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const text = d::FindElfSection(binHandle, d::ElfSections::kShaderHeader);
	return text.m_data;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderMetadataSectionHandle sceShaderGetMetadataSection(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const metadata = d::FindElfSection(binHandle, d::ElfSections::kMetadata);
	return reinterpret_cast<SceShaderMetadataSectionHandle>(metadata.m_data);
}

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderUsesSrt,
	SceShaderMetadataSectionHandle,
	AttribTags::kUsesSrt)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderUsesDerivatives,
	SceShaderMetadataSectionHandle,
	AttribTags::kHasDerivatives)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_TRIBOOL_ATTR_ROOT(
	sceShaderHasDevelopmentFeatures,
	SceShaderMetadataSectionHandle,
	AttribTags::kIsDevelShader)
	
// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_TRIBOOL_ATTR_ROOT(
	sceShaderHasRealTypes,
	SceShaderMetadataSectionHandle,
	AttribTags::kIsRealType)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetCompileTarget,
	SceShaderMetadataSectionHandle,
	AttribTags::kCompileTarget,
	SceShaderCompileTarget,
	GetUint64Attribute,
	SceShaderCompileTargetUnknown)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetProfile,
	SceShaderMetadataSectionHandle,
	AttribTags::kProfile,
	SceShaderProfile,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetWaveMode,
	SceShaderMetadataSectionHandle,
	AttribTags::kWaveMode,
	SceShaderWaveMode,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetVgprCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kVgprCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetSgprCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kSgprCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetSharedVgprCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kSharedVgprCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetScratchSizeInDwords,
	SceShaderMetadataSectionHandle,
	AttribTags::kScratchSizeInDwords,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetLdsSizeInBytes,
	SceShaderMetadataSectionHandle,
	AttribTags::kLdsSizeInBytes,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetGdsSizeInBytes,
	SceShaderMetadataSectionHandle,
	AttribTags::kGdsSizeInBytes,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiVgprStart,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiVgprStart,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiVgprCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiVgprCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiSgprStart,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiSgprStart,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiSgprCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiSgprCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiScratchSizeInDwords,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiScratchSizeInDwords,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiLdsSizeInDwords,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiLdsSizeInDwords,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiVgprInOutCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiVgprInOutCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetFunctionAbiVgprInCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kAbiVgprInCount,
	uint32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetBinaryHash,
	SceShaderMetadataSectionHandle,
	AttribTags::kBinaryHash,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetSourceHash,
	SceShaderMetadataSectionHandle,
	AttribTags::kSourceHash,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetCompilerVersion,
	SceShaderMetadataSectionHandle,
	AttribTags::kCompilerVersion,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetCompilerVersionString,
	SceShaderMetadataSectionHandle,
	AttribTags::kCompilerVersionString,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetComputeNumThreadX,
	SceShaderMetadataSectionHandle,
	AttribTags::kNumThreadX,
	int32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetComputeNumThreadY,
	SceShaderMetadataSectionHandle,
	AttribTags::kNumThreadY,
	int32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetComputeNumThreadZ,
	SceShaderMetadataSectionHandle,
	AttribTags::kNumThreadZ,
	int32_t,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderGetUseThreadDimension,
	SceShaderMetadataSectionHandle,
	AttribTags::kHasUseThreadDimension)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetVertexVariant,
	SceShaderMetadataSectionHandle,
	AttribTags::kShaderVariant,
	SceShaderVertexVariant,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderHasObjPrimId,
	SceShaderMetadataSectionHandle,
	AttribTags::kHasObjPrimId)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderHasDiscard,
	SceShaderMetadataSectionHandle,
	AttribTags::kHasDiscard)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderHasZWrite,
	SceShaderMetadataSectionHandle,
	AttribTags::kHasDepthWrite)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR_ROOT(
	sceShaderHasPops,
	SceShaderMetadataSectionHandle,
	AttribTags::kHasPops)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetExportColorFmtMask,
	SceShaderMetadataSectionHandle,
	AttribTags::kExportFormat,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetZOrder,
	SceShaderMetadataSectionHandle,
	AttribTags::kZOrder,
	SceShaderZOrder,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetConservativeZMode,
	SceShaderMetadataSectionHandle,
	AttribTags::kConservativeZExport,
	SceShaderConservativeZMode,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderExportColorFmt sceShaderGetExportColorFmt(
	SceShaderMetadataSectionHandle metadata,
	uint32_t index)
{
	uint32_t const mask = sceShaderGetExportColorFmtMask(metadata);
	return SceShaderExportColorFmt(0xf & (mask >> (index * 4)));
}


// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetDomainVariant,
	SceShaderMetadataSectionHandle,
	AttribTags::kShaderVariant,
	SceShaderDomainVariant,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetDomainPatchType,
	SceShaderMetadataSectionHandle,
	AttribTags::kPatchType,
	SceShaderPatchType,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetDomainInputControlPointCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kInputCtrlPointCount,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullPatchType,
	SceShaderMetadataSectionHandle,
	AttribTags::kPatchType,
	SceShaderPatchType,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullTopologyType,
	SceShaderMetadataSectionHandle,
	AttribTags::kOutputTopologyType,
	SceShaderTopologyType,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullPartitionType,
	SceShaderMetadataSectionHandle,
	AttribTags::kPartitionType,
	SceShaderPartitioningType,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullInputControlPointCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kInputCtrlPointCount,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullOutputControlPointCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kOutputCtrlPointCount,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullPatchSize,
	SceShaderMetadataSectionHandle,
	AttribTags::kPatchType,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetHullMaxTessLevel,
	SceShaderMetadataSectionHandle,
	AttribTags::kMaxTessFactor,
	float,
	GetFloatAttribute,
	.0f)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetGsInstanceCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kInstanceCount,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetGsMaxVertexCount,
	SceShaderMetadataSectionHandle,
	AttribTags::kMaxVertexCount,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetGsInputPrimType,
	SceShaderMetadataSectionHandle,
	AttribTags::kInputType,
	SceShaderGsPrimType,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR_ROOT(
	sceShaderGetGsOutputPrimType,
	SceShaderMetadataSectionHandle,
	AttribTags::kOutputType,
	SceShaderGsPrimType,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderAnnotationListHandle sceShaderGetAnnotationList(
	SceShaderMetadataSectionHandle metadata)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(metadata))
		return nullptr;
	auto shd = GetFirstChildNodeWithTag(metadata, GetRootNode(metadata), NodeTags::kShader);
	return d::To<SceShaderAnnotationListHandle>(GetFirstChildNodeWithTag(metadata, shd, NodeTags::kAnnotationList));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstAnnotation,
	SceShaderAnnotationHandle,
	SceShaderMetadataSectionHandle,
	SceShaderAnnotationListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextAnnotation,
	SceShaderAnnotationHandle,
	SceShaderMetadataSectionHandle,
	SceShaderAnnotationHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetAnnotationValue,
	SceShaderMetadataSectionHandle,
	SceShaderAnnotationHandle,
	AttribTags::kName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderResourceListHandle sceShaderGetResourceList(
	SceShaderMetadataSectionHandle metadata)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(metadata))
		return nullptr;
	auto shd = GetFirstChildNodeWithTag(metadata, GetRootNode(metadata), NodeTags::kShader);
	if (0 == shd)
		return nullptr;
	return d::To<SceShaderResourceListHandle>(GetFirstChildNodeWithTag(metadata, shd, NodeTags::kReflection));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstResource,
	SceShaderResourceHandle,
	SceShaderMetadataSectionHandle,
	SceShaderResourceListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextResource,
	SceShaderResourceHandle,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderResourceClass sceShaderGetResourceClass(
	SceShaderMetadataSectionHandle binHandle,
	SceShaderResourceHandle resourceHandle)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(binHandle) || nullptr == resourceHandle)
		return SceShaderUnknownResource;
	auto const tag = GetNodeTag(binHandle, d::From(resourceHandle));
	switch (tag)
	{
	case NodeTags::kInputAttribute:		return SceShaderInputAttribute;
	case NodeTags::kOutputAttribute:	return SceShaderOutputAttribute;
	case NodeTags::kSrt:				return SceShaderSrt;
	case NodeTags::kConstantBuffer:		return SceShaderCb;
	case NodeTags::kSamplerState:		return SceShaderSamplerState;
	case NodeTags::kSrv:				return SceShaderSrv;
	case NodeTags::kUav:				return SceShaderUav;
	case NodeTags::kStreamOut:			return SceShaderStreamOut;
	case NodeTags::kGlobalSrt:			return SceShaderGlobalSrt;
	default:							return SceShaderUnknownResource;
	}
}

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetResourceName,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetResourceApiSlot,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kResourceIndex,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetResourceType,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kTypeRef,
	SceShaderTypeHandle,
	GetNodeRefAttribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSemanticIndex,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kSemanticIndex,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetInterpolationClass,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kInterpolationClass,
	SceShaderInterpolationClass,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetCompressionMode,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kCompressionMode,
	SceShaderCompressionMode,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetParameterSpaceOffset,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kOffset,
	int,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSemanticClass,
	SceShaderMetadataSectionHandle,
	SceShaderResourceHandle,
	AttribTags::kSemanticId,
	SceShaderSemantic,
	GetUint64Attribute,
	SceShaderSemanticUser)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetSemanticName(
	SceShaderMetadataSectionHandle metadata,
	SceShaderResourceHandle resourceHandle)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	auto const id = sceShaderGetSemanticClass(metadata, resourceHandle);
	if (id != SceShaderSemanticUser)
		return sceShaderGetSemanticClassName(id);
	else
	{
		auto const attr = GetFirstAttributeWithTag(metadata, d::From(resourceHandle), AttribTags::kSemanticName);
		if (0 != attr)
			return GetStringAttribute(metadata, attr);
		else
			return sceShaderGetSemanticClassName(id);
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderTypeClass sceShaderGetTypeClass(
	SceShaderMetadataSectionHandle binHandle,
	SceShaderTypeHandle typeHandle)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(binHandle) || nullptr == typeHandle)
		return SceShaderUnknownType;
	auto const tag = GetNodeTag(binHandle, d::From(typeHandle));
	switch (tag)
	{
	case NodeTags::kVectorType:			return SceShaderVectorType;
	case NodeTags::kMatrixType:			return SceShaderMatrixType;
	case NodeTags::kArrayType:			return SceShaderArrayType;
	case NodeTags::kStructType:			return SceShaderStructType;
	case NodeTags::kFunctionShaderType:	return SceShaderFunctionShaderType;
	case NodeTags::kSamplerStateType:	return SceShaderSamplerStateType;
	case NodeTags::kPointerType:		return SceShaderPointerType;
	case NodeTags::kTextureType:		return SceShaderTextureType;
	case NodeTags::kBufferType:			return SceShaderBufferType;
	default:							return SceShaderUnknownType;
	}
}

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetTypeName,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetNumericClass,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kClass,
	SceShaderNumericClass,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetColumns,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kColumns,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetRows,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kRows,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderIsRowMajor,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kRowMajor)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetArraySize,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kSize,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetArrayElement,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kTypeRef,
	SceShaderTypeHandle,
	GetNodeRefAttribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetPointerElement,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kTypeRef,
	SceShaderTypeHandle,
	GetNodeRefAttribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetStructSize,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kSize,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstMember,
	SceShaderMemberHandle,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextMember,
	SceShaderMemberHandle,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderIsMemberUsed,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kIsUsed)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetMemberName,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetMemberOffset,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kOffset,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetMemberSize,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kSize,
	int32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetMemberType,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kTypeRef,
	SceShaderTypeHandle,
	GetNodeRefAttribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderIsSrtSignatureResource,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kOriginalResourceType)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSrtSignatureResourceSlotIndex,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kOriginalSlot,
	int32_t,
	GetInt64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSrtSignatureResourceSpaceIndex,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kOriginalSpace,
	int32_t,
	GetInt64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSrtSignatureResourceName,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kOriginalName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetSrtSignatureResourceClass,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kOriginalResourceType,
	SceShaderResourceClass,
	GetUint64Attribute,
	-1)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBufferClass,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kClass,
	SceShaderBufferClass,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderIsRwBuffer,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kIsRw)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetBufferElementType,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kTypeRef,
	SceShaderTypeHandle,
	GetNodeRefAttribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBufferEncoding,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kEncoding,
	SceShaderBufferEncoding,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBufferAddrShift,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kAddrShift,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBufferSizeShift,
	SceShaderMetadataSectionHandle,
	SceShaderTypeHandle,
	AttribTags::kSizeShift,
	uint32_t,
	GetUint64Attribute,
	0)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetDefaultValue,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kDefaultValue,
	void const*,
	GetDataBlockAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetDefaultValueSize,
	SceShaderMetadataSectionHandle,
	SceShaderMemberHandle,
	AttribTags::kDefaultValue,
	uint64_t,
	GetDataBlockAttributeSize,
	0)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderCfgSectionHandle sceShaderGetCfgSection(
	SceShaderBinaryHandle binHandle)
{
	namespace d = libsceshader_details;
	if (!d::VerifyElfHeader(binHandle))
		return 0;
	auto const cfg = d::FindElfSection(binHandle, d::ElfSections::kCfg);
	return reinterpret_cast<SceShaderCfgSectionHandle>(cfg.m_data);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderProgramListHandle sceShaderGetProgramList(
	SceShaderCfgSectionHandle cfg)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(cfg))
		return nullptr;
	return d::To<SceShaderProgramListHandle>(GetRootNode(cfg));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstProgram,
	SceShaderProgramHandle,
	SceShaderCfgSectionHandle,
	SceShaderProgramListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextProgram,
	SceShaderProgramHandle,
	SceShaderCfgSectionHandle,
	SceShaderProgramHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetRangeNumber,
	SceShaderCfgSectionHandle,
	SceShaderProgramHandle,
	AttribTags::kCfgRange,
	uint32_t,
	d::GetRangeCount,
	0u)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetRangeList,
	SceShaderCfgSectionHandle,
	SceShaderProgramHandle,
	AttribTags::kCfgRange,
	SceShaderOperandRange const *,
	GetDataBlockAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderBasicBlockListHandle
sceShaderGetBasicBlockList(
	SceShaderCfgSectionHandle cfg,
	SceShaderProgramHandle program)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(cfg))
		return nullptr;
	return d::To<SceShaderBasicBlockListHandle>(
		GetFirstChildNodeWithTag(
			cfg,
			d::From(program),
			NodeTags::kCfgBasicBlockList));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstBasicBlock,
	SceShaderBasicBlockHandle,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextBasicBlock,
	SceShaderBasicBlockHandle,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderBasicBlockRefListHandle
sceShaderGetBasicBlockSuccessorList(
	SceShaderCfgSectionHandle cfg,
	SceShaderBasicBlockHandle block)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(cfg))
		return nullptr;
	return d::To<SceShaderBasicBlockRefListHandle>(
		GetFirstChildNodeWithTag(
			cfg,
			d::From(block),
			NodeTags::kCfgBasicBlockSuccList));
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderBasicBlockRefListHandle
sceShaderGetBasicBlockVgprSuccessorList(
	SceShaderCfgSectionHandle cfg,
	SceShaderBasicBlockHandle block)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(cfg))
		return nullptr;
	return d::To<SceShaderBasicBlockRefListHandle>(
		GetFirstChildNodeWithTag(
			cfg,
			d::From(block),
			NodeTags::kCfgBasicBlockVgprSuccList));
}

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBasicBlockStartOffset,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle,
	AttribTags::kCfgBasicBlockStartOffset,
	uint32_t,
	GetUint64Attribute,
	0u)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBasicBlockEndOffset,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle,
	AttribTags::kCfgBasicBlockEndOffset,
	uint32_t,
	GetUint64Attribute,
	0u)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetBasicBlockNestingLevel,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle,
	AttribTags::kCfgBasicBlockRegionNesting,
	uint32_t,
	GetUint64Attribute,
	0u)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderIsBasicBlockUniform,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle,
	AttribTags::kCfgBasicBlockUniform)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderBasicBlockOverrideExecForReads,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle,
	AttribTags::kCfgBasicBlockOverrideExecForRead)

// ------------------------------------------------------------------------------------------------/
///
GET_BOOL_ATTR(
	sceShaderBasicBlockOverrideExecForWrites,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockHandle,
	AttribTags::kCfgBasicBlockOverrideExecForWrite)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE SceShaderFunctionListHandle
sceShaderGetFunctionList(
	SceShaderCfgSectionHandle cfg,
	SceShaderProgramHandle program)
{
	namespace d = libsceshader_details;
	using namespace sce::sl;
	if (!d::VerifyHeader(cfg))
		return nullptr;
	return d::To<SceShaderFunctionListHandle>(
		GetFirstChildNodeWithTag(
			cfg,
			d::From(program),
			NodeTags::kCfgFunctionList));
}

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstFunction,
	SceShaderFunctionHandle,
	SceShaderCfgSectionHandle,
	SceShaderFunctionListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextFunction,
	SceShaderFunctionHandle,
	SceShaderCfgSectionHandle,
	SceShaderFunctionHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetFunctionFirstBlock,
	SceShaderCfgSectionHandle,
	SceShaderFunctionHandle,
	AttribTags::kCfgFunctionFirstBlock,
	SceShaderBasicBlockHandle,
	GetNodeRefAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetFunctionLastBlock,
	SceShaderCfgSectionHandle,
	SceShaderFunctionHandle,
	AttribTags::kCfgFunctionLastBlock,
	SceShaderBasicBlockHandle,
	GetNodeRefAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_SIMPLE_ATTR(
	sceShaderGetFunctionName,
	SceShaderCfgSectionHandle,
	SceShaderFunctionHandle,
	AttribTags::kName,
	char const*,
	GetStringAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
GET_FIRST_CHILD(
	sceShaderGetFirstBasicBlockRef,
	SceShaderBasicBlockRefHandle,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockRefListHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NEXT_CHILD(
	sceShaderGetNextBasicBlockRef,
	SceShaderBasicBlockRefHandle,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockRefHandle)

// ------------------------------------------------------------------------------------------------/
///
GET_NODEREF_ATTR(
	sceShaderGetBasicBlock,
	SceShaderCfgSectionHandle,
	SceShaderBasicBlockRefHandle,
	AttribTags::kCfgBasicBlockRef,
	SceShaderBasicBlockHandle,
	GetNodeRefAttribute,
	nullptr)

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetProfileName(const SceShaderProfile profile)
{
	switch (profile)
	{
	case SceShaderProfileFunction:		return "function";
	case SceShaderProfilePrimitive:		return "primitive";
	case SceShaderProfilePrimitiveTess:	return "primitive_tess";
	case SceShaderProfileVertex:		return "vertex";
	case SceShaderProfilePixel:			return "pixel";
	case SceShaderProfileCompute:		return "compute";
	case SceShaderProfileGeometry:		return "geometry";
	case SceShaderProfileHull:			return "hull";
	case SceShaderProfileDomain:		return "domain";
	case SceShaderProfileMesh:			return "mesh";
	default:							return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetVertexVariantName(const SceShaderVertexVariant variant)
{
	switch (variant)
	{
	case SceShaderVertexVariantVertex:			return "vertex";
	case SceShaderVertexVariantExport:			return "export";
	case SceShaderVertexVariantLocal:			return "local";
	default:									return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetDomainVariantName(const SceShaderDomainVariant variant)
{
	switch (variant)
	{
	case SceShaderDomainVariantVertex:			return "vertex";
	case SceShaderDomainVariantExport:			return "export";
	default:									return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetGsPrimTypeName(const SceShaderGsPrimType primType)
{
	switch (primType)
	{
	case SceShaderGsPrimTypeTri:				return "triangle";
	case SceShaderGsPrimTypeLine:				return "line";
	case SceShaderGsPrimTypePoint:				return "point";
	case SceShaderGsPrimTypeAdjTri:				return "triangle with adjacency";
	case SceShaderGsPrimTypeAdjLine:			return "line with adjacency";
	default:									return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetPatchTypeName(const SceShaderPatchType patchType)
{
	switch (patchType)
	{
	case SceShaderPatchTypeIsoline:				return "isoline";
	case SceShaderPatchTypeTri:					return "tri";
	case SceShaderPatchTypeQuad:				return "quad";
	default:									return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetTopologyTypeName(const SceShaderTopologyType topologyType)
{
	switch (topologyType)
	{
	case SceShaderTopologyTypePoint:			return "point";
	case SceShaderTopologyTypeLine:				return "line";
	case SceShaderTopologyTypeCwtri:			return "cwtri";
	case SceShaderTopologyTypeCcwtri:			return "ccwtri";
	default:									return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetPartitionTypeName(const SceShaderPartitioningType partitionType)
{
	switch (partitionType)
	{
	case SceShaderPartitioningTypeInteger:		return "integer";
	case SceShaderPartitioningTypePowerOf2:		return "pow2";
	case SceShaderPartitioningTypeOddFactorial:	return "odd";
	case SceShaderPartitioningTypeEvenFactorial:return "even";
	default:									return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetNumericClassName(const SceShaderNumericClass numericClass)
{
	switch (numericClass)
	{
	case SceShaderFloat16:		return "half";
	case SceShaderFloat32:		return "float";
	case SceShaderFloat64:		return "double";
	case SceShaderInt8:			return "char";
	case SceShaderInt16:		return "short";
	case SceShaderInt32:		return "int";
	case SceShaderInt64:		return "long";
	case SceShaderUint8:		return "unsigned char";
	case SceShaderUint16:		return "unsigned short";
	case SceShaderUint32:		return "unsigned int";
	case SceShaderUint64:		return "unsigned long";
	case SceShaderBool:			return "bool";
	default:					return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetBufferClassName(const SceShaderBufferClass bufferClass)
{
	switch (bufferClass)
	{
	case SceShaderDataBuffer:		return "DataBuffer";
	case SceShaderRegularBuffer:	return "RegularBuffer";
	case SceShaderByteBuffer:		return "ByteBuffer";
	case SceShaderAppendBuffer:		return "AppendBuffer";
	case SceShaderConsumeBuffer:	return "ConsumeBuffer";
	case SceShaderConstantBuffer:	return "ConstantBuffer";
	case SceShaderTexture1d:		return "Texture1D";
	case SceShaderTexture2d:		return "Texture2D";
	case SceShaderTexture3d:		return "Texture3D";
	case SceShaderTexture1dR128:	return "Texture1D_R128";
	case SceShaderTexture2dR128:	return "Texture2D_R128";
	case SceShaderTextureCube:		return "TextureCube";
	case SceShaderTexture1dArray:	return "Texture1D_Array";
	case SceShaderTexture2dArray:	return "Texture2D_Array";
	case SceShaderTextureCubeArray:	return "TextureCube_Array";
	case SceShaderMsTexture2d:		return "MSTexture2D";
	case SceShaderMsTexture2dArray:	return "MSTexture2D_Array";
	case SceShaderMsTexture2dR128:	return "MSTexture2D_R128";
	default:						return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetResourceClassName(SceShaderResourceClass resourceClass)
{
	switch (resourceClass)
	{
	case SceShaderSrt:				return "srt";
	case SceShaderCb:				return "cb";
	case SceShaderUav:				return "uav";
	case SceShaderSrv:				return "srv";
	case SceShaderInputAttribute:	return "input";
	case SceShaderOutputAttribute:	return "output";
	case SceShaderSamplerState:		return "sampler";
	case SceShaderStreamOut:		return "streamout";
	case SceShaderGlobalSrt:		return "globalsrt";
	case SceShaderUnknownResource:
	default:						return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const *sceShaderGetSemanticClassName(const SceShaderSemantic semanticClass)
{
	switch (semanticClass)
	{
	case SceShaderSemanticEdgeTessFactor:			return "S_EDGE_TESS_FACTOR";
	case SceShaderSemanticInsideTessFactor:			return "S_INSIDE_TESS_FACTOR";
	case SceShaderSemanticDomainLocation:			return "S_DOMAIN_LOCATION";
	case SceShaderSemanticPrimitiveId:				return "S_PRIMITIVE_ID";
	case SceShaderSemanticDispatchThreadId:			return "S_DISPATCH_THREAD_ID";
	case SceShaderSemanticGroupId:					return "S_GROUP_ID";
	case SceShaderSemanticGroupIndex:				return "S_GROUP_INDEX";
	case SceShaderSemanticGroupThreadId:			return "S_GROUP_THREAD_ID";
	case SceShaderSemanticOutputControlPointId:		return "S_OUTPUT_CONTROL_POINT_ID";
	case SceShaderSemanticVertexId:					return "S_VERTEX_ID";
	case SceShaderSemanticInstanceId:				return "S_INSTANCE_ID";
	case SceShaderSemanticVertexOffsetId:			return "S_VERTEX_OFFSET_ID";
	case SceShaderSemanticInstanceOffsetId:			return "S_INSTANCE_OFFSET_ID";
	case SceShaderSemanticPosition:					return "S_POSITION";
	case SceShaderSemanticStereoPosition:			return "S_STEREO_POSITION";
	case SceShaderSemanticFrontFace:				return "S_FRONT_FACE";
	case SceShaderSemanticCoverage:					return "S_COVERAGE";
	case SceShaderSemanticSampleIndex:				return "S_SAMPLE_INDEX";
	case SceShaderSemanticEyeIndex:					return "S_EYE_INDEX";
	case SceShaderSemanticRenderTargetIndex:		return "S_RENDER_TARGET_INDEX";
	case SceShaderSemanticViewportIndex:			return "S_VIEWPORT_INDEX";
	case SceShaderSemanticPointCoord:				return "S_POINT_COORD";
	case SceShaderSemanticTargetOutput:				return "S_TARGET_OUTPUT";
	case SceShaderSemanticTargetOutputMask:			return "S_TARGET_OUTPUT_MASK";
	case SceShaderSemanticDepthOutput:				return "S_DEPTH_OUTPUT";
	case SceShaderSemanticDepthLeOutput:			return "S_DEPTH_LE_OUTPUT";
	case SceShaderSemanticDepthGeOutput:			return "S_DEPTH_GE_OUTPUT";
	case SceShaderSemanticDepthStencilValue:		return "S_STENCIL_VALUE";
	case SceShaderSemanticDepthStencilOp:			return "S_STENCIL_OP";
	case SceShaderSemanticClipDistance:				return "S_CLIP_DISTANCE";
	case SceShaderSemanticCullDistance:				return "S_CULL_DISTANCE";
	case SceShaderSemanticObjPrimId:				return "S_OBJPRIM_ID";
	case SceShaderSemanticPointSize:				return "S_POINT_SIZE";
	case SceShaderSemanticGsInstanceId:				return "S_GSINSTANCE_ID";
	case SceShaderSemanticDrawId:					return "S_DRAW_ID";
	case SceShaderSemanticNggPrimitiveCount:		return "S_NGG_PRIMITIVE_COUNT";
	case SceShaderSemanticNggVertexCount:			return "S_NGG_VERTEX_COUNT";
	case SceShaderSemanticNggGroupVertexIndex:		return "S_NGG_GROUP_VERTEX_INDEX";
	case SceShaderSemanticNggGroupPrimitiveCount:	return "S_NGG_GROUP_PRIMITIVE_COUNT";
	case SceShaderSemanticNggGroupVertexCount:		return "S_NGG_GROUP_VERTEX_COUNT";
	case SceShaderSemanticNggWaveIndex:				return "S_NGG_WAVE_INDEX";
	case SceShaderSemanticNggGroupSize:				return "S_NGG_GROUP_SIZE";
	case SceShaderSemanticNggPrimPayload:			return "S_NGG_PRIM_PAYLOAD";
	case SceShaderSemanticNggOutputPrim:			return "S_NGG_OUTPUT_PRIMITIVE";
	case SceShaderSemanticNggOutputPrimPayload:		return "S_NGG_OUTPUT_PRIM_PAYLOAD";
	case SceShaderSemanticNggOutputVertCount:		return "S_NGG_OUTPUT_VERTEX_COUNT";
	case SceShaderSemanticNggOutputPrimCount:		return "S_NGG_OUTPUT_PRIMITIVE_COUNT";
	case SceShaderSemanticShadingRate:				return "S_SHADING_RATE";
	case SceShaderSemanticUser:						return "user";
	default:										return "unknown";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const* sceShaderGetExportColorFmtName(SceShaderExportColorFmt semantic)
{
	switch (semantic)
	{
	default:
	case SceShaderExportColorFmt_ZERO:			return "FMT_ZERO";
	case SceShaderExportColorFmt_32_R:			return "FMT_32_R";
	case SceShaderExportColorFmt_32_GR:			return "FMT_32_GR";
	case SceShaderExportColorFmt_32_AR:			return "FMT_32_AR";
	case SceShaderExportColorFmt_FP16_ABGR:		return "FMT_FP16_ABGR";
	case SceShaderExportColorFmt_UNORM16_ABGR:	return "FMT_UNORM16_ABGR";
	case SceShaderExportColorFmt_SNORM16_ABGR:	return "FMT_SNORM16_ABGR";
	case SceShaderExportColorFmt_UINT16_ABGR:	return "FMT_UINT16_ABGR";
	case SceShaderExportColorFmt_SINT16_ABGR:	return "FMT_SINT16_ABGR";
	case SceShaderExportColorFmt_32_ABGR:		return "FMT_32_ABGR";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const* sceShaderGetInterpolationClassName(SceShaderInterpolationClass interpolationClass)
{
	switch (interpolationClass)
	{
	case SceShaderInterpolationClass_Unknown:
	default:												return "unknown";
	case SceShaderInterpolationClass_PerspectiveCenter:		return "perspectivecenter";
	case SceShaderInterpolationClass_PerspectiveSample:		return "perspectivesample";
	case SceShaderInterpolationClass_PerspectiveCentroid:	return "perspectivecentroid";
	case SceShaderInterpolationClass_LinearCenter:			return "linearcenter";
	case SceShaderInterpolationClass_LinearSample:			return "linearsample";
	case SceShaderInterpolationClass_LinearCentroid:		return "linearcentroid";
	case SceShaderInterpolationClass_Flat:					return "flat";
	case SceShaderInterpolationClass_Custom:				return "custom";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const* sceShaderGetCompressionModeName(SceShaderCompressionMode mode)
{
	switch (mode)
	{
	case SceShaderCompressionMode_Unknown:
	default:								return "unknown";
	case SceShaderCompressionMode_None:		return "none";
	case SceShaderCompressionMode_Fp16:		return "fp16";
	case SceShaderCompressionMode_UNorm16:	return "unorm16";
	case SceShaderCompressionMode_SNorm16:	return "snorm16";
	case SceShaderCompressionMode_UInt16:	return "uint16";
	case SceShaderCompressionMode_Int16:	return "int16";
	case SceShaderCompressionMode_UNorm8:	return "unorm8";
	case SceShaderCompressionMode_SNorm8:	return "snorm8";
	case SceShaderCompressionMode_UInt8:	return "uint8";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const* sceShaderGetZOrderName(SceShaderZOrder zorder)
{
	switch (zorder)
	{
	default:
	case SceShaderZOrder_LateZ:				return "latez";
	case SceShaderZOrder_EarlyZThenLateZ:	return "earlyzthenlatez";
	case SceShaderZOrder_ReZ:				return "rez";
	case SceShaderZOrder_EarlyZThenReZ:		return "earlyzthenrez";
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SHADER_INTERFACE char const* sceShaderGetConservativeZModeName(SceShaderConservativeZMode mode)
{
	switch (mode)
	{
	default:
	case SceShaderConservativeZMode_AnyZ:			return "anyz";
	case SceShaderConservativeZMode_LessThanZ:		return "lessthanz";
	case SceShaderConservativeZMode_GreaterThanZ:	return "greaterthanz";
	}
}

#include <cassert>


namespace sce
{
namespace sl
{

// ------------------------------------------------------------------------------------------------/
/// Header
struct Header
{
	uint32_t  m_fourcc;				//< signature (domain specific)
	NodeHandle m_root;				//< handle to root node
	uint32_t m_size;				//< size from the start of the header
	uint32_t m_pad;
};

// ------------------------------------------------------------------------------------------------/
/// read an uleb and advance the input pointer
static inline uint64_t read_leb(uint8_t const *&ptr, bool is_signed)
{
	uint64_t v = 0, s = 0;
	uint8_t c;
	do {
		c = *ptr++;
		v = v | (uint64_t(c & 0x7f) << s);
		s += 7;
	} while (0 != (c & 0x80));
	if (is_signed && (s < 64) && (0 != (c & 0x40)))
		v |= 0 - (uint64_t(1) << s);
	return v;
}

// ------------------------------------------------------------------------------------------------/
/// read an uleb and advance the input pointer
static inline uint64_t read_uleb(uint8_t const *&ptr)
{
	return read_leb(ptr, false);
}

// ------------------------------------------------------------------------------------------------/
/// read an uleb and advance the input pointer
static inline uint64_t read_sleb(uint8_t const *&ptr)
{
	return read_leb(ptr, true);
}

// ------------------------------------------------------------------------------------------------/
/// make a const pointer inside the tree-store
static inline uint8_t const *make_ptr(void const *treeStore, uint64_t offset)
{
	return static_cast<uint8_t const*>(treeStore) + offset;
}

// ------------------------------------------------------------------------------------------------/
/// get offset from pointer
static inline uint64_t make_ofs(void const *treeStore, uint8_t const *ptr)
{
	return uint64_t(ptr - static_cast<uint8_t const*>(treeStore));
}

// ------------------------------------------------------------------------------------------------/
/// get offset to attribute data
static inline uint32_t get_attr_data_offset(AttrHandle ah)
{
	return ah & 0xffffffff;
}

// ------------------------------------------------------------------------------------------------/
/// get offset to attribute abberviation
static inline uint32_t get_attr_abbrev_offset(AttrHandle ah)
{
	return (ah >> 32) & 0xffffffff;
}

// ------------------------------------------------------------------------------------------------/
/// make an attribute handle
static inline AttrHandle make_attr_handle(uint32_t dataOffset, uint32_t abbrevOffset)
{
	return AttrHandle(dataOffset) | (AttrHandle(abbrevOffset) << 32);
}

// ------------------------------------------------------------------------------------------------/
///
struct NodeInfo
{
	NodeTag			m_tag;
	NodeHandle		m_sibling;
	NodeHandle		m_kids;
	uint8_t const*	m_attributeData;
	uint8_t const*	m_abbreviation;
};

// ------------------------------------------------------------------------------------------------/
///
static inline NodeInfo DecodeNodeInfo(void const *treeStore, NodeHandle node)
{
	auto dataPtr = make_ptr(treeStore, node);
	auto const abbrevOffset = read_uleb(dataPtr);
	auto abbrevPtr = make_ptr(treeStore, abbrevOffset);
	auto const tag = read_uleb(abbrevPtr);
	auto const hasSibling = 0 != (tag & 1);
	auto const hasKids = 0 != (tag & 2);
	auto const siblingOffset = hasSibling ? read_uleb(dataPtr) : 0u;
	auto const kidsOffset = hasKids ? read_uleb(dataPtr) : 0u;
	return {
		NodeTag(tag >> 2),				// node tag
		NodeHandle(siblingOffset),		// sibling
		NodeHandle(kidsOffset),			// kids
		dataPtr,						// start of attribute data
		abbrevPtr						// attribute abbrev
	};
}

// ------------------------------------------------------------------------------------------------/
///
struct AttribInfo
{
	AttrTag			m_tag;
	AttrForm		m_form;
};

// ------------------------------------------------------------------------------------------------/
///
static inline AttribInfo DecodeAttribInfo(void const *base, uint32_t abbrevOffset)
{
	auto const p = make_ptr(base, abbrevOffset);
	auto const tagAndForm = uint16_t(p[0]) | (uint16_t(p[1]) << 8);
	auto const tag = tagAndForm & 0xfff;
	auto const form = tagAndForm >> 0xc;
	return {AttrTag(tag), AttrForm(form)};
}

// ------------------------------------------------------------------------------------------------/
///
static uint64_t GetUlebAttribute(void const *base, AttrHandle attr)
{
	auto dataOfs = get_attr_data_offset(attr);
	auto dataPtr = make_ptr(base, dataOfs);
	return read_uleb(dataPtr);
}

// ------------------------------------------------------------------------------------------------/
///
static uint64_t GetSlebAttribute(void const *base, AttrHandle attr)
{
	auto dataOfs = get_attr_data_offset(attr);
	auto dataPtr = make_ptr(base, dataOfs);
	return read_sleb(dataPtr);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE uint32_t GetLibraryFourCC(void const *base)
{
	return reinterpret_cast<Header const*>(base)->m_fourcc;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE uint32_t GetLibrarySize(void const *base)
{
	return reinterpret_cast<Header const*>(base)->m_size;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeHandle GetRootNode(void const *base)
{
	return reinterpret_cast<Header const*>(base)->m_root;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeTag GetNodeTag(void const *treeStore, NodeHandle node)
{
	auto nodeInfo = DecodeNodeInfo(treeStore, node);
	return nodeInfo.m_tag;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeHandle GetFirstChildNode(void const *treeStore, NodeHandle node)
{
	auto nodeInfo = DecodeNodeInfo(treeStore, node);
	return nodeInfo.m_kids;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeHandle GetSiblingNode(void const *treeStore, NodeHandle node)
{
	auto nodeInfo = DecodeNodeInfo(treeStore, node);
	return nodeInfo.m_sibling;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE AttrHandle GetFirstAttribute(void const *base, NodeHandle node)
{
	auto nodeInfo = DecodeNodeInfo(base, node);
	if (0 == *nodeInfo.m_abbreviation)
		return AttrHandle(0);	// check for no attributes at all
	return make_attr_handle(	// make an attribute handle from a pair of data/abbrev offsets
		uint32_t(make_ofs(base, nodeInfo.m_attributeData)),
			uint32_t(make_ofs(base, nodeInfo.m_abbreviation)));
}

// ------------------------------------------------------------------------------------------------/
/// get the attribute form
SCE_SL_INTERFACE AttrHandle GetNextAttribute(void const *base, AttrHandle attr)
{
	auto const abbrevOffset = get_attr_abbrev_offset(attr);
	auto const attrInfo = DecodeAttribInfo(base, abbrevOffset);
	auto const abbrevDataPtr = make_ptr(base, uint64_t(abbrevOffset) + uint64_t(2));
	if (0 == *abbrevDataPtr)
		return AttrHandle(0);
	// otherwise we need to consume the data for this attribute
	auto dataOfs = get_attr_data_offset(attr);
	if (AttrForm::kBool == attrInfo.m_form)
		{ /* nothing to do*/ }
	else if (AttrForm::kFp32 == attrInfo.m_form)
		dataOfs += 4;
	else
	{	// handle leb/uleb/datablock/string
		// skipa a leb
		auto skipleb = [base](uint32_t ofs) { 
			while (0 != (0x80 & *((uint8_t const*)base + ofs)))
				ofs++;
			return 1 + ofs;
		};
		dataOfs = skipleb(dataOfs);
		if (AttrForm::kDataBlock == attrInfo.m_form)
			dataOfs = skipleb(dataOfs);
	}
	return make_attr_handle(	// make an attribute handle from a pair of data/abbrev offsets
		dataOfs,
		abbrevOffset + 2);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE AttrForm GetAttributeForm(void const *base, AttrHandle attr)
{
	auto const attrInfo = DecodeAttribInfo(base, get_attr_abbrev_offset(attr));
	return attrInfo.m_form;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE AttrTag GetAttributeTag(void const *base, AttrHandle attr)
{
	auto const attrInfo = DecodeAttribInfo(base, get_attr_abbrev_offset(attr));
	return attrInfo.m_tag;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE uint64_t GetUint64Attribute(void const *base, AttrHandle attr)
{
	auto const form = GetAttributeForm(base, attr);
	switch (form)
	{
	case AttrForm::kBool:	return uint64_t(1);
	case AttrForm::kULeb:	return (uint64_t)GetUlebAttribute(base, attr);
	case AttrForm::kSLeb:	return (uint64_t)GetSlebAttribute(base, attr);
	default:
		return 0;
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE int64_t GetInt64Attribute(void const *base, AttrHandle attr)
{
	auto const form = GetAttributeForm(base, attr);
	switch (form)
	{
	case AttrForm::kBool:	return int64_t(1);
	case AttrForm::kULeb:	return (int64_t)GetUlebAttribute(base, attr);
	case AttrForm::kSLeb:	return (int64_t)GetSlebAttribute(base, attr);
	default:
		return 0;
	}
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE float GetFloatAttribute(void const *base, AttrHandle attr)
{
	union { float f; uint32_t u; } _d;
	auto dataOfs = get_attr_data_offset(attr);
	auto p = make_ptr(base, dataOfs);
	_d.u = uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
	return _d.f;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE char const *GetStringAttribute(void const *base, AttrHandle attr)
{
	return reinterpret_cast<char const*>((uint8_t const*)base + GetUlebAttribute(base, attr));
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeHandle GetNodeRefAttribute(void const *base, AttrHandle attr)
{
	return NodeHandle(GetUlebAttribute(base, attr));
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE void const *GetDataBlockAttribute(void const *base, AttrHandle attr)
{
	auto const offset = GetUlebAttribute(base, attr);
	return reinterpret_cast<uint8_t const*>(base) + offset;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE uint64_t GetDataBlockAttributeSize(void const *base, AttrHandle attr)
{
	auto dataOfs = get_attr_data_offset(attr);
	auto dataPtr = make_ptr(base, dataOfs);
	read_uleb(dataPtr); // skip one uleb
	return read_uleb(dataPtr);
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeHandle GetFirstChildNodeWithTag(void const *base, NodeHandle node, NodeTag nodeTag)
{
	for (auto c = GetFirstChildNode(base, node); c; c = GetSiblingNode(base, c))
		if (nodeTag == GetNodeTag(base, c))
			return c;
	return 0;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE NodeHandle GetSiblingWithTag(void const *base, NodeHandle node, NodeTag nodeTag)
{
	for (auto c = GetSiblingNode(base, node); c; c = GetSiblingNode(base, c))
		if (nodeTag == GetNodeTag(base, c))
			return c;
	return 0;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE AttrHandle GetFirstAttributeWithTag(void const *base, NodeHandle node, AttrTag attrTag)
{
	for (auto c = GetFirstAttribute(base, node); c; c = GetNextAttribute(base, c))
		if (attrTag == GetAttributeTag(base, c))
			return c;
	return 0;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE AttrHandle GetNextAttributeWithTag(void const *base, AttrHandle attr, AttrTag attrTag)
{
	for (auto c = GetNextAttribute(base, attr); c; c = GetNextAttribute(base, c))
		if (attrTag == GetAttributeTag(base, c))
			return c;
	return 0;
}

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE bool ValidateShaderBinary(void const* data,
	uint64_t sizeInBytes,
	uint64_t metadataOffestInBytes,
	uint64_t footerOffsetinBytes,
	uint32_t expectedMetadataByteSize,
	uint32_t expectedCrc)
{
	if ((metadataOffestInBytes + sizeof(Header)) > sizeInBytes)
		return false;
	void const* metadata = (uint8_t*)data + metadataOffestInBytes;
	uint32_t lib4cc = GetLibraryFourCC(metadata);
	if (lib4cc != sl::kDefault4cc)
		return false;
	uint32_t libByteSize = GetLibrarySize(metadata);
	if (libByteSize != expectedMetadataByteSize)
		return false;
	if ((metadataOffestInBytes + libByteSize) > sizeInBytes)
		return false;
	uint32_t computedCrc = Crc32(data, footerOffsetinBytes);
	if (computedCrc != expectedCrc)
		return false;
	return true;
}

// ------------------------------------------------------------------------------------------------/
///
static constexpr uint32_t crctab32[256] =
{
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};

// ------------------------------------------------------------------------------------------------/
///
SCE_SL_INTERFACE uint32_t Crc32(const void* data, uint64_t length)
{
	const uint8_t * ptr = (const uint8_t*)data;
	uint32_t crc = ~0U;
	while (length)
	{
		crc = (crc >> 8) ^ crctab32[(crc & 0xFF) ^ *ptr];
		ptr++; length--;
	}
	return ~crc;
}

}
}

#endif
