import math

def softmax(x):
    """
    Compute softmax probabilities for a 1‑D list of real numbers.

    Parameters
    ----------
    x : list[float]
        Input scores (logits).

    Returns
    -------
    list[float]
        Softmax probabilities that sum to 1.
    """
    # 1. Find the maximum to stabilize the exponentials
    m = max(x)

    # 2. Compute the exponentials of the shifted values
    exp_vals = [math.exp(v - m) for v in x]

    # 3. Sum the exponentials
    s = sum(exp_vals)

    # 4. Normalize to get probabilities
    return [v / s for v in exp_vals]

scores = [2.0, 1.0, 0.1]
probs  = softmax(scores)
print(probs)           # [0.6590011377822258, 0.24243297141560594, 0.09856589080216831]
print(sum(probs))      # 1.0