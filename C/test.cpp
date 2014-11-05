#include <cstdio>
#include <cstdlib>
#include <cstring>

int encode(const unsigned char *in, const int in_len, unsigned char *out, const int out_len);
int decode(const unsigned char *in, const int in_len, unsigned char *out, const int out_len);

int main(int argc, char *argv[])
{
    char test[] = "TEST SEQUENCE";

    char *enc = NULL;
    char *dec = NULL;

    int testlen = strlen(test);

    int enclen = encode(NULL, testlen, NULL, 0);
    int declen = decode(NULL, enclen, NULL, 0);

    enc = new char[enclen];
    dec = new char[declen];

    encode((unsigned char *) test, testlen, (unsigned char *) enc, enclen);
    decode((unsigned char *) enc, enclen, (unsigned char *) dec, declen);

    delete[] enc;
    delete[] dec;
    
}