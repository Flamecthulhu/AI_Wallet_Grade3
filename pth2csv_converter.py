import torch
import torch.nn as nn
import csv

class SimpleMLP(nn.Module):
    def __init__(self):
        super().__init__()
        self.fc1 = nn.Linear(18, 32)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(32, 9)

    def forward(self, x):
        x = self.relu(self.fc1(x))
        return self.fc2(x)

# 載入模型架構與權重
model = SimpleMLP()
model.load_state_dict(torch.load("C:\\Users\\allan\\Programs\\Program_Grade3\\simplemlp_weights.pth", map_location="cpu"))
model.eval()

with open("weights_output.csv", "w", newline="") as f:
    writer = csv.writer(f)
    for name, param in model.named_parameters():
        writer.writerow([name])  # 欄位名稱
        data = param.detach().numpy()
        if data.ndim == 1:
            writer.writerow(data.tolist())  # 一維：用 writerow
        else:
            writer.writerows(data.tolist())  # 二維：用 writerows
