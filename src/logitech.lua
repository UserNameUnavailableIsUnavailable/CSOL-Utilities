-- Module Name: logitech.lua 
-- This module encapsulates Logitech Lua APIs

logitech = {}

logitech.DateTime = {}
logitech.Keyboard = {}
logitech.Mouse = {}

logitech.Sleep = Sleep
logitech.OutputLogMessage = OutputLogMessage
logitech.GetRunningTime = GetRunningTime
logitech.GetDate = GetDate
logitech.ClearLog = ClearLog
logitech.PressKey = PressKey
logitech.ReleaseKey = ReleaseKey
logitech.PressAndReleaseKey = PressAndReleaseKey
logitech.IsModifierPressed = IsModifierPressed
logitech.IsMouseButtonPressed = IsMouseButtonPressed
logitech.MoveMouseTo = MoveMouseTo

return logitech