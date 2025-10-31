#ifndef _ITF_COMMANDARGS_H_
#define _ITF_COMMANDARGS_H_


namespace ITF
{

    class CommandArgs
    {
        struct coupleKeyValue
        {
        public:
            String m_szCommand;
            String m_szValue;
        };

    public:

        ITF_VECTOR<coupleKeyValue> m_vKeyValue;

        CommandArgs(const String& _keyDefault,const String& _szName):m_szKeyDefault(_keyDefault),m_szCommandLine(_szName)
        {
            process();
        }

        void    parseValueKey(const char* arg);
        u32     getArgCount() const {return m_vKeyValue.size();}
        bbool   find(const String& _szKey,String& _szResult) const;
        bbool   isSet(const String& _szKey) const;
        i32     getValueI32(const String& _szKey,const i32 _default) const;
        void    process();

    protected:
        u32     find(const String& _szKey) const;

        String m_szCommandLine;
        String m_szKeyDefault;
   };

} // namespace ITF



#endif // _ITF_COMMANDARGS_H_