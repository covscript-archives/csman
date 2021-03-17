#pragma once
/*
 * CovScript Manager: Index File
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/global.hpp>
#include <fstream>

// 一般格式：
//  标题 对象数
//  对象内容...
//
// 第一个版本为Unstable，第二为Stable，之后为历史版本
// 保证历史版本为降序排列，（若无Unstable或Stable，用0占位）
//
// 剩下具体的STD，ABI号内容为：对PAC中各个包的说明与索引
namespace csman {
	class idx_file final {
	private:
		context *cxt = nullptr;

		bool
		readruntime(const std::string &name, const int &cnt, std::ifstream &ifs, std::vector<std::string> &args)   // 改
		{

			rtm_list.reserve(cnt);
			pac_description[name].push_back(name);
			std::string author, description;
			if (!getline(ifs, author))
				return false;
			pac_description[name].push_back(author);
			if (!getline(ifs, description))
				return false;
			pac_description[name].push_back(description);

			if (!str::readline(ifs, args))
				return false;
			for (int i = 0, j = 0; i < cnt; ++i, ++j) {

				if (j == 0)
					un_stable_ver[name].first = args[j];
				if (j == 1)
					un_stable_ver[name].second = args[j];

				if (args[j] != "0") {
					node_id[name][args[j]] = ++G.size;
					G.node_data[G.size] = new rtm_data(name, args[j], args[j + 1], args[j + 2], args[j + 3]);
					rtm_list.push_back(rtm_label(args[j + 1], args[j + 2], args[j + 3], G.size));
					j += 3;
				}
				else
					++G.size;
			}
			return true;
		} // 图论读点

		bool readpackage(const std::string &name, const int &cnt, std::ifstream &ifs, std::vector<std::string> &args)
		{

			std::string author, description;
			if (!getline(ifs, author))
				return false;
			pac_description[name].push_back(author);
			if (!getline(ifs, description))
				return false;
			pac_description[name].push_back(description);

			if (!str::readline(ifs, args))
				return false;
			for (int i = 0, j = 0; i < cnt; i++) {
				if (j == 0)
					un_stable_ver[name].first = args[j];
				if (j == 1)
					un_stable_ver[name].second = args[j];

				if (args[j] != "0") {
					node_id[name][args[j]] = ++G.size;
					G.node_data[G.size] = new pac_data(name, args[j], args[j + 1]);
					++j;
				}
				else
					++G.size;
				++j;
			}
			return true;
		} // 图论读点

		bool readdep(std::ifstream &ifs, std::vector<std::string> &args)   // 图论读边
		{
			if (!str::readline(ifs, args))
				return false;
			std::string label = args[0];
			int cnt = std::stoi(args[1]);


			int rtm_id = -1;
			if (label != "Generic")
				rtm_id = query_rtm_label(label).id;
			for (int i = 1; i <= cnt; i++) {
				str::readline(ifs, args);
				if (rtm_id != -1)
					G.add_edge(std::stoi(args[0]), rtm_id);
				for (int j = 1; j < args.size(); j++)
					G.add_edge(std::stoi(args[0]), std::stoi(args[j]));
			}
			return true;
		}

		std::string last_update_time;

	public:/*公开类*/
		struct pac_data {
			std::string ver, url, name;
			int hash_value;

			pac_data(const std::string &n, const std::string &v, const std::string &u) : name(n), ver(v), url(u)
			{
				hash_value = 0;
				for (char c: name + ver)
					hash_value = (((hash_value * 101) % 19260817) + (int) c) % 19260817;
			}

			bool operator<(const pac_data &rhs) const
			{
				return hash_value < rhs.hash_value;
			}
		}; // 依赖图部分，点的数据类

		struct rtm_data : pac_data {
			std::string ABI, STD;

			rtm_data(const std::string &name,
			         const std::string &ver,
			         const std::string &ABI,
			         const std::string &STD,
			         const std::string &url) : pac_data(name, ver, url),
				ABI(ABI), STD(STD) {}
		}; // 依赖图部分，点数据的类

		struct rtm_label {
			std::string STD, ABI, ver;
			int id;

			rtm_label(const std::string &A, const std::string &S, const std::string v, const int &id)
				: ABI(A), STD(S), ver(v), id(id) {}

			bool operator<(const rtm_label &r) const noexcept
			{
				if (STD != r.STD)
					return STD < r.STD;
				else
					return ABI < r.ABI;
			}
		}; // runtime标签,用于找runtime的

	private:
		/*使用:pac_description[name]*/
		map_t<std::string, std::vector<std::string> > pac_description; // 值, 包的描述信息 : name, author, description
		/*使用:rtm_list.lower_bound(a_runtime)*/
		std::vector<rtm_label> rtm_list; // 用于寻找符合要求的runtime，默认idx文件给出的是单调的
		/*lower_bound的临时替代*/
		rtm_label& query_rtm_label(const std::string &AoS)   // 找合适的ABI，STD版本号的runtime，返回runtime在图中的id，以后用lower_bound代替
		{
			for(auto it = rtm_list.rbegin(); it != rtm_list.rend(); ++it) {
				if(AoS[0]=='A') {
					if(AoS==it->ABI)
						return *it;
				}
				else if(AoS[0]=='S') {
					if(AoS==it->STD)
						return *it;
				}
			}
			throw std::runtime_error(AoS+" is not installed.");
			/*			int l = 0, r = rtm_list.size() - 1, mid = (rtm_list.size() - 1) >> 1;
			//			if (AoS[0] == 'A') {
			//				while (l != r) {
			//					mid = (l + r) >> 1;
			//					if (rtm_list[mid].ABI <= AoS)
			//						r = mid;
			//					else
			//						l = mid + 1;
			//				}
			//			}
			//			else if (AoS[0] == 'S') {
			//				while (l != r) {
			//					mid = (l + r) >> 1;
			//					if (rtm_list[mid].STD <= AoS)
			//						r = mid;
			//					else
			//						l = mid + 1;
			//				}
			//			}
			//			return mid;*/
		}

		/*使用:node_id[name][ver]*/
		map_t<std::string, map_t<std::string, int> > node_id;
		/*使用:un_stable_ver[name], first为unstable, second为stable*/
		map_t<std::string, std::pair<std::string, std::string> > un_stable_ver;

		class graph {
		public:
			struct edge {
				int out;
				bool inv;

				edge(int o, bool i) : out(o), inv(i) {}
			};

			std::vector<std::vector<edge> > head;
			std::vector<pac_data *> node_data;
			std::vector<bool> vis;
			int size = 0; // 0<size

			void init(int s)
			{
				head.resize(s + 5);
				node_data.resize(s + 5);
				vis.resize(s + 5);
			}

			void dfs(const int &u, set_t<int> &sc, bool sgn)
			{
				vis[u] = true;
				sc.insert(u);
				for (auto e : head[u])
					if (sgn == e.inv) {
						int &v = e.out;
						if (!vis[v])
							dfs(v, sc, sgn);
					}
			}

			set_t<int> dfs_sc(const int &u, bool sgn)
			{
				vis.reserve(size);
				for (auto x:vis)
					x = 0;
				set_t<int> sc;
				dfs(u, sc, sgn);
				return sc;
			}

			void add_edge(const int &a, const int &b)
			{
				head[a].push_back(edge(b, 0));
				head[b].push_back(edge(a, 1));
			}

			set_t<pac_data> depend_node_set(const int &u)
			{
				set_t<pac_data> res;
				for (auto x : dfs_sc(u, 0))
					res.insert(*node_data[x]);
				return res;
			}

			set_t<pac_data> support_node_set(const int &u)
			{
				set_t<pac_data> res;
				for (auto x : dfs_sc(u, 1))
					res.insert(*node_data[x]);
				return res;
			}

		};

		graph G;

	public:/*公开接口*/
		std::string get_stable_ver(const std::string &name)
		{
			return this->un_stable_ver[name].second;
		}

		std::string get_unstable_ver(const std::string &name)
		{
			return this->un_stable_ver[name].first;
		}

		set_t<pac_data> get_depend_set(const std::string &name, const std::string &ver)
		{
			int id = this->node_id[name][ver];
			if (id <= 0 || id > G.size) throw std::invalid_argument(name + " has no version named " + ver + ".");
			return G.depend_node_set(id);
		}

		set_t<pac_data> get_support_set(const std::string &name, const std::string &ver)
		{
			int id = this->node_id[name][ver];
			if (id <= 0 || id > G.size) throw std::invalid_argument(name + " has no version named " + ver + ".");
			return G.support_node_set(id);
		}

		idx_file(context *_cxt) : cxt(_cxt)
		{
			std::ifstream ifs(cxt->vars["sources_idx_path"]);
			if (!ifs.is_open())
				throw std::runtime_error("open idx_file failed");
			std::string str;
			std::vector<std::string> args;

			std::getline(ifs, last_update_time); //2020.02.30.10.35
			str::readline(ifs, args); //PAC 14 28
			int cnt_1 = std::stoi(args[1]); // cnt_1 包个数（按名称）
			G.init(std::stoi(args[2])); // cnt_2 包个数（按名称+版本）


			for (int i = 1; i <= cnt_1; i++) { // 0为空位无效包编号，所有包编号为1~cnt_2，共cnt个包 改
				if (!str::readline(ifs, args))
					throw std::runtime_error("incorrect format of sources_idx.");

				std::string name = args[0];
				int cnt = std::stoi(args[1]); // cnt 当前包内 包个数（按名称+版本）

				if (name == "cs.runtime")
					readruntime(name, cnt, ifs, args);
				else
					readpackage(name, cnt, ifs, args);
			}

			//读取依赖
			while (readdep(ifs, args));
		}

		idx_file() = default;
	};
}