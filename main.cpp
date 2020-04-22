#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <cstring>

using namespace std;

struct Path {
    int len{};
    unsigned int path[10]{};
    
    Path() = default;
    Path(int length, const unsigned int *p) {
        len = length;
        for (int i = 0; i < 10; i++)
            path[i] = p[i];
    }

    bool operator<(const Path &rhs) const {
        if (len == rhs.len)
        {
            for (int i = 0; i < len; i++)
                if (path[i] != rhs.path[i])
                    return path[i] < rhs.path[i];
        }
        return len < rhs.len;
    }

};
const int threadNum = 6;
const int maxSize = 580000;
const int pathNum = 1000000;

int G[maxSize][260];
int Ginv[maxSize][260];

unordered_map<unsigned int, int> idHash; // card No -> id
unsigned int ids[maxSize]; //id -> card No
unsigned int inputs[maxSize];
int inputNum;

int inDegrees[maxSize] = {0};
int outDegrees[maxSize] = {0};

Path pathArr[threadNum][3000000];
Path paths[4000000];
int nodeNum;

void init(string &testFile) {
    auto t = clock();

    FILE *file = fopen(testFile.c_str(), "r+");
    unsigned int u, v, value;
    while (fscanf(file, "%u,%u,%u", &u, &v, &value) != EOF) {
        inputs[inputNum++] = u;
        inputs[inputNum++] = v;
    }
    
    // card No -> id
    copy(begin(inputs), end(inputs), begin(ids));
    sort(ids, ids + inputNum);
    nodeNum = unique(ids, ids + inputNum) - ids;
    cout<<nodeNum;
    for (int i = 0; i < nodeNum; i++) {
        idHash[ids[i]] = i;
    }
    for (int i = 0; i < inputNum; i += 2) {
        int s = idHash[inputs[i]], t = idHash[inputs[i + 1]];
        G[s][outDegrees[s]++] = t;
        Ginv[t][inDegrees[t]++] = s;
    }
    cout << "init cost:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    
}

class SolutionThread
{
    Path *pathArr;
    string flag;
    bool vis[maxSize] = {false};
    int isValid[maxSize]={0};
    
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
            if (len >= 3)continue;
            vis[v] = true;
            dfs_range2(head, v, len + 1);
            vis[v] = false;
        }
    }
    void dfs(int head, int cur, int len, int path[]) {
        vis[cur] = true;
        path[len - 1] = cur;
        for (int i = 0; i < outDegrees[cur]; i++) {
            int v = G[cur][i];
            if (v < head || (isValid[v] != head && isValid[v] != -2)) continue;
            if (isValid[v] == -2 && !vis[v] && len >= 2) {
                path[len] = v;
                unsigned int tmp[10];
                for (int j = 0; j <= len; j++)
                    tmp[j] = ids[path[j]];
                pathArr[pathIdx++] = (Path(len + 1, tmp));
            }
            if (len < 6 && !vis[v]) {
                dfs(head, v, len + 1, path);
            }
        }
        vis[cur] = false;
    }
    void findLoop(int begin,int end)
    {
        int path[10];
        for (int i = begin; i < end; ++i) {
            if (outDegrees[i] > 0 && inDegrees[i] > 0) {
                //find 3 neighbor
                dfs_range1(i, i, 1);
                dfs_range2(i, i, 1);
                
                /*6+1 opt*/
                for (int j = 0; j < inDegrees[i]; ++j) {
                    if (Ginv[i][j] > i)
                        isValid[Ginv[i][j]] = -2; // mark the last 2nd p
                }
                
                dfs(i, i, 1, path);
                
                /*6+1 opt*/
                for (int j = 0; j < inDegrees[i]; ++j) {
                    if (Ginv[i][j] > i)
                        isValid[Ginv[i][j]] = -1; // recover the mark -1
                }
            }
        }
        
    }
public:
    int pathIdx = 0;
    SolutionThread()
    {
        pathArr = nullptr;
        flag = "nil";
    }
    void init(Path* pathArr,string flag)
    {
        this->pathArr = pathArr;
        this->flag = flag;
        memset(isValid, -1, sizeof(isValid));
    }
    SolutionThread(Path* pathArr,string flag)
    {
        this->pathArr = pathArr;
        this->flag = flag;
        memset(isValid, -1, sizeof(isValid));
    }
    void run(int begin,int end) {
        auto t = clock();
        findLoop(begin,end);

        cout << "time in thread " <<flag<<":"<< double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    }
};

void output(string &outputFile,int pathIdx,Path *pathArr) {

    FILE *fp = fopen(outputFile.c_str(), "w");
    string tmp = to_string(pathIdx) + "\n";
    const char *t = tmp.c_str();
    fwrite(t, strlen(t), 1, fp);
    for (int i=0;i<pathIdx;i++) {
        auto path= pathArr[i].path;
        string str = to_string(path[0]);
        for (int i = 1; i < pathArr[i].len; i++)
            str += "," + to_string(path[i]);
        str += "\n";
        const char *p = str.c_str();
        fwrite(p, strlen(p), 1, fp);
    }
    fclose(fp);
}
float fibRatio(int a)
{
    //assert(a<threadNum);
    float fib[9] = {0,1,2,3,5,8,13,21,34};
    float sum[9] = {0,1,3,6,11,19,32,53,87};
    return fib[a]/sum[threadNum];
}


int main() {

    string testFile = "../2020HuaweiCodecraft-TestData/3512444/test_data.txt";
    string outputFile = "../result.txt";
    auto t = clock();
    init(testFile);
    vector<SolutionThread> s(threadNum);
    for(int i =0;i<threadNum;i++)
        s[i].init(pathArr[i],to_string(i));
    
    auto t1 = clock();
    vector<thread> threads(threadNum);
    /*
    int p = int(fibRatio(0)*nodeNum+0.5);
    int q = int(fibRatio(1)*nodeNum+0.5);
    for(int i =0;i<threadNum;i++)
    {
        threads[i] = thread(&SolutionThread::run,&s[i],p,q);
        cout<<"thread "<<i<<":"<<"["<<p<<","<<q<<")"<<endl;
        p = q;
        q = p +int(fibRatio(i+2)*nodeNum+0.5);
    }
    
   */
    
    int p = 0;
    int q = int(nodeNum /(exp(threadNum) - 1 )* (exp(1)-1) + 0.5);
    for(int i =0;i<threadNum;i++)
    {
        threads[i] = thread(&SolutionThread::run,&s[i],p,q);
        cout<<"thread "<<i<<":"<<"["<<p<<","<<q<<")"<<endl;
        p = q;
        q = int(nodeNum /(exp(threadNum) - 1 ) * (exp(i+2)-1)+0.5);
    }
    /*
   int p = 0;
   int q = nodeNum /threadNum;
   for(int i =0;i<threadNum;i++)
   {
       threads[i] = thread(&SolutionThread::run,&s[i],p,q);
       cout<<"thread "<<i<<":"<<"["<<p<<","<<q<<")"<<endl;
       p = q;
       q = (i+2)*nodeNum /threadNum;
   }
    */
    
    
    for(int i =0;i<threadNum;i++)
    {
        threads[i].join();
    }
    cout << "thread finished:" << double(clock() - t1) / CLOCKS_PER_SEC << "s" << endl;
    
    Path *pp = paths;
    int pathN=0;
    for(int i =0;i<threadNum;i++)
    {
        memcpy(pp,pathArr[i],sizeof(Path)*s[i].pathIdx);
        pathN += s[i].pathIdx;
        pp += s[i].pathIdx;
    }
    sort(paths,paths+pathN);
    output(outputFile,pathN,paths);
    
    
    /*
    int paths =s1.pathIdx;
    memcpy(pathArr[0]+paths,pathArr[1],sizeof(Path)*s2.pathIdx);
    paths+=s2.pathIdx;
    memcpy(pathArr[0]+paths,pathArr[2],sizeof(Path)*s3.pathIdx);
    paths+=s3.pathIdx;
    sort(pathArr[0],pathArr[0]+paths);
    output(outputFile,paths,pathArr[0]);
     */
     
    cout << "time:" << double(clock() - t) / CLOCKS_PER_SEC << "s" << endl;
    return 0;
}







