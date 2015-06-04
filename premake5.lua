--
-- Premake script (http://premake.github.io)
--

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


    filter 'action:gmake'
        buildoptions    { '-std=c++11 -rdynamic' }
        defines         '__STDC_LIMIT_MACROS'

    project 'libthirsty'
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
            BOOST_ROOT,
        }

    project 'UnitTest'
        location    'build'
        kind        'ConsoleApp'
        files
        {
            'dep/gtest/src/gtest-all.cc',
            'src/test/*.h',
            'src/test/*.cpp',
        }
        includedirs
        {
            'src',
            'dep/gtest',
            'dep/gtest/include',
            BOOST_ROOT,
        }
        links 'libthirsty'
        libdirs 'bin'
        
        filter 'action:gmake'
            links
            {
                'z',
                'rt',
                'pthread',
            }        