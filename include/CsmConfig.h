     //
// Created by Rend on 2020/9/19.
//
#pragma once

#include<set>
#include<vector>
#include<cstdio>
#include<cstring>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<CsmBase.h>

class CsmJsonFile {
public:
	explicit CsmJsonFile(Context *&_cxt,const std::string &_path);/*Reading JSON via constructor*/
	explicit CsmJsonFile(Context *&_cxt,const Json::Value &_root);

protected:
	std::string path;
	Json::Value root;
    Context *cxt;
	virtual ~CsmJsonFile() = default;

	virtual void PreLaunching() {}

	virtual void EndLaunching() {}

	virtual void Initialize() {}

	virtual bool Validate()	{
		return 0;
	}
};

namespace CsmConfig {
	class Config;

	class Sources;

	class Package;

	bool FileInitialize(Context *&cxt, CsmJsonFile *config, CsmJsonFile* sources, CsmJsonFile* package);
}
/*done*/
class CsmConfig::Config : public CsmJsonFile {
public:
	using CsmJsonFile::CsmJsonFile;
private:
	void Initialize() override;

	bool Validate() override;

	void PreLaunching() override;

	void EndLaunching() override;

	virtual ~Config() override;
};

/*all done*/
class CsmConfig::Sources : public CsmJsonFile {
/* package info(JSON)
 * "20.2.2":{
 *      "CurrentUrl":"",
 *      "Dependencies":{}
 * }
 */
public:
	using CsmJsonFile::CsmJsonFile;

	std::vector<Json::Value> GetDep(const std::vector<Json::Value> &vec_pac); /*done*/

	std::vector<Json::Value> GetDep(const std::vector<std::string> &vec_str); /*done*/

	std::vector<Json::Value> GetDep(const Json::Value &pac); /*done*/

	std::vector<Json::Value> GetDep(const std::string &name,const std::string &ver); /*done*/

	std::vector<std::string> DepsUrl(const std::vector<Json::Value> &x); /*done*/

	std::vector<std::string> DepsName(const std::vector<Json::Value> &x); /*done*/ // turn vector of packages(JSON) into vector of pacnames(string)

    Json::Value GetPacJson(const std::string &name, const std::string &ver); /*done*/
// return a package(JSON)
private:
    std::set<std::string> vis;

	void JsonMerge(Json::Value &a, Json::Value &b); /*done*/

	void PreLaunching() override; /*done*/

	std::vector<Json::Value> DFSDeps(const Json::Value &pac); /*done*/
// return a vector of packages(JSON), which means a dependencies set of this package (include itself)
};

class CsmConfig::Package : public CsmJsonFile {
public:
	using CsmJsonFile::CsmJsonFile;

	std::vector<std::string> Absence(std::vector<std::string>Deps); /*done*/

	bool Contains(const std::string &name, const std::string &ver); /*done*/

	void Add(const std::string &name, const std::string &ver); /*done*/

	void Del(const std::string &name, const std::string &ver); /*done*/

private:
    Json::Value lastDelete;

	virtual void PreLaunching() override; /*done*/

	virtual void EndLaunching() override;

	virtual void Initialize() override; /*flag*/

	virtual bool Validate() override; /*done*/

	virtual ~Package() override;
};

