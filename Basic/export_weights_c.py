import torch
import re

pth = r"C:\Users\allan\OneDrive\Documents\AI_Wallet_grade3\samplemlp_weights.pth"
out_header = r"c:\Users\allan\OneDrive\Documents\AI_Wallet_grade3\AI_Wallet_Grade3\Basic\fc_weights.h"

state = torch.load(pth, map_location='cpu')

def c_name(key):
    return re.sub(r"[^0-9a-zA-Z]+", "_", key)

def fmt_num(x):
    # format as C float literal
    return f"{float(x):.8e}f"

with open(out_header, 'w', encoding='utf-8') as f:
    f.write("/* Auto-generated header: fc weights + biases */\n")
    f.write("#ifndef FC_WEIGHTS_H\n#define FC_WEIGHTS_H\n\n")

    for key in ['fc1.weight','fc1.bias','fc2.weight','fc2.bias','fc3.weight','fc3.bias']:
        if key not in state:
            continue
        t = state[key].cpu()
        name = c_name(key)
        if t.dim() == 2:
            rows, cols = t.shape
            f.write(f"// {key} shape=({rows},{cols})\n")
            f.write(f"static const float {name}[{rows}][{cols}] = {{\n")
            for i in range(rows):
                row = t[i].tolist()
                vals = ", ".join(fmt_num(v) for v in row)
                f.write(f"    {{ {vals} }},\n")
            f.write("};\n\n")
        elif t.dim() == 1:
            length = t.shape[0]
            f.write(f"// {key} shape=({length},)\n")
            vals = ", ".join(fmt_num(v) for v in t.tolist())
            f.write(f"static const float {name}[{length}] = {{ {vals} }};\n\n")

    f.write("#endif // FC_WEIGHTS_H\n")

print(f"Wrote header: {out_header}")
