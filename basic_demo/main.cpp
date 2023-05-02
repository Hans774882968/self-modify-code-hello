#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
typedef bool (*flag_func) (string);
typedef unsigned char uint8;
#define rep(i,a,b) for(int i = (a);i <= (b);++i)
#define re_(i,a,b) for(int i = (a);i < (b);++i)
#define dwn(i,a,b) for(int i = (a);i >= (b);--i)

const int enc_key[14] = {67, 91, 48, 88, 80, 75, 41, 93, 116, 92, 33, 83, 68, 69};

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

int main() {
    HMODULE h = LoadLibrary ("enc_e.dll");
    // 不知道为什么，必须强转才能过编译
    flag_func can_get_flag = (flag_func) GetProcAddress (h, "can_get_flag");
    change_page ( (void *) can_get_flag);
    // 故意没有把 SMC 解密的代码封装为独立函数
    int sz = 0xa66 - 0x9b0;
    uint8 *st = (uint8 *) can_get_flag, *ed = (uint8 *) (can_get_flag) + sz;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key[ (i - st) % 14];

    puts ("Input flag:");
    string inp;
    cin >> inp;
    if (can_get_flag (inp) ) {
        cout << "Congratulations! Your flag: " << inp << endl;
    } else {
        puts ("Incorrect flag");
    }
    return 0;
}