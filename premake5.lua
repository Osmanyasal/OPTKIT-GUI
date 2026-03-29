---@diagnostic disable: undefined-global, lowercase-global

workspace "OPTKIT-GUI"
    configurations { "rebug", "release" }
    architecture "x86_64"
    startproject "OPTKIT-GUI"

    -- Output directories
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "OPTKIT-GUI"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir ("bin/%{outputdir}")
    objdir ("bin/obj/%{outputdir}")

    -- Source files
    files {
        "src/**.cpp",
        "src/**.h",

        -- Dear ImGui core
        "lib/imgui/imgui.cpp",
        "lib/imgui/imgui_draw.cpp",
        "lib/imgui/imgui_tables.cpp",
        "lib/imgui/imgui_widgets.cpp",
        "lib/imgui/imgui_demo.cpp",

        -- Dear ImGui backends (GLFW + OpenGL3)
        "lib/imgui/backends/imgui_impl_glfw.cpp",
        "lib/imgui/backends/imgui_impl_opengl3.cpp",

        -- ImPlot
        "lib/implot/implot.cpp",
        "lib/implot/implot_items.cpp",
        "lib/implot/implot_demo.cpp",
    }

    -- Include directories
    includedirs {
        "src",
        "lib/imgui",
        "lib/imgui/backends",
        "lib/implot",
        "lib/glfw/include",
        "lib/OPTKIT/src",
    }

    -- GLFW build from source
    filter "system:linux"
        defines { "_GLFW_X11" }
        links {
            "GL",
            "dl",
            "pthread",
            "X11",
            "Xrandr",
            "Xi",
            "Xxf86vm",
            "Xcursor",
            "Xinerama",
        }

        -- Compile GLFW sources directly
        files {
            "lib/glfw/src/glfw_config.h",
            "lib/glfw/src/context.c",
            "lib/glfw/src/init.c",
            "lib/glfw/src/input.c",
            "lib/glfw/src/monitor.c",
            "lib/glfw/src/vulkan.c",
            "lib/glfw/src/window.c",
            "lib/glfw/src/platform.c",
            "lib/glfw/src/null_init.c",
            "lib/glfw/src/null_joystick.c",
            "lib/glfw/src/null_monitor.c",
            "lib/glfw/src/null_window.c",
            "lib/glfw/src/x11_init.c",
            "lib/glfw/src/x11_monitor.c",
            "lib/glfw/src/x11_window.c",
            "lib/glfw/src/xkb_unicode.c",
            "lib/glfw/src/posix_module.c",
            "lib/glfw/src/posix_poll.c",
            "lib/glfw/src/posix_time.c",
            "lib/glfw/src/posix_thread.c",
            "lib/glfw/src/glx_context.c",
            "lib/glfw/src/egl_context.c",
            "lib/glfw/src/osmesa_context.c",
            "lib/glfw/src/linux_joystick.c",
        }

    -- Configuration-specific settings
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        runtime "Release"

    -- Custom clean action to remove bin directory
    newaction {
        trigger     = "clean",
        description = "Remove all generated files and bin directory",
        execute     = function ()
            print("Removing bin directory...")
            os.rmdir("bin")
            print("Removing Makefile and *.make files...")
            os.remove("Makefile")
            local files = os.matchfiles("*.make")
            for _, f in ipairs(files) do
                os.remove(f)
            end
        end
    }
