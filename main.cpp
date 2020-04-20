#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <string>

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
        for (int i = 0; i < len; i++)
            if (path[i] != rhs.path[i])
                return path[i] < rhs.path[i];
    }

};

const int maxSize = 280005;
int G[maxSize][50];
int Ginv[maxSize][255];
unordered_map<unsigned int, int> idHash;
unsigned int ids[maxSize];
unsigned int inputs[maxSize];
int inDegrees[maxSize] = {0};
int outDegrees[maxSize] = {0};
bool vis[maxSize] = {false};
bool isStart[maxSize] = {false};
vector<Path> pathArr;
int nodeNum;
int inputNum;


void init(string &testFile) {
    FILE *file = fopen(testFile.c_str(), "r");
    unsigned int u, v, value;
    while (fscanf(file, "%u,%u,%u", &u, &v, &value) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }
    copy(begin(inputs), end(inputs), begin(ids));
    sort(ids, ids + inputNum);
    nodeNum = unique(ids, ids + inputNum) - ids;
    for (int i = 0; i < nodeNum; i++) {
        idHash[ids[i]] = i;
    }
    for (int i = 0; i < inputNum; i += 2) {
        int s = idHash[inputs[i]], t = idHash[inputs[i + 1]];
        G[s][outDegrees[s]++] = t;
        Ginv[t][inDegrees[t]++] = s;
    }

}

void dfs(int head, int cur, int len, int path[]) {
    vis[cur] = true;
    path[len - 1] = cur;
    for (int i = 0; i < outDegrees[cur]; i++) {
        int v = G[cur][i];
        if (isStart[v] && !vis[v] && len >= 2 && len < 7) {
            path[len] = v;
            unsigned int tmp[10];
            for (int j = 0; j <= len; j++)
                tmp[j] = ids[path[j]];
            pathArr.emplace_back(Path(len + 1, tmp));
        }
        if (len < 6 && !vis[v] && v > head) {
            dfs(head, v, len + 1, path);
        }
    }
    vis[cur] = false;
}

void run() {
    int path[10];
    for (int i = 0; i < nodeNum; i++) {
        if (outDegrees[i] > 0) {
            for (int j = 0; j < inDegrees[i]; ++j) {
                if (Ginv[i][j] > i)
                    isStart[Ginv[i][j]] = true;
            }
            dfs(i, i, 1, path);
            for (int j = 0; j < inDegrees[i]; ++j) {
                if (Ginv[i][j] > i)
                    isStart[Ginv[i][j]] = false;
            }
        }
    }
    sort(pathArr.begin(), pathArr.end());
}

void output(string &outputFile) {

    FILE *fp = fopen(outputFile.c_str(), "w");
    string tmp = to_string(pathArr.size()) + "\n";
    const char *t = tmp.c_str();
    fwrite(t, strlen(t), 1, fp);
    for (auto &x:pathArr) {
        auto path = x.path;
        string str = to_string(path[0]);
        for (int i = 1; i < x.len; i++)
            str += "," + to_string(path[i]);
        str += "\n";
        const char *p = str.c_str();
        fwrite(p, strlen(p), 1, fp);
    }
    fclose(fp);
}


int main() {
    string testFile = "test_data.txt";
    string outputFile = "result.txt";
//    auto t = clock();
    init(testFile);
    run();
    output(outputFile);
//    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    return 0;
}







