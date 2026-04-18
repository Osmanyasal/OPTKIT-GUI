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

local function dynamic_lib_exists(libname)
    local pipe = io.popen("ldconfig -p 2>/dev/null | grep lib" .. libname .. ".so")
    if pipe then
        local result = pipe:read("*a")
        pipe:close()
        if result and result ~= "" then
            return true
        end
    end

    local search_paths = {
        "/opt/rocm/lib",
        "/usr/local/cuda/lib64",
        "/usr/lib/x86_64-linux-gnu",
    }

    local ld_library_path = os.getenv("LD_LIBRARY_PATH")
    if ld_library_path then
        for path in string.gmatch(ld_library_path, "[^:]+") do
            table.insert(search_paths, path)
        end
    end

    for _, path in ipairs(search_paths) do
        local lib_patterns = {
            path .. "/lib" .. libname .. ".so",
            path .. "/lib" .. libname .. ".so.*",
        }

        for _, pattern in ipairs(lib_patterns) do
            local check_pipe = io.popen("ls " .. pattern .. " 2>/dev/null")
            if check_pipe then
                local check_result = check_pipe:read("*a")
                check_pipe:close()
                if check_result and check_result ~= "" then
                    return true
                end
            end
        end
    end

    return false
end

local function first_existing_dir(candidates)
    for _, candidate in ipairs(candidates) do
        if os.isdir(candidate) then
            return candidate
        end
    end

    return nil
end

local function get_nvml_include()
    return first_existing_dir({
        "/usr/local/cuda/include",
        "/usr/local/cuda/targets/x86_64-linux/include",
        "/usr/local/cuda-12.9/include",
    })
end

local function get_cupti_include()
    return get_nvml_include()
end

local function get_rocm_include()
    return first_existing_dir({
        "/opt/rocm/include",
    })
end

local function get_cuda_libdir()
    return first_existing_dir({
        "/usr/local/cuda/lib64",
        "/usr/local/cuda/targets/x86_64-linux/lib",
        "/usr/local/cuda-12.9/lib64",
    })
end

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
        "src/**.cc",
        "src/**.hh",

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
        "lib/OPTKIT/lib/spdlog/include",
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

        if dynamic_lib_exists("nvidia-ml") then
            local nvml_include = get_nvml_include()
            if nvml_include then
                includedirs { nvml_include }
            end
            libdirs { "/usr/lib/x86_64-linux-gnu" }
            links { "nvidia-ml" }
        end

        if dynamic_lib_exists("amd_smi") then
            local rocm_include = get_rocm_include()
            if rocm_include then
                includedirs { rocm_include }
            end
            libdirs { "/opt/rocm/lib" }
            links { "amd_smi" }
        elseif dynamic_lib_exists("rocm_smi64") or dynamic_lib_exists("rocm_smi") then
            local rocm_include = get_rocm_include()
            if rocm_include then
                includedirs { rocm_include }
            end
            libdirs { "/opt/rocm/lib" }
            links { "rocm_smi64" }
        end

        if dynamic_lib_exists("cupti") then
            local cupti_include = get_cupti_include()
            local cuda_libdir = get_cuda_libdir()
            if cupti_include then
                includedirs { cupti_include }
            end
            if cuda_libdir then
                libdirs { cuda_libdir }
            end
            links { "cupti" }
        end

        if dynamic_lib_exists("netsnmp") then
            links { "netsnmp" }
        end

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
