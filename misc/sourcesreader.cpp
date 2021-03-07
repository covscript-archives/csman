#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
// 一般格式：
//  标题 对象数
//  对象内容...
//
// 第一个版本为Unstable，第二为Stable，之后为历史版本
// 保证历史版本为降序排列，（若无Unstable或Stable，用0占位）
//
// 剩下具体的STD，ABI号内容为：对PAC中各个包的说明与索引
class sources_idx {
public:
    std::string time;

    struct pac_data {
        std::string ver, url, name, idx;

        pac_data(const std::string &n, const std::string &v, const std::string &u) : name(n), ver(v), url(u) {
            idx = name + ver;
        }

        bool operator<(const pac_data &rhs) const {
            return idx < rhs.idx;
        }
    }; // 依赖图部分，点的数据类

    struct rtm_data : pac_data {
        std::string ABI, STD;

        rtm_data(const std::string &n,
                 const std::string &v,
                 const std::string &A,
                 const std::string &S,
                 const std::string &u) : pac_data(n, v, u),
                                         ABI(A), STD(S) {}
    }; // 依赖图部分，点数据的类

    std::unordered_map<std::string, std::vector<std::string> > pac_info; // 包的描述信息 : name, author, description

    struct rtm_label {
        rtm_label(const std::string &A, const std::string &S, const int &id)
                : ABI(A), STD(S), node_id(id) {}

        std::string STD, ABI;
        int node_id;
    };

    std::vector<rtm_label> rtm_list; // 用于寻找符合要求的runtime，默认idx文件给出的是单调的

    int query_rtm(const std::string &AoS){ // 找合适的ABI，STD版本号的runtime，返回runtime在图中的id
        int l=0,r=rtm_list.size()-1,mid = (rtm_list.size()-1)>>1;
        if(AoS[0]=='A'){
            while(l!=r){
                mid = (l+r)>>1;
                if(rtm_list[mid].ABI <= AoS)
                    r = mid;
                else
                    l = mid+1;
            }
        }
        else if(AoS[0]=='S'){
            while(l!=r){
                mid = (l+r)>>1;
                if(rtm_list[mid].STD <= AoS)
                    r = mid;
                else
                    l = mid+1;
            }
        }
        return mid;
    }

    std::unordered_map<std::string,std::unordered_map<std::string,int> > node_id;

    std::unordered_map<std::string,std::pair<std::string,std::string> > un_stable_ver;

    class graph {
    public:
        struct edge {
            int out;
            bool inv;
            edge(int o,bool i):out(o),inv(i){}
        };

        std::vector<std::vector<edge>> head;
        std::vector<pac_data *> node_data;
        std::vector<bool> vis;
        int size=0;

        void init(int s) {
            head.reserve(s+5);
            node_data.reserve(s+5);
            vis.reserve(s+5);
        }

        void dfs(const int &u, std::set<int> &sc, bool sgn) {
            vis[u] = true;
            sc.insert(u);
            for (auto e : head[u])
                if (sgn == e.inv) {
                    int &v = e.out;
                    if (!vis[v])
                        dfs(v, sc, sgn);
                }
        }

        std::set<int> dfs_sc(const int &u, bool sgn) {
            vis.reserve(size);
            for (auto x:vis)
                x = 0;
            std::set<int> sc;
            dfs(u, sc, sgn);
            return sc;
        }

        void add_edge(const int &a, const int &b) {
            head[a].push_back(edge(b, 0));
            head[b].push_back(edge(a, 1));
        }

        std::set<pac_data> depend_data(const int &u) {
            std::set<pac_data> res;
            for (auto x : dfs_sc(u, 0))
                res.insert(*node_data[x]);
            return res;
        }

        std::set<pac_data> support_data(const int &u) {
            std::set<pac_data> res;
            for (auto x : dfs_sc(u, 1))
                res.insert(*node_data[x]);
            return res;
        }

    };

    graph G;

    void split(std::vector<std::string> &args, const std::string &s) {
        std::string buf;
        args.clear();
        for (auto c : s) {
            if (std::isspace(c)) {
                if (!buf.empty()) {
                    args.push_back(buf);
                    buf.clear();
                }
            } else
                buf += c;
        }
        if (!buf.empty())
            args.push_back(buf);
    }

    bool readline(std::ifstream &ifs, std::vector<std::string> &args) {
        std::string s;
        if (!std::getline(ifs, s))
            return false;
        split(args, s);
        return true;
    }

    bool readruntime(std::ifstream &ifs, std::vector<std::string> &args) {
        if (!readline(ifs, args))
            return false;
        std::string name = args[0];
        int cnt = std::stoi(args[1]);
        rtm_list.reserve(cnt);
        pac_info[name].push_back(name);
        std::string author, description;
        if (!getline(ifs, author))
            return false;
        pac_info[name].push_back(author);
        if (!getline(ifs, description))
            return false;
        pac_info[name].push_back(description);
        if (!readline(ifs, args))
            return false;
        for (int i = 0, j = 0; i < cnt; ++i,++j) {

            if(j==0)
                un_stable_ver[name].first = args[j];
            if(j==1)
                un_stable_ver[name].second = args[j];

            if (args[j] != "0") {
                node_id[name][args[j]] = ++G.size;
                G.node_data[G.size] = new rtm_data(name, args[j], args[j + 1], args[j + 2], args[j + 3]);
                rtm_list.push_back(rtm_label (args[j + 1], args[j + 2], G.size));
                j += 3;
            } else
                ++G.size;
        }

        return true;
    } // 图论读点

    bool readpackage(std::ifstream &ifs, std::vector<std::string> &args) {
        if (!readline(ifs, args))
            return false;
        std::string name = args[0];
        int cnt = std::stoi(args[1]);
        std::string author, description;
        if (!getline(ifs, author))
            return false;
        pac_info[name].push_back(author);
        if (!getline(ifs, description))
            return false;
        pac_info[name].push_back(description);

        if (!readline(ifs, args))
            return false;
        for (int i = 0, j = 0; i < cnt; i++) {

            if(j==0)
                un_stable_ver[name].first = args[j];
            if(j==1)
                un_stable_ver[name].second = args[j];

            if (args[j] != "0") {
                node_id[name][args[j]] = ++G.size;
                G.node_data[G.size] = new pac_data(name, args[j], args[j + 1]);
                ++j;
            } else
                ++G.size;
            ++j;
        }
        return true;
    } // 图论读点

    bool readdep(std::ifstream &ifs, std::vector<std::string> &args) { // 图论读边

        if (!readline(ifs, args))
            return false;
        std::string label = args[0];
        int cnt = std::stoi(args[1]);
        int rtm_id=-1;
        if(label!="Generic")
            rtm_id = rtm_list[query_rtm(label)].node_id;
        for (int i = 1; i <= cnt; i++) {
            readline(ifs, args);
            if(label!="Generic")
                G.add_edge(std::stoi(args[0]),rtm_id);
            for (int j = 1; j < args.size(); j++)
                G.add_edge(std::stoi(args[0]), std::stoi(args[j]));
        }

        return true;
    }

    sources_idx(const std::string &path) {

        std::ifstream ifs(path);
        if (!ifs)
            throw std::string("open sources_idx failed");
        std::string str;
        std::vector<std::string> args;

        std::getline(ifs, time); //2020.02.30.10.35
        readline(ifs, args); //PAC 14 28
        int cnt = std::stoi(args[1]);
        G.init(std::stoi(args[2]));
        readruntime(ifs, args);
        for (int i = 2; i <= cnt; i++)
            readpackage(ifs, args);
        //依赖
        while (readdep(ifs, args));
    }
};


int main() {
    try {
        sources_idx si("/mnt/d/mywork/newcsman/sources_idx");
        for (auto x : si.G.support_data(2))
            std::cerr << x.name << std::endl;

    } catch (std::exception e) {
        std::cerr << e.what();
    }

    return 0;
}