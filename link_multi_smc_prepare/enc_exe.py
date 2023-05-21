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
