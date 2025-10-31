
#ifndef ITF_TEMPLATECLIENTHANDLER_H
#define ITF_TEMPLATECLIENTHANDLER_H


namespace ITF
{

class TemplateClientHandler
{
public:
                                ~TemplateClientHandler();
    void                        freeUsedTemplates();
    void                        addUsedTemplate(const TemplateClientHandler& _hdl);
    void                        addUsedTemplate(const StringID& _id);
    void                        removeUsedTemplate(const StringID& _id);
    bbool                       isUsedTemplate(const StringID& _id);
private:
    ITF_VECTOR<StringID>        m_usedTemplates;
};

}

#endif // ITF_TEMPLATECLIENTHANDLER_H
