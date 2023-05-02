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
