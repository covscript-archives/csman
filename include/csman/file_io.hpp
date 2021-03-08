//
// Created by Administrator on 2021/2/11.
//
#pragma once
#include<iostream>
#include<fstream>
#include<vector>

void str_split(std::vector<std::string> &args, const std::string &s)
{
	std::string buf;
	args.clear();
	for (auto c : s) {
		if (std::isspace(c)) {
			if (!buf.empty()) {
				args.push_back(buf);
				buf.clear();
			}
		}
		else
			buf += c;
	}
	if (!buf.empty())
		args.push_back(buf);
}
bool readline(std::ifstream &ifs, std::vector<std::string> &args)
{
	std::string s;
	if (!std::getline(ifs, s))
		return false;
	str_split(args, s);
	return true;
}