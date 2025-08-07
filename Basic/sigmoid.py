import math

def sigmoid(x):
    return 1 / (1 + math.exp(-x))

# Example usage
print(sigmoid(0))     # Output: 0.5
print(sigmoid(2))     # Output: ~0.88
