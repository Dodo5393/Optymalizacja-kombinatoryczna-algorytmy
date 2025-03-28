//I'm not going to sleep today
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/distance.hpp>
#include <boost/range/size.hpp>
#include <cstdio>
#include <ios>
#include <iostream>
#include <fstream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

using namespace std;
using namespace boost;

// Definicja typu grafu
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                              boost::no_property,
                              boost::property<boost::edge_weight_t, double>> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;


Graph load_graph_from_file(const char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "no file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    Graph g;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int u, v;
        double weight;

        if (!(iss >> u >> v >> weight)) {
            std::cerr << "no line" << line << std::endl;
            continue;
        }

        add_edge(u, v, weight, g);
    }

    return g;
}

void compute_shortest_path(const Graph& g, Vertex source, std::vector<double>& distances, std::vector<Vertex>& predecessors){

    distances.resize(boost::num_vertices(g));
    predecessors.resize(boost::num_vertices(g));

    boost::dijkstra_shortest_paths(g, source, boost::predecessor_map(&predecessors[0]).distance_map(&distances[0]));
}

// Odtwarzanie krawędzi najkrótszej ścieżki
vector<Edge> get_shortest_path_edges(const Graph& g, Vertex source, Vertex target, const vector<Vertex>& predecessors) {
    vector<Edge> path_edges;
    Vertex current = target;

    while (current != source && predecessors[current] != static_cast<Vertex>(-1)) {
        Vertex pred = predecessors[current];
        auto edge_pair = edge(pred, current, g);
        if (edge_pair.second) {
            path_edges.push_back(edge_pair.first);
        } else {
            cerr << "Błąd: Brak krawędzi między " << pred << " a " << current << endl;
            break;
        }
        current = pred; // Aktualizacja current
    }

    if (current != source) {
        cerr << "Błąd: Nie udało się dotrzeć do źródła od " << target << endl;
    }

    return path_edges;
}



Graph algorytm_kou_markovsky_berman(const Graph& g, const std::vector<Vertex>& terminals){
    int n = boost::num_vertices(g);
    int t = terminals.size();


    std::vector<std::vector<double>> distances(t, std::vector<double>(t));

    std::map<std::pair<Vertex, Vertex>, std::vector<Edge>> shortest_patchs;



    for (int i = 0; i < t; i++){
        std::vector<double> dist;
        std::vector<Vertex> pred;

        compute_shortest_path(g, terminals[i], dist, pred);
        for (int j = 0; j < t; j++){
            distances[i][j] = dist[terminals[j]];
            if (i != j){
                auto path = get_shortest_path_edges(g, terminals[i], terminals[j], pred);
                shortest_patchs[{terminals[i], terminals[j]}] = path;
            }

        }
    }
    Graph complite_graph(t);

    for(int i = 0; i < t; i++){
        for(int j = 0; j < t; j++){
            boost::add_edge(i,j,distances[i][j], complite_graph);
        }
    }


    std::vector<Edge> mst_edges;
    boost::kruskal_minimum_spanning_tree(complite_graph, std::back_inserter(mst_edges));


    std::vector<Edge> h_edges;
    for (const auto& e: mst_edges){
        Vertex v = boost::source(e, g);
        Vertex u = boost::target(e, g);
        auto path = shortest_patchs[{terminals[u],terminals[v]}];
        h_edges.insert(h_edges.end(), path.begin(),path.end());
    }

    //skaładamy graf h
    Graph h(n);
    for (const auto& e : h_edges){
        Vertex v = boost::source(e, g);
        Vertex u = boost::target(e, g);
        Vertex w = boost::get(boost::edge_weight, g, e);
        boost::add_edge(u, v, w, h);
    }
    //MST dla h
    std::vector<Edge> h_mst;
    boost::kruskal_minimum_spanning_tree(h,std::back_inserter(h_mst));


    double total_cost = 0.0;
    std::cout << "krawędzie drzewa Sternera:\n";
    for (const auto& e : h_mst){
        Vertex v = boost::source(e, g);
        Vertex u = boost::target(e, g); //target nie source
        Vertex w = boost::get(boost::edge_weight, g, e);
        total_cost += w;
        std::cout << u << " -- " << v << " (waga: " << w << ")\n";

    }
    std::cout << "Całkowity koszt: "<< total_cost << std::endl;

    return h;
}

int main(int argc, char *argv[]) {

    Graph g = load_graph_from_file(argv[1]);
    Graph h ;
    boost::print_graph(g);

    std::vector<Vertex> t = {0, 2, 8, 5};

    h = algorytm_kou_markovsky_berman(g, t);
    boost ::print_graph(h);

    return 0;

}
