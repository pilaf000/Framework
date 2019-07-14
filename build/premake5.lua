-- premake5.lua
workspace "Framework"
    location "../generated/"
    configurations {"Debug", "Release"}

    project "graphics"
        location "../generated/graphics/"
        targetdir "../generated/bin/%{cfg.buildcfg}" 
        objdir "../generated/obj/%{cfg.platform}"
        do
            kind "SharedLib"
            language "C++"
            defines {"NOMINMAX"}
            includedirs {"include", "src"}

            files {
                "../runtime/graphics/**.h",
                "../runtime/graphics/**.cpp"
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
        end