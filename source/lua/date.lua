
TimeDate = {}
---Judge whether `year` is leap 
---@param year number
---@return boolean
function TimeDate:is_leap(year)
	if year % 4 == 0 and year % 100 ~= 0 then
		return true
	elseif year % 400 == 0 then
		return true
	end
	return false
end

---Calculate timestamp based on given time (UTC+0)
---@param year number
---@param month number
---@param day number
---@param hour number
---@param minute number
---@param second number
---@return number
function TimeDate:timestamp(year, month, day, hour, minute, second, time_zone)
	time_zone = time_zone or 0
	local total_days = 0
	local ret = 0
	local i = 1970
	while i < year do
		if is_leap(i) then
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
	if month > 2 and is_leap(year) then
		total_days = total_days + 1
	end
	total_days = total_days + day - 1
	ret = total_days * 86400 + hour * 3600 + minute * 60 + second
	return ret
end