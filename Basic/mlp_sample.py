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

with open("C:\\Users\\allan\\OneDrive\\Documents\\AI_Wallet_grade3\\AI_Wallet_Grade3\\Data\\training_data.csv", "r") as f:
    reader = csv.reader(f)
    next(reader)
    for row in reader:
        inputs.append(list(map(float, row[:18])))
        labels.append(int(row[-1]))

X = torch.tensor(inputs, dtype=torch.float32).to(device)
y = torch.tensor(labels, dtype=torch.long).to(device)

class SimpleMLP(nn.Module):
    def __init__(self, input_size, output_size):
        super().__init__()
        self.relu = nn.ReLU()
        self.fc1 = nn.Linear(input_size, 64)
        self.fc2 = nn.Linear(64, 32)
        self.fc3 = nn.Linear(32, output_size)

    def forward(self, x):
        x = self.relu(self.fc1(x))    # fc1 + ReLU
        x = self.relu(self.fc2(x))
        x = self.fc3(x)
        return x


input_size = X.shape[1]
num_classes = len(set(labels))
model = SimpleMLP(input_size, num_classes).to(device)
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
    test_sample = [24.073544,20.699775,7,457,5,-1,-1,-1,1]
    # Ensure test sample matches the model input size
    if len(test_sample) > input_size:
        test_sample = test_sample[:input_size]
    elif len(test_sample) < input_size:
        test_sample = test_sample + [0.0] * (input_size - len(test_sample))

    with torch.no_grad():
        test_tensor = torch.tensor([test_sample], dtype=torch.float32).to(device)
        output = model(test_tensor)
        predicted = torch.argmax(output, dim=1).item()

    print(f"Predicted class index: {predicted}")

torch.save(model.state_dict(), "samplemlp_weights.pth")
end_time = time.time()
print(f"Elapsed time: {end_time-start_time:.3f} seconds")


