if (not StateMachine_lua)
then
    StateMachine_lua = true
    StateMachine = {}

    function StateMachine:new()
        self.rsp = 4096
        self.stack = {}
        self.heap = {}
    end

end -- StateMachine_lua