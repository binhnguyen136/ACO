#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <math.h>
#include <time.h>

#define MAX_N 3001
#define init_pher 1 
#define co_eva 0.05 //coefficient of evaporation in range 0.001 ... 0.01
#define alpha 5
#define belta 2
#define accuracy 5
#define max_count 100
#define time_tree_limit 500
#define time_path_limit 5000
#define max_pher 15
#define min_pher 1.7

using namespace std;

struct edge{
	int u, v, w;
};

struct next_v {
	int s, next;
};

int n, m, graph[MAX_N][MAX_N], 
	t_n, terminal[MAX_N], 
	cost = INT_MAX;
//store given edges and edges of result tree	
vector<edge> graph_edge,  T;	
//store pheromone on each vertex of graph
//index is vertex
double	pheromone[MAX_N];

bool switch_terminal = false;

//----------------------------
// used for find smt
vector<int> tmp_vertices;
vector<edge> tmp_T;
int tmp_cost = 0;
// used for local search
vector<int> cycle_vertices;
//---------------------------

void setup_pher(){
	for(int i=1; i<=n; i++){
		pheromone[i] = init_pher;
	}
}

void update_pher(vector<int> vertices){
	for(int i=1; i<=n; i++){
		if(find(vertices.begin(), vertices.end(), i) != vertices.end()){
			if(pheromone[i]+1.0/cost < max_pher)
				pheromone[i] += 1.0/cost;
		}else {
			if(pheromone[i]*(1 - co_eva) > min_pher)
				pheromone[i] *= (1 - co_eva);
		}
	}
}

double calculate(double pheromone, int dist){
	//if(pheromone >= 1)
		return pow(pheromone, alpha) * pow((double)1/(dist*cost), belta);
	//else {
		//return pheromone * pow((double)1/(dist*cost), belta);
	//}
}

void cal_prob(int num, int list_ver[], double prob[], int current){
	//double universe;
	double universe = 0 ;
	
	for(int i=0; i<num; i++){
		universe += calculate(pheromone[list_ver[i]], graph[current][list_ver[i]]);
	}

	//cout << endl << "Universe : " << universe << endl;
	for(int i=0; i<num; i++){
		prob[i] = ((calculate(pheromone[list_ver[i]], graph[current][list_ver[i]])) / universe) * 100;
		//cout << "Probability at vertex : " << list_ver[i] << " : " << prob[i] << endl;
	}
}

int probability(int num,int list_ver[], double prob[]){
	double ran = rand() % 101,
	tmp = 0;
	for(int i=0; i<num; i++){
		if(ran <= (tmp += prob[i]))
			return list_ver[i];
	}
	return -1;
}

int select_random(int num, int list_ver[], int current){
	int count[MAX_N] = {0}, max = 0, index = -1;
	double prob[MAX_N];
	cal_prob(num, list_ver, prob, current);
	
	srand(time(NULL));
	//accuracy (IMPORTANT)
	for(int i=0; i<accuracy; i++){
		int tmp = probability(num, list_ver, prob);
		count[tmp]++;
	}
	for(int i=0; i<num; i++){
		if(count[list_ver[i]] > max){
			max = count[list_ver[i]];
			index = list_ver[i];
		}
	}
	return index;
}

bool belong_tree(int current, vector<int> vertices){
	return find(vertices.begin(), vertices.end(), current) != vertices.end();
}

bool belong_path(int v, vector<int> path){
	return find(path.begin(), path.end(), v) != path.end();
}

bool hanging_vertex(int current){
	int count = 0;
	for(int i=1; i<=n; i++)
		if(graph[current][i])
			count++;
	return count == 1;
}

vector<int> choose_path(int terminal, vector<int> vertices, double begin){
	bool visited[MAX_N];
	vector<int> path;
	vector<next_v> list;

	int current = terminal;
	path.push_back(current);
	
	//cout << "terminal : " << terminal << endl;
	
	for(int i=1; i<=n; i++)
		visited[i] = false;
	
	while(!belong_tree(current, vertices)){
		if((clock() - begin)/1000.0 >= time_path_limit){
			switch_terminal = true;
			vector<int> null;
			return null;
		}
		
		if(!visited[current]){
			for(int i=1; i<=n; i++){
				if(graph[current][i] && !belong_path(i, path) && !hanging_vertex(i)){
					next_v v;
					v.s = current;
					v.next = i;
					list.push_back(v);
				}
			}
			visited[current] = true;
		}
				
		int list_choose[MAX_N], num = 0;
		int index = list.size()-1;

		for(int i=0; i<list.size(); i++){
			if(list[i].s == current){
				list_choose[num] = list[i].next;
				num++;
			}
		}
				
		if(num == 0){
		//	cout << "num = 0" << endl;
			visited[current] = false;
			path.pop_back();
			current = path.back();
			continue;
		}
		
		int tmp = current;
		current = select_random(num, list_choose, current);
		path.push_back(current);
		
		//cout << "selected vertex : " << current << endl;
		//cout <<endl<< "Vertex Stack : " << endl;
		//for(int i=0; i<path.size(); i++)
		//	cout << path[i] << " "; cout << endl;
		
		vector<next_v>::iterator it;
		for(it = list.begin(); it != list.end(); it++){
			if(it->s == tmp && it->next == current){
				list.erase(it);
				break;
			}				
		}

	}
	return path;
}

bool visit_all(vector<int> tmp_vertices){
	for(int i=0; i<t_n; i++)
		if(find(tmp_vertices.begin(), tmp_vertices.end(), terminal[i]) == tmp_vertices.end())
			return false;
	return true;
}

//-------------------------------------------------------

bool is_used_edge(edge e, vector<edge> t){
	vector<edge>::iterator it;
	for(it = t.begin(); it != t.end(); it++)
		if((it->u == e.u && it->v == e.v && it->w == e.w) || (it->u == e.v && it->v == e.u && it->w == e.w))
			return true;
	return false;
}

bool is_in_tree(int u, int v){
	return find(tmp_vertices.begin(), tmp_vertices.end(), u)!=tmp_vertices.end() &&
		   find(tmp_vertices.begin(), tmp_vertices.end(), v)!=tmp_vertices.end();
}

bool compare(edge a, edge b){
	return a.w > b.w;
}

void init_component(int parent[],int num){
	for(int i=1; i<=n; i++)
		parent[i] = 0;

	while(!cycle_vertices.empty())
		cycle_vertices.pop_back();
}

void cycle(int par, int v,int num, int parent[]){
	parent[v] = par;
	for(int i=1; i<=n; i++){
		edge e;
		e.u = v; 
		e.v = i;
		e.w = graph[v][i];
	 	if(is_used_edge(e, tmp_T) && i!=par && !parent[i]){
	 		cycle(v, i, num, parent);
	 	}		
	}

}

void getcycle(int parent[], int i){
	if(find(cycle_vertices.begin(), cycle_vertices.end(), i)== cycle_vertices.end()){
		cycle_vertices.push_back(i);
		getcycle(parent, parent[i]);
	}else{
		cycle_vertices.push_back(i);
	}
}

void remove_max_edge(){
	edge max_e;
	max_e.w = 0;
	max_e.u = 0;
	max_e.v = 0;
	for(int i=0; i<cycle_vertices.size()-1; i++){
		//cout << cycle_vertices[i] << " - " << cycle_vertices[i+1] << " : " << tmp_graph[cycle_vertices[i]][cycle_vertices[i+1]] << endl;
		
		edge e;
		e.u = cycle_vertices[i];
		e.v = cycle_vertices[i+1];
		e.w = graph[e.u][e.v];
		
		if(e.w > max_e.w){
			max_e.w = e.w;
			max_e.u = e.u;
			max_e.v = e.v;
		}		
	}
	bool found = false;
	vector<edge>::iterator it;
	for(it = tmp_T.begin(); it != tmp_T.end(); it++){
		if((it->u == max_e.u && it->v == max_e.v && it->w == max_e.w) || 
			(it->u == max_e.v && it->v == max_e.u && it->w == max_e.w)){
			//cout << "erase : " << max_e.u << " - " << max_e.v << " : " << max_e.w << endl;
			tmp_T.erase(it);
			found = true;
			break;
		}
	}	
	if(!found)
		cout << "edge to erase not found !!! : " <<max_e.u << " - " << max_e.v << " : " << max_e.w <<endl;
}


void local_search(){
	int num = tmp_vertices.size()+1,
		parent[MAX_N];
	
	vector<edge> list_add;
	for(int i=0; i<graph_edge.size(); i++)
		if(!is_used_edge(graph_edge[i], tmp_T) && is_in_tree(graph_edge[i].u, graph_edge[i].v))
			list_add.push_back(graph_edge[i]);
			
	if(!list_add.empty()){
		sort(list_add.begin(), list_add.end(), compare);
	
		tmp_cost = 0;
		for(int i=0; i<tmp_T.size(); i++){
			tmp_cost += tmp_T[i].w;
		}
		
		int tmp_cost_changed;

		do{	
			tmp_cost_changed = tmp_cost;
			edge add_edge = list_add.back();
			//cout << "add : " << add_edge.u <<" - " <<add_edge.v << " : " << add_edge.w << endl;
			tmp_T.push_back(add_edge);
			
			list_add.pop_back();
			
			init_component(parent, num);
			
			cycle(0, add_edge.u,num, parent);
			getcycle(parent, add_edge.u);
			remove_max_edge();
			
			tmp_cost = 0;
			for(int i=0; i<tmp_T.size(); i++){
				tmp_cost += tmp_T[i].w;
			}
		}while(tmp_cost_changed != tmp_cost && !list_add.empty());		
	}				
}

//-------------------------------------------------------

void aco(){
	vector<int> vertices;
	setup_pher();
	
	int count = 0;
	while(count < max_count ){
	int ter_index = 0;
	for(int i=0; i<t_n; i++){
		while(!tmp_T.empty())
			tmp_T.pop_back();
		while(!tmp_vertices.empty())
			tmp_vertices.pop_back();
		tmp_cost = 0;
		
		if(switch_terminal){
			ter_index++;
           switch_terminal = false;
		}
		tmp_vertices.push_back(terminal[ter_index++]);
		
		double begin_choose_tree = clock();
		while(!visit_all(tmp_vertices)){
	        if((clock() - begin_choose_tree)/1000.0 >= time_tree_limit){
	                    switch_terminal = true;
	                    break;
	        }
	        int path_cost = INT_MAX;
	        vector<int> path;
			for(int j=0; j<t_n; j++){
				if(find(tmp_vertices.begin(), tmp_vertices.end(), terminal[j]) == tmp_vertices.end()){
					//Set time area
					double begin_choose_path = clock();
					//
					vector<int> tmp_path = choose_path(terminal[j], tmp_vertices, begin_choose_path);
					if(switch_terminal)
						break;
					int tmp_path_cost = 0;
					for(int i=0; i<tmp_path.size()-1; i++){
						tmp_path_cost += graph[tmp_path[i]][tmp_path[i+1]];
					}	
					if(tmp_path_cost < path_cost){
						path_cost = tmp_path_cost;
						path = tmp_path;
					}
				}
			}
			if(switch_terminal)
				break;
			for(int z=0; z<path.size()-1; z++){
				tmp_vertices.push_back(path[z]);
				edge e ;
				e.u = path[z];
				e.v = path[z+1];
				e.w = graph[e.u][e.v];
				tmp_T.push_back(e);
			}			
		}
		if(switch_terminal)
			continue;
		tmp_cost = 0;
		for(int i=0; i<tmp_T.size(); i++){
			tmp_cost += tmp_T[i].w;
		}
		cout << "SMT tmp cost before local search : " << tmp_cost << endl;		
		
		local_search();
		
		tmp_cost = 0;
		for(int i=0; i<tmp_T.size(); i++){
			tmp_cost += tmp_T[i].w;
		}
		cout << "SMT tmp cost after local search : " << tmp_cost << endl << endl;
		
		if(tmp_cost < cost){
			vertices = tmp_vertices;
			T = tmp_T;
			cost = tmp_cost;
			//count = 0;
			
		}else{
              count++;
        }
        update_pher(vertices);
	}
    }
	
	cout << "SMT cost : " << cost << endl;
	
}

void read_file(){
	ifstream in("Steiner_Test/TB/Steinb18.txt");
	in >> n;
	in >> m;
	for(int i=0; i<m; i++){
		edge tmp;
		in >> tmp.u;
		in >> tmp.v;
		in >> tmp.w;
		graph_edge.push_back(tmp);
		graph[tmp.u][tmp.v] = graph[tmp.v][tmp.u] = tmp.w;
	}
	in >> t_n;
	for(int i=0; i<t_n; i++){
		in >> terminal[i]; 
	}

}

int main(){
	read_file();
	int start = clock();
	aco();
	cout << (clock()-start)/1000.0 << "s";
}


