#ifndef _ITF_OBJECTGROUP_H_
#define _ITF_OBJECTGROUP_H_

namespace ITF
{
    class Pickable;
    class SubSceneActor;

    class ObjectGroup
    {
    public:
        enum GroupIndex // entries are sorted by update priority
        {
            GroupIndex_Collision = 0,
            GroupIndex_Default,

            GroupIndex_Max              // must remain the last item
        };

        ITF_INLINE  void    addElement      (   Pickable* _elem ) {ITF_ASSERT(m_elements.find(_elem) == -1); m_elements.push_back(_elem);}
 //       void                removeElement   (   Pickable* _elem );
        void                update          (   f32 _elapsed    );
        void                postUpdate      (                   );
        void                draw            (   SafeArray<class LightComponent*>& _activeLights  );
        void                removeElement   ( BaseObject* _pObj);

        ITF_INLINE  void    addSubSceneAABBRefresh ( SubSceneActor* _elem ) {ITF_ASSERT(m_subsceneAABBrefresh.find(_elem) == -1); m_subsceneAABBrefresh.push_back(_elem); }

        ITF_INLINE const SafeArray<Pickable*>& GetElements() const { return m_elements; }
        ITF_INLINE SafeArray<Pickable*>& GetMutableElements() { return m_elements; }

    private:
        SafeArray<Pickable*>        m_elements;
        SafeArray<SubSceneActor*>   m_subsceneAABBrefresh;
    };

}

#endif // _ITF_OBJECTGROUP_H_
