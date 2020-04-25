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

int G[maxSize][60];
int Ginv[maxSize][60];

int idHash[maxSize]; // card No -> id
unsigned int ids[maxSize * 2]; //id -> card No
unsigned int inputs[maxSize * 2];
int inputNum;
int nodeNum;

int inDegrees[maxSize] = {0};
int outDegrees[maxSize] = {0};

bool vis[maxSize] = {false};
int isValid[maxSize] = {0};
int isTail[maxSize] = {0};

char cycle3[30 * 500000];
char cycle4[30 * 500000];
char cycle5[40 * 1000000];
char cycle6[50 * 2000000];
char cycle7[50 * 3000000];
char *cycle[] = {cycle3, cycle4, cycle5, cycle6, cycle7};
int cycleNum[10] = {0};
int ansNum = 0;

vector<unordered_map<int, vector<int>>> pre;

void next(const char *buf, int &len, unsigned int &u, unsigned int &v) {
    const char *tmp = buf + len;
    u = 0;
    v = 0;
    while (*tmp && (*tmp != ',' && *tmp != 0x0d && *tmp != 0x0a && *tmp != '\n')) {
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
    } else {
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

void init(string &inputFile) {

    FILE *file = fopen(inputFile.c_str(), "r");
    unsigned int u, v, value;
    while (fscanf(file, "%u,%u,%u", &u, &v, &value) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }

//    unsigned int u, v;
//    int fd = open(inputFile.c_str(),O_RDONLY);
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

    copy(begin(inputs), end(inputs), begin(ids));
//    sort(ids, ids + inputNum);
//    nodeNum = unique(ids, ids + inputNum) - ids;
    nodeNum = bucketSort(ids, ids + inputNum);
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
    pre.resize(maxSize);
}

void dfs_range1(int head, int cur, int len) {
    vis[cur] = true;
    for (int i = 0; i < outDegrees[cur]; ++i) {
        int &v = G[cur][i];
        if (v < head || vis[v])continue;
        isValid[v] = head + 100;
        if (len >= 3)continue;
        dfs_range1(head, v, len + 1);
    }
    vis[cur] = false;
}

void dfs_range2(int head, int cur, int len) {
    vis[cur] = true;
    for (int i = 0; i < inDegrees[cur]; ++i) {
        int &v = Ginv[cur][i];
        if (v < head || vis[v])continue;
        isValid[v] = head + 100;
        if (len == 2) {
            isTail[v] = -100 - head;
            if (pre[head].find(v) == pre[head].end()) {
                vector<int> a(1, cur);
                pre[head][v] = a;
            } else {
                pre[head][v].push_back(cur);
            }
        }
        if (len >= 3)continue;
        dfs_range2(head, v, len + 1);
    }
    vis[cur] = false;
}

void dfs(int head, int cur, int len, int path[]) {
    vis[cur] = true;
    path[len - 1] = cur;
    for (int i = 0; i < outDegrees[cur]; ++i) {
        int &v = G[cur][i];
        if (isValid[v] != (head + 100) || vis[v])continue;
        if (isTail[v] == (-100 - head)) {
            for (auto &x :pre[head][v]) {
                if (!vis[x]) {
                    path[len] = v;
                    path[len + 1] = x;
                    for (int j = 0; j < len + 1; ++j) {
                        cycleNum[len + 2] += sprintf(cycle[len - 1] + cycleNum[len + 2], "%d", ids[path[j]]);
                        cycle[len - 1][cycleNum[len + 2]] = ',';
                        ++cycleNum[len + 2];
                    }
                    cycleNum[len + 2] += sprintf(cycle[len - 1] + cycleNum[len + 2], "%d", ids[path[len + 1]]);
                    cycle[len - 1][cycleNum[len + 2]] = '\n';
                    ++cycleNum[len + 2];
                    ++ansNum;
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
        if (outDegrees[i] < 1 || inDegrees[i] < 1)continue;
        dfs_range1(i, i, 1);
        dfs_range2(i, i, 1);
        dfs(i, i, 1, path);
    }
}

void output(string &outputFile) {
    FILE *fp = fopen(outputFile.c_str(), "w");
    const char *t = (to_string(ansNum) + "\n").c_str();
    fwrite(t, strlen(t), 1, fp);
    for (int i = 3; i <= 7; ++i) {
        fwrite(cycle[i - 3], cycleNum[i], 1, fp);
    }
    fclose(fp);
}

int main() {
    string inputFile = "../data/test_data.txt";
    string outputFile = "../projects/student/result.txt";
    auto t = clock();
    init(inputFile);
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    t = clock();
    run();
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    t = clock();
    output(outputFile);
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    cout << ansNum << endl;

    return 0;
}