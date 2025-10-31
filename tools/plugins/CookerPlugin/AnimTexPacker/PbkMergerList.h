#ifndef _ITF_PBKMERGERLIST_H_
#define _ITF_PBKMERGERLIST_H_

#ifndef _ITF_PBKMERGER_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PbkMerger.h"
#endif

#ifndef _ITF_PATCHDEPOT_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PatchDepot.h"
#endif

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

namespace ITF
{
    class PbkRedirect;

    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///List of several PBKMergers, for storing several merged PBK (each item is thus independent)
        class PbkMergerList : public ITF_LIST<PbkMerger>
        {
        public:
            ///////////////////////////////////////////////////////////////////////////////////////////
            ~PbkMergerList();
            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Find pbkmerger which contains this template
            PbkMerger *getPbkMergerFromTemplateID(const StringID &_templateID);

            ///fill pbkRedirect
            void   fillPbkRedirect(const AnimPackerConfiguration &_config, PbkRedirect & _redirect);

            ///Build a PbkMergerList from the configuration and a patch depot
            static void buildPbkMergerList(int & _serialMergerIndex, const AnimPackerConfiguration &_config, PbkMergerList &_mergers, PatchDepot &_depot);

        };

    }
}

#endif
