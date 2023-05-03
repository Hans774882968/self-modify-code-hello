#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
typedef bool (*flag_func) (string);
typedef void (*cp_func) (void *);
typedef void (*smc_func) (void);
typedef unsigned char uint8;
#define rep(i,a,b) for(int i = (a);i <= (b);++i)
#define re_(i,a,b) for(int i = (a);i < (b);++i)
#define dwn(i,a,b) for(int i = (a);i >= (b);--i)

const int enc_key4[14] = {16, 113, 174, 24, 78, 9, 90, 95, 207, 146, 136, 125, 69, 203};

int main() {
    HMODULE h = LoadLibrary ("enc_e.dll");
    // 不知道为什么，必须强转才能过编译
    flag_func can_get_flag = (flag_func) GetProcAddress (h, "can_get_flag");
    cp_func change_page = (cp_func) GetProcAddress (h, "change_page");
    smc_func smc1 = (smc_func) GetProcAddress (h, "smc1");
    smc_func smc2 = (smc_func) GetProcAddress (h, "smc2");
    smc_func smc3 = (smc_func) GetProcAddress (h, "smc3");
    vector<void *> funcs = { (void *) can_get_flag, (void *) smc1, (void *) smc2, (void *) smc3};
    for (auto func : funcs) {
        change_page (func);
    }
    // 故意没有把 SMC 解密的代码封装为独立函数
    const int sz2 = 0xC5F - 0xB6C;
    uint8 *st = (uint8 *) smc3, *ed = (uint8 *) (smc3) + sz2;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key4[ (i - st) % 14];
    smc3();

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