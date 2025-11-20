import torch
import torch.nn as nn
import torch.optim as optim
import time 
import csv

start_time = time.time()
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
#device = torch.device("cpu")
print(f"Using device: {device}")

inputs = []
labels = []

with open("Data\\training_data.csv", "r") as f:
    reader = csv.reader(f)
    next(reader)
    for row in reader:
        inputs.append(list(map(float, row[:18])))
        labels.append(int(row[-1]))

X = torch.tensor(inputs, dtype=torch.float32).to(device)
y = torch.tensor(labels, dtype=torch.long).to(device)

class SimpleMLP(nn.Module):
    def __init__(self):
        super().__init__()
        self.relu = nn.ReLU()
        self.fc1 = nn.Linear(18, 64)
        self.fc2 = nn.Linear(64, 32)
        self.fc3 = nn.Linear(32, 9)

    def forward(self, x):
        x = self.relu(self.fc1(x))    # fc1 + ReLU
        x = self.relu(self.fc2(x))
        x = self.fc3(x)
        return x


model = SimpleMLP().to(device)
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)

for i in range(100):
    print(f'round {i+1}')
    for epoch in range(200):
        model.train()
        optimizer.zero_grad()
        outputs = model(X)
        loss = criterion(outputs, y)
        loss.backward()
        optimizer.step()
        if epoch % 100 == 0:
            print(f"Epoch {epoch}, Loss: {loss.item():.8f}")

    model.eval()
    test_sample = [24.073544,20.699775,7,457,5,-1,-1,-1,1,-1,1,0,1,-1,0,0,0,1]
    with torch.no_grad():
        test_tensor = torch.tensor([test_sample], dtype=torch.float32).to(device)
        output = model(test_tensor)
        predicted = torch.argmax(output, dim=1).item()

    print(f"Predicted class index: {predicted}")

torch.save(model.state_dict(), "samplemlp_weights.pth")
end_time = time.time()
print(f"Elapsed time: {end_time-start_time:.3f} seconds")


