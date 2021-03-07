#pragma once
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "zip.h"
#include "optional.hpp"

namespace cov {
	class zip {
	public:
		enum class openmode {
			read = 'r', write = 'w', append = 'a'
		};
		struct entry {
			std::string name;
			bool is_dir = false;
			std::size_t size = 0;
			std::size_t crc32 = 0;
			entry(const std::string& n, bool is, std::size_t s, std::size_t c) :name(n), is_dir(is), size(s), crc32(c) {}
		};
		struct buffer {
			std::size_t size = 0;
			void *data = nullptr;
		private:
			friend class zip;
			buffer(std::size_t s) : size(s)
			{
				data = ::malloc(s);
			}
		public:
			buffer(buffer&& buff) noexcept
			{
				std::swap(size, buff.size);
				std::swap(data, buff.data);
			}
			~buffer()
			{
				::free(data);
			}
		};
	private:
		zip_t *m_zip = nullptr;
		openmode m_mode;
	public:
		explicit zip(const std::string &path, openmode mode = openmode::read):m_mode(mode)
		{
			m_zip = zip_open(path.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, static_cast<char>(mode));
		}
		zip(const zip&) = delete;
		~zip()
		{
			zip_close(m_zip);
		}
		bool is_open() const
		{
			return m_zip != nullptr;
		}
		operator bool() const
		{
			return is_open();
		}
		optional<std::vector<entry>> get_entries()
		{
			if (m_mode != openmode::read)
				return nullopt;
			std::vector<entry> entries;
			int n = zip_total_entries(m_zip);
			for (int i = 0; i < n; ++i) {
				zip_entry_openbyindex(m_zip, i);
				const char *name = zip_entry_name(m_zip);
				int isdir = zip_entry_isdir(m_zip);
				unsigned long long size = zip_entry_size(m_zip);
				unsigned int crc32 = zip_entry_crc32(m_zip);
				entries.emplace_back(name, isdir, size, crc32);
				zip_entry_close(m_zip);
			}
			return optional<std::vector<entry>>(in_place, std::move(entries));
		}
		optional<buffer> read_entry(const std::string& path)
		{
			if (zip_entry_open(m_zip, path.c_str()) < 0)
				return nullopt;
			buffer buff(zip_entry_size(m_zip));
			zip_entry_noallocread(m_zip, buff.data, buff.size);
			zip_entry_close(m_zip);
			return optional<buffer>(in_place, std::move(buff));
		}
		bool read_entry_stream(const std::string& path, std::ostream& os)
		{
			auto opt = read_entry(path);
			if (!opt.has_value())
				return false;
			buffer &buff = opt.value();
			os.write(reinterpret_cast<char*>(buff.data), buff.size*sizeof(char));
			return true;
		}
		bool write_entry_stream(const std::string& path, std::istream& is)
		{
			if (zip_entry_open(m_zip, path.c_str()) < 0)
				return false;
			char buff[1024];
			std::size_t actual_read = 0;
			while (is.rdbuf()->in_avail() > 0) {
				actual_read = is.readsome(buff, sizeof(buff));
				zip_entry_write(m_zip, buff, actual_read);
			}
			zip_entry_close(m_zip);
			return true;
		}
		bool entry_add(const std::string& zip_path, const std::string& target_path)
		{
			if (zip_entry_open(m_zip, zip_path.c_str()) < 0)
				return false;
			zip_entry_fwrite(m_zip, target_path.c_str());
			zip_entry_close(m_zip);
			return true;
		}
		bool entry_extract(const std::string& zip_path, const std::string& target_path)
		{
			if (zip_entry_open(m_zip, zip_path.c_str()) < 0)
				return false;
			zip_entry_fread(m_zip, target_path.c_str());
			zip_entry_close(m_zip);
			return true;
		}
		bool entry_delete(const std::string& path)
		{
			auto ptr = path.c_str();
			return zip_entries_delete(m_zip, const_cast<char* const*>(&ptr), 1) == 0;
		}
	};

	bool zip_extract(const std::string& zip_path, const std::string& target_path)
	{
		return ::zip_extract(zip_path.c_str(), target_path.c_str(), nullptr, nullptr) == 0;
	}
}