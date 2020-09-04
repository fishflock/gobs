import networkx as nx
import matplotlib.pyplot as plt

def main():
    G_symmetric = nx.Graph()

    G_symmetric.add_edge(1,2)
    G_symmetric.add_edge(1,3)
    G_symmetric.add_edge(1,4)
    G_symmetric.add_edge(1,5)
    G_symmetric.add_edge(2,5)
    G_symmetric.add_edge(5,3)
    G_symmetric.add_edge(6,4)
    G_symmetric.add_edge(6,1)

    #print(G_symmetric.number_of_nodes())

    nx.draw(G_symmetric, with_labels=True, font_weight='bold')
    plt.show()
    #plt.savefig("graph.png")
