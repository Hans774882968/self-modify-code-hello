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
