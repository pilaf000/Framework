-- premake5.lua
workspace "Framework"
    location "../generated/"
    configurations {"Debug", "Release"}
    platforms {"x64"}

    filter {
        "system:windows", 
        "action:vs*"
    }
    systemversion "10.0.17763.0"

    language "C++"
    cppdialect "C++17"
    defines {"NOMINMAX"}

    targetdir "../generated/bin/%{cfg.platform}/%{cfg.buildcfg}"
    objdir "../generated/obj/"

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
    
    project "foundation"
    do
        location "../generated/foundation/"
        kind "StaticLib"

        files {
            "../runtime/foundation/**.h",
            "../runtime/foundation/**.cpp"
        }

        includedirs {
            "../runtime/foundation/public",
            "../runtime/foundation/private",
        }
    end

    project "graphics"
    do
        location "../generated/graphics/"
        kind "StaticLib"

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
    end
    
    project "test"
    do
        location "../generated/test/"
        kind "WindowedApp"

        files {
            "../runtime/test/**.h",
            "../runtime/test/**.cpp"
        }

        includedirs {
            "../runtime/test/public",
            "../runtime/foundation/public",
            "../runtime/graphics/public",            
        }
        links {
            "graphics",
            "foundation",
        }
    end