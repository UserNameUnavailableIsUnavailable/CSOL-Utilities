# For ABI compatibility and proper testing environment, always run Lua from this script.

# $Env:LUA_PATH = luarocks path --lr-path --full
# $Env:LUA_CPATH = luarocks path --lr-cpath --full

$Env:LUA_INIT = 'package.cpath = ".\\?.dll;" .. package.cpath'
$lua = "$PSScriptRoot/dependencies/bin/lua-5.4.0/lua.exe"
& $lua $args