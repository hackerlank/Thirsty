--
-- Premake4 build script (http://industriousone.com/premake/download)
--

-- windows only
local BOOST_ROOT = os.getenv('BOOST_ROOT')

solution 'ServerExample'
    configurations {'Debug', 'Release'}
    language 'C++'
    flags {'ExtraWarnings'}
    platforms {'x64'}
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
        files
        {
            'echo_server.cpp',
            '../../src/**.h',
            '../../src/**.cpp',
            '../../src/**.cc',
            '../../src/**.c',
        }
        excludes
        {
            '../../src/main.cpp',
        }        


        if os.get() == 'windows' then
        defines
        {
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',            
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_REGEX_NO_LIB',
            'BOOST_ASIO_HAS_MOVE',
            'BOOST_ASIO_HAS_VARIADIC_TEMPLATES',
            'BOOST_ASIO_HAS_STD_ARRAY',
            'BOOST_ASIO_HAS_STD_ATOMIC',
            'BOOST_ASIO_HAS_STD_SHARED_PTR',
            'BOOST_ASIO_HAS_STD_CHRONO',
        }
        libdirs
        {
            BOOST_ROOT .. '/stage/lib',
        }
        includedirs
        {
            '../../src',
            BOOST_ROOT,
        }
        links
        {
        }        
        end
        
        if os.get() == 'linux' then
        defines
        {
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_REGEX_NO_LIB',
            'BOOST_ASIO_HAS_MOVE',
            'BOOST_ASIO_HAS_VARIADIC_TEMPLATES',
            'BOOST_ASIO_HAS_STD_ARRAY',
            'BOOST_ASIO_HAS_STD_ATOMIC',
            'BOOST_ASIO_HAS_STD_SHARED_PTR',
            'BOOST_ASIO_HAS_STD_CHRONO',
        }
        buildoptions
        { 
            '-std=c++11',
        }
        includedirs
        {
            '../../src',
        }
        links
        {
            'rt',
            'pthread',
            'bfd',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }
        end
        
    
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
        files
        {
            'echo_client.cpp',
            '../../src/**.h',
            '../../src/**.cpp',  
            '../../src/**.cc',
            '../../src/**.c',
        }
        excludes
        {
            '../../src/main.cpp',
        }
        
        if os.get() == 'windows' then
        defines
        {
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',            
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_REGEX_NO_LIB',
            'BOOST_ASIO_HAS_MOVE',
            'BOOST_ASIO_HAS_VARIADIC_TEMPLATES',
            'BOOST_ASIO_HAS_STD_ARRAY',
            'BOOST_ASIO_HAS_STD_ATOMIC',
            'BOOST_ASIO_HAS_STD_SHARED_PTR',
            'BOOST_ASIO_HAS_STD_CHRONO',
        }
        libdirs
        {
            BOOST_ROOT .. '/stage/lib',
        }
        includedirs
        {
            '../../src',
            BOOST_ROOT,
        }
        links
        {
        }        
        end
        
        if os.get() == 'linux' then
        defines
        {
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_REGEX_NO_LIB',
            'BOOST_ASIO_HAS_MOVE',
            'BOOST_ASIO_HAS_VARIADIC_TEMPLATES',
            'BOOST_ASIO_HAS_STD_ARRAY',
            'BOOST_ASIO_HAS_STD_ATOMIC',
            'BOOST_ASIO_HAS_STD_SHARED_PTR',
            'BOOST_ASIO_HAS_STD_CHRONO',
        }
        buildoptions 
        { 
            '-std=c++11', 
        }
        includedirs
        {
            '../../src',
        }
        libdirs
        {
            'usr/local/lib',
        }
        links
        {
            'rt',
            'pthread',
            'bfd',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }
        end
        