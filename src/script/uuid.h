#pragma once

struct lua_State;

#define UUID_META_HANDLE "UUID*.GC"

int luaopen_uuid(lua_State* L);
