#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/pending/property.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <limits>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace boost;

struct task_parameters {
    int duration;
    int earliest_start;
    int latest_start;
    int earliest_finish;
    int latest_finish;
};

typedef boost::adjacency_list<vecS, vecS, bidirectionalS, task_parameters> Network_Activity_On_Node;
typedef boost::graph_traits<Network_Activity_On_Node>::vertex_descriptor Task;
typedef boost::graph_traits<Network_Activity_On_Node>::edge_descriptor Dependence;

// Funkcja do ładowania grafu (bez zmian)
Network_Activity_On_Node loade_task_to_network(const std::string& filename) {
    Network_Activity_On_Node my_network;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // ignoruj nagłówek

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell, dependencies;
        int task_id;
        task_parameters task_from_file;

        std::getline(ss, cell, ',');
        task_id = std::stoi(cell);
        std::getline(ss, cell, ',');
        task_from_file.duration = std::stoi(cell);

        boost::add_vertex(task_from_file, my_network);

        std::getline(ss, dependencies);
        if (!dependencies.empty() && dependencies.front() == '"' && dependencies.back() == '"') {
            dependencies = dependencies.substr(1, dependencies.size() - 2);
        }

        std::stringstream dep_ss(dependencies);
        while (std::getline(dep_ss, cell, ',')) {
            if (!cell.empty()) {
                boost::add_edge(std::stoi(cell), task_id, my_network);
            }
        }
    }
    return my_network;
}

// Funkcja obliczająca czasy CPM (bez zmian)
void alghorytm_for_AN(Network_Activity_On_Node& network_AN) {
    typedef boost::graph_traits<Network_Activity_On_Node>::in_edge_iterator in_edge_iterator;
    typedef boost::graph_traits<Network_Activity_On_Node>::out_edge_iterator out_edge_iterator;

    std::vector<Task> topo_order;
    boost::topological_sort(network_AN, std::back_inserter(topo_order)); // Odwrotna kolejność topologiczna

    // Forward Pass
    std::vector<Task> forward_order(topo_order.rbegin(), topo_order.rend());
    for (Task v : forward_order) {
        std::vector<Task> predecessors;
        auto inEdges = boost::in_edges(v, network_AN);
        for (in_edge_iterator iter = inEdges.first; iter != inEdges.second; ++iter) {
            Task u = boost::source(*iter, network_AN);
            predecessors.push_back(u);
        }
        int max_time_cost = 0;
        for (Task dep : predecessors) {
            int finish_time = network_AN[dep].earliest_start + network_AN[dep].duration;
            if (finish_time > max_time_cost) {
                max_time_cost = finish_time;
            }
        }
        network_AN[v].earliest_start = max_time_cost;
        network_AN[v].earliest_finish = network_AN[v].earliest_start + network_AN[v].duration;


    }

    // Backward Pass
    for (Task v : topo_order) {
        std::vector<Task> successors;
        auto outEdges = boost::out_edges(v, network_AN);
        for (out_edge_iterator iter = outEdges.first; iter != outEdges.second; ++iter) {
            Task u = boost::target(*iter, network_AN);
            successors.push_back(u);
        }
        if (successors.empty()) {
            network_AN[v].latest_finish = network_AN[v].earliest_finish;
        } else {
            int min_latest_start = std::numeric_limits<int>::max();
            for (Task dep : successors) {
                if (network_AN[dep].latest_start < min_latest_start) {
                    min_latest_start = network_AN[dep].latest_start;
                }
            }
            network_AN[v].latest_finish = min_latest_start;
        }
        network_AN[v].latest_start = network_AN[v].latest_finish - network_AN[v].duration;


    }
}

// Funkcja do znajdowania ścieżki krytycznej
std::vector<Task> find_critical_path(const Network_Activity_On_Node& network_AN) {
    std::vector<Task> critical_path;
    boost::graph_traits<Network_Activity_On_Node>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(network_AN); vi != vi_end; ++vi) {
        Task v = *vi;
        if (network_AN[v].earliest_start == network_AN[v].latest_start) {
            critical_path.push_back(v);
        }
    }
    return critical_path;
}

// Funkcja do wyświetlania harmonogramu
void print_schedule(const Network_Activity_On_Node& network_AN) {
    std::cout << "\nHarmonogram projektu:\n";
    boost::graph_traits<Network_Activity_On_Node>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(network_AN); vi != vi_end; ++vi) {
        Task v = *vi;
        std::cout << "Zadanie " << v
                  << ": Start = " << network_AN[v].earliest_start
                  << ", Koniec = " << network_AN[v].earliest_finish << std::endl;
    }
}

// Funkcja do obliczania długości harmonogramu
int get_schedule_length(const Network_Activity_On_Node& network_AN) {
    int max_finish = 0;
    boost::graph_traits<Network_Activity_On_Node>::vertex_iterator vi , vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(network_AN); vi != vi_end; ++vi) {
        Task v = *vi;
        if (network_AN[v].earliest_finish > max_finish) {
            max_finish = network_AN[v].earliest_finish;
        }
    }
    return max_finish;
}

int main() {
    Network_Activity_On_Node graph = loade_task_to_network("tasks.csv");

     alghorytm_for_AN(graph);

    // Znajdź i wydrukuj ścieżkę krytyczną
    std::vector<Task> critical_path = find_critical_path(graph);
    std::cout << "\nŚcieżka krytyczna: ";
    for (Task v : critical_path) {
        std::cout << v + 1<< " ";
    }
    std::cout << std::endl;

    // Stwórz i wydrukuj harmonogram
    print_schedule(graph);

    // Oblicz i wydrukuj długość harmonogramu
    int schedule_length = get_schedule_length(graph);
    std::cout << "\nDługość harmonogramu: " << schedule_length << std::endl;

    return 0;
}
