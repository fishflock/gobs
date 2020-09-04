def read_ep_matrix():
    ep_in = open("..\Main\output.txt")

    #labels for the epsilon state transition matrix
    labels = []

    #each value of the matrix, represented row first and then column
    values = []

    i = 0
    for line in ep_in:    
        if i == 1:
            labels = line.split()
            #uncomment below and change 'labels' to 'spl' in
            #line above to have labels be ints instead of strs
            #for x in spl:
                #labels.append(int(x))

            #print(labels)
        elif i != 0:
            row = line.split()
            row = row[1:]
            row_int = []
            for x in row:
                row_int.append(int(x))
            values.append(row_int)
        i = i + 1
            
    return [labels, values]
            
    
