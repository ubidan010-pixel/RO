#ifndef _ITF_RAY_BOSSMORAYNODECOMPONENT_H_
#define _ITF_RAY_BOSSMORAYNODECOMPONENT_H_

namespace ITF
{
    // disables a node link, meaning the parent will never select the child node, until a node selector tells him to
    #define s_Disabled ITF_GET_STRINGID_CRC(Disabled,3798487816)

    class LinkComponent;
    class ChildEntry;

    class Ray_BossMorayNodeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMorayNodeComponent, ActorComponent,733639517)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossMorayNodeComponent();
        virtual ~Ray_BossMorayNodeComponent();

        //virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );


        typedef FixedArray<Ray_BossMorayNodeComponent*, 64> NodeList;
        class NodeData
        {
        public:
            NodeData()
                : m_node(NULL)
                , m_linkData(NULL)
            {}
            Ray_BossMorayNodeComponent* m_node;
            const ChildEntry* m_linkData;
        };
        typedef FixedArray<NodeData, 64> NodeDataList;

        void                getParentNodes( NodeList& _nodes ) const;
        void                getChildNodes( NodeList& _nodes ) const { getChildNodes(m_linkComponent, _nodes); }
        static void         getChildNodes( const LinkComponent* _linkComponent, NodeList& _nodes );
        void                getChildNodes( NodeDataList& _nodes ) const { getChildNodes(m_linkComponent, _nodes); }
        static void         getChildNodes( const LinkComponent* _linkComponent, NodeDataList& _nodes );
        bbool               hasParentNode() const;

        bbool               move( const Actor* _prevNode, const Actor* _nextNode, f32& _dist, Vec3d& _newPos );
        void                onNodeReached( Actor* _activator );


#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void        onEditorCreated( Actor* _original );

        // TODO: move all this crap to an editor plugin
        virtual bbool       needsUpdate() const { return btrue; }
        virtual void        Update( f32 _dt );

        ITF_INLINE static bbool getDrawDebug() { return s_drawDebug; }
        ITF_INLINE static void  setDrawDebug( bbool _value ) { s_drawDebug = _value; }
#else
        virtual bbool       needsUpdate() const { return bfalse; }
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_BossMorayNodeComponent_Template* getTemplate() const;

        LinkComponent* m_linkComponent;

        struct Segment
        {
            Segment( const Actor* _prev,
                     const Actor* _current,
                     const Actor* _next );

            bbool   getPosAtDist( f32& _dist, Vec3d& _pos ) const;
            void    getCurvePosAtT( const f32 _t, Vec3d& _pos ) const;  // debug only

        //private:
            bbool hasPrev;

            Vec3d p0;   // intersection prev-to-current
            Vec3d p1;   // current pos
            Vec3d p2;   // intersection current-to-next
            Vec3d p3;   // intersection next-to-current

            Vec3d dir12;
            f32 p12Length;  // start straight part (== if no prev)
            f32 p23Length;  // straight part
        };

#ifdef ITF_SUPPORT_EDITOR
        // TODO: move all this crap to an editor plugin

        /********************/
        /*      DEBUG       */
        /********************/

        static bbool s_drawDebug;

        typedef Ray_BossMorayNodeComponent Node;

        class DebugSegment
        {
        public:
            DebugSegment()
                : m_n0(NULL)
                , m_n1(NULL)
                , m_n2(NULL)
                , m_linkData(NULL)
                , m_isEnd(bfalse)
            {}

            const Node* m_n0; // prev
            const Node* m_n1; // cur
            const Node* m_n2; // next
            const ChildEntry* m_linkData;
            bbool m_isEnd;
        };
        typedef FixedArray<DebugSegment, 1024> DebugSegmentList;

        void        drawDebug() const;
        void        drawSegment( const Ray_BossMorayNodeComponent* n0, const Ray_BossMorayNodeComponent* n1, const Ray_BossMorayNodeComponent* n2, const ChildEntry& _linkData, const bbool _isEnd, const bbool _drawCorridor, const bbool _drawCircle ) const;

        typedef FixedArray<const Node*, 1024> ProcessedNodeList;
        void        buildNodeList( ProcessedNodeList& _processedNodes, DebugSegmentList& _segments, const Node* _prev ) const;
        bbool       buildChildrenList( ProcessedNodeList& _processedNodes, DebugSegmentList& _segments, const Node* _prev ) const;
#endif // ITF_SUPPORT_EDITOR
    };


    //---------------------------------------------------------------------

    class Ray_BossMorayNodeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMorayNodeComponent_Template, TemplateActorComponent,1261188335);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossMorayNodeComponent);

    public:

        Ray_BossMorayNodeComponent_Template();
        ~Ray_BossMorayNodeComponent_Template() {}

        const Ray_EventBossMorayNodeReached&    getEventNodeReached() const { return m_eventNodeReached; }
        f32                                     getDebugCorridorWidth() const { return m_debugCorridorWidth; }

    private:

        Ray_EventBossMorayNodeReached   m_eventNodeReached;
        f32                             m_debugCorridorWidth;
    };


    //---------------------------------------------------------------------

    ITF_INLINE const Ray_BossMorayNodeComponent_Template* Ray_BossMorayNodeComponent::getTemplate() const
    {
        return static_cast<const Ray_BossMorayNodeComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BOSSMORAYNODECOMPONENT_H_