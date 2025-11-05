import math
import random

class initial():
    def weight(fan_in, fan_out):
        weight_limit = math.sqrt(6 / (fan_in + fan_out))
        weight = [[random.uniform(-weight_limit, weight_limit) for _ in range(fan_out)] for _ in range(fan_in)]
        return weight
    
    def bias(fan_out):
        bias_limit = math.sqrt(1 / fan_out)
        bias = [random.uniform(-bias_limit, bias_limit) for _ in range(fan_out)]
        return bias
    
class forward():
    @staticmethod
    def linear_comb(data, weight, bias):
        z = []
        fan_out = len(weight[0])
        for j in range(fan_out):
            zj = sum(data[i] * weight[i][j] for i in range(len(data))) + bias[j]
            z.append(zj)
        return z
    
    @staticmethod
    def softmax(z):
        max_z = max(z)
        exp_z = [math.exp(zi - max_z) for zi in z]
        sum_exp = sum(exp_z)
        return [zi / sum_exp for zi in exp_z]
    
    @staticmethod
    def cce_loss(y_true, y_pred):
        eps = 1e-9
        return -sum(yt * math.log(yp + eps) for yt, yp in zip(y_true, y_pred))
    
    @staticmethod
    def batch_cce_loss(y_trues, y_preds):
        losses = [forward.cce_loss(y_true, y_pred) for y_true, y_pred in zip(y_trues, y_preds)]
        return sum(losses) / len(losses)



raw_data = [[1, 2, 3, 4], [5, 6, 7, 8]]

weight = initial.weight(4, 6)
bias = initial.bias(6)

z = [forward.linear_comb(sample, weight, bias) for sample in raw_data]

activated = [forward.softmax(sample_z) for sample_z in z]

y_true = [[0,1,0,0,0,0],[1,0,0,0,0,0]]  # one-hot
y_pred = activated  # softmax output from forward

loss = forward.batch_cce_loss(y_true, y_pred)

print(f"Weight:{weight}\nBias:{bias}\nLinear Comb:{z}\nActivated:{activated}\nLoss:{loss}")