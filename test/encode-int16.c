#include <freepoi/net/codec.h>
#include "test.h"

uint8_t buf[2];
const uint8_t n1[2] = { 0x12, 0x13 };
const uint8_t n2[2] = { 0x43, 0x21 };

int main(void)
{
    void *p;
    print_test_message("Checking if the returned offset is correct");
    p = freepoi_encode_int16(buf, sizeof(buf), 0x1213);
    is_equal(p, buf + 2);
    print_test_message("Checking if the encoded value is correct");
    memcmp_test(buf, n1, 2);
    print_test_message("Checking if values larger than 16 bits are truncated correct");
    freepoi_encode_int16(buf, sizeof(buf), 0x654321);
    memcmp_test(buf, n2, 2);
    return ALL_CLEAR;
}
