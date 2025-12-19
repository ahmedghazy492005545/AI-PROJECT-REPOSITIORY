#include <bits/stdc++.h>
using namespace std;

/* ===================== Common Structures ===================== */
struct Item {
    int weight;
    int value;
};

struct algo {
    string name;
    string bigO;
    long long timecomp;
    int bestWeight;
    int bestValue;
};

vector<Item> items;
int n, W;

/* ===================== BFS ===================== */
algo runBFS() {
    vector<int> wL = {0}, vL = {0};
    long long steps = 1;
    int bW = 0, bV = 0;

    for (int i = 0; i < n; i++) {
        vector<int> nW, nV;
        for (int j = 0; j < wL.size(); j++) {
            if (wL[j] + items[i].weight <= W) {
                nW.push_back(wL[j] + items[i].weight);
                nV.push_back(vL[j] + items[i].value);
                steps++;
            }
            nW.push_back(wL[j]);
            nV.push_back(vL[j]);
            steps++;
        }
        wL = nW; vL = nV;
    }

    for (int i = 0; i < vL.size(); i++)
        if (vL[i] > bV) { bV = vL[i]; bW = wL[i]; }

    return {"BFS", "O(2^n)", steps, bW, bV};
}

/* ===================== DFS ===================== */
void dfsLogic(int idx, int cW, int cV, int &bW, int &bV, long long &steps) {
    steps++;
    if (cW > W) return;
    if (idx == n) {
        if (cV > bV) { bV = cV; bW = cW; }
        return;
    }
    dfsLogic(idx + 1, cW + items[idx].weight, cV + items[idx].value, bW, bV, steps);
    dfsLogic(idx + 1, cW, cV, bW, bV, steps);
}

algo runDFS() {
    int bW = 0, bV = 0;
    long long steps = 0;
    dfsLogic(0, 0, 0, bW, bV, steps);
    return {"DFS", "O(2^n)", steps, bW, bV};
}

/* ===================== UCS ===================== */
struct UCSNode {
    int index, weight, value;
    bool operator<(const UCSNode &o) const { return value < o.value; }
};

algo runUCS() {
    priority_queue<UCSNode> pq;
    pq.push({0, 0, 0});
    long long steps = 1;
    int bW = 0, bV = 0;

    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop();
        if (cur.value > bV) { bV = cur.value; bW = cur.weight; }
        if (cur.index == n) continue;

        pq.push({cur.index + 1, cur.weight, cur.value}); steps++;
        if (cur.weight + items[cur.index].weight <= W) {
            pq.push({cur.index + 1,
                     cur.weight + items[cur.index].weight,
                     cur.value + items[cur.index].value});
            steps++;
        }
    }
    return {"UCS", "O(b^d)", steps, bW, bV};
}

/* ===================== IDS ===================== */
void dls(int idx, int depth, int limit, int cW, int cV,
         int &bW, int &bV, long long &steps) {
    steps++;
    if (cW > W) return;
    if (depth == limit || idx == n) {
        if (cV > bV) { bV = cV; bW = cW; }
        return;
    }
    dls(idx + 1, depth + 1, limit,
        cW + items[idx].weight, cV + items[idx].value, bW, bV, steps);
    dls(idx + 1, depth + 1, limit,
        cW, cV, bW, bV, steps);
}

algo runIDS() {
    int bW = 0, bV = 0;
    long long steps = 0;
    for (int limit = 1; limit <= n; limit++)
        dls(0, 0, limit, 0, 0, bW, bV, steps);
    return {"IDS", "O(b^d)", steps, bW, bV};
}

/* ===================== Greedy ===================== */
algo runGreedy() {
    vector<Item> temp = items;
    sort(temp.begin(), temp.end(), [](Item a, Item b) {
        return (double)a.value / a.weight > (double)b.value / b.weight;
    });

    long long steps = (long long)(n * log2(n));
    int cW = 0, cV = 0, cap = W;

    for (auto &it : temp) {
        steps++;
        if (it.weight <= cap) {
            cap -= it.weight;
            cW += it.weight;
            cV += it.value;
        }
    }
    return {"Greedy", "O(n log n)", steps, cW, cV};
}

/* ===================== Hill Climbing ===================== */
int fitness(const vector<int>& s, int &w) {
    w = 0; int v = 0;
    for (int i = 0; i < n; i++)
        if (s[i]) { w += items[i].weight; v += items[i].value; }
    return (w > W) ? 0 : v;
}

algo runHillClimbing() {
    srand(time(0));
    vector<int> cur(n);
    for (int i = 0; i < n; i++) cur[i] = rand() % 2;

    int cW, cV = fitness(cur, cW);
    long long steps = 0;
    bool improved = true;

    while (improved) {
        improved = false;
        for (int i = 0; i < n; i++) {
            steps++;
            vector<int> nb = cur;
            nb[i] = 1 - nb[i];
            int nW, nV = fitness(nb, nW);
            if (nV > cV) { cur = nb; cV = nV; cW = nW; improved = true; }
        }
    }
    return {"Hill Climbing", "O(N*Iterations)", steps, cW, cV};
}

/* ===================== Genetic ===================== */
algo runGenetic() {
    const int POP = 50, GEN = 100;
    const double MUT = 0.05;
    srand(time(0));

    vector<vector<int>> pop(POP, vector<int>(n));
    for (auto &c : pop) for (int &x : c) x = rand() % 2;

    long long steps = 0;
    int bW = 0, bV = 0;

    for (int g = 0; g < GEN; g++) {
        vector<vector<int>> newPop;
        for (int i = 0; i < POP / 2; i++) {
            int a = rand() % POP, b = rand() % POP, w1, w2;
            auto p1 = (fitness(pop[a], w1) > fitness(pop[b], w2)) ? pop[a] : pop[b];
            int c = rand() % POP, d = rand() % POP;
            auto p2 = (fitness(pop[c], w1) > fitness(pop[d], w2)) ? pop[c] : pop[d];
            int pt = rand() % n;
            for (int j = pt; j < n; j++) swap(p1[j], p2[j]);
            for (int j = 0; j < n; j++)
                if ((double)rand() / RAND_MAX < MUT) p1[j] ^= 1;
            newPop.push_back(p1);
            newPop.push_back(p2);
        }
        for (auto &c : newPop) {
            int w, v = fitness(c, w);
            steps++;
            if (v > bV) { bV = v; bW = w; }
        }
        pop = newPop;
    }
    return {"Genetic", "O(Gen*Pop*N)", steps, bW, bV};
}

/* ===================== A* ===================== */
struct AStarNode {
    int level, weight, value, f;
    bool operator>(const AStarNode& o) const { return f > o.f; }
};

int heuristicAStar(int level, int cw) {
    int rv = 0, tw = cw;
    for (int i = level; i < n; i++) {
        if (tw + items[i].weight <= W) {
            tw += items[i].weight;
            rv += items[i].value;
        }
    }
    return -rv;
}

algo runAStar() {
    sort(items.begin(), items.end(), [](Item a, Item b) {
        return (double)a.value / a.weight > (double)b.value / b.weight;
    });

    priority_queue<AStarNode, vector<AStarNode>, greater<AStarNode>> pq;
    long long steps = 0;
    int bW = 0, bV = 0;

    pq.push({0, 0, 0, heuristicAStar(0, 0)});

    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop(); steps++;

        if (cur.level == n) {
            if (cur.value > bV) { bV = cur.value; bW = cur.weight; }
            continue;
        }

        if (cur.weight + items[cur.level].weight <= W) {
            int nw = cur.weight + items[cur.level].weight;
            int nv = cur.value + items[cur.level].value;
            int f = -nv + heuristicAStar(cur.level + 1, nw);
            pq.push({cur.level + 1, nw, nv, f});
        }

        int f2 = -cur.value + heuristicAStar(cur.level + 1, cur.weight);
        pq.push({cur.level + 1, cur.weight, cur.value, f2});
    }

    return {"A*", "O(b^d)", steps, bW, bV};
}

/* ===================== Print ===================== */
void printResult(const algo &r) {
    cout << "\n--- " << r.name << " ---\n";
    cout << "Complexity : " << r.bigO << endl;
    cout << "Steps      : " << r.timecomp << endl;
    cout << "Best Value : " << r.bestValue
         << " | Weight: " << r.bestWeight << "/" << W << endl;
}

/* ===================== MAIN ===================== */
int main() {
    cout << "Enter number of items: ";
    cin >> n;
    cout << "Enter capacity (W): ";
    cin >> W;

    items.resize(n);
    for (int i = 0; i < n; i++) {
        cout << "Item " << i + 1 << " (Weight Value): ";
        cin >> items[i].weight >> items[i].value;
    }

    cout << "\nChoose Algorithm:\n";
    cout << "1-BFS  2-DFS  3-UCS  4-IDS\n";
    cout << "5-Greedy  6-Hill  7-Genetic  8-A*\n";
    cout << "9-Run ALL\nChoice: ";

    int ch; cin >> ch;

    if (ch == 1) printResult(runBFS());
    else if (ch == 2) printResult(runDFS());
    else if (ch == 3) printResult(runUCS());
    else if (ch == 4) printResult(runIDS());
    else if (ch == 5) printResult(runGreedy());
    else if (ch == 6) printResult(runHillClimbing());
    else if (ch == 7) printResult(runGenetic());
    else if (ch == 8) printResult(runAStar());
    else if (ch == 9) {
        printResult(runBFS());
        printResult(runDFS());
        printResult(runUCS());
        printResult(runIDS());
        printResult(runGreedy());
        printResult(runHillClimbing());
        printResult(runGenetic());
        printResult(runAStar());
    }
    else cout << "Invalid Choice\n";

    return 0;
}
