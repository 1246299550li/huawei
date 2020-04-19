#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <fstream>

using namespace std;

struct Path {
    int len;
    unsigned int path[10];

    Path(int len, unsigned int *p) : len(len) {
        for (int i = 0; i < 10; i++)
            path[i] = p[i];
    }

    bool operator<(const Path &rhs) const {
        if (len != rhs.len) return len < rhs.len;
        for (int i = 0; i < len; i++) {
            if (path[i] != rhs.path[i])
                return path[i] < rhs.path[i];
        }
    }

};
const int maxSize = 280005;
int G[maxSize][50];
unordered_map<unsigned int, int> idHash;
unsigned int ids[maxSize];
unsigned int inputs[maxSize];
int inDegrees[maxSize] = {0};
bool vis[maxSize] = {false};
vector<Path> ans;
int nodeCnt;
int outDegrees[maxSize] = {0};
int inputNum;


void init(string &testFile) {
    FILE *file = fopen(testFile.c_str(), "r");

    unsigned int u, v, c;
    while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }
    copy(begin(inputs), end(inputs), begin(ids));
    sort(ids, ids + inputNum);
    nodeCnt = unique(ids, ids + inputNum) - ids;

    for (int i = 0; i < nodeCnt; i++) {

        idHash[ids[i]] = i;
    }
    for (int i = 0; i < inputNum; i += 2) {
        int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
        G[u][outDegrees[u]++] = v;
        ++inDegrees[v];
    }

}

void dfs(int head, int cur, int depth, int path[]) {
    vis[cur] = true;
    path[depth - 1] = cur;
    for (int i = 0; i < outDegrees[cur]; i++) {
        int v = G[cur][i];
        if (v == head && depth >= 3 && depth <= 7) {
            unsigned int tmp[10];
            for (int j = 0; j < depth; j++)
                tmp[j] = ids[path[j]];

            ans.emplace_back(Path(depth, tmp));

        }
        if (depth < 7 && !vis[v] && v > head) {
            dfs(head, v, depth + 1, path);
        }
    }
    vis[cur] = false;
}

void solve() {
    int path[10];
    for (int i = 0; i < nodeCnt; i++) {
        if (outDegrees[i] > 0) {
            dfs(i, i, 1, path);
        }
    }
    sort(ans.begin(), ans.end());
}

void output(string &outputFile) {
    ofstream out(outputFile);
    out << ans.size() << endl;
    for (auto &x:ans) {
        auto path = x.path;
        out << path[0];
        for (int i = 1; i < x.len; i++)
            out << "," << path[i];
        out << endl;
    }
}


int main() {
    string testFile = "test_data.txt";
    string outputFile = "output.txt";

    auto t = clock();
    init(testFile);
    solve();
    output(outputFile);
    cout << " time : " << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    return 0;
}







