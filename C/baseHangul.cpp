#define INVALID_INPUT -1

static const unsigned char padding[2] = { 0xC8, 0xE5 };     // padding char "»Â" (U+D750)

static const int _10BIT_MASK = 0x03FF;
static const int _08BIT_MASK = 0x00FF;

// KS X 1001 constants
static const int _ROW_LENGTH = 94;
static const int _ROW_OFFSET = 0x80 + 0x20 + 0x10;
static const int _COL_OFFSET = 0x80 + 0x20 + 0x01;

int encode(void *in, const int in_len, void *out, const int out_len)
{
    int i;

    int packet_cnt = ( in_len + 4 ) / 5;
    int target_len = packet_cnt * 4 * 2;        // output length

    int lastpacket_size = in_len % 5;

    const unsigned char *_in  = (const unsigned char *) in;
          unsigned char *_out = (      unsigned char *) out;

    // check length
    if(out_len <= 0) return target_len;
    if(in_len  <  0 || out_len < target_len || !in || !out) return INVALID_INPUT;

    // adjust lastpacket size. it should be in range of [1 5]
    if(lastpacket_size == 0) lastpacket_size = 5;

    // encode complete(5-byte) packets
    for(i = 0; i < packet_cnt - 1; ++i) {
        const unsigned char *__in  = _in  + 5 * i;
              unsigned char *__out = _out + 8 * i; // a packet = 4 korean chars = 8 bytes

        int b[4] = { 0, };

        // pack data
        b[0] = ( ( __in[0] << 2 ) | ( __in[1] >> 6 ) ) & _10BIT_MASK;
        b[1] = ( ( __in[1] << 4 ) | ( __in[2] >> 4 ) ) & _10BIT_MASK;
        b[2] = ( ( __in[2] << 6 ) | ( __in[3] >> 2 ) ) & _10BIT_MASK;
        b[3] = ( ( __in[3] << 8 ) | ( __in[4] >> 0 ) ) & _10BIT_MASK;

        // replace to korean
        __out[0] = b[0] / _ROW_LENGTH + _ROW_OFFSET;
        __out[1] = b[0] % _ROW_LENGTH + _COL_OFFSET;

        __out[2] = b[1] / _ROW_LENGTH + _ROW_OFFSET;
        __out[3] = b[1] % _ROW_LENGTH + _COL_OFFSET;

        __out[4] = b[2] / _ROW_LENGTH + _ROW_OFFSET;
        __out[5] = b[2] % _ROW_LENGTH + _COL_OFFSET;

        __out[6] = b[3] / _ROW_LENGTH + _ROW_OFFSET;
        __out[7] = b[3] % _ROW_LENGTH + _COL_OFFSET;
    }

    // do not process if in_len == 0
    if(packet_cnt)
    {
        unsigned char  __in[5] = { 0, };
        unsigned char *__out   = _out + 4 * 2 * i;

        int b[4] = { 0, };

        // copy last packet to local buffer
        for(int j = 0; j < lastpacket_size; ++j)
            __in[j] = *( _in + 5 * i + j );

        // pack data
        b[0] = ( ( __in[0] << 2 ) | ( __in[1] >> 6 ) ) & _10BIT_MASK;
        b[1] = ( ( __in[1] << 4 ) | ( __in[2] >> 4 ) ) & _10BIT_MASK;
        b[2] = ( ( __in[2] << 6 ) | ( __in[3] >> 2 ) ) & _10BIT_MASK;
        b[3] = ( ( __in[3] << 8 ) | ( __in[4] >> 0 ) ) & _10BIT_MASK;

        // handle incomplete packet
        if(lastpacket_size == 4)
            b[3] += 0x400;

        // replace to korean
        __out[0] = b[0] / _ROW_LENGTH + _ROW_OFFSET;
        __out[1] = b[0] % _ROW_LENGTH + _COL_OFFSET;

        __out[2] = b[1] / _ROW_LENGTH + _ROW_OFFSET;
        __out[3] = b[1] % _ROW_LENGTH + _COL_OFFSET;

        __out[4] = b[2] / _ROW_LENGTH + _ROW_OFFSET;
        __out[5] = b[2] % _ROW_LENGTH + _COL_OFFSET;

        __out[6] = b[3] / _ROW_LENGTH + _ROW_OFFSET;
        __out[7] = b[3] % _ROW_LENGTH + _COL_OFFSET;

        // handle incomplete packet
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
                // ALREADY HANDLED
            case 5:
                // DO NOTHING
                break;

            default:
                // OOPS SOMETHING WRONG...
                return -2;
        }
    }

    return target_len;
}

int decode(void *in, const int in_len, void *out, const int out_len)
{
    int i;

    int packet_cnt = in_len / 8;
    int target_len = packet_cnt * 5;

    int lastpacket_size = in_len % 8;           // should be zero
    
    const unsigned char *_in  = (const unsigned char *) in;
          unsigned char *_out = (      unsigned char *) out;

    // check length
    if(out_len <= 0) return target_len;
    if(in_len  <  0 || out_len < target_len || !in || !out) return INVALID_INPUT;

    if(lastpacket_size > 0) return INVALID_INPUT;

    // decode pakcet
    for(i = 0; i < packet_cnt - 1; ++i) {
        const unsigned char *__in  = _in  + 8 * i;
              unsigned char *__out = _out + 5 * i;

        int b[4] = { 0, };

        // revert from korean
        b[0] = ( __in[0] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[1] - _COL_OFFSET );
        b[1] = ( __in[2] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[3] - _COL_OFFSET );
        b[2] = ( __in[4] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[5] - _COL_OFFSET );
        b[3] = ( __in[6] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[7] - _COL_OFFSET );

        // unpack data
        __out[0] = (                 ( b[0] >> 2 ) ) & _08BIT_MASK;
        __out[1] = ( ( b[0] << 6 ) | ( b[1] >> 4 ) ) & _08BIT_MASK;
        __out[2] = ( ( b[1] << 4 ) | ( b[2] >> 6 ) ) & _08BIT_MASK;
        __out[3] = ( ( b[2] << 2 ) | ( b[3] >> 8 ) ) & _08BIT_MASK;
        __out[4] = ( ( b[3] << 0 )                 ) & _08BIT_MASK;
    }

    // handle last packet
    {
        unsigned char  __in[8] = { 0, };
        unsigned char *__out   = _out + 5 * i;

        int b[4] = { 0, };
        int len_dec = 0;

        // copy last packet to local buffer
        for(int j = 0; j < 8; ++j)
            __in[j] = *( _in + 8 * i + j );


        // padding handling
        if(__in[2] == padding[0] && __in[3] == padding[1]) { __in[2] = 0xB0; __in[3] = 0xA1; len_dec += 1; }
        if(__in[4] == padding[0] && __in[5] == padding[1]) { __in[4] = 0xB0; __in[5] = 0xA1; len_dec += 1; }
        if(__in[6] == padding[0] && __in[7] == padding[1]) { __in[6] = 0xB0; __in[7] = 0xA1; len_dec += 1; }

        // no     padding char = no    padding byte
        // single padding char = two   padding bytes
        // double padding char = three padding bytes
        // triple padding char = four  padding bytes
        if(len_dec) len_dec += 1;
        target_len -= len_dec;

        // revert from korean
        b[0] = ( __in[0] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[1] - _COL_OFFSET );
        b[1] = ( __in[2] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[3] - _COL_OFFSET );
        b[2] = ( __in[4] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[5] - _COL_OFFSET );
        b[3] = ( __in[6] - _ROW_OFFSET ) * _ROW_LENGTH + ( __in[7] - _COL_OFFSET );

        // padding handling
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
