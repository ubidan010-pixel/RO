#ifndef ITF_BINFILEREADER_H
#define ITF_BINFILEREADER_H





namespace ITF 
{

    class BinFileReader
    {
    public:

        BinFileReader();
        ~BinFileReader();

        bool readFrom( const String & _path );

        const u64 getSize();
        void * getData();

    private:

        u64 m_size;
        void * m_data;
    };




} // namespace ITF 


#endif // ITF_BINFILEREADER_H