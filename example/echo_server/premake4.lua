--
-- Premake4 build script (http://industriousone.com/premake/download)
--

local BOOST_ROOT = os.getenv('BOOST_ROOT')

solution 'ServerExample'
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

    project 'echo_server'
        location 'build'
        kind 'ConsoleApp'
        uuid '8701594A-72B8-4a6a-AEF3-6B41BBC33E65'
        defines
        {
            'BOOST_DATE_TIME_NO_LIB',
            'BOOST_REGEX_NO_LIB',
            '_WIN32_WINNT=0x0501',
            '_CRT_SECURE_NO_WARNINGS',
        }
        files
        {
            'echo_server.cpp',
            '../src/**.h',
            '../src/**.cpp',
        }
        excludes
        {
            '../src/main.cpp',
        }
        includedirs
        {
            '../src/',
            '../dep/double-conversion/src',
            '../dep/zlib/src',
            BOOST_ROOT,
        }
        libdirs
        {
            BOOST_ROOT .. '/stage/lib',
        }
        links
        {
            'zlib',
            'double-conversion',
        }

    project 'zlib'
        location 'build'
        kind 'StaticLib'
        uuid '2E166B7C-8830-4FC9-8CFD-8163E5EB04A4'
        language 'C'
        defines
        {
        }
        files
        {
            '../dep/zlib/src/*.h',
            '../dep/zlib/src/*.c',
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
            '../dep/double-conversion/src/*.cc',
            '../dep/double-conversion/src/*.h',
        }
        includedirs
        {
            '../dep/double-conversion/src',
        }
        
    
--
-- client
--    
solution 'ClientExample'
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
        
    project 'echo_client'
        location 'build'
        kind 'ConsoleApp'
        uuid 'AB7D1C15-7A44-41a7-8864-230D8E345608'
        defines
        {
            'BOOST_DATE_TIME_NO_LIB',
            'BOOST_REGEX_NO_LIB',
            '_WIN32_WINNT=0x0501',
            '_CRT_SECURE_NO_WARNINGS',
        }
        files
        {
            'echo_client.cpp',
            '../src/**.h',
            '../src/**.cpp',            
        }
        excludes
        {
            '../src/main.cpp',
        }
        includedirs
        {
            '../src',
            '../dep/double-conversion/src',
            '../dep/zlib/src',
            BOOST_ROOT,
        }
        libdirs
        {
            BOOST_ROOT .. '/stage/lib',
        }        
        links
        {
            'double-conversion',
            'zlib',
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
            '../dep/double-conversion/src/*.cc',
            '../dep/double-conversion/src/*.h',
        }
        includedirs
        {
            '../dep/double-conversion/src',
        }
        
    project 'zlib'
        location 'build'
        kind 'StaticLib'
        uuid '2E166B7C-8830-4FC9-8CFD-8163E5EB04A4'
        language 'C'
        defines
        {
        }
        files
        {
            '../dep/zlib/src/*.h',
            '../dep/zlib/src/*.c',
        }
            