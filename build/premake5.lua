-- premake5.lua
workspace "Framework"
    location "../generated/"
    configurations {"Debug", "Release"}
    platforms {"x64"}
    
    project "graphics"
        location "../generated/graphics/"
        targetdir "../generated/bin/%{cfg.platform}/%{cfg.buildcfg}"
        objdir "../generated/obj/"
        filter {
            "system:windows", 
            "action:vs*"
        }
        systemversion "10.0.17763.0"

        language "C++"
        cppdialect "C++17"
        kind "SharedLib"
        defines {"NOMINMAX"}

        files {
            "../runtime/graphics/**.h",
            "../runtime/graphics/**.cpp"
        }

        includedirs {
            "../runtime/graphics/public",
            "../runtime/graphics/private",
        }
        links {
            "d3d12.lib",
            "dxgi.lib",
            "d3dcompiler.lib"
        }

        filter "configurations:Debug"
        do
            defines {"DEBUG"}
            flags {"Symbols"}
        end

        filter "configurations:Release"
        do
            defines {"NDEBUG"}
            optimize "On"
        end