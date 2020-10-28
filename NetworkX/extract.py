import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import random
import sys

def colors():
    return ['red',
            'b',
            'green',
            'gold',
            'm',
            'c',
            ]     

def read_ep_matrix(gobs_in):
    ep_in = open(gobs_in)

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

    #iterates through each node and compares it to each other node (including itself)
    #calculates in, out, and total weights
    for g1 in G:
        in_weight = 0
        out_weight = 0

        #calculate weight for each node connected to g1
        for g2 in G:            
            if G.has_edge(g1,g2):
                in_weight = in_weight + G[g1][g2][0]["weight"]
            if g1 != g2 and G.has_edge(g2,g1):
                out_weight = out_weight + G[g2][g1][0]["weight"]

        total_weight = in_weight + out_weight
        
        if total_weight > 0:
            G.nodes[g1]['weight'] = total_weight
        else:
            node_arr.append(g1)

    #remove nodes with weight < 1 (no connections)
    G.remove_nodes_from(node_arr)

    #perform Label Propogation Algorithm to group nodes
    groups = []
    lpa = nx.algorithms.community.label_propagation.asyn_lpa_communities(G)
    for x in lpa:
        groups.append(x)

    #create layout of nodes
    #k is optimal distance between nodes
    #seed sets the 'randomness' of the layout to be constant
    #iterations determine how many times algorithm is run,
    #    which results in closer nodes that are more strongly connected
    layout = nx.spring_layout(G, k=1, seed=24, iterations=40)

    #change size of output window
    fig, ax = plt.subplots(figsize=(15,10))

    #returns an array of the first 6 colors used in graph
    color_arr = colors()

    #draw the graph
    i = 0
    for g in groups:
        edge_list_arr = []
        edge_size_arr = []
        s_arr = []
        if i > 5:
            c = [[random.random(),random.random(),random.random()]]
        else:
            c = color_arr[i]
        for n in g:
            s_arr.append(300 * G.nodes[n]['weight'])
            for e in G.edges(n):
                if(e[0] == e[1]):
                    w = G.nodes[e[0]]['weight']
                edge_list_arr.append(e)
                edge_size_arr.append(G[e[0]][e[1]][0]['weight'])   

        #draw the nodes
        nx.draw_networkx(G,
                pos=layout,
                with_labels=True,
                #font_weight='bold',
                nodelist=g,
                node_size=s_arr,
                node_color=c,
                edgelist=[],
                )

        #draw the edges
        nx.draw_networkx_edges(G,
                pos=layout,
                width=edge_size_arr,
                edge_color=c,
                edgelist=edge_list_arr,
                connectionstyle='arc3,rad=0.2',
                arrowsize=1
                )
        i = i + 1
    output = 'out.png'
    plt.savefig(output)

#function for testing with Facebook data
def face():
    ep_in = open("Facebook2.csv")
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

    graph(G)


################################################################  
#main function, reads input from output of GOBS
gobs_in = sys.argv[1]

#read in values from epsilon state matrix, separate into labels and data
ep_arr = read_ep_matrix(gobs_in)
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
################################################################  
