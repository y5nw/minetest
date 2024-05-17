
core.async_jobs = {}

function core.async_event_handler(jobid, retval)
	local callback = core.async_jobs[jobid]
	assert(type(callback) == "function")
	callback(unpack(retval, 1, retval.n))
	core.async_jobs[jobid] = nil
end

local function prepare_async_args(func, callback, ...)
	assert(type(func) == "function" and type(callback) == "function",
		"Invalid invocation of minetest.handle_async or AsyncJob:replace")
	local args = {n = select("#", ...), ...}
	local mod_origin = core.get_last_run_mod()

	return func, args, mod_origin
end

function core.handle_async(func, callback, ...)
	local job = core.do_async_callback(prepare_async_args(func, callback, ...))
	core.async_jobs[job:get_id()] = callback

	return job
end

if core.async_job_methods then
	local replace_job = core.async_job_methods.replace
	function core.async_job_methods:replace(func, callback, ...)
		local newjob = replace_job(self, prepare_async_args(func, callback, ...))
		core.async_jobs[newjob:get_id()] = callback
		return newjob
	end

	local dummy = function() end
	function core.async_job_methods:cancel()
		return self:get_id() == self:replace(dummy, dummy):get_id()
	end

	core.async_job_methods = nil
end
