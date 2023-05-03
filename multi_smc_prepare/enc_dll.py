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
