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
		}

project "slvn-tech"
	targetname "slvn-tech"
	kind "ConsoleApp"
	language "C++"
	flags { "MultiProcessorCompile" }
	files { "./slvn-tech/include/*.h",
			"./slvn-tech/include/*.inl",
			"./slvn-tech/include/abstract/*.h",
			"./slvn-tech/src/*.cc"}

	defines { "VK_USE_PLATFORM_WIN32_KHR" }
	links { "vulkan-1.lib", "glfw3.lib" }
	configuration "x64"
		libdirs {}
		
	configuration "x86"
		libdirs {}
		
	configuration "not macosx"
		includedirs {	"./slvn-tech/include",
						"$(VULKAN_SDK)/include",
						"./slvn-tech/VULKAN_SDK/include",
						"./slvn-tech/dependencies/glfw/include",
						"./slvn-tech/dependencies/glm/",
						"./slvn-tech/dependencies/OBJ-Loader/include",
						"./VULKAN_SDK/include"}
		libdirs { 	"$(VULKAN_SDK)/lib",
					"./slvn-tech/VULKAN_SDK/lib",
					"./VULKAN_SDK/lib",
					"./src/Debug",
					"../slvn-tech-local-dependencies/glfw/precompiled/"}
		cppdialect "C++17"
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
	flags { "MultiProcessorCompile" }
	files { "./slvn-tech/src/test/*.cpp",
			"./slvn-tech/include/test/*.h"}

	defines {}
	links { "vulkan-1.lib",
			"glfw3.lib",
			"slvn-tech.lib",
			"gtest_maind.lib", 
			"gtestd.lib",
			"gmock_maind.lib",
			"gmockd.lib"
			}
	configuration "x64"
		libdirs {}
		
	configuration "x86"
		libdirs {}
		
	configuration "not macosx"
		includedirs {	"./slvn-tech/include",
						"./slvn-tech/include/test",
						"$(VULKAN_SDK)/include",
						"./slvn-tech/VULKAN_SDK/include",
						"./slvn-tech/dependencies/glfw/include",
						"./slvn-tech/dependencies/glm/",
						"./slvn-tech/dependencies/OBJ-Loader/include",
						"./VULKAN_SDK/include",
						"./googletest/googletest/include",
						"./googletest/googlemock/include"}
						
		libdirs { 	"$(VCInstallDir)UnitTest/lib",
					"$(VULKAN_SDK)/lib",
					"./slvn-tech/VULKAN_SDK/lib", 
					"./VULKAN_SDK/lib", 
					"./builds/Debug/x64",
					"./builds/Release/x64",
					"./src/Debug",
					"./googletest/lib/Debug",
					"../slvn-tech-local-dependencies/glfw/precompiled"}
		cppdialect "C++17"
	configuration "macosx"

	configuration "Debug"
		defines {"DEBUG"}
		symbols "On"	

	configuration "Release"
		defines {"NDEBUG"}
		optimize "On"



