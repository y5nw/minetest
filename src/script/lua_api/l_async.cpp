#include "lua_api/l_internal.h"
#include "lua_api/l_async.h"
#include "cpp_api/s_async.h"

static std::string get_serialized_function(lua_State *L, int index)
{
	luaL_checktype(L, index, LUA_TFUNCTION);
	call_string_dump(L, index);
	size_t func_length;
	const char *serialized_func_raw = lua_tolstring(L, -1, &func_length);
	return std::string(serialized_func_raw, func_length);
}

// do_async_callback(func, params, mod_origin)
int ModApiAsync::l_do_async_callback(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	ScriptApiAsync *script = getScriptApi<ScriptApiAsync>(L);

	luaL_checktype(L, 2, LUA_TTABLE);
	luaL_checktype(L, 3, LUA_TSTRING);

	auto serialized_func = get_serialized_function(L, 1);
	PackedValue *param = script_pack(L, 2);
	std::string mod_origin = readParam<std::string>(L, 3);

	u32 jobId = script->queueAsync(
		std::move(serialized_func),
		param, mod_origin);

	lua_settop(L, 0);
	lua_pushinteger(L, jobId);
	return 1;
}

// replace_async_callback(id, func, params, mod_origin)
int ModApiAsync::l_replace_async_callback(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	ScriptApiAsync *script = getScriptApi<ScriptApiAsync>(L);

	luaL_checktype(L, 1, LUA_TNUMBER);
	luaL_checktype(L, 3, LUA_TTABLE);
	luaL_checktype(L, 4, LUA_TSTRING);

	u32 id = lua_tointeger(L, 1);
	auto serialized_func = get_serialized_function(L, 2);
	PackedValue *param = script_pack(L, 3);
	std::string mod_origin = readParam<std::string>(L, 4);

	u32 jobId = script->replaceAsync(id,
		std::move(serialized_func),
		param, mod_origin);

	lua_settop(L, 0);
	lua_pushinteger(L, jobId);
	return 1;
}

void ModApiAsync::Initialize(lua_State *L, int top)
{
	API_FCT(do_async_callback);
	API_FCT(replace_async_callback);
}
