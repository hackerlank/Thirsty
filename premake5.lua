--
-- Premake script (http://premake.github.io)
--

-- windows only
local USR_DIR = os.getenv('USR_DIR') or 'E:/usr'
local BOOST_ROOT = os.getenv('BOOST_ROOT') or ''

solution 'Thirsty'
    configurations  {'Debug', 'Release'}
    language        'C++'
    targetdir       'bin'

    filter 'configurations:Debug'
        defines     { 'DEBUG' }
        flags       { 'Symbols' }

    filter 'configurations:Release'
        defines     { 'NDEBUG' }
        flags       { 'Symbols', 'Optimize' }

    filter 'action:vs*'
        defines
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            '_SCL_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }
        includedirs {USR_DIR .. '/include'}
        libdirs
        {
            'dep/lua/lib',
            BOOST_ROOT .. '/stage/lib',
            USR_DIR .. '/lib/x86',
        }


    filter 'system:linux'
        buildoptions    { '-std=c++11 -mcrc32 -rdynamic' }
        defines         '__STDC_LIMIT_MACROS'
        includedirs     '/usr/local/include/luajit-2.0'

    project 'Thirsty'
        location    'build'
        kind        'ConsoleApp'
        defines
        {
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_DATE_TIME_NO_LIB',
            'BOOST_REGEX_NO_LIB',
            'BOOST_ASIO_HAS_MOVE',
            'BOOST_ASIO_HAS_VARIADIC_TEMPLATES',
            'BOOST_ASIO_HAS_STD_ARRAY',
            'BOOST_ASIO_HAS_STD_ATOMIC',
            'BOOST_ASIO_HAS_STD_SHARED_PTR',
            'BOOST_ASIO_HAS_STD_CHRONO',
            'FOLLY_HAVE_LIBLZ4',
        }
        files
        {
            'src/**.h',
            'src/**.hpp',
            'src/**.cpp',
            'src/**.cc',
            'src/**.c',
        }
        includedirs
        {
            'src',
            'dep/lua/include',
            'dep/zmq/include',
            BOOST_ROOT,
        }

        filter 'action:gmake'
            defines
            {
                'FOLLY_HAVE_LIBZ',
                'FOLLY_HAVE_LIBSNAPPY',
                'FOLLY_HAVE_LIBLZMA',
            }
            links
            {
                'z',
                'rt',
                'zmq',
                'lzma',
                'snappy',
                'pthread',
                'luajit-5.1',
                'boost_system',
            }

    project 'zmq'
        location    'build'
        kind        'StaticLib'

        defines     'ZMQ_STATIC'
        includedirs 'dep/zmq/include'
        files
        {
            'dep/zmq/include/*.h',
            'dep/zmq/src/*.hpp',
            'dep/zmq/src/*.cpp',
        }

        filter 'system:windows'
            includedirs 'dep/zmq/msvc'

        filter 'system:linux'
            includedirs 'dep/zmq/gcc'

    project 'unit-test'
        location    'build/test'
        kind        'ConsoleApp'
        defines
        {
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_DATE_TIME_NO_LIB',
            'BOOST_REGEX_NO_LIB',        
        }
        files
        {
            'dep/gtest/src/gtest-all.cc',
            'test/*.h',
            'test/*.cpp',
        }
        includedirs
        {
            'src',
            'dep/gtest',
            'dep/gtest/include',
            BOOST_ROOT,
        }
        removefiles 
        {
            'test/CompressionTest.cpp'
        }
        libdirs 'bin'