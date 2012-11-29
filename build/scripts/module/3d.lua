if (not _OPTIONS["united"]) then
	project "Nazara3D"
end

files
{
	"../include/Nazara/3D/**.hpp",
	"../include/Nazara/3D/**.inl",
	"../src/Nazara/3D/**.hpp",
	"../src/Nazara/3D/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/3D/Posix/*.hpp", "../src/Nazara/3D/Posix/*.cpp" }
else
	excludes { "../src/Nazara/3D/Win32/*.hpp", "../src/Nazara/3D/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/3D/Debug/Leaks.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraUtility-s-d"
		links "NazaraRenderer-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"
		links "NazaraRenderer-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraUtility-d"
		links "NazaraRenderer-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
		links "NazaraRenderer"
end