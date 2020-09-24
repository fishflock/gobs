import networkx as nx
from networkx.algorithms import community
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
        elif i != 0:
            row = line.split()
            row = row[1:]
            row_int = []
            for x in row:
                row_int.append(int(x))
            values.append(row_int)
        i = i + 1
    return [labels, values]

def graph(G):

    node_arr = [] #stores nodes with total weight < 1
    edge_arr = [] #stores edges of graph G
    size_arr = [] #stores sizes of edges

    #iterates through each node and compares it to each other node (including itself)
    #calculates in, out, and total weights
    for g1 in G:
        in_weight = 0
        out_weight = 0

        #calculate weight for each node connected to g1
        for g2 in G:   
            if G.has_edge(g1,g2):
                in_weight = in_weight + G[g1][g2][0]["weight"]
                edge_arr.append(G[g1][g2][0]["weight"])
            if g1 != g2 and G.has_edge(g2,g1):
                out_weight = out_weight + G[g2][g1][0]["weight"]

        total_weight = in_weight + out_weight
        
        if total_weight > 0:
            size_arr.append(300 * (total_weight))
            G.nodes[g1]['weight'] = total_weight
        else:
            node_arr.append(g1)

    #remove nodes with weight < 1 (no connections)
    print("DONE PARSING NODES")
    G.remove_nodes_from(node_arr)

    communities_generator = community.girvan_newman(G)
    print("GIRVAN 1")
    top_level_communities = next(communities_generator)
    print("GIRVAN 2")
    next_level_communities = next(communities_generator)
    print("GIRVAN 3")
    comms = sorted(map(sorted, next_level_communities))
    print(len(comms))

    #create layout of nodes
    #k is optimal distance between nodes
    #seed sets the 'randomness' of the layout to be constant
    #iterations determine how many times algorithm is run,
    #    which results in closer nodes that are more strongly weight
    layout = nx.spring_layout(G, k=1, seed=1, iterations=50)

    #change size of output window
    plt.figure(figsize=(12,7))

    color_arr = ['#FF0000','#0000FF','#00FF00','#FFFF00','#00FFFF','#FF00FF']

    #draw the nodes
    i = 0
    for c in comms:
        s_arr = []
        for node in c:
            s_arr.append(300 * G.nodes[node]['weight'])
        nx.draw(G,
                pos=layout,
                with_labels=True,
                font_weight='bold',
                nodelist=c,
                node_size=s_arr,
                node_color=color_arr[i],
                edgelist=[]
                )
        i = i + 1

    #draw the edges
    nx.draw_networkx_edges(G,
                           pos=layout,
                           width=edge_arr,
                           connectionstyle='arc3,rad=0.2',
                           node_size=size_arr
                           )
    plt.show()

def face():
    ep_in = open("Facebook1.csv")
    G = nx.MultiDiGraph()

    mapping = {}
    i = 0
    for line in ep_in:
        if i == 0:
            i = i + 1
        else:
            row = line.split(',')
            n1 = int(row[0])
            n2 = int(row[1])
            G.add_edge(n1,n2)
            G[n1][n2][0]['weight'] = 1
            mapping[i] = row[3]
            i = i + 1

    print("DONE PARSING EXCEL")
    graph(G)
            

def main():

    #read in values from epsilon state matrix, separate into labels and data
    ep_arr = read_ep_matrix()
    labels = ep_arr[0]
    values = np.array(ep_arr[1])

    #convert values from matrix into a graph
    G = nx.convert_matrix.from_numpy_matrix(values,create_using=nx.MultiDiGraph)

    #relabel the nodes using labels
    mapping = {}
    i = 0
    for l in labels:
        mapping[i] = l
        i = i + 1
    G = nx.relabel_nodes(G,mapping)

    graph(G)
    

                

    
            
    
