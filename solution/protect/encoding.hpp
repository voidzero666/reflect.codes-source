#pragma once

#include <string>

namespace nnx {
	namespace encoding {
		static __forceinline std::string utf16to8(const std::wstring& inp_wstr) noexcept {
			std::string out_str;
			const size_t& len = inp_wstr.length();
			for (size_t i = 0; i < len; i++) {
				const wchar_t& c = inp_wstr.at(i);
				if ((c >= 0x0001) && (c <= 0x007F))
					out_str += inp_wstr.at(i);
				else if (c > 0x07FF) {
					out_str += 0xE0 | ((c >> 12) & 0x0F);
					out_str += 0x80 | ((c >> 6) & 0x3F);
					out_str += 0x80 | ((c >> 0) & 0x3F);
				}
				else {
					out_str += 0xC0 | ((c >> 6) & 0x1F);
					out_str += 0x80 | ((c >> 0) & 0x3F);
				}
			}
			return out_str;
		}

		static __forceinline std::wstring utf8to16(const std::string& inp_str) noexcept {
			std::wstring out_wstr;
			size_t i = 0;
			const size_t& len = inp_str.length();
			while (i < len) {
				const unsigned char& c = inp_str.at(i++);
				unsigned char char2, char3;
				switch (c >> 4) {
				case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
					// 0xxxxxxx
					out_wstr += inp_str.at(i - 1);
					break;
				case 12: case 13:
					// 110x xxxx   10xx xxxx
					char2 = inp_str.at(i++);
					out_wstr += ((c & 0x1F) << 6) | (char2 & 0x3F);
					break;
				case 14:
					// 1110 xxxx  10xx xxxx  10xx xxxx
					char2 = inp_str.at(i++);
					char3 = inp_str.at(i++);
					out_wstr += ((c & 0x0F) << 12) | ((char2 & 0x3F) << 6) | ((char3 & 0x3F) << 0);
					break;
				}
			}
			return out_wstr;
		}
	}
}