#pragma once

#include "object.h"
#include "component.h"

#include <string>

class Manager
{
public:
	static Object* make_object(const std::string& type)
	{
		
		return nullptr;
	}
	static Component* make_component(const std::string& type)
	{

		return nullptr;
	}

	static void delete_object()
	{

	}
	static void delete_component()
	{

	}

private:
	Manager() = default;
	~Manager() = default;

};