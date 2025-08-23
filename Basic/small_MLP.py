import math
import random
import sympy as sy

x = [[1,2,3], [4,5,6]]
y = [1, 0]
"""
3個parameter對應3個weight做fan-in, 2組陣列, 共6個weights.
fan-out = 4, 會有3組陣列, 每個陣列有4個weights, 共12個weights.

[wi11,wi12,wi13]    [wh11,wh12,wh13,wh14]
                    [wh21,wh22,wh23,wh24]
                    [wh31,wh32,wh33,wh34]
[wi21,wi22,wi23]    [wh41,wh42,wh43,wh44]


"""
def activation_function(z):
    return 1 / (1 + math.exp(-z))

def xavier_weight_init(fan_in, fan_out):
    weight_limit = math.sqrt(6 / (fan_in + fan_out))
    return [[random.uniform(-weight_limit, weight_limit) for _ in range(fan_out)] for _ in range(fan_in)]

def xavier_bias_init(fan_out):
    bias_limit = math.sqrt(1 / fan_out)
    return [random.uniform(-bias_limit, bias_limit) for _ in range(fan_out)]

def process_layer(input_data, fan_in, fan_out, layer_name):
    weight = xavier_weight_init(fan_in, fan_out)
    bias = xavier_bias_init(fan_out)

    output = [sum(input_data[i] * weight[i][j] for i in range(len(input_data))) + bias[j] for j in range(len(bias))]
    activated = [activation_function(z) for z in output]
    
    print(f"Layer {layer_name}:")
    print("Weights:", weight)
    print("Bias:", bias)
    print("Linear combination:", output)
    print("Activated:", activated)
    print()
    
    return activated

fan_sizes = [len(x[0]), 4, 4, 2] #input, fc1, fc2, output
current_input = x[0]

for i in range(len(fan_sizes) - 1):
    current_input = process_layer(current_input, fan_sizes[i], fan_sizes[i+1], i+1)

activated = current_input
m = len(y)
epsilon = 1e-15  # 避免 log(0)

cel = -(1 / m) * sum(
    y[i] * math.log(activated[i] + epsilon) + (1 - y[i]) * math.log(1 - activated[i] + epsilon)
    for i in range(m)
)

print("Cross Entropy Loss:", cel)

y, y_hat = sy.symbols('y y_hat')
L = - (y * sy.log(y_hat) + (1 - y) * sy.log(1 - y_hat))
dL_dyhat = sy.diff(L, y_hat)

print(dL_dyhat)