TOOL.Name = "SDK"

TOOL.CopyTargetToExampleDir = true

TOOL.Directory = "../SDK/lib"
TOOL.Kind = "Library"

TOOL.Defines = {
	"NDK_BUILD"
}

TOOL.Includes = {
	"../SDK/include",
	"../SDK/src"
}

TOOL.Files = {
	"../SDK/include/NDK/**.hpp",
	"../SDK/include/NDK/**.inl",
	"../SDK/src/NDK/**.hpp",
	"../SDK/src/NDK/**.inl",
	"../SDK/src/NDK/**.cpp"
}

TOOL.Libraries = function()
    local libraries = {}
    for k,v in pairs(NazaraBuild.Modules) do
        table.insert(libraries, "Nazara" .. v.Name)
    end
    
    return libraries
end