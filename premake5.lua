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
	files { "./slvn-tech/include/*.h",
			"./slvn-tech/include/abstract/*.h",
			"./slvn-tech/src/*.cc",
			"./slvn-tech/src/abstract/*.cc"}

	defines {}
	links { "vulkan-1.lib", "glfw3.lib" }
	configuration "x64"
		libdirs {}
		
	configuration "x86"
		libdirs {}
		
	configuration "not macosx"
		includedirs {	"./slvn-tech/include",
						"./slvn-tech/include/*",
						"$(VULKAN_SDK)/include",
						"./slvn-tech/VULKAN_SDK/include",
						"./slvn-tech/dependencies/glfw/include",
						"./slvn-tech/dependencies/glm/glm",
						"./VULKAN_SDK/include",}
		libdirs { "$(VULKAN_SDK)/lib", "./slvn-tech/VULKAN_SDK/lib", "./VULKAN_SDK/lib", "./src/Debug" }
	configuration "macosx"

	configuration "Debug"
		defines {"DEBUG"}
		symbols "On"	

	configuration "Release"
		defines {"NDEBUG"}
		optimize "On"
		
project "slvn-tech-unittest"
	targetname "slvn-tech-unittest"
	kind "ConsoleApp"
	language "C++"
	files { "./slvn-tech/src/test/*.cpp",
			"./slvn-tech/include/test/*.h"}

	defines {}
	links { "vulkan-1.lib", "glfw3.lib", "slvn_render_engine.obj", "slvn_abstract_engine.obj", "slvn_debug.obj", "gtest_maind.lib", "gtestd.lib"}
	configuration "x64"
		libdirs {}
		
	configuration "x86"
		libdirs {}
		
	configuration "not macosx"
		includedirs {	"./slvn-tech/include",
						"./slvn-tech/include/*",
						"$(VULKAN_SDK)/include",
						"./slvn-tech/VULKAN_SDK/include",
						"./slvn-tech/dependencies/glfw/include",
						"./slvn-tech/dependencies/glm/glm",
						"./VULKAN_SDK/include",
						"./googletest/googletest/include"}
						
		libdirs { 	"$(VCInstallDir)UnitTest/lib",
					"$(VULKAN_SDK)/lib",
					"./slvn-tech/VULKAN_SDK/lib", 
					"./VULKAN_SDK/lib", 
					"./builds/obj/Debug/x64/x64/Debug/slvn-tech", 
					"./src/Debug",
					"./googletest/lib/Debug"}
	configuration "macosx"

	configuration "Debug"
		defines {"DEBUG"}
		symbols "On"	

	configuration "Release"
		defines {"NDEBUG"}
		optimize "On"



