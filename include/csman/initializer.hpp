#pragma once
/*
 * CovScript Manager: CLI Parser
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/global.hpp>
#include <csman/pac_repo.hpp>
#include <csman/idx_file.hpp>

namespace csman {
	class initializer { // 默认已安装covscript，未安装csman
	private:
		void init_all(csman::context *, pac_repo &, idx_file &);
		void init_config(csman::context *);
		void init_pac_repo(pac_repo &, csman::context *);
		void init_sources_idx(idx_file &, csman::context *);
	public:
		initializer() = delete;
		explicit initializer(csman::context *);
		~initializer() = default;
	};
}