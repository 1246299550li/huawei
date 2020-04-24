#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;


struct Path {
    int len{};
    unsigned int path[15]{};

    explicit Path() = default;

    Path(int length, const unsigned int *p) {
        len = length;
        for (int i = 0; i < len; i++)
            path[i] = p[i];
    }

    bool operator<(const Path &rhs) const {
        if (len == rhs.len) {
            for (int i = 0; i < len; i++)
                if (path[i] != rhs.path[i])
                    return path[i] < rhs.path[i];
        }
        return len < rhs.len;
    }

};

const int maxSize = 580005;

int G[maxSize][100];
int Ginv[maxSize][100];

unordered_map<unsigned int, int> idHash; // card No -> id
unsigned int ids[maxSize]; //id -> card No
unsigned int inputs[maxSize];
int inputNum;
int nodeNum;

int inDegrees[maxSize] = {0};
int outDegrees[maxSize] = {0};

bool vis[maxSize] = {false};
int isValid[maxSize];
int isTail[maxSize];

unsigned int cycle3[3 * 500000];
unsigned int cycle4[4 * 500000];
unsigned int cycle5[5 * 1000000];
unsigned int cycle6[6 * 2000000];
unsigned int cycle7[7 * 3000000];
unsigned int *cycle[] = {cycle3, cycle4, cycle5, cycle6, cycle7};
int cycleNum[10] = {0};
int out = 0;
Path pathArr[3000000];
int pathIdx = 0;
vector<unordered_map<int, vector<int>>> pre(maxSize);

void init(string &testFile) {
    FILE *file = fopen(testFile.c_str(), "r");
    unsigned int u, v, value;
    while (fscanf(file, "%u,%u,%u", &u, &v, &value) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }

    // card No -> id
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
    memset(isValid, -1, sizeof(isValid));
    memset(isTail, 0, sizeof(isTail));

}

void dfs_range1(int head, int cur, int len) {

    for (int i = 0; i < outDegrees[cur]; ++i) {
        int v = G[cur][i];
        if (v < head || vis[v])continue;
        isValid[v] = head;
        if (len >= 3)continue;
        vis[v] = true;
        dfs_range1(head, v, len + 1);
        vis[v] = false;
    }
}

void dfs_range2(int head, int cur, int len) {
    for (int i = 0; i < inDegrees[cur]; ++i) {
        int v = Ginv[cur][i];
        if (v < head || vis[v])continue;
        isValid[v] = head;
        if (len == 2) {
            isTail[v] = -100 - head;
            if (pre[head].find(v) == pre[head].end()) {
                vector<int> a;
                a.push_back(cur);
                pre[head][v] = a;
            } else {
                pre[head][v].push_back(cur);
            }
        }
        if (len >= 3)continue;
        vis[v] = true;
        dfs_range2(head, v, len + 1);
        vis[v] = false;
    }
}

void dfs(int head, int cur, int len, unsigned int path[]) {
    vis[cur] = true;
    path[len - 1] = cur;
    for (int i = 0; i < outDegrees[cur]; ++i) {
        int v = G[cur][i];
        if (v < head || vis[v]) continue;
        if (isValid[v] != head) continue;
        if (isTail[v] == (-100 - head)) {
            for (auto &x :pre[head][v]) {
                if (!vis[x]) {
                    path[len] = v;
                    path[len + 1] = x;
                    unsigned int tmp[16];
                    for (int j = 0; j < len + 2; j++)
                        tmp[j] = ids[path[j]];
                    pathArr[pathIdx++] = (Path(len + 2, tmp));
                    out++;
                }
            }
//            path[len] = v;
//            for (int j = 0; j <= len; ++j) {
//                cycle[len - 2][cycleNum[len + 1] * (len + 1) + j] = ids[path[j]];
//                ++cycleNum[len + 1];
//            }

        }
        if (len < 5) {
            dfs(head, v, len + 1, path);
        }
    }
    vis[cur] = false;
}

void run() {
    unsigned int path[16];
    for (int i = 0; i < nodeNum; ++i) {
        if (outDegrees[i] > 0 && inDegrees[i] > 0) {
            vis[i] = true;
            dfs_range1(i, i, 1);
            dfs_range2(i, i, 1);
            vis[i] = false;
            dfs(i, i, 1, path);
        }
    }

    sort(pathArr, pathArr + pathIdx);

}

//void output(string &outputFile) {
//    int ansNum = 0;
//    for (int k = 3; k <= 7; ++k) {
//        ansNum += cycleNum[k];
//    }
//
//    FILE *fp = fopen(outputFile.c_str(), "w");
//    string tmp = to_string(ansNum) + "\n";
//    const char *t = tmp.c_str();
//    fwrite(t, strlen(t), 1, fp);
//    for (int i = 3; i <= 7; ++i) {
//        for (int j = 0; j < cycleNum[i]; j++) {
//            string str = to_string(cycle[i - 3][i * j]);
//            for (int k = 0; k < i; ++k) {
//                str += "," + to_string(cycle[i - 3][i * j + k]);
//            }
//            str += "\n";
//            const char *p = str.c_str();
//            fwrite(p, strlen(p), 1, fp);
//        }
//    }
//    fclose(fp);
//}
void output(string &outputFile) {

    FILE *fp = fopen(outputFile.c_str(), "w");
    string tmp = to_string(pathIdx) + "\n";
    const char *t = tmp.c_str();
    fwrite(t, strlen(t), 1, fp);
    for (int j = 0; j < pathIdx; j++) {
        auto x = pathArr[j];
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

    string testFile = "../data/test_data.txt";
    string outputFile = "../projects/student/result.txt";
    auto t = clock();
    init(testFile);
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    t = clock();
    run();
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    t = clock();
    output(outputFile);
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    cout << out << endl;
    return 0;
}







