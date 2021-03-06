#include <bits/stdc++.h>
using namespace std;

auto start = std::chrono::high_resolution_clock::now();
auto stop = start;
chrono::milliseconds time_limit(100);

struct edge {
    int flow;
    const int capacity, to, from, cost;
    edge *reversed;
    edge(int from, int to, int cost, int flow = 0, int capacity = 1) 
        : to(to), from(from), cost(cost), flow(flow), capacity(capacity), reversed(nullptr) {}  

    bool used() {
        return !(flow < capacity);
    }

    void inc_flow() {
        flow++;
        reversed->flow--;
    }
};

void add_inner_edge(vector< vector<edge*> > &g, int v) {
    edge *forward = new edge(v * 2, v * 2 + 1, 0);
    edge *backward = new edge(v * 2 + 1, v * 2, 0, 1);
    forward->reversed = backward;
    backward->reversed = forward;
    g[v * 2].push_back(forward);
    g[v * 2 + 1].push_back(backward);
}

void add_edge(vector< vector<edge*> > &g, const int from, const int to, const int cost) {
    edge *fromto = new edge(from * 2 + 1, to * 2, cost);
    edge *fromto_reversed = new edge(to * 2, from * 2 + 1, -cost, 1);
    fromto->reversed = fromto_reversed;
    fromto_reversed->reversed = fromto;
    g[from * 2 + 1].push_back(fromto);
    g[to * 2].push_back(fromto_reversed);

    if (from != to) {
        edge *tofrom = new edge(to * 2 + 1, from * 2, cost);
        edge *tofrom_reversed = new edge(from * 2, to * 2 + 1, -cost, 1);
        tofrom->reversed = tofrom_reversed;
        tofrom_reversed->reversed = tofrom;
        g[to * 2 + 1].push_back(tofrom);
        g[from * 2].push_back(tofrom_reversed);
    }
}

long long find_shortest_paht_for_dummies(vector< vector<edge*> > &g, int from, int to) {
    vector< edge* > parent_edge(g.size(), nullptr);
    vector< long long > d(g.size(), __LONG_LONG_MAX__);
    vector< bool > used(g.size(), false);
    set< int, function<bool (int, int)> > set([&](int a, int b) {
        return d[a] == d[b] ? a < b : d[a] < d[b]; 
    });

    d[from] = 0;
    d[from - 1] = 0;
    set.insert(from);
    
    while (!set.empty()) {
        int v = *(set.begin());
        set.erase(set.begin());

        if (v == to) {
            break;
        }

        used[v] = true;
        for (auto e : g[v]) {
            if (e->used()) continue;
            if (used[e->to]) continue;
            int to = e->to;
            int cost = e->cost;
            if (d[to] > d[v] + cost) {
                set.erase(to);
                d[to] = d[v] + cost;
                parent_edge[to] = e;
                set.insert(to);
            }
        }
    }
    
    auto curr = parent_edge[to];
    while (curr) {
        curr->inc_flow();
        curr = parent_edge[curr->from];
    }

    return d[to];
}

long long find_shortest_paht(vector< vector<edge*> > &g, int from, int to) {
    vector< edge* > parent_edge(g.size(), nullptr);
    vector< long long > d(g.size(), __LONG_LONG_MAX__);
    static vector< long long > potential(g.size(), 0);
    set< int, function<bool (int, int)> > set([&](int a, int b) {
        return d[a] == d[b] ? a < b : d[a] < d[b]; 
    });

    d[from] = 0;
    d[from - 1] = 0;
    set.insert(from);
    
    while (!set.empty()) {

        // мой кастыль
        stop = std::chrono::high_resolution_clock::now();
        if (chrono::duration_cast<std::chrono::duration<double>>(stop - start) >= time_limit) {
            throw 20; // magic number
        }

        int v = *(set.begin());
        set.erase(set.begin());

        for (auto e : g[v]) {
            if (e->used()) continue;

            int to = e->to;
            int cost = e->cost + (potential[to] - potential[v]);
            if (d[to] > d[v] + cost) {
                set.erase(to);
                d[to] = d[v] + cost;
                parent_edge[to] = e;
                set.insert(to);
            }
        }
    }
    
    auto curr = parent_edge[to];
    while (curr) {
        curr->inc_flow();
        curr = parent_edge[curr->from];
    }

    long long ans = d[to] == __LONG_LONG_MAX__ ? __LONG_LONG_MAX__ : d[to] - potential[to];

    for (int i = 0; i < g.size(); i++) {
        potential[i] -= d[i] == __LONG_LONG_MAX__ ? 0 : d[i];
    }

    return ans;
}

int main() {

    int flow = 0;
    long long shortest_path = 0;
    int n, m;
    int a, b, k;
    vector< vector<edge*> > graph;

    try {
        ifstream in("input.txt");

        in >> n >> m;

        graph = vector< vector<edge*> >(n * 2 + 1);
        for (int i = 0; i < n; i++) {
            add_inner_edge(graph, i);
        }
        for (int i = 0; i < m; i++) {
            int from, to, cost;
            in >> from >> to >> cost;
            add_edge(graph, from - 1, to - 1, cost);
        }

        in >> a >> b >> k;
        a--; b--;

        in.close();

        while (flow <= k) {
            long long tmp = find_shortest_paht(graph, a * 2 + 1, b * 2);
            if (tmp == __LONG_LONG_MAX__) {
                break;
            }
            shortest_path += tmp;
            flow++;
            if (chrono::duration_cast<std::chrono::duration<double>>(stop - start) >= time_limit) {
                throw 20; // magic number
            }
        }
    } catch (...) {

        ifstream in("input.txt");

        in >> n >> m;

        vector< vector<edge*> > graph(n * 2 + 1);
        for (int i = 0; i < n; i++) {
            add_inner_edge(graph, i);
        }
        for (int i = 0; i < m; i++) {
            int from, to, cost;
            in >> from >> to >> cost;
            add_edge(graph, from - 1, to - 1, cost);
        }

        in >> a >> b >> k;
        a--; b--;

        in.close();

        flow = 0;
        shortest_path = 0;
        while (flow <= k) {
            long long tmp = find_shortest_paht_for_dummies(graph, a * 2 + 1, b * 2);
            if (tmp == __LONG_LONG_MAX__) {
                break;
            }
            shortest_path += tmp;
            flow++;
        }
    }

    ofstream out("output.txt");

    if (flow <= k) {
        out << "No\n" << flow;
    } else {
        out << "Yes\n" << shortest_path;
    }

    out.close();

    for (vector<edge*> edges : graph) {
        for (edge* e : edges) {
            delete e;
        }
    }

    return 0;
}