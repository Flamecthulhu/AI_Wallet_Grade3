import math
import random

x = [1, 2, 3]

def xavier_weight_init(fan_in, fan_out):
    weight_limit = math.sqrt(6 / (fan_in + fan_out))
    weight = [[random.uniform(-weight_limit, weight_limit) for _ in range(fan_out)] for _ in range(fan_in)]
    return weight

def xavier_bias_init(fan_out):
    bias_limit = math.sqrt(1 / fan_out)
    bias = [random.uniform(-bias_limit, bias_limit) for _ in range(fan_out)]
    return bias

print(f"Weights init: {xavier_weight_init(len(x), 4)}") 
print(f"Bias init: {xavier_bias_init(4)}")

