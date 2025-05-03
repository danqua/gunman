workspace "Gunman"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "Gunman"
    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "Gunman"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    debugdir "data"
    targetdir "build/%{cfg.buildcfg}"
    objdir "build/%{cfg.buildcfg}/obj"
    files {
        "src/**.h",
        "src/**.cpp",
        "extern/glad-3.3/src/glad.c"
    }
    includedirs {
        "src",
		"extern/SDL3-3.2.8/include",
        "extern/glad-3.3/include",
        "extern/glm-1.0.1-light",
        "extern/stb-master"
    }
    
    filter "system:windows"
		defines {
			"WIN32"
		}
		libdirs {
			"extern/SDL3-3.2.8/lib/x64"
		}
		links {
            "SDL3"
		}
        postbuildcommands {
            "{COPY} extern/SDL3-3.2.8/lib/x64/SDL3.dll data"
        }
