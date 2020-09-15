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
    
    G = nx.convert_matrix.from_numpy_matrix(values,parallel_edges=False,create_using=nx.DiGraph)

    mapping = {}
    i = 0
    for l in labels:
        mapping[i] = l
        i = i + 1
    G = nx.relabel_nodes(G,mapping)

    size_array = []
    for g1 in G:
        in_weight = 0
        out_weight = 0
        for g2 in G:
            if G.has_edge(g1,g2):
                in_weight = in_weight + G[g1][g2]["weight"]
            if g1 != g2 and G.has_edge(g2,g1):
                out_weight = out_weight + G[g2][g1]["weight"]
        G.nodes[g1]['weight'] = in_weight + out_weight
        size_array.append(300 * (in_weight + out_weight))
    print(G.nodes(data=True))
    
    #print(list(G.edges(data=True)))
    #print(list(G.nodes(data=True)))
    nx.draw(G, with_labels=True, font_weight='bold', node_size=size_array)
    plt.show()

#below function replaced by convert_matrix.from_numpy_matrix
#will remove once positive above code works
def dummy_double_loop():
    row_num = 0
    for row in values:
        col_num = 0
        for col in row:
            #i = 0
            if col != 0:
                G.add_edge(labels[row_num], labels[col_num], weight=col)
                
                #print("adding edge to " + labels[row_num] + ", " + labels[col_num])
                #i = i + 1
            col_num = col_num + 1
        row_num = row_num + 1

    print(list(G.edges(data=True)))
    nx.draw(G, with_labels=True, font_weight='bold')
    plt.show()
                

    
            
    
