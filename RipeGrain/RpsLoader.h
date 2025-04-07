#pragma once
#include <map>
#include <array>
#include <fstream>
#include <ranges>
#include <string>
#include <filesystem>
#include <debugapi.h>
#include <sstream> // Add this include

namespace RipeGrain
{
	class RipeGrainSettings
	{
	private:
		std::map<std::string, std::string> configs;
	public:
		void Load(std::filesystem::path path)
		{
			std::ifstream ifs(path);
			std::stringstream buffer;
			buffer << ifs.rdbuf();
			std::string content = buffer.str();

			auto rng = std::ranges::subrange(
				content.begin(),
				content.end()
			);

			auto line_view = rng | std::views::split('\n') | std::views::transform([](auto&& line) {
				return std::string(line.begin(), line.end());
				});

			for (auto line : line_view)
			{
				if (line.empty()) continue;
				auto delimiter_pos = line.find(':');
				if (delimiter_pos != std::string::npos)
				{
					std::string key = line.substr(0, delimiter_pos);
					std::string value = line.substr(delimiter_pos + 1);
					configs[key] = value;
				}
			}
		}
		std::string GetValue(const std::string& key)
		{
			auto it = configs.find(key);
			if (it != configs.end())
			{
				return it->second;
			}
			throw std::runtime_error("Key not found");
		}
	};
}