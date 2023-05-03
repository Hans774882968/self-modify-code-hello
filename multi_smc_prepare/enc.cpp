#include "enc.h"

extern "C" __declspec (dllexport) void change_page (void *addr);
extern "C" __declspec (dllexport) bool can_get_flag (string s);
extern "C" __declspec (dllexport) void smc1 (void);
extern "C" __declspec (dllexport) void smc2 (void);
extern "C" __declspec (dllexport) void smc3 (void);
void internal_func();

void change_page (void *addr) {
    DWORD old;
    SYSTEM_INFO si;
    UINT64 _addr = (UINT64) addr;
    GetSystemInfo (&si);
    int page_size = si.dwPageSize;
    _addr -= (UINT64) addr % page_size;

    if (!VirtualProtect ( (PVOID) _addr, page_size, PAGE_EXECUTE_READWRITE, &old) )
        printf ("Error: %x\n", GetLastError() );

    return;
}

bool can_get_flag (string s) {
    internal_func();
    const int enc_arr[14] = {67, 91, 48, 88, 80, 75, 41, 93, 116, 92, 33, 83, 68, 69};
    const int private_key[4] = {43, 56, 68, 62};
    int n = s.size();
    if (n < 14) return false;
    re_ (i, 0, 14) {
        if ( (s[i] ^ private_key[i % 4]) != enc_arr[i]) return false;
    }
    return true;
}

void internal_func () {
    puts ("internal_func");
}

const int sz1 = 0xB50 - 0xA3D;
const int sz2 = 0xC5F - 0xB6C;

void smc1() {
    const int enc_key1[14] = {55, 7, 39, 227, 9, 26, 205, 157, 109, 61, 254, 56, 207, 69};
    uint8 *st = (uint8 *) can_get_flag, *ed = (uint8 *) (can_get_flag) + sz1;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key1[ (i - st) % 14];
    internal_func();
}

void smc2() {
    const int enc_key2[14] = {155, 254, 177, 21, 171, 126, 74, 154, 136, 15, 99, 93, 175, 252};
    uint8 *st = (uint8 *) smc1, *ed = (uint8 *) (smc1) + sz2;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key2[ (i - st) % 14];
    smc1();
}

void smc3() {
    const int enc_key3[14] = {40, 124, 172, 226, 104, 12, 179, 114, 101, 183, 45, 169, 121, 164};
    uint8 *st = (uint8 *) smc2, *ed = (uint8 *) (smc2) + sz2;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key3[ (i - st) % 14];
    smc2();
}