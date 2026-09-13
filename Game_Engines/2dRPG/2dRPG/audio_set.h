#ifndef _AUDIO_SET_H_
#define _AUDIO_SET_H_

#define START 0
#define END -1

#include "log.h"
#include "util.h"

#include <set>
#include <iostream>
#include <Windows.h>
#include <graphics.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

extern Log* log_p;

// 音频集
class AudioSet
{
public:
	AudioSet()
	{
		cur_alias_p = alias_set.end();
	}
	~AudioSet();

	// 根据含一个 %d 的格式字符串从路径中加载连续 num 个音频
	void load_from_file(LPCTSTR file_name_template, size_t num);
	// 根据文件名（带后缀）加载一个音频
	void load_from_file(LPCTSTR file_name);
	// 当前路径设置（自动调整末尾\\或/为一个）
	static void set_cur_path(LPCTSTR path_lpctstr)
	{
		cur_path = path_lpctstr;
		log_p->setTarget(LogTarget::Console);
		log_p->info("Set cur_path to: ", lpctstr_to_str_s(cur_path));
	}
	// 关闭所有音频
	void close_all();
	// 关闭指定音频（如果存在）
	void close_audio(LPCTSTR alias);
	// 关闭当前音频（如果存在）
	void close_cur();

	// 播放音频（如果已经加载；会自动停止播放当前正在播放的音乐）
	void play_audio(LPCTSTR alias, bool repeat = false, int start = START, int end = END);
	// 跳转到指定位置播放（如果正在播放）
	void seek_to(int ms);

	// 暂停当前音频（如果正在播放）
	void pause();
	// 继续播放当前音频（如果已经暂停）
	void resume();
	// 停止播放当前音频（如果正在播放）
	void stop();

	// 音量调整
	void set_volume(int volume);

	// 倍速播放
	void switch_play_speed_time_to(){}

	// 检查正在操作的音频是否正在播放
	bool check_playing();
	// 获取正在操作的音频的别名
	LPCTSTR get_cur_alias() const 
	{ 
		return (cur_alias_p != alias_set.end() ? *cur_alias_p : nullptr);
	}
	// 获取正在操作的音频的音量
	int get_cur_volume() const;
	// 获取此时音乐播放的长度（单位ms）
	int get_cur_position() const;
	// 获取音乐总时长（单位ms）
	int get_cur_length() const;

	std::set<LPCTSTR>::iterator find_alias(LPCTSTR alias) const;

public:
	void test_output_cur_path()
	{
		std::cout << "cur_path : " << lpctstr_to_str_s(cur_path) << std::endl;
	}
	void test_output_alias_list()
	{
		std::cout << "alias_set elems : [";
		for (LPCTSTR lpctstr : alias_set)
			std::cout << lpctstr_to_str_s(lpctstr) << ", ";
		std::cout << "]" << std::endl;
	}

protected:

	static void path_str_to_lpctstr_s(const std::string& path_str)
	{
		TCHAR* tchar_str = str_to_tchar(path_str);
		cur_path = tchar_to_lpctstr_s(tchar_str);
	}

private:
	std::set<LPCTSTR> alias_set;						// 音乐的别名
	std::set<LPCTSTR>::iterator cur_alias_p;			// 当前正在操作的音乐

	static LPCTSTR cur_path;							// 当前路径

};

#endif // !_AUDIO_SET_H_

