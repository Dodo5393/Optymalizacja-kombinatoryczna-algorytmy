#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/pending/property.hpp>
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
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace boost;

struct task_parameters {
    int duration;
    int earliest_start;
    int latest_start;
};

typedef boost::adjacency_list<vecS, vecS, directedS, task_parameters> Network_Activity_On_Node;

typedef boost::graph_traits<Network_Activity_On_Node>::vertex_descriptor Task;
typedef boost::graph_traits<Network_Activity_On_Node>::edge_descriptor Dependence;


Network_Activity_On_Node loade_task_to_network(const std::string& filename){

   Network_Activity_On_Node my_network;
   std::ifstream file(filename);
   std::string line;

   //ignore header
   std::getline(file, line);


   while(std::getline(file, line)){
       std::stringstream ss(line);
       std::string cell, dependencies;

       int task_id;
       task_parameters task_from_file;

       std::getline(ss, cell, ',');


       task_id = std::stoi(cell);


       std::getline(ss, cell, ',');
       task_from_file.duration = std::stoi(cell);

       boost::add_vertex(task_from_file, my_network);


       //format np."1,3,5,4" -> 1,3,5,4
       std::getline(ss, dependencies);
       if (!dependencies.empty() && dependencies.front() == '"' && dependencies.back() == '"') {
           dependencies = dependencies.substr(1, dependencies.size() - 2);
       }

       std::stringstream dep_ss(dependencies);
       while(std::getline(dep_ss, cell, ',')){
           if(!cell.empty()){
               boost::add_edge(std::stoi(cell), task_id, my_network);
           }
       }
   }

   return my_network;
}

int main() {

    Network_Activity_On_Node graph = loade_task_to_network("tasks.csv");

    boost::print_graph(graph);




    return 0;
}
