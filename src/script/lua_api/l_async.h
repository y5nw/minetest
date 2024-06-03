#pragma once

#include "lua_api/l_base.h"
#include "util/basic_macros.h"

class LuaAsyncJob : public ModApiBase
{
private:
	const int id;

	LuaAsyncJob(const int id): id(id) {}

	static const luaL_Reg methods[];

	// garbage collector
	static int gc_object(lua_State *L);

	// get_id(self) -> id
	static int l_get_id(lua_State *L);

	// replace(self, func, params, mod_origin) -> id: implemented in Lua
	// cancel(self) -> boolean: implemented in Lua

public:
	DISABLE_CLASS_COPY(LuaAsyncJob)

	int get_id() const { return id; }

	static int create(lua_State *L, const int id);

	static void Register(lua_State *L);
	static const char className[];
};

class ModApiAsync : public ModApiBase
{
public:
	static void Initialize(lua_State *L, int top);
private:
	// do_async_callback(func, params, mod_origin)
	static int l_do_async_callback(lua_State *L);
	// cancel_async_callback(id)
	static int l_cancel_async_callback(lua_State *L);
	// get_async_threading_capacity()
	static int l_get_async_threading_capacity(lua_State *L);
};
