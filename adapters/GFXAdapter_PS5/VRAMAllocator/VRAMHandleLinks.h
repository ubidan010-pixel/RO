#ifndef VRAMHANDLELINKS_H_
#define VRAMHANDLELINKS_H_

namespace ITF::Gnmp
{
    // any VRAM handle is an element of a double linked list (thus inherit from VRAMHandleLinks)
    struct VRAMHandleLinks
    {
        mutable const VRAMHandleLinks * next;
        mutable const VRAMHandleLinks * prev;
        bool isValid() const { return next!=0 || prev!=0; }
    };
}

#endif // VRAMHANDLELINKS_H_
