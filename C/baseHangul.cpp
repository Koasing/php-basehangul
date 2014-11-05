#define INVALID_INPUT -1

int encode(const unsigned char *in, const int in_len, unsigned char *out, const int out_len)
{
    const unsigned char padding[2] = { 0xC8, 0xE5 };     // padding char "»Â"

    int i;

    int packet_cnt = ( in_len + 4 ) / 5;
    int target_len = packet_cnt * 4 * 2;

    int lastpacket_size = in_len % 5;

    // check length
    if(out_len <= 0) return target_len;
    if(in_len < 0 || out_len < target_len || !in || !out) return INVALID_INPUT;

    // adjust lastpacket size... we will not process last packet in iteration
    if(lastpacket_size == 0) lastpacket_size = 5;

    // encode complete(5-byte) packet
    for(i = 0; i < packet_cnt - 1; ++i) {
        const unsigned char *__in = in + 5 * i;
        unsigned char *__out = out + 4 * 2 * i;

        int b[4] = { 0, };
        
        // copy data
        b[0] = ( ( __in[0] << 2 ) | ( __in[1] >> 6 ) ) & 0x03FF;
        b[1] = ( ( __in[1] << 4 ) | ( __in[2] >> 4 ) ) & 0x03FF;
        b[2] = ( ( __in[2] << 6 ) | ( __in[3] >> 2 ) ) & 0x03FF;
        b[3] = ( ( __in[3] << 8 ) | ( __in[4] >> 0 ) ) & 0x03FF;

        // replace to korean
        // 0x80 = KS-X-1001 offset
        // 0x30 = ROW OFFSET
        // 0x21 = COLUMN OFFSET
        __out[0] = b[0] / 94 + 0x80 + 0x30;
        __out[1] = b[0] % 94 + 0x80 + 0x21;

        __out[2] = b[1] / 94 + 0x80 + 0x30;
        __out[3] = b[1] % 94 + 0x80 + 0x21;

        __out[4] = b[2] / 94 + 0x80 + 0x30;
        __out[5] = b[2] % 94 + 0x80 + 0x21;

        __out[6] = b[3] / 94 + 0x80 + 0x30;
        __out[7] = b[3] % 94 + 0x80 + 0x21;
    }

    if(packet_cnt)
    {
        unsigned char  __in[5] = { 0, };
        unsigned char *__out = out + 4 * 2 * i;

        int b[4] = { 0, };

        // copy last packet to local buffer
        for(int j = 0; j < lastpacket_size; ++j)
            __in[j] = *( in + 5 * i + j );

        // pack data
        b[0] = ( ( __in[0] << 2 ) | ( __in[1] >> 6 ) ) & 0x03FF;
        b[1] = ( ( __in[1] << 4 ) | ( __in[2] >> 4 ) ) & 0x03FF;
        b[2] = ( ( __in[2] << 6 ) | ( __in[3] >> 2 ) ) & 0x03FF;
        b[3] = ( ( __in[3] << 8 ) | ( __in[4] >> 0 ) ) & 0x03FF;

        // handle special case
        if(lastpacket_size == 4)
            b[3] += 0x400;

        // replace to korean
        __out[0] = b[0] / 94 + 0x80 + 0x30;
        __out[1] = b[0] % 94 + 0x80 + 0x21;

        __out[2] = b[1] / 94 + 0x80 + 0x30;
        __out[3] = b[1] % 94 + 0x80 + 0x21;

        __out[4] = b[2] / 94 + 0x80 + 0x30;
        __out[5] = b[2] % 94 + 0x80 + 0x21;

        __out[6] = b[3] / 94 + 0x80 + 0x30;
        __out[7] = b[3] % 94 + 0x80 + 0x21;

        // process
        switch(lastpacket_size) {
            case 1:
                __out[2] = padding[0];
                __out[3] = padding[1];

            case 2:
                __out[4] = padding[0];
                __out[5] = padding[1];

            case 3:
                __out[6] = padding[0];
                __out[7] = padding[1];
                break;
                
            case 4:
            case 5:                             // DO NOTHING
                break;

            default:
                // OOPS SOMETHING WRONG...
                return -2;
        }
    }

    return target_len;
}

int decode(const unsigned char *in, const int in_len, unsigned char *out, const int out_len)
{
    const unsigned char padding[2] = { 0xC8, 0xE5 };     // padding char "»Â"

    int i;

    int packet_cnt = in_len / 8;
    int target_len = packet_cnt * 5;

    int lastpacket_size = in_len % 8;           // should be zero or dead

    if(out_len <= 0) return target_len;
    if(in_len < 0 || out_len < target_len || !in || !out) return INVALID_INPUT;
    if(lastpacket_size) return INVALID_INPUT;

    // decode pakcet
    for(i = 0; i < packet_cnt - 1; ++i) {
        const unsigned char *__in = in + 4 * 2 * i;
        unsigned char *__out = out + 5 * i;

        int b[4] = { 0, };

        // revert from korean
        b[0] = ( __in[0] - 0x80 - 0x30 ) * 94 + ( __in[1] - 0x80 - 0x21 );
        b[1] = ( __in[2] - 0x80 - 0x30 ) * 94 + ( __in[3] - 0x80 - 0x21 );
        b[2] = ( __in[4] - 0x80 - 0x30 ) * 94 + ( __in[5] - 0x80 - 0x21 );
        b[3] = ( __in[6] - 0x80 - 0x30 ) * 94 + ( __in[7] - 0x80 - 0x21 );

        // unpack data
        __out[0] = (                 ( b[0] >> 2 ) ) & 0xFF;
        __out[1] = ( ( b[0] << 6 ) | ( b[1] >> 4 ) ) & 0xFF;
        __out[2] = ( ( b[1] << 4 ) | ( b[2] >> 6 ) ) & 0xFF;
        __out[3] = ( ( b[2] << 2 ) | ( b[3] >> 8 ) ) & 0xFF;
        __out[4] = ( ( b[3] << 0 )                 ) & 0xFF;
    }

    // handle last packet
    {
        unsigned char  __in[8] = { 0, };
        unsigned char *__out = out + 5 * i;

        int b[4] = { 0, };
        int len_dec = 0;

        // copy last packet to local buffer
        for(int j = 0; j < 8; ++j)
            __in[j] = *( in + 4 * 2 * i + j );

        if(__in[2] == padding[0] && __in[3] == padding[1]) { __in[2] = 0xB0; __in[3] = 0xA1; len_dec += 1; }
        if(__in[4] == padding[0] && __in[5] == padding[1]) { __in[4] = 0xB0; __in[5] = 0xA1; len_dec += 1; }
        if(__in[6] == padding[0] && __in[7] == padding[1]) { __in[6] = 0xB0; __in[7] = 0xA1; len_dec += 1; }

        if(len_dec) len_dec += 1;
        target_len -= len_dec;

        // revert from korean
        b[0] = ( __in[0] - 0x80 - 0x30 ) * 94 + ( __in[1] - 0x80 - 0x21 );
        b[1] = ( __in[2] - 0x80 - 0x30 ) * 94 + ( __in[3] - 0x80 - 0x21 );
        b[2] = ( __in[4] - 0x80 - 0x30 ) * 94 + ( __in[5] - 0x80 - 0x21 );
        b[3] = ( __in[6] - 0x80 - 0x30 ) * 94 + ( __in[7] - 0x80 - 0x21 );

        // handling 
        if(b[3] >= 0x400) {
            b[3] -= 0x400;
            target_len -= 1;
        }

        // unpack data
        __out[0] = (                 ( b[0] >> 2 ) ) & 0xFF;
        __out[1] = ( ( b[0] << 6 ) | ( b[1] >> 4 ) ) & 0xFF;
        __out[2] = ( ( b[1] << 4 ) | ( b[2] >> 6 ) ) & 0xFF;
        __out[3] = ( ( b[2] << 2 ) | ( b[3] >> 8 ) ) & 0xFF;
        __out[4] = ( ( b[3] << 0 )                 ) & 0xFF;
    }
    
    return target_len;
}
