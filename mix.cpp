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
#define alpha 3
#define belta 2
#define accuracy 10
#define max_pher 15
#define min_pher 1

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
	//debug
	for(int i=0; i<num; i++){
		universe += calculate(pheromone[list_ver[i]], graph[current][list_ver[i]]);
	}
	//debug
	//cout << endl << "Universe : " << universe << endl;
	for(int i=0; i<num; i++){
		prob[i] = ((calculate(pheromone[list_ver[i]], graph[current][list_ver[i]])) / universe) * 100;
		//debug
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
		if((clock() - begin)/1000.0 >= 0.05){
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

void local_search(vector<edge> T, vector<int> vertices){
	
}

void aco(){
	vector<int> vertices;
	setup_pher();
	
	/*int index = 0; 
	vertices.push_back(terminal[index]);
	setup_pher();
	double begin_choose_path = clock();
	for(int i=0; i<t_n; i++){
		if(i != index){
			vector<int> path = choose_path(terminal[i], vertices, begin_choose_path);
			int cost = 0;
			for(int j=0; j<path.size()-1; j++)
				cost += graph[path[j]][path[j+1]];
			cout << "-------------------------Path's Cost : " << cost << "----------------------------" << endl;
		}
	}*/
	int count = 0;
	while(count < 250 ){
	int ter_index = 0;
	for(int i=0; i<t_n; i++){
		
		if(switch_terminal){
			ter_index++;
            switch_terminal = false;
		}
		tmp_vertices.push_back(terminal[ter_index++]);
		
		double begin_choose_tree = clock();
		while(!visit_all(tmp_vertices)){
	        if((clock() - begin_choose_tree)/1000.0 >= 0.5){
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
		
		local_search(tmp_T, tmp_vertices);
		
		for(int i=0; i<tmp_T.size(); i++){
			tmp_cost += tmp_T[i].w;
		}
		cout << "SMT tmp cost : " << tmp_cost << endl;
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
	ifstream in("Steiner_Test/TB/Steinb13.txt");
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













