if (not _OPTIONS["united"]) then
	project "NazaraUtility"
end

files
{
	"../include/Nazara/Utility/**.hpp",
	"../include/Nazara/Utility/**.inl",
	"../src/Nazara/Utility/**.hpp",
	"../src/Nazara/Utility/**.cpp"
}

links "freetype-s"

if (os.is("windows")) then
	excludes { "../src/Nazara/Utility/Posix/*.hpp", "../src/Nazara/Utility/Posix/*.cpp" }
	links "gdi32"
else
	excludes { "../src/Nazara/Utility/Win32/*.hpp", "../src/Nazara/Utility/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/Utility/Debug/NewOverload.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"

	configuration "DebugDLL"
		links "NazaraCore-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
end

configuration "Debug*"
	links "stb_image-s-d"

configuration "Release*"
	links "stb_image-s"
