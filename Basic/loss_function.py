import math

y = [1, 0, 0]
y_hat = [0.48273, 0.73642, 0.12589]

m = len(y)

# Mean Squared Error (MSE)
mse = (1 / m) * sum((y[i] - y_hat[i]) ** 2 for i in range(m))

# Cross-Entropy Loss (Binary version per label)
cel = -(1 / m) * sum(
    y[i] * math.log(y_hat[i]) + (1 - y[i]) * math.log(1 - y_hat[i])
    for i in range(m)
)

print("MSE:", mse)
print("Cross-Entropy Loss:", cel)
