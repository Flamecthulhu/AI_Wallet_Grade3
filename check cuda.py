import torch
print(torch.__version__)
print("CUDA Available:", torch.cuda.is_available())
print("GPU Model:", torch.cuda.get_device_name(0) if torch.cuda.is_available() else "Cannot detect GPU")
