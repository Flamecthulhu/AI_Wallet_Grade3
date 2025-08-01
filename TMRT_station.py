import csv

with open("C:\\Users\\allan\\Programs\\Program_Grade3\\source\\TaichungMRT_Station.csv", newline='', encoding='utf-8-sig') as csvfile:
    reader = csv.DictReader(csvfile)
    header = next(reader)
    for col in reader:
        print(col)