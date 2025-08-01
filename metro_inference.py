hour = 6
min = 0

#平日(週一到週五)尖峰時段：07:00-09:00、17:00-19:00。
#假日(週六、週日及國定假日)尖峰時段：10:00-20:00。

print(f'首班 Hour: 6, Minute: 0')
for i in range(7):
    min=min+8
    print(f'離峰 Hour: {hour}, Minute: {min}')
for i in range(21):
    min=min+6
    if min>=60:
        min=min-60
        hour=hour+1
    print(f'尖峰 Hour: {hour}, Minute: {min}')
for i in range(59):
    min=min+8
    if min>=60:
        min=min-60
        hour=hour+1
    print(f'離峰 Hour: {hour}, Minute: {min}')
for i in range(21):
    min=min+6
    if min>=60:
        min=min-60
        hour=hour+1
    print(f'尖峰 Hour: {hour}, Minute: {min}')
for i in range(30):
    min=min+8
    if min>=60:
        min=min-60
        hour=hour+1
    print(f'離峰 Hour: {hour}, Minute: {min}')
for i in range(3):
    min=min+20
    if min>=60:
        min=min-60
        hour=hour+1
    if hour>=24:
        hour=0
    print(f'深夜 Hour: {hour}, Minute: {min}')
print(f'末班 Hour: 0, Minute: 15')