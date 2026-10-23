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
add_rules("mode.debug", "mode.release")
add_requires("nlohmann_json")

local PROJECT_NAME = "BlazonDB"

target(PROJECT_NAME)
    set_kind "binary" 
    
    add_packages("nlohmann_json")

    if is_mode("debug") then
        set_basename(PROJECT_NAME .. "_debug_$(arch)")
        set_configdir("bin/dat")
    else 
        set_basename(PROJECT_NAME)
        set_configdir("package/bin/dat")
    end

    -- wxwidgets doesn't build through xrepo anymore, for some reason
    -- so we link manually here
    -- change WX_ROOT to wherever your wx install is
    if is_plat("windows") then
        local WX_ROOT = "D:/SDK/wx/3.3.1/"

        add_defines("__WXMSW__", "WXUSINGDLL", "_UNICODE", "wxMSVC_VERSION_AUTO", "wxMSVC_VERSION_ABI_COMPAT", "NDEBUG")
        add_includedirs(WX_ROOT .. "include/", WX_ROOT .. "lib/vc14x_x64_dll/mswu/")
        add_linkdirs(WX_ROOT .. "lib/vc14x_x64_dll/")
        add_files(WX_ROOT .. "include/wx/msw/wx.rc")

        add_ldflags("/SUBSYSTEM:WINDOWS")
    end

    add_configfiles("dat/*", {onlycopy = true})
    
    
    set_targetdir "bin"
    set_objectdir "build/obj"
    
    add_includedirs "src/"
    add_headerfiles{ "src/**.h", "src/**.hpp" }
    add_files{ "src/**.c", "src/**.cpp" }
target_end()
