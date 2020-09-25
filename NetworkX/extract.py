import networkx as nx
from networkx.algorithms import community
import matplotlib.pyplot as plt
import numpy as np

def group(G):
    #print(G.edges(data=True))
    groups = []
    done = []

   
    i = 1
    for n in G:
        if i == 1:
            groups.append([n])
            done.append(n)
            i = 0
        elif n not in done:
            node = -1
            in_edges = G.in_edges(n)
            in_degree = -1
            in_node = -1
            
            out_edges = G.out_edges(n)
            out_degree = -1
            out_node = -1
            
            for i in in_edges:
                weight = G[i[0]][i[1]][0]['weight']
                if weight > in_degree:
                    in_node = i[0]
                    in_degree = weight        
            
            for o in out_edges:
                weight = G[o[0]][o[1]][0]['weight']
                if weight > out_degree:                
                    out_node = o[1]
                    out_degree = weight

            if in_degree > out_degree:
                node = in_node
            elif in_degree < out_degree:
                node = out_node
            elif in_degree > -1:
                node = in_node
            elif out_degree > -1:
                node = out_node
            
            if node in done:
                for g in groups:
                    if node in g:
                        g.append(n)

            else:
                groups.append([n, node])
                done.append(node)

            done.append(n)
                
    return groups
        
                         
        

def read_ep_matrix():
    ep_in = open("../Main/output2.txt")#/output.txt")

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
    G.remove_nodes_from(node_arr)

    groups = group(G)

    #create layout of nodes
    #k is optimal distance between nodes
    #seed sets the 'randomness' of the layout to be constant
    #iterations determine how many times algorithm is run,
    #    which results in closer nodes that are more strongly weight
    layout = nx.spring_layout(G, k=1, seed=3, iterations=50)

    #change size of output window
    plt.figure(figsize=(12,7))

    color_arr = ['#FF0000','#0000FF','#00FF00','#FFFF00','#00FFFF','#FF00FF']

    #draw the nodes
    i = 0
    for g in groups:
        s_arr = []
        for n in g:
            s_arr.append(300 * G.nodes[n]['weight'])
        nx.draw(G,
                pos=layout,
                with_labels=True,
                font_weight='bold',
                nodelist=g,
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
                           node_size=size_arr,
                           arrowsize=15
                           )
    plt.show()

def face():
    ep_in = open("Facebook3.csv")
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
    

                

    
            
    
