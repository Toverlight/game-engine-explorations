#ifndef _WINDOW_MANAGER_H_
#define _WINDOW_MANAGER_H_

#include "windowplus.h"

class WindowManager
{
public:
	static Window* make_one()
	{
		if (!single_one_p){
			Window* window_p = new Window(true);
			single_one_p = window_p;
			return window_p;
		}
		return nullptr;
	}

	static void del_if_one()
	{
		if (single_one_p) 
		{
			delete single_one_p;
			single_one_p = nullptr;
		}
	}

private:
	WindowManager() = default;
	~WindowManager() = default;

private:
	static Window* single_one_p;

};

#endif // !_WINDOW_MANAGER_H_