if (not TimeDate)
then

---@class TimeDate
TimeDate = {}

---判断 `year` 是否为闰年。
---@param year integer 
---@return boolean
function TimeDate:is_leap(year)
	if year % 4 == 0 and year % 100 ~= 0 then
		return true
	elseif year % 400 == 0 then
		return true
	end
	return false
end

---计算给定时间的 UNIX 时间戳。
---@param year integer
---@param month integer 
---@param day integer
---@param hour integer 
---@param minute integer
---@param second integer 
---@return integer
function TimeDate:timestamp(year, month, day, hour, minute, second, time_zone)
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
	return ret
end

end -- if (not TimeDate)