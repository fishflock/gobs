import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

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

        elif i != 0:
            row = line.split()
            row = row[1:]
            row_int = []
            for x in row:
                row_int.append(int(x))
            values.append(row_int)
        i = i + 1
            
    return [labels, values]

def main():
    G = nx.Graph()
    ep_arr = read_ep_matrix()
    labels = ep_arr[0]
    values = np.array(ep_arr[1])
    
    G = nx.convert_matrix.from_numpy_matrix(values,parallel_edges=False, create_using=None)
    nx.draw(G, with_labels=True, font_weight='bold')
    plt.show()

#below function replaced by convert_matrix.from_numpy_matrix
#will remove once positive above code works
def dummy_double_loop():
    row_num = 0
    for row in values:
        col_num = 0
        for col in row:
            i = 0
            while i != col:
                G.add_edge(labels[row_num], labels[col_num])
                #print("adding edge to " + labels[row_num] + ", " + labels[col_num])
                i = i + 1
            col_num = col_num + 1
        row_num = row_num + 1

    nx.draw(G, with_labels=True, font_weight='bold')
    plt.show()
                

    
            
    
