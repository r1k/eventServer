------------------------------------------------------------------
-- premake 4 test solution
------------------------------------------------------------------
solution "EventServer"

    ------------------------------------------------------------------
    -- setup common settings
    ------------------------------------------------------------------
    configurations { "Debug", "Release" }
    linkoptions { }
    includedirs { "$(BOOST_ROOT)",
                  "$(WEBSOCKETPPDIR)",
                  "$(THEMELIOSDIR)/include",
                  "$(THEMELIOSDIR)/build",
                  "eventServer/src",
                  "eventServer/src/boost_http/server" }
    libdirs { "$(BOOST_ROOT)/stage/libstatic",
              "$(THEMELIOSDIR)/build",
              "lib" }

    defines { "_WEBSOCKETPP_CPP11_THREAD_",
              "NOMINMAX",
              "_WEBSOCKETPP_CPP11_FUNCTIONAL_",
              "_WEBSOCKETPP_CPP11_SYSTEM_ERROR_",
              "_WEBSOCKETPP_CPP11_RANDOM_DEVICE_",
              "_WEBSOCKETPP_CPP11_MEMORY_" }

    configuration { "linux", "gmake" }
        buildoptions { "-std=c++11" }

    configuration { "windows" }
        defines { "WIN32", "_WIN32_WINNT=0x0502"}
    ------------------------------------------------------------------
    -- setup the build configs
    ------------------------------------------------------------------
    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }


    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

    ------------------------------------------------------------------
    -- "EventServer" app project
    ------------------------------------------------------------------
    project "webserver"
        kind "StaticLib"
        language "C++"
        targetdir "lib"
        files { "eventServer/src/boost_http/server/**.h",
                "eventServer/src/boost_http/server/**.cpp" }
        links { "boost_system" }
        configuration "Debug"
            targetsuffix "_dbg"

    project "App"
        targetname ("EventServer")
        kind "ConsoleApp"
        language "C++"
        files { "eventServer/src/**.h",
                "eventServer/src/**.cpp" }
        links { "pthread",
                "themelios" ,
                "webserver",
                "boost_system",
                "boost_filesystem" }
        configuration "Debug"
            targetdir "Debug"
        configuration "Release"
            targetdir "Release"
