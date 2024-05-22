[TOC]

# 【reverse】手把手带你基于dll实现SMC

## 引言

![](./README_assets/1-谁会二进制逆向.jpg)

SMC，即self modifying code，自修改代码，逆向入门SMC可以看一下我的[题解](https://www.52pojie.cn/thread-1667202-1-1.html)。我打算实现一个类似于【网鼎杯2020青龙组】jocker的SMC方案。这个方案不需要用到汇编，因此门槛极低（~~连小小前端都能学会~~）。为什么要基于dll呢？因为代码段加密功能是通过外部python脚本完成的，将自修改代码拆分为独立dll实现上更方便。

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

思路很简单：先编译出`enc.dll`，接着写一个脚本修改`enc.dll`，最后写C++代码`main.cpp`调用`enc.dll`和解密`enc.dll`中的加密函数。`main.cpp`至少需要编译2次。

首先打开IDA，查看`enc.dll`待加密函数`can_get_flag`的文件偏移（IDA下方状态栏会显示光标位置对应的文件偏移和Virtual Address）：`[0x9b0, 0xa66]`。然后写一个小脚本来实现`enc.dll`的修改（TODO：有佬教教我更优雅的做法嘛QAQ）。

`enc_dll.py`

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

值得注意的是，`GetProcAddress`是根据dll的导出表来找到函数地址的，因此我们修改了`can_get_flag`的数据后，依旧能够找到其地址。

用x64dbg动态调试，进入`enc_e.dll`的内存空间，就能看到自修改代码的效果。

## 基于dll实现多次SMC

在[这里](https://github.com/pwnslinger/SMC)看到了一道看上去很难的题，它的第一步就是要解密多次SMC。即：解密一段代码后，发现这段代码也是SMC，于是需要继续解密。这就导致你的`IDApython`脚本需要写**递归**，更为复杂。我在这里打算实现一个多次SMC，同样不需要直接操作汇编，门槛极低。~~因为我比较菜~~，所以把这件事分为两步：前期准备、正式实现。前期准备把多次SMC的demo代码写好；正式实现则是参考上述demo代码，使用**模板引擎**来生成所需代码，期望的效果是：在第一次编译`enc.dll`后只需要进行**少量**人工操作就能进行第二次编译。

### 前期准备

为了实现多次SMC，我们需要考虑更多的问题：

1. 每个SMC函数的起点、终点如何确定。
2. 有许多函数需要导出，许多函数所在的内存空间要修改为可写……

问题2可以用代码生成技术解决，这就是为什么我选用了**模板引擎**。问题1比较棘手，我采用的可行但粗糙的做法是：让每个SMC函数结构相同，于是它们的大小也都相同。再通过读dll的导出表，来确定每个SMC函数的起点。这样每个SMC函数的起点和终点都可以确定。

操作过程：

1. 编写`enc.h, enc.cpp`，第一次编译`enc.dll`（编译命令同上）。
2. 查看函数大小，修改`enc.cpp`，第二次编译`enc.dll`。
3. 编写`enc_dll.py`，生成`enc_e.dll`。
4. 编写、编译`main.cpp`。

`enc.h`

```cpp
#pragma once

#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
typedef unsigned char uint8;
#define rep(i,a,b) for(int i = (a);i <= (b);++i)
#define re_(i,a,b) for(int i = (a);i < (b);++i)
#define dwn(i,a,b) for(int i = (a);i >= (b);--i)
```

`enc.cpp`

```cpp
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
```

注意点：

1. 这里的`smc1~3`需要尽量写成模板引擎容易实现的形式，即需要**高度相似**。
2. `enc_arr, enc_key1`等数组都应该放到栈里，从而能被加密，否则逆向难度极低。
3. `sz1, sz2`分别是`can_get_flag`和`smc1~3`的函数大小，需要先编译一次dll，看到大小以后再进行修改。这也是上面说`enc.dll`需要编译至少2次的原因。
4. `main.cpp`负责解密`smc3`，随后调用`smc3`实现所有函数的运行时解密。

`main.cpp`

```cpp
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
```

注意点：

1. 需要修改内存页为可写。
2. 需要运行时解密`smc3`，随后调用。

`enc_dll.py`

```python
import pefile

peobj = pefile.PE('enc.dll')


def get_smc_funcs():
    candidate_funcs = [[symb.name.decode('utf-8'), symb.address] for symb in peobj.DIRECTORY_ENTRY_EXPORT.symbols]
    smc_funcs = list(filter(lambda x: 'can_get_flag' in x[0] or 'smc' in x[0], candidate_funcs))
    return smc_funcs


enc_keys = {
    "can_get_flag": [55, 7, 39, 227, 9, 26, 205, 157, 109, 61, 254, 56, 207, 69],
    "smc1": [155, 254, 177, 21, 171, 126, 74, 154, 136, 15, 99, 93, 175, 252],
    "smc2": [40, 124, 172, 226, 104, 12, 179, 114, 101, 183, 45, 169, 121, 164],
    "smc3": [16, 113, 174, 24, 78, 9, 90, 95, 207, 146, 136, 125, 69, 203]
}


def enc(st, ed, enc_key):
    dat = peobj.get_data(st, ed - st + 1)
    res = bytes([v ^ enc_key[i % len(enc_key)] for i, v in enumerate(dat)])
    peobj.set_bytes_at_rva(st, res)


def main():
    smc_funcs = get_smc_funcs()
    print([[f[0], hex(f[1])] for f in smc_funcs])  # dbg
    sz1 = 0xB50 - 0xA3D
    sz2 = 0xC5F - 0xB6C
    for i, (func_name, st) in enumerate(smc_funcs):
        ed = st + (sz1 if func_name == 'can_get_flag' else sz2)
        enc_key = enc_keys[func_name]
        enc(st, ed, enc_key)
    res = peobj.write()
    with open('enc_e.dll', 'wb') as res_f:
        res_f.write(res)


if __name__ == '__main__':
    main()
```

和上一节《实现基于dll的SMC》不同，我改成了使用`pefile`包来进行dll的读写，这样代码会优雅得多。注意点：

1. `pefile`官方文档语焉不详，建议直接看源码，`<py安装路径>\Lib\site-packages\pefile.py`。
2. 我们需要从导出表读取函数起始地址，这也是为什么需要使用`pefile`包。从源码中看到，`peobj.DIRECTORY_ENTRY_EXPORT.symbols`是`ExportData[]`类型，`ExportData`的`address`就是我们需要的函数起始地址，它是`rva`。所以在实现代码的过程中，选用读写API时，挑选入参要求为`rva`的API即可。
3. `enc_keys`是为每个SMC函数随机生成的密钥数组，在后文《正式实现》中需要做到自动生成。

### 正式实现

有了前期准备，这一步就是纯体力活了。具体操作：

1. 我们需要使用**模板引擎**，来生成`enc.cpp, enc_dll.py, main.cpp`。这里选择了`jinja2`。记生成代码的文件名为`generate_files.py`，写好代码后运行一下，生成代码。
2. 编译`enc.h, enc.cpp`，获取`enc.dll`，查看`can_get_flag`和`smc1`的大小，修改`generate_files.py`，再次运行生成代码。接下来再次编译`enc.h, enc.cpp`生成`enc.dll`，然后运行`enc_dll.py`生成`enc_e.dll`。
3. 最后编译`main.cpp`即可。

相关代码都可以在[这里](https://github.com/Hans774882968/self-modify-code-hello/tree/main/multi_smc)查看。`generate_files.py`

```python
import jinja2
import random

SMC_NUM = 100
sz1 = 0xC9B - 0xA3D
sz2 = 0xDAA - 0xCB7
enc_key_obj_list = []
smc_func_names = []
smc_func_body_obj_list = []


def prepare_data():
    global smc_func_names
    for i in range(SMC_NUM + 1):
        random_enc_key = [random.randint(0, 255) for _ in range(14)]
        enc_key_obj_list.append({
            'name': ('smc%s' % i) if i else 'can_get_flag',
            'enc_key': random_enc_key,
            'enc_key_str': ', '.join([str(v) for v in random_enc_key])
        })
    smc_func_names = ['smc%s' % (i + 1) for i in range(SMC_NUM)]
    for i in range(SMC_NUM):
        # gen_main_cpp 用了 enc_key_obj_list[-1]['enc_key_str']，因此不需要再用
        smc_func_body_obj_list.append({
            'name': smc_func_names[i],
            'enc_key_str': enc_key_obj_list[i]['enc_key_str'],
            'to_decrypt': smc_func_names[i - 1] if i else 'can_get_flag',
            'to_call': smc_func_names[i - 1] if i else 'internal_func',
        })


def gen_enc_dll_py():
    fname = 'enc_dll.py.jinja'
    with open(fname, 'r', encoding='utf-8') as f:
        template = f.read()
        t = jinja2.Template(template)
        code = t.render(
            sz1=hex(sz1),
            sz2=hex(sz2),
            enc_key_obj_list=enc_key_obj_list
        )
        with open(fname[:-6], 'w', encoding='utf-8') as res_f:
            res_f.write(code)


def gen_main_cpp():
    fname = 'main.cpp.jinja'
    with open(fname, 'r', encoding='utf-8') as f:
        template = f.read()
        t = jinja2.Template(template)
        code = t.render(
            enc_key_last_str=enc_key_obj_list[-1]['enc_key_str'],
            smc_func_names=smc_func_names,
            sz2=hex(sz2)
        )
        with open(fname[:-6], 'w', encoding='utf-8') as res_f:
            res_f.write(code)


def gen_enc_cpp():
    fname = 'enc.cpp.jinja'
    with open(fname, 'r', encoding='utf-8') as f:
        template = f.read()
        t = jinja2.Template(template)
        code = t.render(
            smc_func_names=smc_func_names,
            sz1=hex(sz1),
            sz2=hex(sz2),
            smc_func_body_obj_list=smc_func_body_obj_list
        )
        with open(fname[:-6], 'w', encoding='utf-8') as res_f:
            res_f.write(code)


def main():
    prepare_data()
    gen_enc_dll_py()
    gen_main_cpp()
    gen_enc_cpp()


if __name__ == '__main__':
    main()
```

## 基于链接脚本+自定义section实现SMC

### 前期准备

我们知道：

1. 链接脚本可以在链接过程为exe提供数据，比如：一个段的起始地址和末尾地址。
2. `g++`允许我们用`__attribute__ ( (section (".acmer1") ) )`将变量或函数声明到一个自定义的段。

所以我们可以设计一个动态调用函数的方案，来增大逆向难度，灵感来自[参考链接3](https://blog.csdn.net/nyist327/article/details/59481809)。核心代码如下：

```cpp
typedef void (*smc_call_type) (void);
typedef unsigned char uint8;

extern smc_call_type __my_smc_call_start;
extern smc_call_type __my_smc_call_end;

#define __func_section __attribute__ ( (section (".acmer1") ) )
#define func_ptr_init(func) __attribute__ ( (section (".acmer2") ) ) smc_call_type _fn_##func = func;

__func_section void smc1() {
    change_page ( (void *) can_get_flag);
    const int enc_key1[14] = { 107, 50, 142, 41, 124, 82, 115, 38, 3, 94, 141, 186, 254, 226 };
    uint8 *st = (uint8 *) can_get_flag, *ed = (uint8 *) (can_get_flag) + 0x328;
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key1[ (i - st) % 14];
}

__func_section void smc2() {/* 函数体略 */}

func_ptr_init (smc1);
func_ptr_init (smc2);
```

这段代码：

1. 声明了由链接脚本提供的数据`__my_smc_call_start`和`__my_smc_call_end`。
2. 定义了在`.acmer2`段的函数指针`_fn_smc1, _fn_smc2`和在`.acmer1`段的函数`smc1, smc2`，前者指向后者。
3. 调用方式：`(* (&__my_smc_call_start + i) ) ();`，`&__my_smc_call_start + i`表示在`.acmer2`段取址，获得的是指向函数指针的指针，取一次值，就得到了函数指针。

`main.cpp`完整代码：

```cpp
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
    change_page (st); // .acmer1 块只有函数定义，所以编译结果默认是只读的
    for (uint8 *i = st; i <= ed; ++i) *i ^= enc_key5[ (i - st) % 14];
    (* (&__my_smc_call_start + 4) ) ();

    can_get_flag();
    return 0;
}
```

`smc1`是最后一个smc函数，那么为什么`smc1`没有像之前《基于dll实现多次SMC》一样调用`null_fn`呢？我遗憾地发现，因为`g++`编译器的优化，`(* (&__my_smc_call_start + 0) ) ();`生成的指令长度和`(* (&__my_smc_call_start + 1) ) ();`是不一样的，所以我们索性就允许这段代码出现3种指令长度，不妨命名为`sz_can_get_flag, sz_smc1, sz_other_smc`。

我们先用`ld --verbose`命令，导出`g++`使用的默认链接脚本。然后在定义`.bss`段的代码：

```
  .bss BLOCK(__section_alignment__) :
  {
    __bss_start__ = . ;
    *(.bss)
    *(COMMON)
    __bss_end__ = . ;
  }
```

之前添加自定义段的代码：

```lds
  .acmer1 BLOCK(__section_alignment__) : {*(.acmer1)}
  .acmer2 BLOCK(__section_alignment__) :
  {
    __my_smc_call_start = . ;
    *(.acmer2)
    __my_smc_call_end = . ;
  }
```

于是`&__my_smc_call_start`表示`.acmer2`段的起始地址。

编译命令：

```bash
g++ main.cpp -g -T main.lds -o main.exe
```

编译后和之前一样，只需要运行加密脚本对PE文件相关函数进行加密。

和之前章节不同，我将加密脚本拆成了两个文件`enc_exe.py, enc_exe_utils.py`，其中`enc_exe_utils.py`是不需要动态生成代码的部分，这不仅是为了clean code，也是在为后文使用模板引擎生成代码做准备。相比于《基于dll实现多次SMC》，我们需要多面对一个问题：如何方便地获取函数地址？

基于`dll`时，我们可以直接用导出表来读取，基于`exe`则只能通过**符号表**来读取。在命令行，可以用`nm -C main.exe`来获取符号表，但查阅咕果、chatgpt后我认为目前并没有一个能方便地读取PE文件符号表的python包。那么只能自己动手实现了！我们可以观察到`nm -C`的输出有3列：符号地址、符号类型和符号名，其中“符号地址”是虚拟地址VA，为了对接`pefile`包需要先转为相对虚拟地址RVA。我实现的相关函数：`read_symbol_table`。

`enc_exe.py`

```python
import pefile
from enc_exe_utils import get_smc_funcs, enc, get_output_file_name, run_strip

input_file_name = 'main.exe'
peobj = pefile.PE(input_file_name)
should_run_strip = False


enc_keys = {
    'can_get_flag': [107, 50, 142, 41, 124, 82, 115, 38, 3, 94, 141, 186, 254, 226],
    'smc1': [68, 41, 223, 212, 96, 89, 4, 193, 75, 254, 194, 112, 62, 7],
    'smc2': [129, 50, 171, 174, 220, 135, 228, 185, 10, 41, 190, 89, 84, 253],
    'smc3': [58, 137, 226, 153, 117, 216, 152, 29, 210, 117, 161, 16, 131, 126],
    'smc4': [16, 113, 174, 24, 78, 9, 90, 95, 207, 146, 136, 125, 69, 203]
}


def main():
    smc_funcs = get_smc_funcs(input_file_name, peobj)
    print([[f[0], hex(f[1])] for f in smc_funcs])  # dbg
    sz_can_get_flag = 0x328
    sz_smc1 = 0xfa
    sz_other_smc = 0x10F
    for i, (func_name, st) in enumerate(smc_funcs):
        ed = st + (sz_can_get_flag if func_name == 'can_get_flag' else (sz_smc1 if func_name == 'smc1' else sz_other_smc))
        enc_key = enc_keys[func_name]
        enc(st, ed, enc_key, peobj)
    res = peobj.write()
    output_file_name = get_output_file_name(input_file_name, '-enc')
    with open(output_file_name, 'wb') as res_f:
        res_f.write(res)
    if should_run_strip:
        run_strip(output_file_name)


if __name__ == '__main__':
    main()
```

`enc_exe_utils.py`

```python
import pefile
import subprocess
import os


def read_symbol_table(input_file_name, peobj=None):
    if not peobj:
        peobj = pefile.PE(input_file_name)
    p = subprocess.Popen(
        ['nm', '-C', input_file_name],
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        encoding='utf-8'
    )
    shell_output = p.communicate()[0]
    shell_output_arr = shell_output.split('\n')
    symbol_table = {}
    symbol_arr = []
    for ln in shell_output_arr:
        ln_arr = ln.split(' ')
        if len(ln_arr) < 3:
            continue
        [symbol_val, symbol_type, *rest] = ln_arr
        symbol_name = ' '.join(rest)
        symbol_table[symbol_name] = symbol_val
        symbol_arr.append((symbol_val, symbol_type, symbol_name))
    return symbol_table, symbol_arr


def get_smc_funcs(input_file_name, peobj=None):
    def remove_brackets(s):
        idx = s.find('(')
        return s if idx == -1 else s[:idx]
    symbol_table, symbol_arr = read_symbol_table(input_file_name, peobj)

    def str_va_to_rva(str_addr):
        return int(str_addr, 16) - peobj.OPTIONAL_HEADER.ImageBase
    smc_funcs = list(map(
        lambda x: (remove_brackets(x[2]), str_va_to_rva(x[0])),
        filter(lambda x: x[2] == 'can_get_flag()' or x[2].startswith('smc'), symbol_arr)
    ))
    return smc_funcs


def enc(st, ed, enc_key, peobj):
    dat = peobj.get_data(st, ed - st + 1)
    res = bytes([v ^ enc_key[i % len(enc_key)] for i, v in enumerate(dat)])
    peobj.set_bytes_at_rva(st, res)


def get_output_file_name(input_file_name, add_suffix=''):
    fname, extname = os.path.splitext(os.path.basename(input_file_name))
    return fname + add_suffix + extname


def run_strip(file_name):
    p = subprocess.Popen(
        ['strip', file_name],
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        encoding='utf-8'
    )


if __name__ == '__main__':
    input_file_name = 'main.exe'
    print(get_output_file_name(input_file_name, '_enc'))
```

flag：`flag{1ink_mu1ti_3mc_dem0_by_3cuctf}`

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
3. gcc的__attribute__编译属性有很多子项，用于改变作用对象的特性。这里讨论section子项的作用：https://blog.csdn.net/nyist327/article/details/59481809