#include <freepoi/net/codec.h>
#include "test.h"

uint8_t buf[3];
const uint8_t n1[3] = { 0x00, 0x00, 0x01 };
const uint8_t n2[3] = { 0x65, 0x43, 0x21 };

int
main(void)
{
    void *p;
    print_test_message("Checking if the returned offset is correct");
    p = freepoi_encode_int24(buf, sizeof(buf), 0x01);
    is_equal(p, buf + 3);
    print_test_message("Checking if the encoded value is correct");
    memcmp_test(buf, n1, 3);
    print_test_message("Checking if values larger than 24 bits are truncated correct");
    freepoi_encode_int24(buf, sizeof(buf), 0x87654321);
    memcmp_test(buf, n2, 3);
    return ALL_CLEAR;
}
