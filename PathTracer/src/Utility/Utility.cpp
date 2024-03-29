#include "Utility.hpp"

namespace PathTracer {
	std::vector<std::string> SplitString(const std::string& str, const std::string& separator)
	{
		std::vector<std::string> list = std::vector<std::string>();
		size_t offset = std::string::size_type(0);
		size_t separatorLength = separator.length();

		while (true)
		{
			size_t pos = str.find(separator, offset);
			if (pos == std::string::npos) {
				list.push_back(str.substr(offset));
				break;
			}
			// 区切り文字が文字列の先頭にあるまたは連続している場合を除く
			if (pos != offset)
				list.push_back(str.substr(offset, pos - offset));
			offset = pos + separatorLength;
			// 末尾が改行文字の場合は何もしない
			if (pos == str.length() - 1) {
				break;
			}
		}

		return list;
	}
}