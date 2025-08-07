#24.073273,120.701243,7,473,5,-1,-1,-1,1,-1,1,0,1,-1,0,0,0,1,0
import csv
import math

x = [] #inputs
with open('training_data.csv', newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|') #csv.reader(csvfile, /, dialect='excel', **fmtparams)
    next(spamreader)
    for row in spamreader:
        #print(','.join(row))
        x.append(row)

def sigmoid(z):
    return 1 / (1 + math.exp(-z))

print(x)

