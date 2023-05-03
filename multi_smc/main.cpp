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

const int enc_key_last[14] = { 197, 53, 171, 36, 108, 75, 19, 139, 151, 52, 100, 202, 41, 192 };

int main() {
    HMODULE h = LoadLibrary ("enc_e.dll");
    // 不知道为什么，必须强转才能过编译
    flag_func can_get_flag = (flag_func) GetProcAddress (h, "can_get_flag");
    cp_func change_page = (cp_func) GetProcAddress (h, "change_page");
    smc_func smc1 = (smc_func) GetProcAddress (h, "smc1");
    smc_func smc2 = (smc_func) GetProcAddress (h, "smc2");
    smc_func smc3 = (smc_func) GetProcAddress (h, "smc3");
    smc_func smc4 = (smc_func) GetProcAddress (h, "smc4");
    smc_func smc5 = (smc_func) GetProcAddress (h, "smc5");
    smc_func smc6 = (smc_func) GetProcAddress (h, "smc6");
    smc_func smc7 = (smc_func) GetProcAddress (h, "smc7");
    smc_func smc8 = (smc_func) GetProcAddress (h, "smc8");
    smc_func smc9 = (smc_func) GetProcAddress (h, "smc9");
    smc_func smc10 = (smc_func) GetProcAddress (h, "smc10");
    smc_func smc11 = (smc_func) GetProcAddress (h, "smc11");
    smc_func smc12 = (smc_func) GetProcAddress (h, "smc12");
    smc_func smc13 = (smc_func) GetProcAddress (h, "smc13");
    smc_func smc14 = (smc_func) GetProcAddress (h, "smc14");
    smc_func smc15 = (smc_func) GetProcAddress (h, "smc15");
    smc_func smc16 = (smc_func) GetProcAddress (h, "smc16");
    smc_func smc17 = (smc_func) GetProcAddress (h, "smc17");
    smc_func smc18 = (smc_func) GetProcAddress (h, "smc18");
    smc_func smc19 = (smc_func) GetProcAddress (h, "smc19");
    smc_func smc20 = (smc_func) GetProcAddress (h, "smc20");
    smc_func smc21 = (smc_func) GetProcAddress (h, "smc21");
    smc_func smc22 = (smc_func) GetProcAddress (h, "smc22");
    smc_func smc23 = (smc_func) GetProcAddress (h, "smc23");
    smc_func smc24 = (smc_func) GetProcAddress (h, "smc24");
    smc_func smc25 = (smc_func) GetProcAddress (h, "smc25");
    smc_func smc26 = (smc_func) GetProcAddress (h, "smc26");
    smc_func smc27 = (smc_func) GetProcAddress (h, "smc27");
    smc_func smc28 = (smc_func) GetProcAddress (h, "smc28");
    smc_func smc29 = (smc_func) GetProcAddress (h, "smc29");
    smc_func smc30 = (smc_func) GetProcAddress (h, "smc30");
    smc_func smc31 = (smc_func) GetProcAddress (h, "smc31");
    smc_func smc32 = (smc_func) GetProcAddress (h, "smc32");
    smc_func smc33 = (smc_func) GetProcAddress (h, "smc33");
    smc_func smc34 = (smc_func) GetProcAddress (h, "smc34");
    smc_func smc35 = (smc_func) GetProcAddress (h, "smc35");
    smc_func smc36 = (smc_func) GetProcAddress (h, "smc36");
    smc_func smc37 = (smc_func) GetProcAddress (h, "smc37");
    smc_func smc38 = (smc_func) GetProcAddress (h, "smc38");
    smc_func smc39 = (smc_func) GetProcAddress (h, "smc39");
    smc_func smc40 = (smc_func) GetProcAddress (h, "smc40");
    smc_func smc41 = (smc_func) GetProcAddress (h, "smc41");
    smc_func smc42 = (smc_func) GetProcAddress (h, "smc42");
    smc_func smc43 = (smc_func) GetProcAddress (h, "smc43");
    smc_func smc44 = (smc_func) GetProcAddress (h, "smc44");
    smc_func smc45 = (smc_func) GetProcAddress (h, "smc45");
    smc_func smc46 = (smc_func) GetProcAddress (h, "smc46");
    smc_func smc47 = (smc_func) GetProcAddress (h, "smc47");
    smc_func smc48 = (smc_func) GetProcAddress (h, "smc48");
    smc_func smc49 = (smc_func) GetProcAddress (h, "smc49");
    smc_func smc50 = (smc_func) GetProcAddress (h, "smc50");
    smc_func smc51 = (smc_func) GetProcAddress (h, "smc51");
    smc_func smc52 = (smc_func) GetProcAddress (h, "smc52");
    smc_func smc53 = (smc_func) GetProcAddress (h, "smc53");
    smc_func smc54 = (smc_func) GetProcAddress (h, "smc54");
    smc_func smc55 = (smc_func) GetProcAddress (h, "smc55");
    smc_func smc56 = (smc_func) GetProcAddress (h, "smc56");
    smc_func smc57 = (smc_func) GetProcAddress (h, "smc57");
    smc_func smc58 = (smc_func) GetProcAddress (h, "smc58");
    smc_func smc59 = (smc_func) GetProcAddress (h, "smc59");
    smc_func smc60 = (smc_func) GetProcAddress (h, "smc60");
    smc_func smc61 = (smc_func) GetProcAddress (h, "smc61");
    smc_func smc62 = (smc_func) GetProcAddress (h, "smc62");
    smc_func smc63 = (smc_func) GetProcAddress (h, "smc63");
    smc_func smc64 = (smc_func) GetProcAddress (h, "smc64");
    smc_func smc65 = (smc_func) GetProcAddress (h, "smc65");
    smc_func smc66 = (smc_func) GetProcAddress (h, "smc66");
    smc_func smc67 = (smc_func) GetProcAddress (h, "smc67");
    smc_func smc68 = (smc_func) GetProcAddress (h, "smc68");
    smc_func smc69 = (smc_func) GetProcAddress (h, "smc69");
    smc_func smc70 = (smc_func) GetProcAddress (h, "smc70");
    smc_func smc71 = (smc_func) GetProcAddress (h, "smc71");
    smc_func smc72 = (smc_func) GetProcAddress (h, "smc72");
    smc_func smc73 = (smc_func) GetProcAddress (h, "smc73");
    smc_func smc74 = (smc_func) GetProcAddress (h, "smc74");
    smc_func smc75 = (smc_func) GetProcAddress (h, "smc75");
    smc_func smc76 = (smc_func) GetProcAddress (h, "smc76");
    smc_func smc77 = (smc_func) GetProcAddress (h, "smc77");
    smc_func smc78 = (smc_func) GetProcAddress (h, "smc78");
    smc_func smc79 = (smc_func) GetProcAddress (h, "smc79");
    smc_func smc80 = (smc_func) GetProcAddress (h, "smc80");
    smc_func smc81 = (smc_func) GetProcAddress (h, "smc81");
    smc_func smc82 = (smc_func) GetProcAddress (h, "smc82");
    smc_func smc83 = (smc_func) GetProcAddress (h, "smc83");
    smc_func smc84 = (smc_func) GetProcAddress (h, "smc84");
    smc_func smc85 = (smc_func) GetProcAddress (h, "smc85");
    smc_func smc86 = (smc_func) GetProcAddress (h, "smc86");
    smc_func smc87 = (smc_func) GetProcAddress (h, "smc87");
    smc_func smc88 = (smc_func) GetProcAddress (h, "smc88");
    smc_func smc89 = (smc_func) GetProcAddress (h, "smc89");
    smc_func smc90 = (smc_func) GetProcAddress (h, "smc90");
    smc_func smc91 = (smc_func) GetProcAddress (h, "smc91");
    smc_func smc92 = (smc_func) GetProcAddress (h, "smc92");
    smc_func smc93 = (smc_func) GetProcAddress (h, "smc93");
    smc_func smc94 = (smc_func) GetProcAddress (h, "smc94");
    smc_func smc95 = (smc_func) GetProcAddress (h, "smc95");
    smc_func smc96 = (smc_func) GetProcAddress (h, "smc96");
    smc_func smc97 = (smc_func) GetProcAddress (h, "smc97");
    smc_func smc98 = (smc_func) GetProcAddress (h, "smc98");
    smc_func smc99 = (smc_func) GetProcAddress (h, "smc99");
    smc_func smc100 = (smc_func) GetProcAddress (h, "smc100");
    vector<void *> funcs = { (void *) can_get_flag, (void *) smc1, (void *) smc2, (void *) smc3, (void *) smc4, (void *) smc5, (void *) smc6, (void *) smc7, (void *) smc8, (void *) smc9, (void *) smc10, (void *) smc11, (void *) smc12, (void *) smc13, (void *) smc14, (void *) smc15, (void *) smc16, (void *) smc17, (void *) smc18, (void *) smc19, (void *) smc20, (void *) smc21, (void *) smc22, (void *) smc23, (void *) smc24, (void *) smc25, (void *) smc26, (void *) smc27, (void *) smc28, (void *) smc29, (void *) smc30, (void *) smc31, (void *) smc32, (void *) smc33, (void *) smc34, (void *) smc35, (void *) smc36, (void *) smc37, (void *) smc38, (void *) smc39, (void *) smc40, (void *) smc41, (void *) smc42, (void *) smc43, (void *) smc44, (void *) smc45, (void *) smc46, (void *) smc47, (void *) smc48, (void *) smc49, (void *) smc50, (void *) smc51, (void *) smc52, (void *) smc53, (void *) smc54, (void *) smc55, (void *) smc56, (void *) smc57, (void *) smc58, (void *) smc59, (void *) smc60, (void *) smc61, (void *) smc62, (void *) smc63, (void *) smc64, (void *) smc65, (void *) smc66, (void *) smc67, (void *) smc68, (void *) smc69, (void *) smc70, (void *) smc71, (void *) smc72, (void *) smc73, (void *) smc74, (void *) smc75, (void *) smc76, (void *) smc77, (void *) smc78, (void *) smc79, (void *) smc80, (void *) smc81, (void *) smc82, (void *) smc83, (void *) smc84, (void *) smc85, (void *) smc86, (void *) smc87, (void *) smc88, (void *) smc89, (void *) smc90, (void *) smc91, (void *) smc92, (void *) smc93, (void *) smc94, (void *) smc95, (void *) smc96, (void *) smc97, (void *) smc98, (void *) smc99, (void *) smc100, };
    for (auto func : funcs) {
        change_page (func);
    }
    // 故意没有把 SMC 解密的代码封装为独立函数
    uint8 *st = (uint8 *) smc100, *ed = (uint8 *) (smc100) + 0xf3;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key_last[ (i - st) % 14];
    smc100();

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