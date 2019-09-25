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
            symbols "On"
        end

    filter "configurations:Release"
        do
            defines {"NDEBUG"}
            optimize "On"
        end

    runtimeDir = "../source/runtime/"
    samplesDir = "../source/samples/"

    group "runtime"
        project "foundation"
        do
            location "../generated/runtime/foundation/"
            kind "StaticLib"
            
            files {
                runtimeDir .. "foundation/**.h",
                runtimeDir .. "foundation/**.cpp"
            }
            includedirs {
                runtimeDir .. "foundation/public",
            }
        end

        project "graphics"
        do
            location "../generated/runtime/graphics/"
            kind "StaticLib"

            files {
                runtimeDir .. "graphics/**.h",
                runtimeDir .. "graphics/**.cpp"
            }
            includedirs {
                runtimeDir .. "foundation/public",
                runtimeDir .. "graphics/public",
                runtimeDir .. "graphics/private",
            }
            links {
                "d3d12.lib",
                "dxgi.lib",
                "d3dcompiler.lib"
            }
        end

    group "samples"
        project "test"
        do
            location "../generated/samples/test/"
            kind "WindowedApp"
            
            files {
                samplesDir .. "test/**.h",
                samplesDir .. "test/**.cpp"
            }
            includedirs {
                samplesDir .. "test/public",
                runtimeDir .. "foundation/public",
                runtimeDir .. "graphics/public",            
            }
            links {
                "graphics",
                "foundation",
            }
        end
    
        project "helloPBR"
        do
            location "../generated/samples/helloPBR/"
            kind "WindowedApp"

            files {
                samplesDir .. "helloPBR/**.h",
                samplesDir .. "helloPBR/**.cpp",
                samplesDir .. "helloPBR/assets/*"
            }
            includedirs {
                samplesDir .. "helloPBR/public",
                samplesDir .. "helloPBR/external",
            }
            links {
                "d3d12.lib",
                "dxgi.lib",
                "d3dcompiler.lib"
            }
            filter {"files:**.hlsl"}
                shadermodel "5.0"
            filter {"files:**PS.hlsl"}
                shadertype "Pixel"
                shaderentry "PSMain"
            filter {"files:**VS.hlsl"}
                shadertype "Vertex"
                shaderentry "VSMain"
        end