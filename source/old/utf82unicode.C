/*
    bytes | bits | representation
          1 |    7 | 0xxxxxxx
          2 |   11 | 110xxxxx 10xxxxxx
          3 |   16 | 1110xxxx 10xxxxxx 10xxxxxx
          4 |   21 | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

/* Reference: Unicode Transformation Format, Roman Czyborra
 * A little tweak on the code of course. */
unsigned int unicode2utf8(unsigned int uc)
{
    unsigned char u1, u2, u3, u4;
    unsigned int ret = 0;

    if(uc < 0x80) return uc;
    else if(uc < 0x800) {
        u2 = 0xC0 | uc >> 6;
        u1 = 0x80 | uc & 0x3F;
        ret = (u2 << 8) | u1;
        return ret;
    } else if(uc < 0x10000) {
        u3 = 0xE0 | uc >> 12;
        u2 = 0x80 | uc >> 6 & 0x3F;
        u1 = 0x80 | uc & 0x3F;
        ret = (u2 << 8) | u1;
        ret = (u3 << 16) | ret;
        return ret;
    } else if(uc < 0x200000) {
        u4 = 0xF0 | uc >> 18;
        u3 = 0x80 | uc >> 12 & 0x3F;
        u2 = 0x80 | uc >> 6 & 0x3F;
        u1 = 0x80 | uc & 0x3F;
        ret = (u2 << 8) | u1;
        ret = (u3 << 16) | ret;
        ret = (u4 << 24) | ret;
        return ret;
    }

    return 0;
}

/*
    bytes | bits | representation
          1 |    7 | 0xxxxxxx
          2 |   11 | 110xxxxx 10xxxxxx
          3 |   16 | 1110xxxx 10xxxxxx 10xxxxxx
          4 |   21 | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

/* Based on RFC2279 */
#define BYTE1(code) (code <= 0x7f)
#define BYTE2(code) ((code >> 5) & 0x6)
#define BYTE3(code) ((code >> 4) & 0xe)
#define BYTE4(code) ((code >> 3) & 0x1e)
#define CLEAR_MSB(code) (code & (~0x80))

unsigned int utf82unicode(unsigned int utf_char)
{
    unsigned int uni = 0;
    unsigned char u1, u2, u3, u4;

    u1 = utf_char & 0xff;
    u2 = (utf_char >> 8) & 0xff;
    u3 = (utf_char >> 16) & 0xff;
    u4 = (utf_char >> 24) & 0xff;

    if(BYTE4(u4)) {
        uni = ((u4 & 0x7) << 2) | ((u3 >> 4) & 0x3);
        uni = (uni << 8) | ((u3 & 0xf) << 4 | ((u2 >> 2) & 0xf));
        uni = (uni << 8) | ((u2 & 0x3) << 6 | (u1 & 0x3f));
    } else if(BYTE3(u3)) {
        uni = (u3 & 0xf);
        uni = (uni << 4) | ((u2 >> 2) & 0xf);
        uni = (uni << 8) | ((u2 & 0x3) << 6 | (u1 & 0x3f));
    } else if (BYTE2(u2)) {
        /* Ok, First six bit of the 1st byte goes to the first six bits in the
         * first byte. Then bits # 0, 1 in the 2nd byte goes to bits # 6, 7 in
         * the first byte. So we're done with the first byte.
         * Then bits # 2-4 from the 2nd byte goes to the bits # 0-2 in the 2nd
         * byte. So we're done!
         */
        uni = (u2 >> 2) & 0x7;
        uni = (uni << 8) | ((u2 & 0x3) << 6 | (u1 & 0x3f));
    } else if (BYTE1(u1)) uni = utf_char;

    return uni;
}
