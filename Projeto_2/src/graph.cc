#include "../include/graph.h"

void Graph::addVertice(string subject, int weight) {
    vector<pair<string, int>> vazio;
    graph[subject] = vazio;
}

void Graph::writeGraph() {
    fstream out;
    out.open("output.txt");
    if(!out.is_open()) {
        cout << "Nao foi possivel abrir o arquivo de output\n";
        exit(-2);
    }
    out << "// G(e) é o grau de entrada de cada matéria, ou seja, quantos pré-requisitos ela tem.\n";
    out << "// G(o) é o grau de saida de cada matéria, ou seja, de quantas materias ela é pré-requisito.\n\n";
    for (auto node : graph) {
        out << "G(e)=[" << inputDegree[node.first] << "], ";
        out << "G(o)=[" << outDegree[node.first] << "] | ";
        out << node.first << ": ";
        for (auto par : node.second) {
            out << par.first << ", " << par.second << "; ";
        }
        out << "\n";
    }
}

void Graph::addEdge(string src, pair<string, int> dest) {
    if (graph.find(dest.first) == graph.end()) {
        this->addVertice(dest.first, dest.second);
    }
    inputDegree[dest.first]++;
    outDegree[src]++;
    graph[src].push_back(dest);
}

void Graph::showTopSort() {
    if (topologicSorted.empty()) {
        this->TopSort();
    }

    cout << "\tOrdenacao Topologica: \n";

    for (auto el : topologicSorted) {
        cout << el << "\n";
    }
}

void Graph::TopSort() {
    map<string, bool> visited;
    for (auto node : graph) {
        if (inputDegree[node.first] == 0) {
            dfs(node.first, visited);
        }
    }
    reverse(topologicSorted.begin(), topologicSorted.end());
}

void Graph::dfs(string vertex, map<string, bool>& visited) {
    visited[vertex] = true;

    for (auto neighb : graph[vertex]) {
        if (!visited[neighb.first]) {
                dfs(neighb.first, visited);
        }
    }

    topologicSorted.push_back(vertex);
}

void Graph::showCriticalPath() {
    if (topologicSorted.empty()) {
        this->TopSort();
    }

    cout << "\tCaminho Critico: \n";

    map<string, bool> visited;
    map<string,int> pesos;
    map<string,int> pesosAntigos;

    for (auto starter : topologicSorted) {
        if (inputDegree[starter] != 0) {
            continue;
        }
        computeCriticalPath(starter, visited, pesos, pesosAntigos);
    }

    ofstream allPaths;
    allPaths.open("caminhos.txt");
    if (!allPaths.is_open()) {
        cout << "nao foi possivel abrir o arquivo dos caminhos.\n";
        exit(-2);
    }

    caminhos.sort( 
        [] (const pair<vector<string>, int>& x, const pair<vector<string>, int>& y) {
            return (x.second < y.second);
        }
    );

    // como dei sort, entao o maior caminho está em ultimo
    vector<string> maior_caminho = caminhos.back().first;
    int maiorPeso = caminhos.back().second;

    for (auto caminho: caminhos) {
        for (auto x : caminho.first) {
            allPaths << x << "->";
        }
        allPaths << " | Peso Total = " << caminho.second << "\n";
    }

    for (auto x : maior_caminho) {
        cout << x << "->";
    }
    cout << " | Peso Total = " << maiorPeso << "\n";
}

void Graph::computeCriticalPath(string partida, map<string, bool>& visited, map<string, int>& pesos, map<string, int>& pesosAntigos) {
    
    visited[partida] = true;
    criticalPath.push_back(partida);

    for (auto adj : graph[partida]) {
        if (pesosAntigos[adj.first] != 0) {
            pesos[adj.first] = max(pesos[adj.first], pesosAntigos[adj.first]);
        }
        pesosAntigos[adj.first] = max(pesosAntigos[partida], pesosAntigos[partida] + adj.second);
        if (!visited[adj.first]) {
            this->computeCriticalPath(adj.first, visited, pesos, pesosAntigos);
        }
    }

    if (outDegree[partida] == 0) {
    //     cout << "\tCaminhos: \n";
    //     for (auto x : criticalPath) {
    //         cout << x << " -> ";
    //     }
    //     cout << " | peso do caminho: " << pesosAntigos[partida] << "\n\n";



        //////// ACHO Q ESSA É A SOLUÇÃO, MAS PRECISA DE MAIS CRITERIA PRA INSERIR PQ TA DANDO BAD ALLOC
        /////// MAS NO LINUX FUNCIONA
        pair<vector<string>, int> toPush;
        toPush.first = criticalPath;
        toPush.second = pesosAntigos[partida];
        caminhos.push_back(toPush);
    }
    visited.clear();    
    criticalPath.pop_back();
}