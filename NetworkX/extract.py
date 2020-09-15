import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

def read_ep_matrix():
    ep_in = open("..\Main\output2.txt")

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

    node_arr = []
    size_arr = []
    edge_arr = []
    for g1 in G:
        in_weight = 0
        out_weight = 0
        for g2 in G:
            if G.has_edge(g1,g2):
                in_weight = in_weight + G[g1][g2]["weight"]
                edge_arr.append(G[g1][g2]["weight"])
            if g1 != g2 and G.has_edge(g2,g1):
                out_weight = out_weight + G[g2][g1]["weight"]
        total_weight = in_weight + out_weight
        G.nodes[g1]['weight'] = total_weight
        if total_weight > 0:
            size_arr.append(300 * (in_weight + out_weight))
        else:
            node_arr.append(g1)
            
    G.remove_nodes_from(node_arr)

    layout = nx.drawing.layout.spring_layout(G,k=1,scale=None)
    nx.draw_networkx(G, pos=layout, with_labels=True, font_weight='bold', node_size=size_arr, edgelist=[])
    nx.draw_networkx_edges(G, pos=layout, width=edge_arr)
    plt.show()





##############################################################################

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

    #print(list(G.edges(data=True)))
    nx.draw_networkx(G, with_labels=True, font_weight='bold')
    plt.show()
                

    
            
    
