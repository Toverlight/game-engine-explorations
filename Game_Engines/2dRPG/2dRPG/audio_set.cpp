#include "audio_set.h"

LPCTSTR AudioSet::cur_path = nullptr;

void AudioSet::load_from_file(LPCTSTR file_name_template, size_t num)
{
	TCHAR prev[] = _T("open \"");
	TCHAR file_name[128] = {};
	TCHAR mid[] = _T("\" alias ");

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(cur_path);
	size_t len4 = _tcslen(mid);

	for (int i = 0; i < num; i++)
	{

		_stprintf_s(file_name, 128, file_name_template, i + 1);
		size_t len3 = _tcslen(file_name);

		TCHAR* file_name_without_extension = lpctstr_to_tchar(ExtractFileNameWithoutExtension(file_name));
		size_t len5 = _tcslen(file_name_without_extension);

		size_t total_len = len1 + len2 + len3 + len4 + len5 + 1;
		TCHAR* dest = new TCHAR[total_len];

		_tcscpy_s(dest, total_len, prev);
		_tcscat_s(dest, total_len, cur_path);
		_tcscat_s(dest, total_len, file_name);
		_tcscat_s(dest, total_len, mid);
		_tcscat_s(dest, total_len, file_name_without_extension);

		mciSendString(dest, 0, 0, 0);

		alias_set.insert(tchar_to_lpctstr_s(file_name_without_extension));

		delete[] dest;
	}

}

void AudioSet::load_from_file(LPCTSTR file_name)
{
	MCIERROR mciError;

	TCHAR prev[] = _T("open \"");
	TCHAR* path = lpctstr_to_tchar(cur_path);
	TCHAR mid[] = _T("\" alias ");
	LPCTSTR extracted = ExtractFileNameWithoutExtension(file_name);
	TCHAR* file_name_without_extension = lpctstr_to_tchar(extracted);

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(path);
	size_t len3 = _tcslen(file_name);
	size_t len4 = _tcslen(mid);
	size_t len5 = _tcslen(file_name_without_extension);
	size_t total_len = len1 + len2 + len3 + len4 + len5 + 1;

	TCHAR* dest = new TCHAR[total_len];

	_tcscpy_s(dest, total_len, prev);
	_tcscat_s(dest, total_len, path);
	_tcscat_s(dest, total_len, file_name);
	_tcscat_s(dest, total_len, mid);
	_tcscat_s(dest, total_len, file_name_without_extension);

	//std::cout << "load_from_file cmd: " << tchar_to_str_s(dest) << std::endl;

	mciError = mciSendString(dest, 0, 0, 0);

	if (mciError != 0) {
		TCHAR errorText[256];
		mciGetErrorString(mciError, errorText, sizeof(errorText));
		/*MessageBox(NULL, errorText, _T("MCI Error"), MB_OK);*/
		log_p->setTarget(LogTarget::Console);
		log_p->error("Audio failed to load: ", tchar_to_str_s(errorText));
	}

	alias_set.insert(tchar_to_lpctstr_s(file_name_without_extension));
	log_p->setTarget(LogTarget::Console);
	log_p->info("Loaded audio: ", lpctstr_to_str_s(file_name));

	delete[] path;
	processAndFreeTcharArray(extracted);
	delete[] dest;
}

// 当前路径设置（自动调整末尾\\或/为一个）

void AudioSet::close_all()
{
	for (cur_alias_p = alias_set.begin(); cur_alias_p != alias_set.end(); ++cur_alias_p)
	{
		close_cur();
	}
}

void AudioSet::close_audio(LPCTSTR alias)
{
	auto it = alias_set.find(alias);
	if (it == alias_set.end()) return;

	TCHAR prev[] = _T("close ");

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(alias);
	size_t total_len = len1 + len2 + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, prev);
	_tcscat_s(dest, total_len, alias);

	mciSendString(dest, 0, 0, 0);

	if (_tcscmp(alias, cur_path) == 0)
		cur_alias_p = alias_set.end();

	alias_set.erase(alias);

	delete[] dest;
}

void AudioSet::close_cur()
{
	if (cur_alias_p == alias_set.end()) return;

	TCHAR prev[] = _T("close ");

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(*cur_alias_p);
	size_t total_len = len1 + len2 + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, prev);
	_tcscat_s(dest, total_len, *cur_alias_p);

	mciSendString(dest, 0, 0, 0);

	alias_set.erase(cur_alias_p);
	cur_alias_p = alias_set.end();

	delete[] dest;
}

void AudioSet::play_audio(LPCTSTR alias, bool repeat, int start, int end)
{
	MCIERROR mciError;

	auto it = find_alias(alias);
	if (it == alias_set.end())
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("Audio named ", lpctstr_to_str_s(alias), " can\'t be found!");
		return;
	}
	if (cur_alias_p != alias_set.end() && check_playing())
	{ 
		stop();
	}
	TCHAR play_tchar_a[] = _T("play ");
	TCHAR from_tchar_a[] = _T(" from ");
	TCHAR start_tchar_a[16] = {};
	TCHAR to_tchar_a[] = _T(" to ");
	TCHAR end_tchar_a[16] = {};
	TCHAR repeat_tchar_a[] = _T(" repeat");

	size_t len_play = _tcslen(play_tchar_a);
	size_t len_alias = _tcslen(alias);
	size_t len_from = _tcslen(from_tchar_a);
	size_t len_start = 0;
	size_t len_to = 0;
	size_t len_end = 0;
	size_t len_repeat = 0;
	
	_stprintf_s(start_tchar_a, 16, _T("%d"), start);
	len_start = _tcslen(start_tchar_a);
	if (end != END)
	{
		_tcprintf_s(end_tchar_a, _T("%d"), end);
		len_to = _tcslen(to_tchar_a);
		len_end = _tcslen(end_tchar_a);
	}
	if (repeat)
	{
		len_repeat = _tcslen(repeat_tchar_a);
	}
	size_t total_len = len_play + len_alias + len_from + len_start + len_to + len_end + len_repeat + 1;

	TCHAR* dest = new TCHAR[total_len];

	_tcscpy_s(dest, total_len, play_tchar_a);
	_tcscat_s(dest, total_len, alias);
	_tcscat_s(dest, total_len, from_tchar_a);
	_tcscat_s(dest, total_len, start_tchar_a);

	if (end != END)
	{
		_tcscat_s(dest, total_len, to_tchar_a);
		_tcscat_s(dest, total_len, end_tchar_a);
	}
	if (repeat)
	{
		_tcscat_s(dest, total_len, repeat_tchar_a);
	}

	mciError = mciSendString(dest, 0, 0, 0);

	if (mciError != 0) {
		TCHAR errorText[256];
		mciGetErrorString(mciError, errorText, sizeof(errorText));
		log_p->setTarget(LogTarget::Console);
		log_p->error("Audio \"", lpctstr_to_str_s(alias),
			"\" can\'t be played,there must be something wrong with the device and etc: ",
			tchar_to_str_s(errorText));
	}
	
	cur_alias_p = it;
	delete[] dest;
}

void AudioSet::seek_to(int ms)
{
	if (cur_alias_p == alias_set.end()) return;

	TCHAR seek_tchar_a[] = _T("seek ");
	TCHAR to_tchar_a[] = _T(" to ");
	TCHAR ms_tchar_a[16] = {};
	if (ms != END)
		_tcprintf_s(ms_tchar_a, _T("%d"), ms);
	else
		_tcprintf_s(ms_tchar_a, _T("end"));

	size_t len_seek = _tcslen(seek_tchar_a);
	size_t len_alias = _tcslen(*cur_alias_p);
	size_t len_to = _tcslen(to_tchar_a);
	size_t len_ms = _tcslen(ms_tchar_a);

	size_t total_len = len_seek + len_alias + len_to + len_ms + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, seek_tchar_a);
	_tcscat_s(dest, total_len, *cur_alias_p);
	_tcscat_s(dest, total_len, to_tchar_a);
	_tcscat_s(dest, total_len, ms_tchar_a);

	mciSendString(dest, 0, 0, 0);

	delete[] dest;
}

void AudioSet::pause()
{
	if (cur_alias_p == alias_set.end()) return;

	TCHAR prev[] = _T("pause ");

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(*cur_alias_p);
	size_t total_len = len1 + len2 + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, prev);
	_tcscat_s(dest, total_len, *cur_alias_p);

	mciSendString(dest, 0, 0, 0);

	delete[] dest;
}

void AudioSet::resume()
{
	if (cur_alias_p == alias_set.end()) return;

	TCHAR prev[] = _T("resume ");

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(*cur_alias_p);
	size_t total_len = len1 + len2 + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, prev);
	_tcscat_s(dest, total_len, *cur_alias_p);

	mciSendString(dest, 0, 0, 0);

	delete[] dest;
}

void AudioSet::stop()
{
	if (cur_alias_p == alias_set.end()) return;

	TCHAR prev[] = _T("stop ");

	size_t len1 = _tcslen(prev);
	size_t len2 = _tcslen(*cur_alias_p);
	size_t total_len = len1 + len2 + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, prev);
	_tcscat_s(dest, total_len, *cur_alias_p);

	mciSendString(dest, 0, 0, 0);

	delete[] dest;
}

void AudioSet::set_volume(int volume)
{
	if (cur_alias_p == alias_set.end()) return;

	TCHAR setaudio_tchar_a[] = _T("setaudio ");
	TCHAR vto_tchar_a[] = _T(" volume to ");
	TCHAR volume_tchar_a[16] = {};
	_tcprintf_s(volume_tchar_a, _T("%d"), volume % 1000);

	size_t len_sa = _tcslen(setaudio_tchar_a);
	size_t len_alias = _tcslen(*cur_alias_p);
	size_t len_vto = _tcslen(vto_tchar_a);
	size_t len_volume = _tcslen(volume_tchar_a);
	size_t total_len = len_sa + len_alias + len_vto + len_volume + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, setaudio_tchar_a);
	_tcscat_s(dest, total_len, *cur_alias_p);
	_tcscat_s(dest, total_len, vto_tchar_a);
	_tcscat_s(dest, total_len, volume_tchar_a);

	mciSendString(dest, 0, 0, 0);

	delete[] dest;
}

bool AudioSet::check_playing()
{
	if (cur_alias_p == alias_set.end()) return false;

	TCHAR status_tchar_a[] = _T("status ");
	TCHAR mode_tchar_a[] = _T(" mode");

	size_t len_status = _tcslen(status_tchar_a);
	size_t len_alias = _tcslen(*cur_alias_p);
	size_t len_mode = _tcslen(mode_tchar_a);
	size_t total_len = len_status + len_alias + len_mode + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, status_tchar_a);
	_tcscat_s(dest, total_len, *cur_alias_p);
	_tcscat_s(dest, total_len, mode_tchar_a);

	TCHAR status[64] = {};
	mciSendString(dest, status, 64, 0);

	delete[] dest;
	
	return _tcscmp(status, _T("playing")) == 0;
}

int AudioSet::get_cur_volume() const
{
	if (cur_alias_p == alias_set.end()) return -1;
	TCHAR status_tchar_a[] = _T("status ");
	TCHAR volume_tchar_a[] = _T(" volume");

	size_t len_status = _tcslen(status_tchar_a);
	size_t len_alias = _tcslen(*cur_alias_p);
	size_t len_volume = _tcslen(volume_tchar_a);
	size_t total_len = len_status + len_alias + len_volume + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, status_tchar_a);
	_tcscat_s(dest, total_len, *cur_alias_p);
	_tcscat_s(dest, total_len, volume_tchar_a);

	TCHAR volume[50] = {};
	mciSendString(dest, volume, 50, 0);

	int res = atoi(tchar_to_str_s(volume).c_str());

	delete[] dest;

	return res;
}

int AudioSet::get_cur_position() const
{
	if (cur_alias_p == alias_set.end()) return 0;
	TCHAR status_tchar_a[] = _T("status ");
	TCHAR position_tchar_a[] = _T(" position");

	size_t len_status = _tcslen(status_tchar_a);
	size_t len_alias = _tcslen(*cur_alias_p);
	size_t len_position = _tcslen(position_tchar_a);
	size_t total_len = len_status + len_alias + len_position + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, status_tchar_a);
	_tcscat_s(dest, total_len, *cur_alias_p);
	_tcscat_s(dest, total_len, position_tchar_a);

	TCHAR position[100] = {};
	mciSendString(dest, position, 100, 0);
	
	int res = atoi(tchar_to_str_s(position).c_str());

	delete[] dest;

	return res;
}

int AudioSet::get_cur_length() const
{
	if (cur_alias_p == alias_set.end()) return 0;
	TCHAR status_tchar_a[] = _T("status ");
	TCHAR length_tchar_a[] = _T(" length");

	size_t len_status = _tcslen(status_tchar_a);
	size_t len_alias = _tcslen(*cur_alias_p);
	size_t len_length = _tcslen(length_tchar_a);
	size_t total_len = len_status + len_alias + len_length + 1;

	TCHAR* dest = new TCHAR[total_len];
	_tcscpy_s(dest, total_len, status_tchar_a);
	_tcscat_s(dest, total_len, *cur_alias_p);
	_tcscat_s(dest, total_len, length_tchar_a);

	TCHAR length[100] = {};
	mciSendString(dest, length, 100, 0);

	int res = atoi(tchar_to_str_s(length).c_str());

	delete[] dest;

	return res;
}

std::set<LPCTSTR>::iterator AudioSet::find_alias(LPCTSTR alias) const
{
	for (auto it = alias_set.begin(); it != alias_set.end(); ++it)
	{
		if (_tcscmp(*it, alias) == 0)
			return it;
	}
	return alias_set.end();
}

AudioSet::~AudioSet()
{
	for (LPCTSTR lpctstr : alias_set)
	{
		processAndFreeTcharArray(lpctstr);
	}

}
