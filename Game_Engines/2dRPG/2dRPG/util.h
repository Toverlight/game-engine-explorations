#ifndef _UTIL_H_
#define _UTIL_H_

#include "camera.h"
#include "log.h"
#include "var_g.h"
#include "RGB_t.h"

#include <iostream>
#include <string>
#include <vector>
#include <codecvt> // For wide-character conversion
#include <tchar.h>
#include <windows.h>
#include <graphics.h>

#pragma comment(lib, "MSIMG32.LIB")

class GVar;
class Log;

extern GVar* gv_p;
extern Log* log_p;

void processAndFreeTcharArray(LPCTSTR tchar_ptr);

// * 用完手动释放 LPCTSTR 指向的内存
inline LPCTSTR tchar_to_lpctstr_s(TCHAR* tcharStr)
{
	return static_cast<LPCTSTR>(tcharStr);
}

// * （拷贝）使用完TCHAR*后注意释放内存
inline TCHAR* lpctstr_to_tchar(LPCTSTR lpctstr)
{
	if (lpctstr == nullptr) {
		return nullptr;
	}

	size_t length = _tcslen(lpctstr) + 1;
	TCHAR* tcharStr = new TCHAR[length];
	_tcscpy_s(tcharStr, length, lpctstr);

	return tcharStr;
}

inline std::wstring str_to_wstr_s(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}

// * 外部提供std::wstring的缓冲区（共享内存，不建议使用）
inline LPCTSTR str_to_lpctstr(const std::string& str, std::wstring& out_wstr)
{
#ifdef UNICODE
	out_wstr = std::wstring(str.begin(), str.end());
	return out_wstr.c_str();
#else
	return str.c_str();
#endif // UNICODE
}

inline std::string lpctstr_to_str_s(LPCTSTR lpcstr) 
{
#ifdef UNICODE
	std::wstring wstr(lpcstr);
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(bufferSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);
	return str;
#else
	return std::string(lpcstr);  // 直接转换
#endif
}

inline std::string tchar_to_str_s(TCHAR* tchar_str)
{
	LPCTSTR lpctstr = tchar_to_lpctstr_s(tchar_str);
	return lpctstr_to_str_s(lpctstr);
}
// * （拷贝）注意内存释放
inline TCHAR* str_to_tchar(const std::string& str)
{
#ifdef UNICODE
	std::wstring wstr = str_to_wstr_s(str);

	size_t len = wstr.length();
	TCHAR* tchar_array = new TCHAR[len + 1]; // +1 for null terminator
	std::wmemcpy(tchar_array, wstr.c_str(), len);
	tchar_array[len] = L'\0'; // Null terminate

	return tchar_array; // 返回堆区分配的 TCHAR 数组首地址
#else
	size_t len = str.length();
	TCHAR* tchar_array = new TCHAR[len + 1]; // +1 for null terminator

	// 拷贝字符串内容到 tchar_array
	std::strcpy(tchar_array, str.c_str());

	return tchar_array; // 返回堆区分配的 TCHAR 数组首地址

#endif // UNICODE
}
// * （拷贝）用完手动释放 LPCTSTR 指向的内存
inline LPCTSTR ExtractFileNameWithoutExtension(LPCTSTR path) {
	// 将LPCTSTR转换为std::string
#ifdef UNICODE
	std::wstring wPath(path);
	std::string filePath(wPath.begin(), wPath.end());
#else
	std::string filePath(path);
#endif // UNICODE

	// 查找最后一个反斜杠的位置（路径分隔符）
	size_t lastSlashPos = filePath.find_last_of("\\/");

	// 提取文件名（包括扩展名）
	std::string fileName = (lastSlashPos == std::string::npos) ? filePath : filePath.substr(lastSlashPos + 1);

	// 查找最后一个点的位置（扩展名分隔符）
	size_t lastDotPos = fileName.find_last_of('.');

	// 提取不带扩展名的文件名
	std::string baseName = (lastDotPos == std::string::npos) ? fileName : fileName.substr(0, lastDotPos);
	TCHAR* tchar_str = str_to_tchar(baseName);
	LPCTSTR lpctstr = tchar_to_lpctstr_s(tchar_str);

	return lpctstr;
}

void putimage(int dst_x, int dst_y, IMAGE* img, bool using_camera);
void putimage_alpha(int dst_x, int dst_y, IMAGE* img, bool using_camera = true);
void putimage_alpha(int dst_x, int dst_y, IMAGE* img, float scale, bool using_camera = true);

void printImageRGBValues(IMAGE* img_p);

void line(int left, int top, int right, int bottom, bool using_camera);
void circle(int x, int y, int radius, bool using_camera);
void fillcircle(int x, int y, int radius, bool using_camera);
void solidcircle(int x, int y, int radius, bool using_camera);
void rectangle(int left, int top, int right, int bottom, bool using_camera);
void fillrectangle(int left, int top, int right, int bottom, bool using_camera);
void solidrectangle(int left, int top, int right, int bottom, bool using_camera);

void line(int left, int top, int right, int bottom, float scale, bool using_camera);
void circle(int x, int y, int radius, float scale, bool using_camera);
void fillcircle(int x, int y, int radius, float scale, bool using_camera);
void solidcircle(int x, int y, int radius, float scale, bool using_camera);
void rectangle(int left, int top, int right, int bottom, float scale, bool using_camera);
void fillrectangle(int left, int top, int right, int bottom, float scale, bool using_camera);
void solidrectangle(int left, int top, int right, int bottom, float scale, bool using_camera);

void drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, bool using_camera);
void drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, float scale, bool using_camera);
// 大写锁定是否开启
inline bool isCapsLockOn() 
{
	// 使用 GetKeyState 检查 CapsLock 键的状态
	return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}


// 计算分层设色地形图中块的颜色，写入给定vector变量
// * 仅特定用途：海拔高度在 -800 ~ 1000 时适用
inline void transformElevationsToRGB(std::vector<std::vector<RGB_T>>& _RGBs, 
	std::vector<std::vector<float>> _elevations)
{
	static int maxElevation = 1000;
	static int minElevation = -800;
	static int deltaElevation = (maxElevation - minElevation) / 18;
	for (int i = 0; i < _elevations.size(); ++i)
	{
		for (int j = 0; j < _elevations[0].size(); ++j)
		{
			// 低于0的海拔用蓝色画笔，颜色越深表示海拔高度越低
			if (_elevations[i][j] < minElevation + deltaElevation)
				//setfillcolor(RGB(9, 51, 113));
				_RGBs[i][j] = { 9,51,113 };
			else if (_elevations[i][j] < minElevation + 2 * deltaElevation)
				//setfillcolor(RGB(12, 64, 139));
				_RGBs[i][j] = { 12,64,139 };
			else if (_elevations[i][j] < minElevation + 3 * deltaElevation)
				//setfillcolor(RGB(14, 71, 155));
				_RGBs[i][j] = { 14,71,155 };
			else if (_elevations[i][j] < minElevation + 4 * deltaElevation)
				//setfillcolor(RGB(14, 80, 176));
				_RGBs[i][j] = { 14,80,176 };
			else if (_elevations[i][j] < minElevation + 5 * deltaElevation)
				//setfillcolor(RGB(14, 86, 192));
				_RGBs[i][j] = { 14,86,192 };
			else if (_elevations[i][j] < minElevation + 6 * deltaElevation)
				//setfillcolor(RGB(13, 94, 213));
				_RGBs[i][j] = { 13,94,213 };
			else if (_elevations[i][j] < minElevation + 7 * deltaElevation)
				//setfillcolor(RGB(16, 108, 244));
				_RGBs[i][j] = { 16,108,244 };
			// 高于0但低于500的海拔用绿色画笔，颜色越深表示海拔高度越低
			else if (_elevations[i][j] < minElevation + 8 * deltaElevation)
				//setfillcolor(RGB(42, 82, 23));
				_RGBs[i][j] = { 42,82,23 };
			else if (_elevations[i][j] < minElevation + 9 * deltaElevation)
				//setfillcolor(RGB(54, 102, 31));
				_RGBs[i][j] = { 54,102,31 };
			else if (_elevations[i][j] < minElevation + 10 * deltaElevation)
				//setfillcolor(RGB(66, 122, 38));
				_RGBs[i][j] = { 66, 122, 38 };
			else if (_elevations[i][j] < minElevation + 11 * deltaElevation)
				//setfillcolor(RGB(65, 144, 27));
				_RGBs[i][j] = { 65, 144, 27 };
			else if (_elevations[i][j] < minElevation + 12 * deltaElevation)
				//setfillcolor(RGB(62, 170, 10));
				_RGBs[i][j] = { 62, 170, 10 };

			// 高于500的海拔用棕色画笔，颜色越深表示海拔高度越高
			else if (_elevations[i][j] < minElevation + 13 * deltaElevation)
				//setfillcolor(RGB(228, 193, 11));
				_RGBs[i][j] = { 228, 193, 11 };
			else if (_elevations[i][j] < minElevation + 14 * deltaElevation)
				//setfillcolor(RGB(200, 170, 15));
				_RGBs[i][j] = { 200, 170, 15 };
			else if (_elevations[i][j] < minElevation + 15 * deltaElevation)
				//setfillcolor(RGB(165, 141, 15));
				_RGBs[i][j] = { 165, 141, 15 };
			else if (_elevations[i][j] < minElevation + 16 * deltaElevation)
				//setfillcolor(RGB(133, 114, 15));
				_RGBs[i][j] = { 133, 114, 15 };
			else if (_elevations[i][j] < minElevation + 17 * deltaElevation)
				//setfillcolor(RGB(104, 89, 13));
				_RGBs[i][j] = { 104, 89, 13 };
			else
				//setfillcolor(RGB(66, 57, 8));
				_RGBs[i][j] = { 66, 57, 8 };

			//std::cout << "(" << (int)_RGBs[i][j].R << ", " << (int)_RGBs[i][j].G << ", " <<
			//	(int)_RGBs[i][j].B << "), ";
		}
		/*std::cout << ";\n";*/
	}
}

// 将赋予颜色的块渲染入给定IMAGE变量
inline void drawColorfulBlocksToImage(IMAGE* _pDest, std::vector<std::vector<RGB_T>>& _colorfulBlocks, 
	int _blockLen)
{
	SetWorkingImage(_pDest);
	if (_pDest && !(
			_pDest->getwidth() == _colorfulBlocks[0].size() * _blockLen &&
			_pDest->getheight() == _colorfulBlocks.size() * _blockLen
			)
		)
	{
		_pDest->Resize(_colorfulBlocks[0].size() * _blockLen, _colorfulBlocks.size() * _blockLen);
	}
	for (int i = 0; i < _colorfulBlocks.size(); ++i)
	{
		for (int j = 0; j < _colorfulBlocks[0].size(); ++j)
		{
			RGB_T& rgb = _colorfulBlocks[i][j];
			setfillcolor(RGB(rgb.R, rgb.G, rgb.B));
			solidrectangle(j * _blockLen, i * _blockLen, (j + 1) * _blockLen - 1, (i + 1) * _blockLen - 1);
			//std::cout << "(" << j * _blockLen << ", " << i * _blockLen << "): " <<
			//	"(" << (int)rgb.R << ", " << (int)rgb.G << ", " << (int)rgb.B << "), ";
		}
		/*std::cout << ";\n";*/
	}

	SetWorkingImage(NULL);
}

#endif // !_UTIL_H_