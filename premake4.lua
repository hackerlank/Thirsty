--
-- Premake4 build script (http://industriousone.com/premake/download)
--

-- windows only
local BOOST_ROOT = os.getenv('BOOST_ROOT') or '/usr/local/include'

--
-- Thirsty
--
solution 'Thirsty'
    configurations {'Debug', 'Release'}
    language 'C++'
    --flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x64'}

    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }
        
	configuration "vs*"
		defines 
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }
		
	configuration "gmake"
		buildoptions { '-std=c++11 -mcrc32 -rdynamic' }
        defines { '__STDC_LIMIT_MACROS' }

    project 'Thirsty'
        location 'build'
        kind 'ConsoleApp'
        uuid '8701594A-72B8-4a6a-AEF3-6B41BBC33E65'
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
        libdirs
        {
            BOOST_ROOT .. '/stage/lib-x64',
        }
        
        if os.get() == 'linux' then
        links
        {
            'rt',
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
    --flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x64'}
    
    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }

	configuration "vs*"
		defines 
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }
		
	configuration "gmake"
		buildoptions { '-std=c++11 -mcrc32 -rdynamic' }
        
    project 'unittest'
        location 'build/test'
        kind 'ConsoleApp'
        uuid '31BC2F58-F374-4984-B490-F1F08ED02DD3'
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
        libdirs
        {
            BOOST_ROOT .. '/stage/lib-x64',
        }
        links
        {
            'thirsty',
        }
        if os.get() == 'linux' then
        links
        {
            'rt',
            'pthread',
            'boost_system',
            'boost_date_time',
            'boost_chrono',
        }
        end
        
    project 'thirsty'
        location 'build/test'
        kind 'StaticLib'
        uuid 'AB7D1C15-7A44-41a7-8864-230D8E345608'
        defines
        {
            '__STDC_LIMIT_MACROS',
            'BOOST_ASIO_SEPARATE_COMPILATION',
            'BOOST_REGEX_NO_LIB',
            'BOOST_ASIO_HAS_MOVE',
            'BOOST_ASIO_HAS_VARIADIC_TEMPLATES',
            'BOOST_ASIO_HAS_STD_ARRAY',
            'BOOST_ASIO_HAS_STD_ATOMIC',
            'BOOST_ASIO_HAS_STD_SHARED_PTR',
            'BOOST_ASIO_HAS_STD_CHRONO',
        }        
        files
        {
            'src/**.h',
            'src/**.cpp',
            'src/**.cc',
            'src/**.c',
        }
        excludes
        {
            'src/main.cpp',
        }
        includedirs
        {
            'src',
            'dep/gtest/include',
            BOOST_ROOT,
        }     
