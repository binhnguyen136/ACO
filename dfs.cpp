#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#define maxn 100
using namespace std;

struct edge{
	int u, v, w;
};

vector<edge> T;
int cost = 0,
	graph[maxn][maxn], n, m,
	visited[maxn],
	parent[maxn];
vector<int> vertices;

bool dfs(int v){
	visited[v] = true;
	vertices.push_back(v);
	cout << v << " ";
	for(int i=1; i<=n; i++)
	 	if(graph[v][i] && !visited[i])
	 		dfs(i);
}

void cycle(int par, int v){
	parent[v] = par;
	for(int i=1; i<=n; i++)
	 	if(graph[v][i] && i!=par && !parent[i]){
	 		cycle(v, i);
	 	}
}

void init_parent(){
	for(int i=1; i<=n; i++)
		parent[i] = 0;	
}

void getcycle(int i){
	if(find(vertices.begin(), vertices.end(), i)==vertices.end()){
		vertices.push_back(i);
		getcycle(parent[i]);
	}else{
		vertices.push_back(i);
	}
}

void remove_max_edge(){
	vector<edge> list;
	int index = 0, max = 0;	
	for(int i=0; i<vertices.size()-1; i++){
		cout << vertices[i] << " - " << vertices[i+1] << " : " << graph[vertices[i]][vertices[i+1]] << endl;
		
		edge e;
		e.u = vertices[i];
		e.v = vertices[i+1];
		e.w = graph[e.u][e.v];
		list.push_back(e);
		
		if(e.w > max){
			max = e.w;
			index = i;
		}		
	}

	edge e = list[index];
	vector<edge>::iterator it;
	for(it = T.begin(); it != T.end(); it++){
		if((it->u == e.u && it->v == e.v && it->w == e.w) || 
			it->u == e.v && it->v == e.u && it->w == e.w){
			T.erase(it);
			break;
		}
	}	
}

void print_T(){
	cost = 0;
	for(int i=0; i<T.size(); i++){
		cout << T[i].u << " - " << T[i].v << " : " << T[i].w << endl;
		cost += T[i].w;
	}
	cout << "Cost = " << cost << endl;
	
}

void print_cycle(){
	for(int i=0; i<vertices.size(); i++)
		cout << vertices[i] << " ";	
}

void local_search(){
	print_T();	

	init_parent();
	cycle(0,2);
	getcycle(2);
	
	cout << "\n";
	print_cycle();
			
	cout << "\n\n";
	remove_max_edge();
	
	cout << endl;
	print_T();	
}

void read_file(){
	ifstream in("data1.txt");
	in >> n >> m;
	for(int i=1; i<=m ;i++){
		int u, v, w;
		in >> u >> v >> w;
		graph[u][v] = graph[v][u] = w;
		
		edge e;
		e.u = u;
		e.v = v;
		e.w = w;
		T.push_back(e);
	}
	in.close();	
}

int main(){
	read_file();
	local_search();
}









