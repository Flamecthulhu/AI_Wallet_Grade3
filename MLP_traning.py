#24.073273,120.701243,7,473,5,-1,-1,-1,1,-1,1,0,1,-1,0,0,0,1,0
import csv
import math

x = [] #inputs
with open('C:\\Users\\allan\\Programs\\Program_Grade3\\source\\training_data.csv', newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|') #csv.reader(csvfile, /, dialect='excel', **fmtparams)
    next(spamreader)
    for row in spamreader:
        #print(','.join(row))
        x.append(row)

def sigmoid(z):
    sigmoid_func = math.
    return

print(x)

