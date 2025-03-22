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
    cppdialect "C++11"
    debugdir "data"
    targetdir "build/%{cfg.buildcfg}"
    objdir "build/%{cfg.buildcfg}/obj"
    files {
        "src/**.h",
        "src/**.cpp"
    }
    includedirs {
        "src",
		"extern/SDL3-3.2.8/include"
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
