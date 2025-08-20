import math

def sigmoid(x):
    return 1 / (1 + math.exp(-x))

def tanh(x):
    return math.tanh(x)

def ReLU(x):
    return 1 if x > 0 else 0

def softmax(x):
    max_value = max(x)
    sum_exp = sum(math.exp(val - max_value) for val in x)
    return [math.exp(val - max_value) / sum_exp for val in x]

print(f"tanh(0): {tanh(0)}")
print(f"tanh(2): {tanh(2)}")
print(f"ReLU(0): {ReLU(0)}")
print(f"ReLU(2): {ReLU(2)}")
print(f"sigmoid(0):{sigmoid(0)}")
print(f"sigmoid(2): {sigmoid(2)}")
print(f"softmax([-1, 0, 1]): {softmax([-1, 0, 1])}")
print(f"softmax([-2, 0, 2]): {softmax([-2, 0, 2])}")