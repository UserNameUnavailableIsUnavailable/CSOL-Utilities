if (not DateTime_lua)
then
DateTime_lua = true
---@class DateTime
---@field TIME_ZONE number 时区
DateTime = {}

DateTime.TIME_ZONE = 8

---判断 `year` 是否为闰年。
---@param year integer 
---@return boolean
function DateTime:is_leap(year)
	if year % 4 == 0 and year % 100 ~= 0 then
		return true
	elseif year % 400 == 0 then
		return true
	end
	return false
end

---根据给定的日期和时间，计算的 UNIX 时间戳
---@param year integer
---@param month integer 
---@param day integer
---@param hour integer 
---@param minute integer
---@param second integer 
---@param time_zone number | nil 时区，默认为 `0`
---@return integer
function DateTime:timestamp(year, month, day, hour, minute, second, time_zone)
	time_zone = time_zone or 0
	local total_days = 0
	local ret = 0
	local i = 1970
	while i < year do
		if self:is_leap(i) then
			total_days = total_days + 366
		else
			total_days = total_days + 365
		end
		i = i + 1
	end
	i = 1
	while i < month do
		if i == 1 or i == 3 or i == 5 or i == 7 or i == 8 or i == 10 or i == 12 then
			total_days = total_days + 31
		elseif i == 2 then
			total_days = total_days + 28
		else
			total_days = total_days + 30
		end
		i = i + 1
	end
	if month > 2 and self:is_leap(year) then
		total_days = total_days + 1
	end
	total_days = total_days + day - 1
	ret = total_days * 86400 + hour * 3600 + minute * 60 + second
	ret = ret - time_zone * 3600
	return ret
end

---设定时区。
---@param tz number
function DateTime:set_time_zone(tz)
	if (not tz)
	then
		return
	end
	self.TIME_ZONE = tz
end

function DateTime:get_time_zone()
	return self.TIME_ZONE
end

---获取操作系统本地时间，并将其转换为 UNIX 时间戳。
---@return integer # 时间戳
function DateTime:get_local_timestamp()
	local year = tonumber(GetDate("%Y"))
	local month = tonumber(GetDate("%m"))
	local day = tonumber(GetDate("%d"))
	local hour = tonumber(GetDate("%H"))
	local minute = tonumber(GetDate("%M"))
	local second = tonumber(GetDate("%S"))
	return self:timestamp(
		year--[[@as integer]],
		month--[[@as integer]],
		day--[[@as integer]],
		hour--[[@as integer]],
		minute--[[@as integer]],
		second--[[@as integer]],
		self.TIME_ZONE
	)
end


end -- DateTime_lua