--
-- Premake4 build script (http://industriousone.com/premake/download)
--

-- windows only
local BOOST_ROOT = os.getenv('BOOST_ROOT')

--
-- Thirsty
--
solution 'Thirsty'
    configurations {'Debug', 'Release'}
    language 'C++'
    flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x64'}

    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }

    project 'Thirsty'
        location 'build'
        kind 'ConsoleApp'
        uuid '8701594A-72B8-4a6a-AEF3-6B41BBC33E65'
        
        files
        {
            'src/**.h',
            'src/**.hpp',
            'src/**.cpp',
            'src/**.cc',
            'src/**.c',
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
            BOOST_ROOT .. '/stage/lib-x64',
        }
        includedirs
        {
            'src',
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
            'src',
        }
        links
        {
            'rt',
            'bfd',
            'pthread',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }
        end
        
    
--
-- UnitTest
--    
solution 'UnitTest'
    configurations {'Debug', 'Release'}
    language 'C++'
    flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x64'}
    
    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }
        
    project 'unittest'
        location 'build/test'
        kind 'ConsoleApp'
        uuid 'AB7D1C15-7A44-41a7-8864-230D8E345608'
        files
        {
            'test/*.h',
            'test/*.cpp',
            'src/**.h',
            'src/**.cpp',
            'src/**.cc',
            'src/**.c',
        }
        excludes
        {
            'src/main.cpp',
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
            BOOST_ROOT .. '/stage/lib-x64',
        }
        includedirs
        {
            'src',
            'dep/gtest/include',
            BOOST_ROOT,
        }      
        links
        {           
            'libgtest',
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
            'src',
            'dep/gtest/include',
        }    
        links
        {
            'rt',
            'bfd', 
            'pthread',
            'libgtest',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }        
        end
        
    project 'libgtest'
        location 'build/test'
        kind 'StaticLib'
        uuid '31BC2F58-F374-4984-B490-F1F08ED02DD3'
        files
        {
            'dep/gtest/src/gtest-all.cc',
        }
        includedirs
        {
            'dep/gtest',
            'dep/gtest/include',
        }
