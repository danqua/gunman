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
        "extern/glad-3.3/src/glad.c",

        "extern/imgui-docking/imgui.cpp",
        "extern/imgui-docking/imgui_demo.cpp",
        "extern/imgui-docking/imgui_draw.cpp",
        "extern/imgui-docking/imgui_tables.cpp",
        "extern/imgui-docking/imgui_widgets.cpp",
        "extern/imgui-docking/backends/imgui_impl_sdl3.cpp"
        "extern/imgui-docking/backends/imgui_impl_opengl3.cpp"
    }
    includedirs {
        "src",
		"extern/SDL3-3.2.8/include",
        "extern/glad-3.3/include",
        "extern/glm-1.0.1-light",
        "extern/stb-master",
        "extern/nlohmann/single_include",

        "extern/imgui-docking",
        "extern/imgui-docking/backends/",
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
