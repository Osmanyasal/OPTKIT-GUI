---@diagnostic disable: undefined-global, lowercase-global

local function trim(value)
    return (value:gsub("^%s+", ""):gsub("%s+$", ""))
end

local function get_command_output(command)
    local ok, output = pcall(os.outputof, command)
    if not ok or not output then
        return ""
    end

    return trim(output)
end

local function get_optkit_version()
    local revision = get_command_output('git -C "lib/OPTKIT" rev-parse --short=12 HEAD 2>/dev/null')
    if revision == "" then
        return "unknown"
    end

    local dirty = get_command_output('git -C "lib/OPTKIT" status --short 2>/dev/null')
    if dirty ~= "" then
        return revision .. "-dirty"
    end

    return revision
end

local optkit_version = get_optkit_version()

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
    defines {
        'OPTKIT_GUI_OPTKIT_VERSION="' .. optkit_version .. '"',
    }

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
        libdirs {
            "lib/OPTKIT/bin/Release",
        }
        links {
            "optkit_dynamic",
            "GL",
            "dl",
            "png16",
            "pthread",
            "X11",
            "Xrandr",
            "Xi",
            "Xxf86vm",
            "Xcursor",
            "Xinerama",
        }
        linkoptions {
            "-Wl,-rpath,lib/OPTKIT/bin/Release",
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
            "lib/OPTKIT/src/optkit.hh"
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
