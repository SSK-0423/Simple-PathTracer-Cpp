#pragma once
#include <string>
#include <vector>

namespace PathTracer {
	enum class RESULT {
		SUCCESS = 0,
		FAILED = -1,
	};
	
	std::vector<std::string> SplitString(const std::string& str, const std::string& separator);
}