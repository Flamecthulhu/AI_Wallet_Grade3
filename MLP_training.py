#24.073273,120.701243,7,473,5,-1,-1,-1,1,-1,1,0,1,-1,0,0,0,1,0
import csv
import math
import random

x = []

with open('training_data.csv', newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    next(spamreader)
    for row in spamreader:
        x.append([float(v) for v in row])

def sigmoid(z):
    return 1 / (1 + math.exp(-z))

def xavier_weight_init(fan_in, fan_out):
    weight_limit = math.sqrt(6 / (fan_in + fan_out))
    weight = [[random.uniform(-weight_limit, weight_limit) for _ in range(fan_out)] for _ in range(fan_in)]
    return weight

def xavier_bias_init(fan_out):
    bias_limit = math.sqrt(1 / fan_out)
    bias = [random.uniform(-bias_limit, bias_limit) for _ in range(fan_out)]
    return bias

fan_in = len(x[0])
fan_out = 32

weight = xavier_weight_init(fan_in, fan_out)
bias = xavier_bias_init(fan_out)
output = [sum(x[0][i] * weight[i][j] for i in range(len(x[0]))) + bias[j]for j in range(len(bias))]
activated = [sigmoid(o) for o in output]

print("Weights:", weight)
print("Bias:", bias)
print("Output:", output)
print("Activated:", activated)