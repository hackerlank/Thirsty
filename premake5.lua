--
-- Premake script (http://premake.github.io)
--

// https://github.com/ichenq/usr
local USR_ROOT = os.getenv('USR_ROOT') or 'E:/Library/usr'

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
        buildoptions '/GF'
        includedirs { USR_ROOT .. '/include' }
        libdirs { USR_ROOT .. '/lib/x64' }


    filter 'action:gmake'
        USR_ROOT = 'usr/local/'
        buildoptions    { '-std=c++11 -rdynamic' }
        defines         '__STDC_LIMIT_MACROS'

    project 'thirsty'
        location    'build'
        kind        'StaticLib'
        files
        {
            'src/**.h',
            'src/**.cpp',
        }
        includedirs
        {
            'src',
        }

    project 'UnitTest'
        location    'build'
        kind        'ConsoleApp'
        files
        {
            'test/*.h',
            'test/*.cpp',
        }
        includedirs 'src'
        links 
        {
            'thirsty', 
            'gtest',
        }
        libdirs 'bin'
        
        filter 'action:gmake'
            links
            {
                'z',
                'rt',
                'pthread',
            }        