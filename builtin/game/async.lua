
core.async_jobs = {}

function core.async_event_handler(jobid, retval)
	local callback = core.async_jobs[jobid]
	assert(type(callback) == "function")
	callback(unpack(retval, 1, retval.n))
	core.async_jobs[jobid] = nil
end

local function prepare_async_args(func, callback, ...)
	assert(type(func) == "function" and type(callback) == "function",
		"Invalid invocation of minetest.handle_async or minetest.replace_async")
	local args = {n = select("#", ...), ...}
	local mod_origin = core.get_last_run_mod()

	return func, args, mod_origin
end

function core.handle_async(func, callback, ...)
	local jobid = core.do_async_callback(prepare_async_args(func, callback, ...))
	core.async_jobs[jobid] = callback

	return jobid
end

function core.replace_async(jobid, func, callback, ...)
	assert(type(jobid) == "number",
		"Invalid jobid for minetest.replace_async")

	local newid = core.replace_async_callback(jobid, prepare_async_args(func, callback, ...))
	core.async_jobs[newid] = callback
	return newid
end

local dummy = function() end
function core.cancel_async(jobid)
	return jobid == core.replace_async(jobid, dummy, dummy)
end
