#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
typedef void (*smc_call_type) (void);
typedef unsigned char uint8;
#define rep(i,a,b) for(int i = (a);i <= (b);++i)
#define re_(i,a,b) for(int i = (a);i < (b);++i)
#define dwn(i,a,b) for(int i = (a);i >= (b);--i)

extern smc_call_type __my_smc_call_start;
extern smc_call_type __my_smc_call_end;

#define __func_section __attribute__ ( (section (".acmer1") ) )
#define func_ptr_init(func) __attribute__ ( (section (".acmer2") ) ) smc_call_type _fn_##func = func;

const int enc_key5[14] = {16, 113, 174, 24, 78, 9, 90, 95, 207, 146, 136, 125, 69, 203};

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

void can_get_flag() {
    puts ("Input flag:");
    string inp;
    cin >> inp;

    bool fl = true;
    int n = inp.size();
    const int enc_arr[28] = {233, 240, 201, 197, 244, 173, 193, 204, 228, 195, 197, 215, 190, 232, 193, 253, 188, 241, 203, 253, 235, 249, 197, 146, 208, 254, 209, 253};
    const int private_key[4] = {143, 156, 168, 162};
    if (n < 35) fl = false;
    re_ (i, 0, 28) {
        if ( (inp[i] ^ private_key[i % 4]) != enc_arr[i]) fl = false;
    }
    if (inp[28] * 2 != 102) fl = false;
    if (inp[29] * 2 != 198) fl = false;
    if (inp[30] * 2 != 234) fl = false;
    if (inp[31] * 2 + 1 != 199) fl = false;
    if (inp[32] * 2 + 1 != 233) fl = false;
    if (inp[33] * 3 + 1 != 307) fl = false;
    if (inp[34] + 40 != 165) fl = false;

    if (fl) {
        cout << "Congratulations! Your flag: " << inp << endl;
    } else {
        puts ("Incorrect flag");
    }
}

__func_section void null_fn() {}

__func_section void smc1() {
    change_page ( (void *) can_get_flag);
    const int enc_key1[14] = { 107, 50, 142, 41, 124, 82, 115, 38, 3, 94, 141, 186, 254, 226 };
    uint8 *st = (uint8 *) can_get_flag, *ed = (uint8 *) (can_get_flag) + 0x328;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key1[ (i - st) % 14];
}

__func_section void smc2() {
    const int enc_key2[14] = { 68, 41, 223, 212, 96, 89, 4, 193, 75, 254, 194, 112, 62, 7 };
    uint8 *st = (uint8 *) (* (&__my_smc_call_start + 1) ), *ed = (uint8 *) (* (&__my_smc_call_start + 1) ) + 0xfa;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key2[ (i - st) % 14];
    (* (&__my_smc_call_start + 1) ) ();
}

__func_section void smc3() {
    const int enc_key3[14] = { 129, 50, 171, 174, 220, 135, 228, 185, 10, 41, 190, 89, 84, 253 };
    uint8 *st = (uint8 *) (* (&__my_smc_call_start + 2) ), *ed = (uint8 *) (* (&__my_smc_call_start + 2) ) + 0x10F;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key3[ (i - st) % 14];
    (* (&__my_smc_call_start + 2) ) ();
}

__func_section void smc4() {
    const int enc_key4[14] = { 58, 137, 226, 153, 117, 216, 152, 29, 210, 117, 161, 16, 131, 126 };
    uint8 *st = (uint8 *) (* (&__my_smc_call_start + 3) ), *ed = (uint8 *) (* (&__my_smc_call_start + 3) ) + 0x10F;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key4[ (i - st) % 14];
    (* (&__my_smc_call_start + 3) ) ();
}

func_ptr_init (null_fn);
func_ptr_init (smc1);
func_ptr_init (smc2);
func_ptr_init (smc3);
func_ptr_init (smc4);

int main() {
    // 故意没有把 SMC 解密的代码封装为独立函数
    uint8 *st = (uint8 *) (* (&__my_smc_call_start + 4) ), *ed = (uint8 *) (* (&__my_smc_call_start + 4) ) + 0x10F;
    change_page (st);
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key5[ (i - st) % 14];
    (* (&__my_smc_call_start + 4) ) ();

    can_get_flag();
    return 0;
}