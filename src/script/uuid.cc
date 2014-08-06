#include "uuid.h"
#include <lua.hpp>
#include <cassert>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


typedef boost::uuids::uuid      UUID;


#define check_uuid(L, idx)  (*(UUID**)luaL_checkudata(L, idx, UUID_META_HANDLE))


static int new_uuid(lua_State* L)
{
    UUID* puid = new UUID(boost::uuids::random_generator()());
    void* udata = lua_newuserdata(L, sizeof(puid));
    memcpy(udata, &puid, sizeof(puid));
    luaL_getmetatable(L, UUID_META_HANDLE);
    lua_setmetatable(L, -2);
    return 1;
}

static int uuid_gc(lua_State* L)
{
    UUID** ppuid = (UUID**)luaL_checkudata(L, 1, UUID_META_HANDLE);
    if (ppuid && *ppuid)
    {
        delete *ppuid;
        *ppuid = nullptr;
    }
    return 0;
}

static int uuid_tostring(lua_State* L)
{
    UUID* puid = check_uuid(L, 1);
    assert(puid);
    std::string str = boost::uuids::to_string(*puid);
    lua_pushlstring(L, str.c_str(), str.length());
    return 1;
}

static int uuid_len(lua_State* L)
{
    UUID* puid = check_uuid(L, 1);
    assert(puid);
    lua_pushinteger(L, sizeof(*puid));
    return 1;
}

static int uuid_equal(lua_State* L)
{
    UUID* a = check_uuid(L, 1);
    UUID* b = check_uuid(L, 2);
    assert(a && b);
    lua_pushboolean(L, *a == *b);
    return 1;
}

static int uuid_less_than(lua_State* L)
{
    UUID* a = check_uuid(L, 1);
    UUID* b = check_uuid(L, 2);
    assert(a && b);
    lua_pushboolean(L, *a < *b);
    return 1;
}

static int uuid_less_equal(lua_State* L)
{
    UUID* a = check_uuid(L, 1);
    UUID* b = check_uuid(L, 2);
    assert(a && b);
    lua_pushboolean(L, *a <= *b);
    return 1;
}

static void make_meta(lua_State* L)
{
    static const luaL_Reg meta_lib[] =
    {
        { "__gc", uuid_gc },
        { "__tostring", uuid_tostring },
        { "__len", uuid_len },
        { "__eq", uuid_equal },
        { "__lt", uuid_less_than },
        { "__le", uuid_less_equal },
        { NULL, NULL },
    };
    luaL_newmetatable(L, UUID_META_HANDLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, meta_lib);
}

int luaopen_uuid(lua_State* L)
{
    static const luaL_Reg uuidlib[]
    {
        { "create", new_uuid },
        { NULL, NULL },
    };
    make_meta(L);
    lua_newtable(L);
    luaL_register(L, NULL, uuidlib);
    return 1;
}
