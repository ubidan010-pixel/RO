#ifndef _CTRSPECIFICCONSTANTS_
#define _CTRSPECIFICCONSTANTS_

#define NB_FLUID_LEVELS (48 + 1)

#define MAX_VERTEX_BY_MESH      256
#define MAX_INDEX_BY_ELEMENT    1024
#define NB_ELEMENT_IN_VERTEX_BUFFER 6

#define MESH_DEFAULT_HDIV           2
#define MESH_DEFAULT_VDIV           48

#define USE_GR

#define LAYER_0_CELL_SIZE               20.0f
#define LAYER_0_MAX_HORIZ_CELLS         250
#define LAYER_0_MAX_VERT_CELLS          250
#define CELL_LAYERS_COUNT               4
#define CELL_LAYERS_SCALE               4
//#define USE_LOAD_RESOURCE_MAX_RANGE
#define CELLLAYERSSETTINGS_TARGET   CellsLayersSettings::CellTgt_3DS

#define RESOURCE_PHYSICAL_DEFAULTLIFETIME 600 //keep it for 10 secs assuming we are at 60fps

#endif // _CTRSPECIFICCONSTANTS_
