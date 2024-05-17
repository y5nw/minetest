#pragma once

#include "lua_api/l_base.h"

class ModApiAsync : public ModApiBase
{
public:
	static void Initialize(lua_State *L, int top);
private:
	static int l_do_async_callback(lua_State *L);
	static int l_replace_async_callback(lua_State *L);
};
