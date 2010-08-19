-- This test program ensures that the modules are loaded without polluting
-- the global namespace.  It can't be part of the main test suite because
-- by the time the code runs the modules will already have been loaded.

require "test-setup"

local orig_globals = {}
for key in pairs(_G) do orig_globals[key] = true end

local Cairo = require "oocairo"

for key in pairs(_G) do
    if not orig_globals[key] then
        error("namespace pollution loading module:" ..
              " global '" .. key .. "' created by lib")
    end
end
for key in pairs(orig_globals) do
    if _G[key] == nil then
        error("namespace pollution loading module:" ..
              " global '" .. key .. "' deleted by lib")
    end
end

-- vi:ts=4 sw=4 expandtab
