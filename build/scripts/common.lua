-- Configuration g�n�rale
configurations 
{
--	"DebugStatic",
--	"ReleaseStatic",
	"DebugDLL",
	"ReleaseDLL"
}

defines "NAZARA_BUILD"
language "C++"
location(_ACTION)

includedirs
{
	"../include",
	"../src/",
	"../extlibs/include"
}

libdirs "../lib"

if (_OPTIONS["x64"]) then
	libdirs "../extlibs/lib/x64"
end

libdirs "../extlibs/lib/x86"

targetdir "../lib"

configuration "Debug*"
	defines "NAZARA_DEBUG"
	flags "Symbols"

configuration "Release*"
	flags { "EnableSSE", "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

-- Activation du SSE c�t� GCC
configuration { "Release*", "codeblocks or codelite or gmake or xcode3*" }
	buildoptions "-mfpmath=sse"

configuration "*Static"
	defines "NAZARA_STATIC"
	kind "StaticLib"

configuration "*DLL"
	kind "SharedLib"

configuration "DebugStatic"
	targetsuffix "-s-d"

configuration "ReleaseStatic"
	targetsuffix "-s"

configuration "DebugDLL"
	targetsuffix "-d"

configuration "codeblocks or codelite or gmake or xcode3*"
	buildoptions "-std=c++11"

configuration { "linux or bsd or macosx", "gmake" }
	buildoptions "-fvisibility=hidden"
