#include "script_rt.h"
#include <lua.hpp>

namespace script {

static int traceback(lua_State* L)
{
    if (!lua_isstring(L, 1))  /* Non-string error object? Try metamethod. */
    {
        if (lua_isnoneornil(L, 1) ||
            !luaL_callmeta(L, 1, "__tostring") ||
            !lua_isstring(L, -1))
            return 1;  /* Return non-string error object. */
        lua_remove(L, 1);  /* Replace object by result of __tostring metamethod. */
    }
    luaL_traceback(L, L, lua_tostring(L, 1), 1);
    return 1;
}

int docall(lua_State* L, int narg, int clear)
{
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushcfunction(L, traceback);  /* push traceback function */
    lua_insert(L, base);  /* put it under chunk and args */

    int status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);

    lua_remove(L, base);  /* remove traceback function */
    if (status != 0)
    {
        fprintf(stderr, lua_tostring(L, -1));
    }
    return status;
}

} // namespace script
