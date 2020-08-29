import networkx as nx
import matplotlib.pyplot as plt

def main():
    G_symmetric = nx.Graph()

    G_symmetric.add_edge('Steven',  'Laura')
    G_symmetric.add_edge('Steven',  'Marc')
    G_symmetric.add_edge('Steven',  'John')
    G_symmetric.add_edge('Steven',  'Michelle')
    G_symmetric.add_edge('Laura',   'Michelle')
    G_symmetric.add_edge('Michelle','Marc')
    G_symmetric.add_edge('George',  'John')
    G_symmetric.add_edge('George',  'Steven')

    print(G_symmetric.number_of_nodes())

    nx.draw(G_symmetric, with_labels=True, font_weight='bold')
    plt.show()
