/*
 * CovScript Manager: OS Depended Functions Implementation
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/initializer.hpp>

namespace csman {
	/*
	 * all variables are in "configure vars" :
	 * COVSCRIPT_HOME, CS_IMPORT_PATH, CS_DEV_PATH
	 * config_path, csman_path, pac_repo_path, sources_idx_path, max_reconnect_time
	 */
	void initializer::init_all(csman::context *cxt, pac_repo &repo, idx_file &sources_idx)
	{
		try {
			init_config(cxt);
			init_pac_repo(repo,cxt);
			init_sources_idx(sources_idx,cxt);
		}
		catch(const std::exception &e) {
			throw std::runtime_error("error occurred while using \"csman init\": " + std::string(e.what()));
		}
	}
	void initializer::init_config(csman::context *cxt)
	{
		cxt->write_config();
	}
	void initializer::init_pac_repo(pac_repo &repo,csman::context *cxt)
	{
	}
	void initializer::init_sources_idx(idx_file &sources_idx, csman::context *cxt)
	{
		sources_idx = csman::idx_file(cxt);
	}
};