import torch

pth = r"C:\Users\allan\OneDrive\Documents\AI_Wallet_grade3\samplemlp_weights.pth"

state = torch.load(pth, map_location='cpu')

keys = ['fc1.weight','fc1.bias','fc2.weight','fc2.bias','fc3.weight','fc3.bias']
for k in keys:
    if k in state:
        t = state[k]
        print(f"{k} shape={tuple(t.shape)}")
        print(t)
        print()
    else:
        print(f"{k} not found in state_dict. Available keys: {list(state.keys())}")
