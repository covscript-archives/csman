#include <../include/csman/global.hpp>

namespace csman::str {
    bool is_abi(const std::string &str) {
        static const std::regex reg("^ABI[0-9]{4}[0-9A-F]{2}$");
        return std::regex_match(str, reg);
    }
    bool is_std(const std::string &str) {
        static const std::regex reg("^STD[0-9]{4}[0-9A-F]{2}$");
        return std::regex_match(str, reg);
    }
    bool is_ver(const std::string &str) {
        static const std::regex reg("^([0-9]+\\.){0,3}[0-9]+\\w*$");
        return std::regex_match(str, reg);
    }
    bool is_pac(const std::string &str) {
        static const std::regex reg("^(\\w+\\.)+\\w+$");
        return std::regex_match(str, reg);
    }
    bool weak_equal(const std::string &a, const std::string &b) {
        if(a.size()!=b.size())
            return false;
        for(int i=0;i<a.size();i++)
            if( (a[i]!=b[i]) && (a[i]-'a'!=b[i]-'a') && (a[i]-'a'!=b[i]-'A') && (a[i]-'A'!=b[i]-'a') && (a[i]-'A'!=b[i]-'A') )
                return false;
        return true;
    }
    std::vector<std::string> split(const std::string &s){
        std::vector<std::string> res;
        std::string buf;
        for(auto &c : s){
            if(c==' ' || c==',' || c=='/' || c=='\\' || c=='\n' || c=='\r')
                res.push_back(buf), buf.clear();
            else
                buf.push_back(c);
        }
        return res;
    }
    std::vector<std::string> split(const std::string &s, const std::string &d){
        std::vector<std::string> res;
        std::string buf;
        bool _continue = false;
        for(auto &c : s){
            for(auto &sg : d)
                if(c == sg){
                    res.push_back(buf), buf.clear();
                    _continue = true;
                    break;
                }
            if(_continue)
                continue;
            buf.push_back(c);
        }
        return res;
    }
    bool readline(std::ifstream &ifs, std::vector<std::string> &args) {
        std::string s;
        if (!std::getline(ifs, s))
            return false;
        args = split(s);
        return true;
    }
    bool path_char(const char &c){
#ifdef _WIN32
        if(c=='\\' || c=='/' || c==':' or c=='*' || c=='?' || c=='\"' || c=='<' || c=='>' || c=='|')
            return false;
#endif
        return true;
    }
    bool is_path(const std::string &s){
        bool flag = false;
        for(auto &c : s){
            if(!path_char(c))
                return false;
            if(c==delimiter)
                flag = true;
        }
        return flag;
    }
    bool path_compare(std::string s1, std::string s2){
        if(!is_path(s1) || !is_path(s2))
            return false;
        if(*(s1.end()-1)==delimiter)
            s1.erase(s1.end()-1);
        if(*(s2.end()-1)==delimiter)
            s2.erase(s2.end()-1);
        if(s1.size()!=s2.size())
            return false;
        for(int i=0;i<s1.size();i++)
            if(s1[i]!=s2[i])
                return false;

        return true;
    }
    std::string to_dir_path(const std::string &path){
        std::string res;
        for(auto &c : path){
            if(c=='\\' || c=='/')
                res.push_back(delimiter);
            else
                res.push_back(c);
        }
        if(*(res.end()-1)!=delimiter)
            res.push_back(delimiter);
        return res;
    }
    std::string to_file_path(const std::string &path){
        std::string res;
        for(auto &c : path){
            if(c=='\\' || c=='/')
                res.push_back(delimiter);
            else
                res.push_back(c);
        }
        if(*(res.end()-1)==delimiter)
            *(res.end()-1) = '\0';
        return res;
    }
}