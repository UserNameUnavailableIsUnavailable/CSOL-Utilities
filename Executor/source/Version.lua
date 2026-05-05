if not __VERSION_LUA__ then
    __VERSION_LUA__ = true
    local __version__ = "1.5.2"
    
    ---@class Version
    ---@field private records table<string, string>
    ---@field private requirements function[]
    Version = {
        records = {
            Version = __version__,
        },
        requirements = {},
    }
    ---检查版本是否处于 [lowest, highest] 范围内，nil 表示不做限制。
    ---@param version string | nil 配置文件版本号
    ---@param lowest string | nil
    ---@param highest string | nil
    ---@return boolean
    function Version:check(version, lowest, highest)
        if not version and (lowest or highest) then
            return false
        end
        if version and lowest then
            if version < lowest then
                return false
            end
        end
        if version and highest then
            if version > highest then
                return false
            end
        end
        return true
    end

    ---设定指定模块的版本号。
    ---@param module_name string 模块名称
    ---@param version string 版本号
    ---@return boolean
    function Version:set(module_name, version)
        self.records[module_name] = version
        return true
    end

    ---获取指定模块的版本号。
    ---@param module_name string 模块名称
    ---@return string
    function Version:get(module_name)
        return self.records[module_name]
    end

    local fmt_1 =
        [[断言失败：%s 模块（当前版本为 %s）要求 %s 模块（当前版本为 %s）版本不低于 %s 且不高于 %s。]]
    local fmt_2 =
        [[断言失败：%s 模块（当前版本为 %s）要求 %s 模块（当前版本为 %s）版本不低于 %s。]]
    local fmt_3 =
        [[断言失败：%s 模块（当前版本为 %s）要求 %s 模块（当前版本为 %s）版本不高于 %s。]]

    ---版本要求，module 要求 required_module 具有指定版本。
    ---@param module string 模块名称
    ---@param required_module string 要求具有指定版本的模块名称
    ---@param lowest string | nil 最低版本限制
    ---@param highest string | nil 最低版本限制
    function Version:require(module, required_module, lowest, highest)
        if lowest and highest then
            self.requirements[#self.requirements + 1] = function()
                assert(
                    self:check(self:get(required_module), lowest, highest),
                    fmt_1:format(module, self:get(module), required_module, self:get(required_module), lowest, highest)
                )
            end
        elseif lowest then
            self.requirements[#self.requirements + 1] = function()
                assert(
                    self:check(self:get(required_module), lowest, highest),
                    fmt_2:format(module, self:get(module), required_module, self:get(required_module), lowest)
                )
            end
        elseif highest then
            self.requirements[#self.requirements + 1] = function()
                assert(
                    self:check(self:get(required_module), lowest, highest),
                    fmt_3:format(module, self:get(module), required_module, self:get(required_module), highest)
                )
            end
        end
    end

    function Version:assert()
        Console:info(
            "执行版本断言检查。如检查不通过，请按照手册中更新步骤进行更新。"
        )
        for _, value in ipairs(self.requirements) do
            value()
        end
        Console:info("版本断言检查通过。")
        self.requirements = {} -- 通过，释放空间
    end
end -- __VERSION_LUA__
