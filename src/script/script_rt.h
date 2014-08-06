#pragma once

struct lua_State;


namespace script {

int docall(lua_State *L, int narg, int clear);

} // script