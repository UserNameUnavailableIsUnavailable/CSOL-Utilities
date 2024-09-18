if (not Stack_lua)
then
    Stack_lua = true
    Stack = {}
    Stack.size = 0
    Stack.capacity = 4096
    ---栈顶指针
    ---@type integer
    Stack.rsp = 4096
    ---实际存放数据的存储区
    ---@type any[]
    Stack.storage = {}
    function Stack:push(element)
        if (self.rsp == 0)
        then
            error("Stack overflow", 2)
            return
        end
        self.rsp = self.rsp - 1
        self.storage[self.rsp] = element
    end
    function Stack:pop()
        if (self.rsp == 4096)
        then
            error("Stack empty", 1)
            return nil
        end
        self.rsp = self.rsp + 1
        return self.storage[self.rsp - 1]
    end
    function Stack:new()

    end

end -- Stack_lua