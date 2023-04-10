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
			// ‹æØ‚è•¶š‚ª•¶š—ñ‚Ìæ“ª‚É‚ ‚é‚Ü‚½‚Í˜A‘±‚µ‚Ä‚¢‚éê‡‚ğœ‚­
			if (pos != offset)
				list.push_back(str.substr(offset, pos - offset));
			offset = pos + separatorLength;
			// ––”ö‚ª‰üs•¶š‚Ìê‡‚Í‰½‚à‚µ‚È‚¢
			if (pos == str.length() - 1) {
				break;
			}
		}

		return list;
	}
}