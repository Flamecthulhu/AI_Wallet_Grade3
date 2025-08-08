X = [[1,2,3],
     [4,5,6]]
w = [0.1, 0.2, 0.3]
b = 0.5

def compute_z(x, w, b):
    z = 0
    for xi, wi in zip(x, w):
        z += xi * wi
    return z + b

Z = [compute_z(x, w, b) for x in X]
print(Z)