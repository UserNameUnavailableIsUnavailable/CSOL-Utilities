local functions = {
    "PressAndReleaseMouseButton",
    "PressAndReleaseHidKey",
    "PressAndReleaseKey",
    "PlayMacro",
    "EnablePrimaryMouseButtonEvents",
    "SetMouseDPITableIndex",
    "SetMouseDPITable",
    "GetMouseSpeed",
    "ReleaseHidKey",
    "DecrementMouseSpeed",
    "IncrementMouseSpeed",
    "SetBacklightColor",
    "PressMouseButton",
    "ReleaseKey",
    "SetMKeyState",
    "IsModifierPressed",
    "GetMKeyState",
    "AbortMacro",
    "ReleaseMacro",
    "GetRunningTime",
    "GetDate",
    "Sleep",
    "MoveMouseTo",
    "OutputDebugMessage",
    "PressMacro",
    "GetMousePosition",
    "PressHidKey",
    "MoveMouseToVirtual",
    "MoveMouseRelative",
    "MoveMouseWheel",
    "IsMouseButtonPressed",
    "SetMouseSpeed",
    "ClearLog",
    "OutputLogMessage",
    "IsKeyLockOn",
    "ReleaseMouseButton",
    "PressKey",
};

local Logitech = {}

for _, name in ipairs(functions) do
    if _G[name] ~= nil then
        Logitech[name] = _G[name]
        _G[name] = nil
    end
end

if Logitech.OutputLogMessage ~= nil then
    function Logitech.OutputMessage(message)
        Logitech.OutputLogMessage("%s", message)
    end
end

return Logitech