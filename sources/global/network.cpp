/*
 * CovScript Manager: Network Functions Implementation
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 */
#include <csman/global.hpp>
#include <curl/curl.h>
namespace csman::network {
	size_t write_char_buff(char *buffer, size_t size, size_t count, std::vector<char> *data)
	{
		size_t recv_size = size * count;
		data->reserve(recv_size);
		for (size_t i = 0; i < recv_size; ++i)
			data->push_back(buffer[i]);
		return recv_size;
	}

	size_t write_bin_file(char *buffer, size_t size, size_t count, FILE *data)
	{
		size_t recv_size = fwrite(buffer, size, count, data);
		return recv_size;
	}

	bool http_get(const std::string &url, std::string path, int reconnectTime)
	{
		path = csman::str::to_file_path(path);
		CURL *curl;
		FILE *fp;
		fp = fopen(path.c_str(), "wb+");
		if (fp == NULL) {
			sys::dir::create(path);
			fp = fopen(path.c_str(), "wb+");
		}

		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_bin_file);
		while (curl_easy_perform(curl) != CURLE_OK && reconnectTime-- > 0);
		if (reconnectTime <= 0) {
			remove(path.c_str());
			return false;
		}
		fclose(fp);
		return true;
	}

	std::vector<char> http_get(const std::string &url, int reconnectTime)
	{
		std::vector<char> response;
		CURL *curl;
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_char_buff);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		while (curl_easy_perform(curl) != CURLE_OK && reconnectTime-- > 0)
			response.clear();
		curl_easy_cleanup(curl);
		return response;
	}
}