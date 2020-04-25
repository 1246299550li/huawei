#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <string>
#include <cstring>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/mman.h>
//#include <fcntl.h>
//#include <unistd.h>
using namespace std;

const int maxSize = 280005;

int G[maxSize][600];
int Ginv[maxSize][600];

unordered_map<unsigned int, int> idHash; // card No -> id
unsigned int ids[560000]; //id -> card No
unsigned int inputs[560000];
int inputNum;
int nodeNum;

int inDegrees[maxSize] = {0};
int outDegrees[maxSize] = {0};

bool vis[maxSize] = {false};
int isValid[maxSize];
int isTail[maxSize] = {0};


struct Cycle {
    unsigned int cycle[7];
};

Cycle cycle3[500000];
Cycle cycle4[500000];
Cycle cycle5[1000000];
Cycle cycle6[2000000];
Cycle cycle7[3000000];
Cycle *cycle[] = {cycle3, cycle4, cycle5, cycle6, cycle7};
int cycleNum[10] = {0};
//int out = 0;

vector<unordered_map<int, vector<int>>> pre(maxSize);
void next(const char* buf, int& len, unsigned int& u, unsigned int& v){
    const char* tmp = buf + len;
    u = 0;
    v = 0;
    while(*tmp && (*tmp != ',' && *tmp != 0x0d && *tmp != 0x0a && *tmp != '\n')){
        u = u * 10 + (*tmp - '0');
        ++tmp;
    }
    if (*tmp == ',') {
        ++tmp;
        while (*tmp != ',') {
            v = v * 10 + (*tmp - '0');
            ++tmp;
        }
        ++tmp;
        while (*tmp != 0x0d && *tmp != 0x0a && *tmp != '\n') {
            ++tmp;
        }
        len = tmp - buf + 1;
    } else{
        len = -1;
    }
}

int bucketSort(unsigned int *begin, const unsigned int *end) {
    unsigned int *p = begin;
    const int MAXID = maxSize;
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
    unsigned int u, v, value;
    while (fscanf(file, "%u,%u,%u", &u, &v, &value) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }

//    unsigned int u, v;
//    int fd = open(testFile.c_str(),O_RDONLY);
//    int len = lseek(fd,0,SEEK_END);
//    char *buf = (char *) mmap(NULL,len,PROT_READ,MAP_PRIVATE,fd,0);
//    int offset = 0;
//    while (true){
//        next(buf, offset, u, v);
//        if (offset != -1){
//            inputs[inputNum++] = u;
//            inputs[inputNum++] = v;
//        } else{
//            break;
//        }
//    }

    // card No -> id
    copy(begin(inputs), end(inputs), begin(ids));
    sort(ids, ids + inputNum);
    nodeNum = unique(ids, ids + inputNum) - ids;
//    nodeNum = bucketSort(ids, ids + inputNum);
    for (int i = 0; i < nodeNum; i++) {
        idHash[ids[i]] = i;
    }
    for (int i = 0; i < inputNum; i += 2) {
        int s = idHash[inputs[i]], t = idHash[inputs[i + 1]];
        G[s][outDegrees[s]++] = t;
        Ginv[t][inDegrees[t]++] = s;
    }
    for (int j = 0; j < nodeNum; ++j) {
        sort(G[j], G[j] + outDegrees[j]);
        sort(Ginv[j], Ginv[j] + inDegrees[j]);
    }
    memset(isValid, -1, sizeof(isValid));
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
        if (isValid[v] != head || vis[v])continue;
        if (isTail[v] == (-100 - head)) {
            for (auto &x :pre[head][v]) {
                if (!vis[x]) {
                    path[len] = v;
                    path[len + 1] = x;
                    for (int j = 0; j < len + 2; ++j) {
                        cycle[len - 1][cycleNum[len + 2]].cycle[j] = ids[path[j]];
                    }
                    ++cycleNum[len + 2];
//                    ++out;
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
    string ans;
    for (int i = 3; i <= 7; ++i) {
        for (int j = 0; j < cycleNum[i]; ++j) {
            string str = to_string(cycle[i - 3][j].cycle[0]);
            for (int k = 1; k < i; ++k) {
                str += "," + to_string(cycle[i - 3][j].cycle[k]);
            }
            str += "\n";
            ans += str;
        }
    }
    const char *p = ans.c_str();
    fwrite(p, strlen(p), 1, fp);
    fclose(fp);
}

int main() {
    string testFile = "../data/test_data.txt";

    string outputFile = "../projects/student/result.txt";
    auto t = clock();
    init(testFile);
//    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
//    t = clock();
    run();
//    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
//    t = clock();
    output(outputFile);
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
//    cout << out << endl;
    return 0;
}

