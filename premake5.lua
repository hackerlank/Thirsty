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
        flags       { 'Symbols' }
        optimize    'On'

    filter 'action:vs*'
        architecture 'x64'
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

    project 'Thirsty'
        location    'build'
        kind        'StaticLib'
        defines
        {
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

    project 'UnitTest'
        location    'build'
        kind        'ConsoleApp'
        defines
        {      
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