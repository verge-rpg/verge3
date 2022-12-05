vx = {}

-- Returns whether or not a file exists.
function FileExists(filename)
    local f = io.open(filename, "r")
    if f == nil then
        return false
    end
    f:close()
    return true
end

-- Flattening function for tables!
function table.flatten(s, t)
    t = t or {}
    if s then
        for idx, value in ipairs(s) do
            if type(value) == "table" then
                table.flatten(value, t)
            else
                t[#t + 1] = value
            end
        end
    end
    return t
end

-- Loads a folder of module files into a single namespace.
function namespace(name, modules, import_debug)
    local ns = _G[name] or {}
    ns._name = name
    import_debug = import_debug or true --false
    
    modules = table.flatten(modules)
    -- Remember globals the before we overwrite them
    local last_value = {}
    for idx, module in ipairs(modules) do
        last_value[module] = _G[module]
    end
    
    -- Load each module required
    -- Each module must use only locals.
    for idx, module in ipairs(modules) do
        local filename = ns._name .. "/" .. module
        local ret, err = pcall(function() require(filename) end)
        if ret then
            if import_debug then
                print(filename .. " imported")
            end
        else
            if import_debug then
                print(filename .. " has error on requirement " .. tostring(err))
            end
        end
    end

    -- Forces explicit package naming on all classes, and restores whatever the globals were before this namespace. 
    for idx, module in ipairs(modules) do
        ns[module] = _G[module]
        _G[module] = last_value[module]
    end
    
    local meta = {}
    meta.__tostring =   function(self)
                    return "namespace \"" .. self._name .. "\" "
                end
    setmetatable(ns, meta)
    _G[name] = ns
    return ns
end



function table.reverse(t)
    local result = {}
    for idx, value in ipairs(t) do
        table.insert(result, 1, value)
    end
    return result
end

-- Shoves all of a namespace's stuff into the global namespace
function import(name, attributes)
    local ns = _G[name]
    if attributes then
        -- Import all attributes that are named
        for idx, attribute in ipairs(attributes) do
            _G[attribute] = ns[attribute]
        end
    else
        -- If no attributes are passed, import everything.
        for k, v in pairs(ns) do
            -- Things that start with _ are considered private.
            if not attributes and not k:startsWith("_") then
                _G[k] = v
            end
        end
    end
end

-- Like tonumber, except it returns 0 instead of nil on fail.
function num(x)
    return tonumber(x) or 0
end

-- not not x forces x to become a boolean!
function bool(x)
    return not not x
end

-- Setup base Object class for class system.
do
    local o = { class_name = 'Object', __init = function(self) end }
    local meta = {}
    function meta:__call()
        local t = { class_name = o.class_name }
        return t
    end
    
    vx.Object = o
    setmetatable(vx.Object, meta)
end

-- Create a global class given a name string and a parent class reference.
function vergeclass(name, parent)
    local cls = {}
    local meta = {}
    local getter = {}
    local setter = {}
    
    cls.class_name = name
    cls.parent = parent or vx.Object
    
    function cls:__index(key)
        local f = getter[key]
        return (f and f(self)) or cls[key] or rawget(self, key)
    end
    
    function cls:__newindex(key, value)
        local f = setter[key]
        if key == 'parent' then
            error('The parent class may not be modified after construction.', 2)
        elseif f then
            f(self, value)
        elseif cls[key] then
            cls[key] = value
        else
            rawset(self, key, value)
        end
        
    end
    
    -- A helper for creating getter/setter things
    function cls._property(key, get, set)
        getter[key] = get
        setter[key] = set
    end
    
    -- An indexer on the class that looks up its parents on fallback
    function meta:__index(key)
        return rawget(self, key) or rawget(self, 'parent')[key]
    end

    local function construct(cls, obj, arg)
        local called_super = (cls.parent == vx.Object)
        local function call_super(...)
            local p = cls.parent
            
            called_super = true
            t = { ... }
            if p then
                construct(p, obj, t)
            else
                -- Invoking super in class where not necessary, whatever...
                return
            end
        end
    
        -- Make 'super' a temporary global to mean the constructor of the parent class.
        local tmp = rawget(_G, 'super')
        rawset(_G, 'super', call_super)

        if arg then
            cls.__init(obj, unpack(arg))
        end
        
        -- Restore old 'super' meaning.
        rawset(_G, 'super', tmp)
        
        -- Yell if the super constructor wasn't called
        -- for classes that don't subclass vx.Object
        if not called_super then
            error("super() must be invoked in constructor for class '" .. cls.class_name .. "'.")
        end
    end

    
    -- The external constructor call.
    function meta:__call(...)
        -- Create a base table
        local obj = {}
        -- Setup class information
        setmetatable(obj, cls)
        
        t = { ... }
        construct(cls, obj, t)
        
        -- Return the finished constructor
        return obj
    end
    
    setmetatable(cls, meta)
    
    -- Shove into global namespace
    _G[name] = cls
    
    -- For convenience so you can write the Python-like: class 'Entity'(Sprite)
    -- instead of the lamer class('Entity', Sprite)
    -- Treat class('Entity', Sprite)(OtherThing) by ignoring last term.
    return function(p) cls.parent = parent or p or vx.Object end
end

require "vx.ToString"
require "vx.String"
require "vx.System"

-- Our verge lua package
namespace("vx", {
            "Image", "Font", "Socket",
            { "Sound", "SoundChannel" },
            "Song",
            { "_vergeMap", "_vergeMapLayer", "_vergeMapZone" },
            "Entity",
            "File",
            "_vergeCamera",
            "_vergeMouseInput",
            "_vergeMouseButtonInput",
            "_vergeKeyboardInput",
            "_vergeJoystickInput",
            "_vergeJoystickDirectionInput",
            "_vergeJoystickButtonInput",
            "_vergeClock",
            "_vergeMusic"
    }
)
-- Debug flags
vx._DEBUG = false
vx._DEBUG_PRINT_GLOBALS = false

require "vx.Color"
require "vx.Lucent"
require "vx.ColorFilter"
require "vx.Input"

-- A bunch of functions wrapped in the verge namespace.
function vx.Render()
    v3.Render()
end

function vx.ShowPage()
    v3.ShowPage()
    collectgarbage("step")
end

function vx.UpdateControls()
    v3.UpdateControls()
end

function vx.CallFunction(name, ...)
    if _G[name] then
        _G[name](...)
    end
end

function vx.HookRetrace(name)
    v3.HookRetrace(name)
end

function vx.Random(low, high)
    return v3.Random(num(low), num(high))
end

-- Verge's default handles
vx.default_font = vx.Font(0)
vx.screen = vx.Image(v3.screen)


-- The Map object, for map stuff!
vx.map = vx._vergeMap()
-- The Camera object, for cameratracking and cameratracker junk
vx.camera = vx._vergeCamera()
-- The Clock object, for systemtime and timer
vx.clock = vx._vergeClock()
-- The Music object, for the lame "simple" music system.
vx.music = vx._vergeMusic()

if verge ~= nil then
    print("V3 namespace initialized successfully.\n")
    if vx._DEBUG then
        ObjectAttributesToLog(verge)
    end
    if vx._DEBUG_PRINT_GLOBALS then
        print "\nGlobals!"
        for k, v in pairs(_G) do
            print("\t" .. k .. " = " .. tostring(v))
        end
    end
end
