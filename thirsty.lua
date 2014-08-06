--
-- Premake4 build script (http://industriousone.com/premake/download)
--

-- windows only
local BOOST_ROOT = os.getenv('BOOST_ROOT') or ''

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
            'dep/lua/include',
            'dep/zmq/include',
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
        libdirs
        {
            'dep/lua/lib',
            BOOST_ROOT .. '/stage/lib-x64',
        }
        links
        {
            'lua51',
            'zmq',
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
            'pthread',
            'luajit-5.1',
            'boost_system',
            'boost_thread',
        }
        end

    project 'zmq'
        location 'build'
        kind 'StaticLib'
        uuid 'A75AF625-DDF0-4E60-97D8-A2FDC6229AF7'        
        files
        {
            'dep/zmq/include/*.h',
            'dep/zmq/src/*.hpp',
            'dep/zmq/src/*.cpp',
        }
        defines 
        {
            'ZMQ_STATIC',            
        }
        if os.get() == 'windows' then
        includedirs 
        { 
            'dep/zmq/msvc',
            'dep/zmq/include',
        }
        defines {'_CRT_SECURE_NO_WARNINGS',}
        else
        includedirs
        {
            'dep/zmq/gcc',
            'dep/zmq/include',
        }
        end      
    
        