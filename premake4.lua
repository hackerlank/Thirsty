--
-- Premake4 build script (http://industriousone.com/premake/download)
--

local BOOST_ROOT = os.getenv('BOOST_ROOT')

solution 'Thirsty'
    configurations {'Debug', 'Release'}
    language 'C++'
    flags {'ExtraWarnings'}
    targetdir 'bin'

    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }

    project 'UnitTest'
        location 'build'
        kind 'ConsoleApp'
        uuid 'AB7D1C15-7A44-41a7-8864-230D8E345608'
        defines
        {
            'WIN32',
            '_DEBUG',
            'NOMINMAX',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_SCL_SECURE_NO_WARNINGS',
            '_CRT_SECURE_NO_WARNINGS',
        }
        files
        {
            'test/*.h',
            'test/*.cpp',
        }
        includedirs
        {
            'src',
            'dep/gtest/include',
            'dep/double-conversion/src',
            BOOST_ROOT,
        }
        links
        {
            'libThirsty',
            'libgtest',
            'double-conversion',
        }

    project 'libThirsty'
        location 'build'
        kind 'StaticLib'
        uuid '8701594A-72B8-4a6a-AEF3-6B41BBC33E65'
        defines
        {
            'WIN32',
            '_DEBUG',
            'NOMINMAX',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_SCL_SECURE_NO_WARNINGS',
            '_CRT_SECURE_NO_WARNINGS',
        }
        files
        {
            'src/**.h',
            'src/**.cpp',
        }
        includedirs
        {
            'src',
            'dep/double-conversion/src',
            BOOST_ROOT,
        }

    project 'libgtest'
        location 'build'
        kind 'StaticLib'
        uuid '31BC2F58-F374-4984-B490-F1F08ED02DD3'
        defines
        {
        }
        files
        {
            'dep/gtest/src/gtest-all.cc',
        }
        includedirs
        {
            'dep/gtest',
            'dep/gtest/include',
        }

    project 'double-conversion'
        location 'build'
        kind 'StaticLib'
        uuid 'AE6D2B6D-1CFB-48DE-A982-BAECACE31AE2'
        defines
        {
        }
        files
        {
            'dep/double-conversion/src/*.cc',
            'dep/double-conversion/src/*.h',
        }
        includedirs
        {
            'dep/double-conversion/src',
        }
