#ifndef _ITF_CRC_H_
#define _ITF_CRC_H_

namespace ITF
{

    class CRC
    {
        static u32 crc32_table[256];

        /* * Build auxiliary table for parallel byte-at-a-time CRC-32. */ 
        #define ITF_CRC32_POLY 0x04c11db7 /* AUTODIN II, Ethernet, & FDDI */ 

        static void init() 
        {   
            for (int i = 0; i < 256; ++i)
            {
                u32 c = i << 24;
                for (int j = 8; j > 0; --j)
                {
                    c = c & 0x80000000 ? (c << 1) ^ ITF_CRC32_POLY : (c << 1);
                }
                crc32_table[i] = c;
            }
        }

    public:
        static u32 compute32(const unsigned char *buf, int len)
        {
            const unsigned char *p;
            u32 crc;
            if (!crc32_table[1]) /* if not already done, */
                CRC::init();
            /* build table */
            crc = 0xffffffff; 
            /* preload shift register, per CRC-32 spec */ 
            for (p = buf; len > 0; ++p, --len) 
                crc = (crc << 8) ^ CRC::crc32_table[(crc >> 24) ^ *p];
            return ~crc; /* transmit complement, per CRC-32 spec */ 
        }
    };


}//namespace ITF

#endif 
