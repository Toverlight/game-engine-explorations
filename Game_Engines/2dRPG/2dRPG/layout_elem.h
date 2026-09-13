#ifndef _LAYOUT_ELEM_H_
#define _LAYOUT_ELEM_H_

#include "log.h"
#include "component.h"

class Panel;
class Layout;

extern Log* log_p;

class LayoutElem : public Component
{
public:
	LayoutElem() : size({ 0,0 }){}
	virtual ~LayoutElem() = default;

	void set_size(const Vector2& size)
	{
		this->size = size;
	}
	void set_size(int w, int h)
	{
		size.x = w;
		size.y = h;
	}
	const Vector2& get_size() const { return size; }

private:
	Vector2 size;						// 大小（长,高）

};

#endif // !_LAYOUT_ELEM_H_

