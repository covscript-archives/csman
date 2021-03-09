#include<bits/stdc++.h>

struct {
/*
 * 每次向终端打印命令时，使用首句first()
 * 之后使用content()把此次消息打印全
 * 每行用\t补齐
 * 如:
 * csman:    hello
 *           hello1 hello2
 *           hello3...
 *           end of hello
 */
    std::string title;

    void first(const std::string &_title, const std::string &_content) {
        title = _title;
        std::cout << title << '\t' << _content << std::endl;
    }

    void content(const std::string &_content) {
        for (int i = 0; i < title.size() / 4 + 1; i++)
            std::cout << '\t';
        std::cout << _content << std::endl;
    }
} message;


int main() {

    message.first("fuck:", "lll");
    message.content("232323");
    return 0;
}