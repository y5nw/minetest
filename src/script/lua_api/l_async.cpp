#include "lua_api/l_internal.h"
#include "lua_api/l_async.h"
#include "cpp_api/s_async.h"

// garbage collector
int LuaAsyncJob::gc_object(lua_State *L)
{
	LuaAsyncJob *o = *(LuaAsyncJob **)(lua_touserdata(L, 1));
	delete o;
	return 0;
}

// get_id() -> id
int LuaAsyncJob::l_get_id(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaAsyncJob *o = checkObject<LuaAsyncJob>(L, 1);
	lua_pushinteger(L, o->get_id());
	return 1;
}

static std::string get_serialized_function(lua_State *L, int index)
{
	luaL_checktype(L, index, LUA_TFUNCTION);
	call_string_dump(L, index);
	size_t func_length;
	const char *serialized_func_raw = lua_tolstring(L, -1, &func_length);
	return std::string(serialized_func_raw, func_length);
}

// replace(self, func, params, mod_origin) -> new_job
// This implements the part that is strictly needed for replacing the job.
// The actual LuaAsyncJob:replace used by mods is implemented in Lua based on this.
int LuaAsyncJob::l_replace(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	ScriptApiAsync *script = getScriptApi<ScriptApiAsync>(L);

	luaL_checktype(L, 3, LUA_TTABLE);
	luaL_checktype(L, 4, LUA_TSTRING);

	LuaAsyncJob *o = checkObject<LuaAsyncJob>(L, 1);
	u32 id = o->get_id();
	auto serialized_func = get_serialized_function(L, 2);
	PackedValue *param = script_pack(L, 3);
	std::string mod_origin = readParam<std::string>(L, 4);

	u32 jobId = script->replaceAsync(id,
		std::move(serialized_func),
		param, mod_origin);

	lua_settop(L, 0);
	create(L, jobId);
	return 1;
}

int LuaAsyncJob::create(lua_State *L, const int id)
{
	NO_MAP_LOCK_REQUIRED;
	LuaAsyncJob *o = new LuaAsyncJob(id);
	*(void **)(lua_newuserdata(L, sizeof(void *))) = o;
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);
	return 1;
}

void LuaAsyncJob::Register(lua_State *L)
{
	static const luaL_Reg metamethods[] = {
		{"__gc", gc_object},
		{0, 0}
	};

	registerClass(L, className, methods, metamethods);

	// Expose __index to be complemented by Lua.
	lua_getglobal(L, "core");
	luaL_getmetatable(L, className);
	lua_getfield(L, -1, "__index");
	lua_setfield(L, -3, "async_job_methods");
}

const char LuaAsyncJob::className[] = "AsyncJob";
const luaL_Reg LuaAsyncJob::methods[] = {
	luamethod(LuaAsyncJob, get_id),
	luamethod(LuaAsyncJob, replace),
	{0, 0}
};


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
	LuaAsyncJob::create(L, jobId);
	return 1;
}

void ModApiAsync::Initialize(lua_State *L, int top)
{
	API_FCT(do_async_callback);
}
