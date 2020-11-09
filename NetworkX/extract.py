import networkx as nx
import json
from networkx.readwrite import json_graph
import matplotlib.pyplot as plt
import numpy as np
import random
import sys
import copy


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

def blondel(G):
    comms = []
    for n in G:
        comms.append([n])
    node_moved = True
    while(node_moved):
        node_moved = False
        for n in G:
            n_comm = []

            for c in comms:
                if n in c:
                    n_comm = c

            max_mod = nx.algorithms.community.quality.modularity(G, comms)
            new_comm = n_comm

            
            for m in G.neighbors(n):
                for c in comms:
                    if (m in c and c != n_comm):
                        comms_temp = copy.deepcopy(comms)
                        comms_temp[comms_temp.index(n_comm)].remove(n)
                        comms_temp[comms_temp.index(c)].append(n)
                        mod = nx.algorithms.community.quality.modularity(G, comms_temp)
                        if (max_mod < mod):
                            max_mod = mod
                            new_comm = c
                            node_moved = True

            if(new_comm != n_comm):
                comms[comms.index(n_comm)].remove(n)
                comms[comms.index(new_comm)].append(n)

    return comms
        

def centrality(G, c):
    if(c == 'eigen'):
        return nx.algorithms.centrality.eigenvector_centrality_numpy(G)
    elif (c == 'close'):
        return nx.algorithms.centrality.closeness_centrality(G)
    elif (c == 'between'):
        return nx.algorithms.centrality.betweenness_centrality_source(G)

def graph(G, nx_out, scale_in):

    node_arr = [] #stores nodes with total weight < 1
    max_weight = 0

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

        if total_weight > max_weight:
            max_weight = total_weight
        
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

    #blondel sorting
    groups = blondel(G)

    #create layout of nodes
    #k is optimal distance between nodes
    #seed sets the 'randomness' of the layout to be constant
    #iterations determine how many times algorithm is run,
    #    which results in closer nodes that are more strongly connected
    
    layout = nx.spring_layout(G, k=1, seed=24, iterations=40)
    #layout = nx.spectral_layout(G)
    #layout = nx.kamada_kawai_layout(G)

    #change size of output window
    fig, ax = plt.subplots(figsize=(15,10))

    #returns an array of the first 6 colors used in graph
    color_arr = colors()

    #scale the nodes based on user input 
    if (scale_in != 'weight'):
        c_dic = centrality(G, scale_in)
        scale = 15000/max(c_dic.values())
    else:
        scale = 15000/max_weight

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
            if(scale_in == 'weight'):
                s_arr.append(scale * G.nodes[n]['weight'])
            else:
                if (c_dic[str(n)] < 0):
                    s_arr.append(0)
                else:
                    s_arr.append(scale * c_dic[str(n)])
            for e in G.edges(n):
                if(e[0] == e[1]):
                    w = G.nodes[e[0]]['weight']
                edge_list_arr.append(e)
                edge_size_arr.append(((15000/max_weight) * G[e[0]][e[1]][0]['weight'])/300)   

        #draw the nodes
        nx.draw_networkx(G,
                pos=layout,
                with_labels=True,
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
    data = json_graph.node_link_data(G)
    with open('graph.json', 'w') as f:
        json.dump(data, f, indent=4)
    plt.savefig(nx_out)

#function for testing with Facebook data
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
    print("DONE PARSING")
    graph(G, 'face.png')

################################################################  
#main function, reads input from output of GOBS, outputs networkx as .png
gobs_in = sys.argv[1]
nx_out = sys.argv[2]
scale_in = sys.argv[3]

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

graph(G, nx_out, scale_in)
################################################################
