from sympy import symbols, diff

x, y = symbols('x y')
f = x**2 * y + y**3 #f(x,y)=x^(2)y+y^(3)

# 對 x 求偏微分
df_dx = diff(f, x) #d/dx (f(x))
print("∂f/∂x =", df_dx)

# 對 y 求偏微分
df_dy = diff(f, y) #d/dy (f(y))
print("∂f/∂y =", df_dy)