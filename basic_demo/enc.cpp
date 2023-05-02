#include "enc.h"

int enc_arr[14] = {67, 91, 48, 88, 80, 75, 41, 93, 116, 92, 33, 83, 68, 69};
int private_key[4] = {43, 56, 68, 62};

extern "C" __declspec (dllexport) bool can_get_flag (string s);
void internal_func();

bool can_get_flag (string s) {
    internal_func();
    int n = s.size();
    if (n != 14) return false;
    re_ (i, 0, n) {
        if ( (s[i] ^ private_key[i % 4]) != enc_arr[i]) return false;
    }
    return true;
}

void internal_func () {
    puts ("internal_func");
}