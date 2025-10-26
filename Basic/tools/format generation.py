import numpy as np
input_matrix = [
    [24.073544, 120.699775, 7, 457, 5, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1],
    [25.013425, 121.214136, 22, 1332, 7, 3, 1, 1, 0, 1, 1, 0, -1, 0, 0, 1, 1, 0]
]
for label_count in range(32):
    print(f"float s{label_count}[INPUT_SIZE] = {input_matrix[]};")
    print(f"for (int i = 0; i < INPUT_SIZE; i++) inputs[{label_count}][i] = s{label_count}[i];")
    print(f"labels[{label_count}] = {label_count % 2};")
# float s31[INPUT_SIZE] = {24.073544, 120.699775, 7, 457, 5, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1};
# for (int i = 0; i < INPUT_SIZE; i++) inputs[31][i] = s31[i];
# labels[31] = 0;