import csv

i = 0
k = 2
j = 1
data = [[]]
raw = [[]]

with open('C:\\Users\\allan\\Documents\\AI_Wallet_Grade3\\Data\\training_data_old.csv', newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|') #csv.reader(csvfile, /, dialect='excel', **fmtparams)
    for row in spamreader:
        print(','.join(row))
        data.append(','.join(row))
        i = i + 1


for j in range(20):
        #print(data[k][j])
        raw.append(data[k][j])

print(raw)

