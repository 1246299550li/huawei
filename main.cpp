#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

const int maxSize = 580005;

int G[maxSize][100];
int Ginv[maxSize][100];

unordered_map<int, int> idHash; // card No -> id
int ids[maxSize]; //id -> card No
int inputs[maxSize];
int inputNum;
int nodeNum;

int inDegrees[maxSize] = {0};
int outDegrees[maxSize] = {0};

bool vis[maxSize] = {false};
int isValid[maxSize];
int isTail[maxSize];


struct Cycle {
    int cycle[7];

    bool operator<(const Cycle &rhs) const {
        for (int i = 0; i < 7; ++i) {
            if (cycle[i] != rhs.cycle[i])
                return cycle[i] < rhs.cycle[i];
        }
    }
};

Cycle cycle3[500000];
Cycle cycle4[500000];
Cycle cycle5[1000000];
Cycle cycle6[2000000];
Cycle cycle7[3000000];
Cycle *cycle[] = {cycle3, cycle4, cycle5, cycle6, cycle7};
int cycleNum[10] = {0};
int out = 0;

vector<unordered_map<int, vector<int>>> pre(maxSize);

int bucketSort(int *begin, const int *end) {
    int *p = begin;
    const int MAXID = 260000;
    bool bucket[MAXID];
    memset(bucket, 0, sizeof(bucket));
    for (; p < end; ++p) {
        bucket[*p] = true;
    }
    p = begin;
    for (int i = 0; i < MAXID; i++) {
        if (bucket[i]) {
            *p = i;
            ++p;
        }
    }
    return p - begin;
}

void init(string &testFile) {
    FILE *file = fopen(testFile.c_str(), "r");
    int u, v, value;
    while (fscanf(file, "%u,%u,%u", &u, &v, &value) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }

    // card No -> id
    copy(begin(inputs), end(inputs), begin(ids));
    nodeNum = bucketSort(ids, ids + inputNum);
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

void dfs(int head, int cur, int len, int path[]) {
    vis[cur] = true;
    path[len - 1] = cur;
    for (int i = 0; i < outDegrees[cur]; ++i) {
        int v = G[cur][i];
        if (v < head || vis[v] || isValid[v] != head) continue;
        if (isTail[v] == (-100 - head)) {
            for (auto &x :pre[head][v]) {
                if (!vis[x]) {
                    path[len] = v;
                    path[len + 1] = x;
                    for (int j = 0; j < len + 2; ++j) {
                        cycle[len - 1][cycleNum[len + 2]++].cycle[j] = ids[path[j]];
                    }
                    out++;
                }
            }
        }
        if (len < 5) {
            dfs(head, v, len + 1, path);
        }
    }
    vis[cur] = false;
}

void run() {
    int path[16];
    for (int i = 0; i < nodeNum; ++i) {
        if (outDegrees[i] > 0 && inDegrees[i] > 0) {
            vis[i] = true;
            dfs_range1(i, i, 1);
            dfs_range2(i, i, 1);
            vis[i] = false;
            dfs(i, i, 1, path);
        }
    }

    sort(cycle3, cycle3 + cycleNum[3]);
    sort(cycle4, cycle4 + cycleNum[4]);
    sort(cycle5, cycle5 + cycleNum[5]);
    sort(cycle6, cycle6 + cycleNum[6]);
    sort(cycle7, cycle7 + cycleNum[7]);

}

void output(string &outputFile) {
    int ansNum = 0;
    for (int k = 3; k <= 7; ++k) {
        ansNum += cycleNum[k];
    }

    FILE *fp = fopen(outputFile.c_str(), "w");
    string tmp = to_string(ansNum) + "\n";
    const char *t = tmp.c_str();
    fwrite(t, strlen(t), 1, fp);
//    for (int i = 3; i <= 7; ++i) {
//        for (int j = 0; j < cycleNum[i]; ++j) {
//            string str = to_string(cycle[i - 3][j].cycle[0]);
//            for (int k = 0; k < i; ++k) {
//                str += "," + to_string(cycle[i - 3][j].cycle[k]);
//            }
//            str += "\n";
//            const char *p = str.c_str();
//            fwrite(p, strlen(p), 1, fp);
//        }
//    }
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







