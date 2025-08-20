import csv

with open('C:\\Users\\allan\\Programs\\Program_Grade3\\source\\training_data.csv', newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|') #csv.reader(csvfile, /, dialect='excel', **fmtparams)
    for row in spamreader:
        print(','.join(row))