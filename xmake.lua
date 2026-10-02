---------------------------------
--! @file xmake.lua
--! @author Argore
--! @brief Template C++ configuration
--! @version 0.2
--! @date 2025-11-28
--! 
--! @copyright Copyright (c) 2025 Argore
---------------------------------

set_languages("clatest", "cxxlatest")

add_rules "mode.debug"
add_rules "mode.release"

--add_requires("nlohmann_json")

local PROJECT_NAME = "BlazonDB"

target(PROJECT_NAME)
    set_kind "binary" 
    
    --add_packages("nlohmann_json")

    if is_mode("debug") then
        set_basename(PROJECT_NAME .. "_debug_$(arch)")
        set_configdir("bin/dat")
    else 
        set_basename(PROJECT_NAME)
        set_configdir("package/bin/dat")
    end

    add_configfiles("dat/*", {onlycopy = true})
    
    
    set_targetdir "bin"
    set_objectdir "build/obj"
    
    add_includedirs "src/"
    add_headerfiles{ "src/**.h", "src/**.hpp" }
    add_files{ "src/**.c", "src/**.cpp" }
target_end()
