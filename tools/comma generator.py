with open("tools\\data.txt") as f:
    nums = f.read().split()
nums = [float(x) for x in nums]

print("const float W3[160] = {")
for i, val in enumerate(nums):
    end = ",\n" if (i+1) % 8 == 0 else ", "
    print(f"    {val:.8f}{end}", end="")
print("\n};")
