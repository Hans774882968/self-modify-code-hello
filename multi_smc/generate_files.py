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
