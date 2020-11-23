import networkx as nx
import matplotlib.pyplot as plt
from fa2 import ForceAtlas2
import numpy as np
import random
import sys
import copy

#returns one of six default colors
def colors():
    return ['red',
            'b',
            'green',
            'gold',
            'm',
            'c'
            ]     

#read data from epsilon matrix/GOBS output
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

#simple implentation of Blondel algorithm (https://www.researchgate.net/publication/1913681_Fast_Unfolding_of_Communities_in_Large_Networks)
def blondel(G):

    #put each node into G into their own communities
    comms = []
    for n in G:
        comms.append([n])

    #run algorithm repeatedely until no nodes are moved
    node_moved = True
    while(node_moved):
        node_moved = False
        for n in G:
            n_comm = []

            #check each community and assign the one containing n
            for c in comms:
                if n in c:
                    n_comm = c

            #calculate current modularity
            max_mod = nx.algorithms.community.quality.modularity(G, comms)
            new_comm = n_comm

            #check the modularity of G resulting from moving n into the community of each of n's neighbors
            for m in G.neighbors(n):
                for c in comms:
                    if (m in c and c != n_comm):
                        comms_temp = copy.deepcopy(comms)
                        comms_temp[comms_temp.index(n_comm)].remove(n)
                        comms_temp[comms_temp.index(c)].append(n)
                        mod = nx.algorithms.community.quality.modularity(G, comms_temp)

                        #check if new modularity is greater
                        if (max_mod < mod):
                            max_mod = mod
                            new_comm = c
                            node_moved = True

            #if G has a higher modularity by moving n, move n into new community
            if(new_comm != n_comm):
                comms[comms.index(n_comm)].remove(n)
                comms[comms.index(new_comm)].append(n)
    return comms
        
#returns centrality values of network
def centrality(G, c):
    if(c == 'eigen'):
        return nx.algorithms.centrality.eigenvector_centrality_numpy(G)
    elif (c == 'close'):
        return nx.algorithms.centrality.closeness_centrality(G)
    elif (c == 'between'):
        return nx.algorithms.centrality.betweenness_centrality_source(G)


#main function to create network graph
def graph(G, nx_out, scale_in, sort_in, layout_in, scale_mult):

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

    #use user input to determine how to group nodes into communities
    groups = []
    if(sort_in == 'lpa'):
        lpa = nx.algorithms.community.label_propagation.asyn_lpa_communities(G)
        for x in lpa:
            groups.append(x)
    elif(sort_in == 'blondel'):
        groups = blondel(G)

    for n in G:
        groups.append([n])

    #create layout of nodes
    #k is optimal distance between nodes
    #seed sets the 'randomness' of the layout to be constant
    #iterations determine how many times algorithm is run,
    #    which results in closer nodes that are more strongly connected
    if(layout_in == 'spring'):
        layout = nx.spring_layout(G, k=1, seed=24, iterations=40)
    elif(layout_in == 'fa2'):
        layout = ForceAtlas2().forceatlas2_networkx_layout(G, pos=None, iterations=40)

    #change size of output window
    fig, ax = plt.subplots(figsize=(15,10))

    #returns an array of the first 6 colors used in graph
    color_arr = colors()

    #scale the nodes based on user input
    if (scale_in != 'weight'):
        c_dic = centrality(G, scale_in)
        scale = 15000/max(c_dic.values())
    else:
        scale = scale_mult * (15000/max_weight)

    #draw the graph
    i = 0
    for g in groups:
        edge_list_arr = []
        edge_size_arr = []
        s_arr = []

        #pick from preset colors first, otherwise generate a random color for group
        if i > 6:
            c = [[random.random(),random.random(),random.random()]]
        else:
            c = color_arr[i]

        #for each node in the group, add node size based on scale to s_arr  
        for n in g:
            if(scale_in == 'weight'):
                s_arr.append(scale * G.nodes[n]['weight'])
            else:
                if (c_dic[str(n)] < 0):
                    s_arr.append(0)
                else:
                    s_arr.append(scale * c_dic[str(n)])

            #for each edge of n, calculate edge size and update edge arrays
            for e in G.edges(n):
                if(e[0] == e[1]):
                    w = G.nodes[e[0]]['weight']
                edge_list_arr.append(e)
                edge_size_arr.append(((15000/max_weight) * G[e[0]][e[1]][0]['weight'])/250)   

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

    #save graph as a PNG file
    plt.savefig(nx_out)


################################################################  
#main function, reads input from output of GOBS, outputs networkx as .png
gobs_in = sys.argv[1]
nx_out = sys.argv[2]
scale_in = sys.argv[3]
sort_in = sys.argv[4]
layout_in = sys.argv[5]
scale_mult = float(sys.argv[6])

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

graph(G, nx_out, scale_in, sort_in, layout_in, scale_mult)
################################################################
