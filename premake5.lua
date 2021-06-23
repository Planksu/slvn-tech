workspace "slvn-tech"
	configurations {"Debug", "Release"}
	platforms { "Win32", "Win64"}
	targetdir "./builds/%{cfg.buildcfg}/%{cfg.platform}"
	objdir "./builds/obj/%{cfg.buildcfg}/%{cfg.platform}"
	location "./project-files"
	
	filter { "platforms:Win32" }
		architecture "x32"
		system "Windows"

	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"

	libdirs {
		"../builds/%{cfg.buildcfg}/%{cfg.platform}"
	}
	
	configuration "Debug"
		defines {
			"DEBUG"
		}
	
	configuration "Release"
		defines {
			"NDEBUG"
		}
	
	configuration "macosx"	
		platforms { "x64" }
	configuration "not macosx"
		platforms { "x86", "x64" }

	configuration "windows"
		characterset "MBCS"
		systemversion "10.0.18362.0"
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"WIN32_LEAN_AND_MEAN"
		}

project "slvn-tech"
	targetname "slvn-tech"
	kind "ConsoleApp"
	language "C++"
	files { "./slvn-tech/include/*.h", "./slvn-tech/src/*.cc" }

	defines {}
	links {}
	configuration "x64"
		libdirs {}
		
	configuration "x86"
		libdirs {}
		
	configuration "not macosx"
		includedirs {"./slvn-tech/include"}
	configuration "macosx"

	configuration "Debug"
		defines {"DEBUG"}
		symbols "On"	

	configuration "Release"
		defines {"NDEBUG"}
		optimize "On"

