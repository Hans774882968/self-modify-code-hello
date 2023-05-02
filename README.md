[TOC]

# 【reverse】手把手带你基于dll实现SMC

## 引言

SMC，即self modifying code，自修改代码，逆向入门SMC可以看一下我的[题解](https://www.52pojie.cn/thread-1667202-1-1.html)。我打算实现一个类似于【网鼎杯2020青龙组】jocker的SMC方案。这个方案不需要用到汇编，因此门槛极低。为什么要基于dll呢？因为代码段加密功能是通过外部python脚本完成的，将自修改代码拆分为独立dll实现上更方便。

仓库：https://github.com/Hans774882968/self-modify-code-hello

编译器：g++8.1.0。

本文juejin：https://juejin.cn/post/7228629361652727845/

本文CSDN：https://blog.csdn.net/hans774882968/article/details/130469189

本文52pojie：https://www.52pojie.cn/thread-1780843-1-1.html

**作者：[hans774882968](https://blog.csdn.net/hans774882968)以及[hans774882968](https://juejin.cn/user/1464964842528888)以及[hans774882968](https://www.52pojie.cn/home.php?mod=space&uid=1906177)**

## 实现普通的dll调用

先编译一个dll。

`enc.h`：

```cpp
#pragma once

#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i = (a);i <= (b);++i)
#define re_(i,a,b) for(int i = (a);i < (b);++i)
#define dwn(i,a,b) for(int i = (a);i >= (b);--i)
```

`enc.cpp`：

```cpp
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
```

命令：

```bash
g++ enc.cpp -g -shared -o enc.dll
```

接下来写一个普通的控制台应用来调用dll，`main.cpp`：

```cpp
#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
typedef bool (*flag_func) (string);
#define rep(i,a,b) for(int i = (a);i <= (b);++i)
#define re_(i,a,b) for(int i = (a);i < (b);++i)
#define dwn(i,a,b) for(int i = (a);i >= (b);--i)

int main() {
    HMODULE h = LoadLibrary ("enc.dll");
    // 不知道为什么，必须强转才能过编译
    flag_func can_get_flag = (flag_func) GetProcAddress (h, "can_get_flag");
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
```

`main.cpp`不需要任何配置，直接编译即可。

## 实现基于dll的SMC

接下来在以上代码的基础上实现SMC。

首先打开IDA，查看`enc.dll`待加密函数`can_get_flag`的文件偏移（IDA下方状态栏会显示光标位置对应的文件偏移和Virtual Address）：`[0x9b0, 0xa66]`。然后写一个小脚本来实现`enc.dll`的修改（TODO：有佬教教我更优雅的做法嘛QAQ）：

```python
enc_key = [67, 91, 48, 88, 80, 75, 41, 93, 116, 92, 33, 83, 68, 69]


def enc(content, st, ed):
    res = []
    for i in range(st, ed + 1):
        res.append(content[i] ^ enc_key[(i - st) % len(enc_key)])
    return bytes(res)


st = 0x9b0
ed = 0xa66
with open('enc.dll', 'rb') as f1:
    content = f1.read()
    res = [content[:st]]
    enc_res = enc(content, st, ed)
    res.append(enc_res)
    res.append(content[ed + 1:])
    with open('enc_e.dll', 'wb') as f2:
        for r in res:
            f2.write(r)
```

最后修改一下`main.cpp`，加上：

1. 修改内存页为可写的代码，`change_page`函数。主要是用了Windows API`VirtualProtect`函数。
2. 运行时解密的代码。

```cpp
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
```

用x64dbg动态调试，进入`enc_e.dll`的内存空间，就能看到自修改代码的效果。

## 赏析：更简洁的SMC方案

在[GitHub](https://github.com/marcusbotacin/Self-Modifying-Code/blob/master/Examples/SMC2.cpp)看到的一种很简洁的SMC方案。原理很简单：在运行时修改一个函数（在这个例子中是`malicious`）的函数体，使得一个变量的初值改变，从而随心所欲地控制函数走向的分支。可以往上添加反调试逻辑，就体现了这种随心所欲。具体实现如下：

1. 首先写一段代码，在这个例子中是`change_page`函数，将SMC函数`malicious`所在的内存页修改为可写。
2. 然后编译一次，查看`malicious`的汇编代码，找到赋值语句对应汇编的位置，并修改初值。注意仓库作者没提到但很重要的一点：这一步是**和平台有关**的。这就是为什么需要编译至少2次。
3. 再编译一次。因为没有改动到`malicious`函数，所以它生成的汇编代码也不会改变。

我在原有代码的基础上做了些微不足道的修改。首先看代码：

```cpp
#include <stdio.h>   // I/O
#include <windows.h> // Mem

// supposed malicious function
void malicious (void) {
    // i is always zero
    int i = 0;
    // thus this check is always fail
    // unless SMC changes this value
    if (i == 0x114514)
        // this is the malicious behavior
        printf ("I'm a malware i = %08x\n", i);
    else printf ("not malware i = %d\n", i);
}

// simple anti analysis function
BOOL anti_analysis() {
    return IsDebuggerPresent();
}

// page permission, as in the previous examples
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

#define FUNC_ADJUST_OFFSET 0x08 // Function entry offset
#define INSTR_OFFSET 0x3        // instruction offset within the function
#define INSTR_DATA 0x114514          // new instruction bytes

// Program entry
int main() {
    // try first time
    // will be non-malicious
    malicious();

    //debug print
    printf ("PID %u\n", GetCurrentProcessId() );

    // anti-analysis trick
    if (!anti_analysis() ) {
        // SMC, as in the previous case
        void *func_addr = (char *) malicious + FUNC_ADJUST_OFFSET;
        change_page ( (void *) malicious);
        int *instruction = (int *) ( (char *) func_addr + INSTR_OFFSET);
        *instruction = INSTR_DATA;
    }

    // try second time
    // this time will be malicious
    malicious();

    return 0;
}
```

我们首先要编译一次，查看`malicious`的汇编代码：

```assembly
.text:0000000000401550                               ; __int64 malicious(void)
.text:0000000000401550                               public _Z9maliciousv
.text:0000000000401550                               _Z9maliciousv proc near                 ; CODE XREF: main+D↓p
.text:0000000000401550                                                                       ; main:loc_40174D↓p
.text:0000000000401550                                                                       ; DATA XREF: main+46↓o
.text:0000000000401550                                                                       ; .pdata:000000000040506C↓o
.text:0000000000401550
.text:0000000000401550                               var_4= dword ptr -4
.text:0000000000401550
.text:0000000000401550 55                            push    rbp
.text:0000000000401551 48 89 E5                      mov     rbp, rsp
.text:0000000000401554 48 83 EC 30                   sub     rsp, 30h
.text:0000000000401558 C7 45 FC 00 00 00 00          mov     [rbp+var_4], 0
.text:0000000000401558
.text:000000000040155F
.text:000000000040155F                               loc_40155F:                             ; DATA XREF: main+3B↓o
.text:000000000040155F 83 7D FC 00                   cmp     [rbp+var_4], 0
.text:0000000000401563 74 13                         jz      short loc_401578
.text:0000000000401563
.text:0000000000401565 8B 45 FC                      mov     eax, [rbp+var_4]
.text:0000000000401568 89 C2                         mov     edx, eax
.text:000000000040156A 48 8D 0D 8F 2A 00 00          lea     rcx, Format                     ; "I'm a malware i = %d\n"
.text:0000000000401571 E8 CA 16 00 00                call    printf
.text:0000000000401571
.text:0000000000401576 EB 11                         jmp     short loc_401589
.text:0000000000401576
.text:0000000000401578                               ; ---------------------------------------------------------------------------
.text:0000000000401578
.text:0000000000401578                               loc_401578:                             ; CODE XREF: malicious(void)+13↑j
.text:0000000000401578 8B 45 FC                      mov     eax, [rbp+var_4]
.text:000000000040157B 89 C2                         mov     edx, eax
.text:000000000040157D 48 8D 0D 92 2A 00 00          lea     rcx, aNotMalwareID              ; "not malware i = %d\n"
.text:0000000000401584 E8 B7 16 00 00                call    printf
```

我们的目标就是这句：`.text:0000000000401558 C7 45 FC 00 00 00 00 mov [rbp+var_4], 0`。于是可以修改代码里定义的`FUNC_ADJUST_OFFSET`和`INSTR_OFFSET`。修改好后再编译一次即可看到效果。

## 参考资料

1. https://www.cnblogs.com/rixiang/p/8954822.html
2. https://www.cygwin.com/cygwin-ug-net/dll.html