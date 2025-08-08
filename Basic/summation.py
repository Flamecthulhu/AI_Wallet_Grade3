x = [1, 2, 3]
w = [1, 1, 1]
b = 5

print(sum(x[i] * w[i] for i in range(len(x))) + b)
