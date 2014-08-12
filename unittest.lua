--
-- Premake4 build script (http://industriousone.com/premake/download)
--

-- windows only
local BOOST_ROOT = os.getenv('BOOST_ROOT') or ''

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

    project 'unittest'
        location 'build/test'
        kind 'ConsoleApp'
        uuid '31BC2F58-F374-4984-B490-F1F08ED02DD3'
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
        libdirs 
        {
            'bin',
        }
        if os.get() == 'windows' then
        defines 
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            '_SCL_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }        
        excludes
        {
            'test/CompressionTest.cpp',
        }
        libdirs
        {
            'dep/lua/lib',
            BOOST_ROOT .. '/stage/lib',
        }
        links
        {
            'lua51',
            'zmq',
            'thirsty',
        }        
        end
        if os.get() == 'linux' then
        buildoptions { '-std=c++11 -mcrc32 -rdynamic' }
        defines 
        { 
            '__STDC_LIMIT_MACROS',
            '__STDC_CONSTANT_MACROS',
        }
        includedirs
        {
            '/usr/local/include/luajit-2.0',
        }         
        links
        {
            'z',
            'rt',
            'zmq',
            'lzma',
            'snappy',
            'thirsty',
            'pthread',
            'luajit-5.1',
            'boost_system',
        }      
        end
        
    project 'thirsty'
        location 'build/test'
        kind 'StaticLib'
        uuid 'AB7D1C15-7A44-41a7-8864-230D8E345608'
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
            'dep/lua/include',
            'dep/gtest/include',
            BOOST_ROOT,
        }
        if os.get() == 'windows' then
        defines 
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            '_SCL_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }        
        end
        if os.get() == 'linux' then
        buildoptions { '-std=c++11 -mcrc32 -rdynamic' }
        defines 
        { 
            '__STDC_LIMIT_MACROS',
            'FOLLY_HAVE_LIBZ',
            'FOLLY_HAVE_LIBSNAPPY',
            'FOLLY_HAVE_LIBLZMA',            
        }
        end
