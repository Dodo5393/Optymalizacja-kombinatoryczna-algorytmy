//I'm not going to sleep today too
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/pending/property.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/distance.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/size.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstddef>
#include <cstdio>
#include <ios>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stack>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>


using namespace std;
using namespace boost;

typedef boost::adjacency_list<  boost::vecS,
                                boost::vecS,
                                boost::undirectedS,
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



bool true_triangle_condition(const Graph& g){
    auto weight_map = boost::get(boost::edge_weight, g);


    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi){
        Vertex u = *vi;
        boost::graph_traits<Graph>::out_edge_iterator ei1, ei1_end;
        for(boost::tie(ei1, ei1_end) = boost::out_edges(u, g); ei1 != ei1_end; ++ei1){
            Vertex v = boost::target(*ei1, g);
            double d_uv = boost::get(weight_map, *ei1);

            boost::graph_traits<Graph>::out_edge_iterator ei2, ei2_end;
            for(boost::tie(ei2, ei2_end) = boost::out_edges(v, g); ei2 != ei2_end; ++ei2){
                Vertex w = boost::target(*ei2 ,g);
                if (w == u) continue;

                double d_vw = boost::get(weight_map, *ei2);

                std::pair<Edge, bool> edge_uw = boost::edge(u, w, g);
                if(!edge_uw.second) continue;

                double d_uw = boost::get(weight_map, edge_uw.first);

                if (d_uv + d_vw < d_uw){
                    return false;
                }
            }
        }
   }
return true;
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


std::vector<std::pair<Vertex, Vertex>> perfect_association_with_minimum_weight(const Graph& complete_graph) {

    using Edge = std::pair<Vertex, Vertex>;
    std::vector<Edge> edges;
    std::cout << "DEBUG: perfect_matching contents:\n";
    auto edge_weight  = boost::get(boost::edge_weight, complete_graph);
    for (auto const& e : boost::make_iterator_range(boost::edges(complete_graph))){
        Vertex u = boost::source(e, complete_graph);
        Vertex v = boost::target(e, complete_graph);
        double w = edge_weight[e];

        if (u != v ){

        edges.push_back({u, v});
    }
    }

    std::sort(edges.begin(), edges.end(), [&](const Edge& a, const Edge& b) {
        return edge_weight[boost::edge(a.first, a.second, complete_graph).first] <
               edge_weight[boost::edge(b.first, b.second, complete_graph).first];
    });

      std::vector<bool> used(boost::num_vertices(complete_graph), false);
      std::vector<std::pair<Vertex, Vertex>> matching;

      for (const auto& e : edges) {
          Vertex u = e.first;
          Vertex v = e.second;



          if (!used[u] && !used[v]) {
              matching.push_back({u, v});
              used[u] = true;
              used[v] = true;
          }

          if (matching.size() == boost::num_vertices(complete_graph) / 2) {
              break;
          }
      }

      return matching;
}


std::vector<Vertex> algorytm_hierholzer(const Graph& H){

    std::vector<Vertex> cycle;
    std::stack<Vertex> stack;
    std::vector<std::vector<Vertex>> adj_list(num_vertices(H));

    boost::graph_traits<Graph>::edge_iterator ei, ei_end;
    for (std::tie(ei, ei_end) = edges(H); ei != ei_end; ei++){
        Vertex u = boost::source(*ei, H);
        Vertex v = boost::target(*ei,H);

        adj_list[u].push_back(v);
        adj_list[v].push_back(u);
    }

    Vertex start = 0;
    for( size_t i = 0 ; i < adj_list.size(); i++){
        if(!adj_list[i].empty()){
            start = i;
        }
    }

    stack.push(start);
    while(!stack.empty()){

        Vertex v = stack.top();
        if (!adj_list[v].empty()){
            Vertex u = adj_list[v].back();
            adj_list[v].pop_back();

            auto it = std::find(adj_list[u].begin(), adj_list[u].end(), v);
            if (it != adj_list[u].end()) adj_list[u].erase(it);

            stack.push(u);
        } else {
             cycle.push_back(v);
             stack.pop();
         }
     }

     return cycle;
    }





Graph algorytm_christofedes(const Graph& g){

    std::vector<Edge> mst_edges;
    boost::kruskal_minimum_spanning_tree(g, std::back_inserter(mst_edges));

    std::unordered_map<Vertex, int> mst_vexters_degrees;
    cout << "------mst-------\n";
    for (const auto& e : mst_edges){
        Vertex u = boost::source(e, g);
        Vertex v = boost::target(e, g);

        cout << u << "<->" << v << std::endl;

        mst_vexters_degrees[u]++;
        mst_vexters_degrees[v]++;
    }

    cout << "-------------odd degress------------\n";
    std::vector<Vertex> mst_vexters_odd_degrees;
    for (const auto& [vexter, degree] : mst_vexters_degrees){



        if( degree %2 == 1 ){
            mst_vexters_odd_degrees.push_back(vexter);

             cout << vexter << std::endl;
        }
    }


    int n = boost::num_vertices(g);
    int mst_v_o_d_size =  mst_vexters_odd_degrees.size();


    std::vector<std::vector<double>> distances(mst_v_o_d_size, std::vector<double>(mst_v_o_d_size));

    std::map<std::pair<Vertex, Vertex>, std::vector<Edge>> shortest_patchs;


    for (int i = 0; i < mst_v_o_d_size; i++){
            std::vector<double> dist;
            std::vector<Vertex> pred;

            compute_shortest_path(g, mst_vexters_odd_degrees[i], dist, pred);
            for (int j = 0; j < mst_v_o_d_size; j++){
                distances[i][j] = dist[mst_vexters_odd_degrees[j]];
                if (i != j){
                    auto path = get_shortest_path_edges(g, mst_vexters_odd_degrees[i], mst_vexters_odd_degrees[j], pred);
                    shortest_patchs[{mst_vexters_odd_degrees[i], mst_vexters_odd_degrees[j]}] = path;
                }
            }
        }



        Graph complite_graph(mst_v_o_d_size);

        for(int i = 0; i < mst_v_o_d_size; i++){
            for(int j = 0; j < mst_v_o_d_size; j++){
                boost::add_edge(i,j,distances[i][j], complite_graph);


            }
        }


        std::cout << "-------------full graph---------\n";
        // Iterujemy po krawędziach pełnego grafu
        auto edges_pair = boost::edges(complite_graph);
        for (auto it = edges_pair.first; it != edges_pair.second; ++it) {
            auto e = *it;
            Vertex u = boost::source(e, complite_graph);
            Vertex v = boost::target(e, complite_graph);
            double weight = boost::get(boost::edge_weight, complite_graph, e);
            std::cout << u << " < -- > " << v << " waga: " << weight << std::endl;
        }

        std::vector<std::pair<Vertex , Vertex>> perfect_matching(boost::num_vertices(complite_graph));


        perfect_matching =  perfect_association_with_minimum_weight(complite_graph);

        std::cout << "-------------perfect mathing---------\n";
        for (int i = 0; i < perfect_matching.size(); ++i) {
            std::pair<Vertex, Vertex> pair = perfect_matching[i];
            Vertex u = perfect_matching[i].first;
            Vertex v = perfect_matching[i].second;
            // Sprawdzamy, czy wierzchołek jest sparowany oraz żeby nie wypisać pary dwa razy
            std::cout << u << " < -- > " << v << std::endl;

        }
        cout << "--------dodane mst -------------\n";
        Graph H(mst_vexters_degrees.size());
        for (auto const& e : mst_edges){
            Vertex u = boost::source(e, g);
            Vertex v = boost::target(e, g);
            double w = boost::get(edge_weight,g ,e);

            cout << u << "< -- >" << v << ":" << w<<std::endl;

            boost::add_edge(u,v,w,H);
        }


        cout << "--------dodane M -------------\n";
        for (std::size_t i = 0 ; i  < perfect_matching.size(); i++){
            Vertex u = perfect_matching[i].first;
            Vertex v = perfect_matching[i].second;

            if ( v != boost::graph_traits<Graph>::null_vertex() && u < v){
                auto edge = boost::edge(u, v, complite_graph);

                if (edge.second){
                    double w = boost::get(boost::edge_weight , complite_graph, edge.first);

                    cout << u << "< -- >" << v << ":" << w << std::endl;

                    add_edge(mst_vexters_odd_degrees[u], mst_vexters_odd_degrees[v], w, H);

                }
            }
        }

        std::vector<Vertex> euler_cycle = algorytm_hierholzer(H);
        cout << "--------------cykl Eulera -----------\n";
        for(std::size_t i = 0; i < euler_cycle.size(); i++){
            cout << euler_cycle[i] << std::endl;
        }

        // Tworzymy cykl Hamiltona poprzez usunięcie powtórzeń (pierwsze wystąpienie danego wierzchołka zostaje zachowane)
        std::vector<Vertex> hamilton_cycle;
        std::unordered_set<Vertex> visited;
        for (Vertex v : euler_cycle) {
            if (visited.find(v) == visited.end()) {
                hamilton_cycle.push_back(v);
                visited.insert(v);
            }
        }

        if (hamilton_cycle.front() != hamilton_cycle.back()) {
            hamilton_cycle.push_back(hamilton_cycle.front());
        }

        double sum_weights = 0.0;
        std::cout << "-------------- Cykl Hamiltona -----------\n";
        for (std::size_t i = 0; i < hamilton_cycle.size() - 1; i++) {
            Vertex u = hamilton_cycle[i];
            Vertex v = hamilton_cycle[i + 1];

            auto edge_pair = boost::edge(u, v, H);
            if (edge_pair.second) {  // krawędź istnieje
                double w = boost::get(boost::edge_weight, H, edge_pair.first);
                sum_weights += w;
                std::cout << u << " -> " << v << "   waga: " << w << std::endl;
            } else {
                std::cout << "Brak krawędzi pomiędzy " << u << " a " << v << std::endl;
            }
        }
        std::cout << "Suma wag: " << sum_weights << std::endl;



        return H;


}




int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();

    Graph g = load_graph_from_file(argv[1]);

    cout << "------g-------\n";
    boost::print_graph(g);

    if (true_triangle_condition(g)) {
        Graph H = algorytm_christofedes(g);

        cout << "------H-------\n";
        boost::print_graph(H);
    } else {
        cout << "------no triangle condition satisfied--------\n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Czas wykonania oryginalnego programu: " << duration.count() << " sekund" << std::endl;

    return 0;
}
