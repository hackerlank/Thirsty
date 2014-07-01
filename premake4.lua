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
            BOOST_ROOT .. '/stage/lib',
        }
        includedirs
        {
            'src',
            'dep/double-conversion/src',
            'dep/zlib/src',
            BOOST_ROOT,
        }
        links
        {
            'zlib',
            'double-conversion',
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
            'dep/double-conversion/src',
            'dep/zlib/src',
        }
        links
        {
            'rt',
            'bfd',
            'pthread',
            'zlib',
            'double-conversion',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }
        end

    project 'zlib'
        location 'build'
        kind 'StaticLib'
        uuid '2E166B7C-8830-4FC9-8CFD-8163E5EB04A4'
        language 'C'
        files
        {
            'dep/zlib/src/*.h',
            'dep/zlib/src/*.c',
        }

    project 'double-conversion'
        location 'build'
        kind 'StaticLib'
        uuid 'AE6D2B6D-1CFB-48DE-A982-BAECACE31AE2'
        files
        {
            'dep/double-conversion/src/*.cc',
            'dep/double-conversion/src/*.h',
        }
        includedirs
        {
            'dep/double-conversion/src',
        }
        
    
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
            BOOST_ROOT .. '/stage/lib',
        }
        includedirs
        {
            'src',
            'dep/gtest/include',
            'dep/double-conversion/src',
            'dep/zlib/src',
            BOOST_ROOT,
        }      
        links
        {           
            'gtest',
            'double-conversion',
            'zlib',          
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
            'dep/double-conversion/src',
            'dep/zlib/src',
        }    
        links
        {
            'rt',
            'bfd', 
            'pthread',
            'zlib',
            'gtest',
            'double-conversion',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }        
        end
        
    project 'gtest'
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

    project 'double-conversion'
        location 'build/test'
        kind 'StaticLib'
        uuid 'AE6D2B6D-1CFB-48DE-A982-BAECACE31AE2'
        files
        {
            'dep/double-conversion/src/*.cc',
            'dep/double-conversion/src/*.h',
        }
        includedirs
        {
            'dep/double-conversion/src',
        }
        
    project 'zlib'
        location 'build/test'
        kind 'StaticLib'
        uuid '2E166B7C-8830-4FC9-8CFD-8163E5EB04A4'
        language 'C'
        files
        {
            'dep/zlib/src/*.h',
            'dep/zlib/src/*.c',
        }
            
