#include <freepoi/net/codec.h>
#include "test.h"

uint8_t buf[4];
uint8_t n1[4] = { 0x01, 0x02, 0x03, 0x04 };
uint8_t n2[4] = { 0x87, 0x65, 0x43, 0x21 };

int
main(void)
{
    void *p;
    print_test_message("Checking if the returned offset is correct");
    p = freepoi_encode_int32(buf, sizeof(buf), 0x01020304);
    is_equal(p, buf + 4);
    print_test_message("Checking if the encoded value is correct");
    memcmp_test(buf, n1, 4);
    print_test_message("Checking if values larger than 32 bits are truncated correct");
    freepoi_encode_int32(buf, sizeof(buf), 0xA987654321);
    memcmp_test(buf, n2, 4);
    return ALL_CLEAR;
}
