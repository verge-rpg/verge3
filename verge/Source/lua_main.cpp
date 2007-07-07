#include "xerxes.h"
#include "lua_main.h"
#include <luabind/object.hpp>

#include <string>
#include <algorithm>
#include <cctype>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


//http://www.codeproject.com/cpp/luaincpp.asp

void funk() {
//lets you do some test things at the beginning of xmain
}

void LUA::compileSystem() {
	loadFile("system.lua");
}

void LUA::loadFile(const char *fname) {
	if(luaL_loadfile(L, fname))
		err("Error loading " + std::string(fname));
	if(lua_pcall(L, 0,0,0)) 
		err("Error compiling " + std::string(fname));
}

void LUA::ExecAutoexec() {
	lua_getglobal(L, "autoexec");
	if(lua_pcall(L, 0,0,0)) 
		err("Error calling autoexec");
}


//gfx routines
static int ___LoadImage(std::string fname) { return HandleForImage(xLoadImage(fname.c_str())); }
static void ___rectfill(int x0, int y0, int x1, int y1, int c, int d) { Rect(x0,y0,x1,y1,c,ImageForHandle(d)); }
static int ___makecolor(int r, int g, int b) { return MakeColor(r,g,b); }
static void ___printstring(int x, int y, int d, int f, std::string text) { 
	image *dest = ImageForHandle(d);
	Font *font = (Font*)f;
	if (font == 0)
	{
		GotoXY(x, y);
		PrintString(va("%s",text.c_str()), dest);
	}
	else
		font->PrintString("%s", x, y, dest, text.c_str());
}
static void ___tblit(int x, int y, int s, int d) { TBlit(x,y,ImageForHandle(s),ImageForHandle(d)); }
static void ___showpage() { UpdateControls(); ShowPage(); }

int ___get_mx() { return mouse_x; }
void ___set_mx(int val) { mouse_set(val,mouse_y); }
int ___get_my() { return mouse_y; }
void ___set_my(int val) { mouse_set(mouse_x,val); }
int ___get_ml() { return mouse_l; }
void ___set_ml(int val) { mouse_l = val; }
int ___get_mm() { return mouse_l; }
void ___set_mm(int val) { mouse_m = val; }
int ___get_mr() { return mouse_r; }
void ___set_mr(int val) { mouse_r = val; }
int ___get_mw() { return (int)mwheel; }
void ___set_mw(int val) { mwheel = (float)val; }

std::string strtolower(std::string val) { std::transform(val.begin(), val.end(), val.begin(), tolower); return val; }

#define LUA_VAR(name) { \
	module(L) [ \
	def("___get_"#name, ___get_##name), \
	def("___set_"#name, ___set_##name)]; \
	lua_getglobal(L,"___getters"); \
	lua_pushstring(L,#name); \
	lua_getglobal(L,"___get_"#name); \
	lua_settable(L,-3); \
	lua_pop(L,1); \
	lua_getglobal(L,"___setters"); \
	lua_pushstring(L,#name); \
	lua_getglobal(L,"___set_"#name); \
	lua_settable(L,-3); \
	lua_pop(L,1); }

#define LUA_VAR_R(name) { \
	module(L) [ \
	def("___get_"#name, ___get_##name)]; \
	lua_getglobal(L,"___getters"); \
	lua_pushstring(L,#name); \
	lua_getglobal(L,"___get_"#name); \
	lua_settable(L,-3); \
	lua_pop(L,1); \
	lua_getglobal(L,"___xsetters"); \
	lua_pushstring(L,#name); \
	lua_getglobal(L,"___set_fail_readonly"); \
	lua_settable(L,-3); \
	lua_pop(L,1); }

#define LUA_FUNC(name) { module(L) [ def(#name, ___##name) ]; }
#define LUA_BIND(name) { module(L) [ def(#name, ##name)]; }
#define LUA_BIND_R(name)  { module(L) [ def("___get_"#name,___get_##name) ]; }
#define LUA_BIND_W(name)  { module(L) [ def("___set_"#name,___get_##name) ]; }
#define LUA_BIND_RW(name) { LUA_BIND_R(name); LUA_BIND_W(name); }

//VI.a. General Utility Functions
//FUNC(CallFunction);
static void ___exit(std::string msg) { err(msg.c_str()); }
//TODO(FunctionExists);
//TODO(GetInt);
//TODO(GetIntArray);
//TODO(GetString);
//TODO(GetStringArray);
static void ___HookButton(int b, std::string script) { b--; if(b<0||b>3) return; bindbutton[b] = script; }
//FUNC(HookKey);
//FUNC(HookRetrace);
//FUNC(HookTimer);
//FUNC(Log);
static void ___MessageBox(std::string msg) { MessageBox(0,msg.c_str(),"Message Box",0); }
//FUNC(Random);
//FUNC(SetAppName);
//FUNC(SetButtonJB);
//FUNC(SetButtonKey);
//FUNC(SetInt);
//FUNC(SetIntArray);
//FUNC(SetRandSeed);
//FUNC(SetResolution);
//FUNC(SetString);
//FUNC(SetStringArray);
//FUNC(Unpress);
//FUNC(UpdateControls);


int ___get_systemtime() { return systemtime; }
int ___get_timer() { return timer; }
void ___set_timer(int val) { timer = val; }
int ___get_lastpressed() { return lastpressed; }
void ___set_lastpressed(int val) { lastpressed = val; }
int ___get_lastkey() { return lastkey; }
void ___set_lastkey(int val) { lastkey = val; }
int ___get_key(int idx) { return keys[idx]; }
void ___set_key(int idx, int val) { keys[idx] = val; }

//VII.c. Joystick Variables
//todo - bounds
int ___get_joy_active(int idx) { return sticks[idx].active; }
int ___get_joy_up(int idx) {return sticks[idx].up; }
int ___get_joy_down(int idx) { return sticks[idx].down; }
int ___get_joy_left(int idx) { return sticks[idx].left; }
int ___get_joy_right(int idx) { return sticks[idx].right; }
int ___get_joy_analogx(int idx) { return sticks[idx].analog_x; }
int ___get_joy_analogy(int idx) { return sticks[idx].analog_y; }
int ___get_joy_button(int idx, int btn) { return sticks[idx].button[btn]; }

//-----------------------------
//VII.d. Entity Variables
//todo - bounds
int ___get_entities() { return entities; }

int ___get_ent_x(int ofs) { return entity[ofs]->getx(); }
void ___set_ent_x(int ofs, int value) { entity[ofs]->setxy(value, entity[ofs]->gety()); }
int ___get_ent_y(int ofs) { return entity[ofs]->gety(); }
void ___set_ent_y(int ofs, int value) { entity[ofs]->setxy(entity[ofs]->getx(), value); }
int ___get_ent_specframe(int ofs) { return entity[ofs]->specframe; }
void ___set_ent_specframe(int ofs, int value) { entity[ofs]->specframe = value; }

int ___get_ent_frame(int ofs) { return entity[ofs]->frame; }
int ___get_ent_hotx(int ofs) { return entity[ofs]->chr->hx; }
int ___get_ent_hoty(int ofs) { return entity[ofs]->chr->hy; }
int ___get_ent_hotw(int ofs) { return entity[ofs]->chr->hw; }
int ___get_ent_hoth(int ofs) { return entity[ofs]->chr->hh; }
int ___get_ent_movecode(int ofs) { return entity[ofs]->movecode; }

int ___get_ent_face(int ofs) { return entity[ofs]->face; }
void ___set_ent_face(int ofs, int value) { entity[ofs]->setface(value); }
int ___get_ent_speed(int ofs) { return entity[ofs]->speed; }
void ___set_ent_speed(int ofs, int value) { entity[ofs]->setspeed(value); }
int ___get_ent_visible(int ofs) { return entity[ofs]->visible; }
void ___set_ent_visible(int ofs, int value) { entity[ofs]->visible = value ? true : false; }
int ___get_ent_obstruct(int ofs) { return entity[ofs]->obstruction; }
void ___set_ent_obsctruct(int ofs, int value) { entity[ofs]->obstruction = (value!=0); }
int ___get_ent_obstructable(int ofs) { return entity[ofs]->obstructable; }
void ___set_ent_obstructable(int ofs, int value) { entity[ofs]->obstructable = (value!=0); }

std::string ___get_ent_script(int ofs) { return std::string(entity[ofs]->script.c_str()); }
std::string ___get_ent_chr(int ofs) { err("entity.chr is not implemented. please post a bug."); return 0; }

//------------------------------
//VII.e. Sprite Variables
//todo - bounds
int ___get_sprite_x(int ofs) { return sprites[ofs].x; }
void ___set_sprite_x(int ofs, int value) { sprites[ofs].x = value; }
int ___get_sprite_y(int ofs) { return sprites[ofs].y; }
void ___set_sprite_y(int ofs, int value) { sprites[ofs].y = value; }
int ___get_sprite_sc(int ofs) { return sprites[ofs].sc; }
void ___set_sprite_sc(int ofs, int value) { sprites[ofs].sc = value; }
int ___get_sprite_image(int ofs) { return sprites[ofs].image; }
void ___set_sprite_image(int ofs, int value) { sprites[ofs].image = value; }
int ___get_sprite_lucent(int ofs) { return sprites[ofs].lucent; }
void ___set_sprite_lucent(int ofs, int value) { sprites[ofs].lucent = value; }
int ___get_sprite_addsub(int ofs) { return sprites[ofs].addsub; }
void ___set_sprite_addsub(int ofs, int value) { sprites[ofs].addsub = value; }
int ___get_sprite_alphamap(int ofs) { return sprites[ofs].alphamap; }
void ___set_sprite_alphamap(int ofs, int value) { sprites[ofs].alphamap = value; }

//VII.f. Camera Variables
int ___get_xwin() { return xwin; }
void ___set_xwin(int val) { xwin = val; }
int ___get_ywin() { return ywin; }
void ___set_ywin(int val) { ywin = val; }
int ___get_cameratracking() { return cameratracking; }
void ___set_cameratracking(int val) { cameratracking = val; }
int ___get_cameratracker() { return cameratracker; }
void ___set_cameratracker(int val) { cameratracker = val; }

//VII.g. Map Variables
int ___get_curmap_w() { return current_map ? current_map->mapwidth : 0; }
int ___get_curmap_h() { return current_map ? current_map->mapheight : 0; }
int ___get_curmap_startx() { return current_map ? current_map->startx : 0; }
int ___get_curmap_starty() { return current_map ? current_map->starty : 0; }
int ___get_curmap_tileset() { return current_map ? 2 : 0; }
std::string ___get_curmap_name() { return current_map ? current_map->mapname : ""; }
std::string ___get_curmap_rstring() { return current_map ? current_map->renderstring : ""; }
std::string ___get_curmap_music() { return current_map ? current_map->musicname : ""; }
std::string ___get_curmap_path() { return current_map ? current_map->mapfname : ""; }

//VII.h. Layer Variables
//todo - bounds
int ___get_layer_visible(int ofs) { err("layer.visible is not implemented. was it ever? post a bug"); return 0; }
void ___set_layer_visible(int ofs, int value) { err("layer.visible is not implemented. was it ever? post a bug"); }
int ___get_layer_lucent(int ofs) { return current_map->layers[ofs]->lucent; }
void ___set_layer_lucent(int ofs, int value) { current_map->layers[ofs]->lucent = value; }
double ___get_layer_parallaxx(int ofs) { return current_map->layers[ofs]->parallax_x; }
void ___set_layer_parallaxx(int ofs, double value) { current_map->layers[ofs]->SetParallaxX(value); }
double ___get_layer_parallaxy(int ofs) { return current_map->layers[ofs]->parallax_y; }
void ___set_layer_parallaxy(int ofs, double value) { current_map->layers[ofs]->SetParallaxY(value); }
int ___get_layer_w(int ofs) { return current_map->layers[ofs]->width; }
int ___get_layer_h(int ofs) { return current_map->layers[ofs]->height; }

//VII.i. Event Variables
int ___get_event_tx() { return event_tx; }
int ___get_event_ty() { return event_ty; }
int ___get_event_zone() { return event_zone; }
int ___get_event_entity() { return event_entity; }
int ___get_event_param() { return event_param; }

//VII.j. Date/Time Variables
int ___get_sysdate_year() { return getYear(); }
int ___get_sysdate_month() { return getMonth(); }
int ___get_sysdate_day() { return getDay(); }
int ___get_sysdate_dayofweek() { return getDayOfWeek(); }
int ___get_sysdate_hour() { return getHour(); }
int ___get_sysdate_minute() { return getMinute(); }
int ___get_sysdate_second() { return getSecond(); }

//VII.k. Clipboard Variables
std::string ___get_clipboard_text() { return clipboard_getText(); }
void ___set_clipboard_text(std::string str) { clipboard_setText(str.c_str()); }

///VIII.a. Version
std::string ___get_version() { return DEF_VERSION; }
std::string ___get_build() { return DEF_BUILD; }
std::string ___get_os() { return DEF_OS; }

void LUA::bindApi() {
	using namespace luabind;
	open(L);
	
	

	//TODO -- add logic to lua global arrays to perform bounds checking

	//install the metatable and utility functions
	int grr = 0;
	grr |= luaL_dostring(L,"function ___getmetatable(tbl) \
				local meta = getmetatable(tbl) \
				if(meta) then return meta else return {} end \
			end");
	grr |= luaL_dostring(L,"function ___set_fail_readonly(k,v) \
			exit('builtin global variable `'..k..'` is readonly')\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_readonly_blah() \
			exit('builtin global variable is readonly')\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_needmember(k,v) \
			exit('builtin global variable `'..k..'` must be accessed by member, i.e. `'..k..'`.something')\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_systemvar(k,v) \
			exit('builtin global variable `'..k..'` cannot be set directly. Please drilldown or index as appropriate.')\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_array(k,v) \
			exit('builtin global array `'..k..'` cannot be accessed directly. Please index it.')\
			end");
	grr |= luaL_dostring(L,"function ___get_fail_array(k) \
			exit('builtin global array `'..k..'` cannot be accessed directly. Please index it.')\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_func(k) \
			exit('`'..k..'` is a builtin global function. You are trying to overwrite it. Please do not.')\
			end");
	grr |= luaL_dostring(L," function ___getname(self) \
			if(self == _G) then return '_G'; \
			else return ___getname(self.___parent)..self.___name; end \
			end");
	grr |= luaL_dostring(L,"function ___newtable(parent,name) \
			local table = {___parent=parent,___name=name} \
			local meta = { __tostring = ___getname}; \
			setmetatable(table,meta); \
			return table; \
			end");
	//???
	//grr |= luaL_dostring(L,"function ___indexer_table(table,getter,setter) \
	//	local meta = ___getmetatable(table); \
	//	function meta.__index(t,k) return getter(k) end \
	//	function meta.__newindex(t,k,v) setter(k,b) end \
	//	setmetatable(table,meta); \
	//	end");

	//magic table things
	//a magic_table is the main 'namespace' hierarchical organization. _G is the root.
	//magic_tables know how to embed other things:
	//0. ___embed(name) is for internal purposes
	//1. ___embed_magic_table embeds another magic_table within the parent. this is used, for example, for _G.mouse.x and _G.mouse.y
	//2. ___embed_magic_collection embeds an indexable collection of 'objects' within the parent, such as _G.joy[0].active
	//3. ___embed_array_collection embeds a simple indexible collection of scalars into the parent, such as _G.key[SCAN_ESC]
	grr |= luaL_dostring(L,"function ___magic_table(table) ___xmagic_table(table,false) end");
	grr |= luaL_dostring(L,"function ___xmagic_table(table,allowdefault) \
			local meta = ___getmetatable(table) \
			setmetatable(table,{}); \
			table.___getters, table.___setters = {},{}; \
			\
			function meta.__index(t, k) \
				local tk = string.lower(k);\
				if table.___getters[tk] then return table.___getters[tk]() \
				elseif allowdefault then return rawget(t,k) \
				else exit(k..' 1 does not exist error tbd'); end \
			end \
			function meta.__newindex(t, k, v) \
				local tk = string.lower(k);\
				if table.___setters[tk] then table.___setters[tk](v) \
				elseif allowdefault then rawset(t, k, v) \
				elseif(string.sub(k,1,3) == '___') then rawset(t, k, v)\
				else exit(k..' 2 does not exist error tbd'); end \
			end \
			function table:___rw(name,getter,setter) \
				name = string.lower(name);\
				if(getter) then self.___getters[name] = getter; else self.___getters[name] = function() exit('3 writeonly error tbd') end; end \
				if(setter) then self.___setters[name] = setter; else self.___setters[name] = function(v) exit('4 readonly error tbd') end; end \
			end \
			function table:___r(name,getter) self:___rw(name,getter,nil) end \
			function table:___w(name,setter) self:___rw(name,nil,setter) end \
			\
			function table:___embed(name)\
				local tbl = ___newtable(self,'.'..name);\
				rawset(self,name,tbl); \
				self.___setters[name] = ___set_fail_systemvar; \
				return tbl; \
			end \
			function table:___embed_magic_table(name) \
				___magic_table(self:___embed(name)); \
			end \
			function table:___embed_magic_collection(name,makeitem) \
				___make_magic_collection(self:___embed(name),makeitem); \
			end \
			function table:___embed_array_collection(name,getitem,setitem) \
				___make_array_collection(self:___embed(name),getitem,setitem); \
			end \
			setmetatable(table,meta);\
		end");

		//make the entity collections
		grr |= luaL_dostring(L,"\
			function ___magic_collection_getindex(parent, index, makeitem) \
				local table = ___newtable(parent, '['..index..']'); \
				___magic_table(table) \
				table.___index = index; \
				function table:___ir(name,rfunc) table:___rw(name, function() return rfunc(index) end, nil) end; \
				function table:___iw(name,wfunc) table:___rw(name, nil, function() return wfunc(index) end) end; \
				function table:___irw(name,rfunc,wfunc) table:___rw(name, function() return rfunc(index) end, function() return wfunc(index) end) end; \
				makeitem(table); \
				return table; \
			end \
			function ___make_magic_collection(table, makeitem) \
				\
				local meta = ___getmetatable(table); \
				function meta.__index(t,k) return ___magic_collection_getindex(table, k, makeitem) end \
				function meta.__newindex(t,k,v) exit('invalid attempt to set system array index. tbd better error message') end \
				setmetatable(table,meta); \
			end  \
			function ___make_array_collection(table, getitem, setitem) \
				\
				local meta = ___getmetatable(table); \
				if(getitem) then function meta.__index(t,k) return getitem(k) end \
				else function meta.__index(t,k) exit('invalid attempt to get system array index. tbd better error message') end end \
				if(setitem) then function meta.__newindex(t,k,v) setitem(k,v) end \
				else function meta.__newindex(t,k,v) exit('invalid attempt to set system array index. tbd better error message') end end \
				setmetatable(table,meta); \
			end");

		if(grr) err("grr");

		//special setup for _G
		grr |= luaL_dostring(L,"\
		___xmagic_table(_G,true); \
		");

		#define FUNC(name) { { module(L) [ def("___"#name, ___##name) ]; }  grr |= luaL_dostring(L,"_G:___rw('"#name"',function() return ___"#name" end,function(val) ___set_fail_func('"#name"') end);"); }
		#define TODO(name) {  grr |= luaL_dostring(L,"_G:___rw('"#name"',function() exit('`"#name"` is not implemented yet in lua. Post a bug or consider whether you need it') end,function(val) ___set_fail_func('"#name"') end);"); }
		//VI.a. General Utility Functions
		//FUNC(CallFunction);
		FUNC(exit);
		TODO(FunctionExists);
		TODO(GetInt);
		TODO(GetIntArray);
		TODO(GetString);
		TODO(GetStringArray);
		FUNC(HookButton);
		//FUNC(HookKey);
		//FUNC(HookRetrace);
		//FUNC(HookTimer);
		//FUNC(Log);
		FUNC(MessageBox);
		//FUNC(Random);
		//FUNC(SetAppName);
		//FUNC(SetButtonJB);
		//FUNC(SetButtonKey);
		//FUNC(SetInt);
		//FUNC(SetIntArray);
		//FUNC(SetRandSeed);
		//FUNC(SetResolution);
		//FUNC(SetString);
		//FUNC(SetStringArray);
		//FUNC(Unpress);
		//FUNC(UpdateControls);

		//some handy functions
		grr |= luaL_dostring(L,"\
			function xr(table,name,funcname) table:___r(name,_G['___get_'..funcname]) end\
			function xw(table,name,funcname) table:___w(name,_G['___set_'..funcname]) end\
			function xrw(table,name,funcname) table:___r(name,_G['___get_'..funcname],_G['___set_'..funcname]) end\
			function r(table,name) table:___r(name,_G['___get_'..name]) end\
			function w(table,name) table:___w(name,_G['___set_'..name]) end\
			function rw(table,name) table:___r(name,_G['___get_'..name],_G['___set_'..name]) end\
		");
	
		//VII.a. General System Variables
		LUA_BIND_R(systemtime);
		LUA_BIND_RW(timer);
		LUA_BIND_RW(lastpressed);
		LUA_BIND_RW(lastkey)
		LUA_BIND_RW(key);
		grr |= luaL_dostring(L,"\
			_G:___r('systemtime',___get_systemtime);  \
			_G:___rw('timer',___get_timer,___set_timer);  \
			_G:___rw('lastpressed',___get_lastpressed,___set_lastpressed);  \
			_G:___rw('lastkey',___get_lastkey,___set_lastkey);  \
			_G:___embed_array_collection('key',  ___get_key, ___set_key);\
		");
		//VII.b. Mouse Variables
		LUA_BIND_RW(mx); LUA_BIND_RW(my);
		LUA_BIND_RW(ml); LUA_BIND_RW(mm); LUA_BIND_RW(mr);
		LUA_BIND_RW(mw);
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_table('mouse'); \
			xr(_G.mouse,'x','mx'); xr(_G.mouse,'y','my'); \
			xr(_G.mouse,'l','ml'); xr(_G.mouse,'m','mm'); xr(_G.mouse,'r','mr'); \
			xr(_G.mouse,'w','mw');  \
		");

		//VII.c. Joystick Variables
		LUA_BIND_R(joy_active);
		LUA_BIND_R(joy_up); LUA_BIND_R(joy_down); LUA_BIND_R(joy_left); LUA_BIND_R(joy_right);
		LUA_BIND_R(joy_analogx); LUA_BIND_R(joy_analogy); 
		LUA_BIND_R(joy_button);
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_collection('joy', function(table) \
				table:___ir('active',___get_joy_active); \
				table:___ir('up',___get_joy_up); \
				table:___ir('down',___get_joy_down); \
				table:___ir('left',___get_joy_left); \
				table:___ir('right',___get_joy_right); \
				table:___ir('analogx',___get_joy_analogx); \
				table:___ir('analogy',___get_joy_analogy); \
				table:___embed_array_collection('button', function(btn) return ___get_joy_button(table.___index,btn) end, nil); \
			end);\
		");
		//note that joy[x].button[y] is an unusual case and so we insert some crazy logic here. hopefully there are no more cases like that

		//VII.d. Entity Variables
		LUA_BIND_R(entities);
		LUA_BIND_RW(ent_x); LUA_BIND_RW(ent_y);
		LUA_BIND_RW(ent_specframe);
		LUA_BIND_R(ent_frame);
		LUA_BIND_R(ent_hotx); LUA_BIND_R(ent_hoty); LUA_BIND_R(ent_hotw); LUA_BIND_R(ent_hoth);
		LUA_BIND_R(ent_movecode);
		LUA_BIND_RW(ent_face); LUA_BIND_RW(ent_speed); LUA_BIND_RW(ent_visible);
		LUA_BIND_RW(ent_obstruct); LUA_BIND_RW(ent_obstructable);
		LUA_BIND_R(ent_script);
		LUA_BIND_R(ent_chr);
		grr |= luaL_dostring(L,"\
			_G:___r('entities',___get_entities); \
			_G:___embed_magic_collection('entity', function(table) \
				table:___irw('x',___get_ent_x,___set_ent_x); \
				table:___irw('y',___get_ent_y,___set_ent_y); \
				table:___irw('specframe',___get_ent_specframe,___set_ent_specframe); \
				table:___ir('frame',___get_ent_frame); \
				table:___ir('hotx',___get_ent_hotx); \
				table:___ir('hoty',___get_ent_hoty); \
				table:___ir('hotw',___get_ent_hotw); \
				table:___ir('hoth',___get_ent_hoth); \
				table:___ir('movecode',___get_ent_movecode); \
				table:___irw('face',___get_ent_face,___set_ent_face); \
				table:___irw('speed',___get_ent_face,___set_ent_face); \
				table:___irw('visible',___get_ent_visible,___set_ent_visible); \
				table:___irw('obstruct',___get_ent_obstruct,___set_ent_obstruct); \
				table:___irw('obstructable',___get_ent_obstructable,___set_ent_obstructable); \
				table:___ir('script',___get_ent_script); \
				table:___ir('chr',___get_ent_chr); \
			end);");

		
		//VII.e. Sprite Variables
		LUA_BIND_RW(sprite_x); LUA_BIND_RW(sprite_y);
		LUA_BIND_RW(sprite_sc);
		LUA_BIND_RW(sprite_image); LUA_BIND_RW(sprite_lucent);
		LUA_BIND_RW(sprite_addsub); LUA_BIND_RW(sprite_alphamap);
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_collection('sprite', function(table) \
				table:___irw('x',___get_sprite_x,___set_sprite_x); \
				table:___irw('x',___get_sprite_y,___set_sprite_y); \
				table:___irw('x',___get_sprite_sc,___set_sprite_sc); \
				table:___irw('x',___get_sprite_image,___set_sprite_image); \
				table:___irw('x',___get_sprite_lucent,___set_sprite_lucent); \
				table:___irw('x',___get_sprite_addsub,___set_sprite_addsub); \
				table:___irw('x',___get_sprite_alphamap,___set_sprite_alphamap); \
			end);");

		//VII.f. Camera Variables
		LUA_BIND_RW(xwin); LUA_BIND_RW(ywin); 
		LUA_BIND_RW(cameratracking); LUA_BIND_RW(cameratracker); 
		grr |= luaL_dostring(L,"\
			r(_G,'xwin'); \
			r(_G,'ywin'); \
			r(_G,'cameratracking'); \
			r(_G,'cameratracker'); \
		");

		//VII.g. Map Variables
		LUA_BIND_R(curmap_w); LUA_BIND_R(curmap_h); 
		LUA_BIND_R(curmap_startx); LUA_BIND_R(curmap_starty); 
		LUA_BIND_R(curmap_tileset); 
		LUA_BIND_R(curmap_name);  LUA_BIND_R(curmap_rstring); 
		LUA_BIND_R(curmap_music);  LUA_BIND_R(curmap_path); 
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_table('curmap'); \
			xr(_G.curmap,'w','curmap_w'); xr(_G.curmap,'h','curmap_h'); \
			xr(_G.curmap,'startx','curmap_startx'); xr(_G.curmap,'starty','curmap_starty'); \
			xr(_G.curmap,'tileset','curmap_tileset'); \
			xr(_G.curmap,'name','curmap_name'); xr(_G.curmap,'rstring','curmap_rstring'); \
			xr(_G.curmap,'music','curmap_music'); xr(_G.curmap,'path','curmap_path'); \
		");

		//VII.h. Layer Variables
		LUA_BIND_RW(layer_visible);
		LUA_BIND_RW(layer_lucent);
		LUA_BIND_RW(layer_parallaxx); LUA_BIND_RW(layer_parallaxy);
		LUA_BIND_R(layer_w); LUA_BIND_R(layer_h);
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_collection('layer', function(table) \
				table:___irw('visible',___get_layer_visible,___set_layer_visible); \
				table:___irw('lucent',___get_layer_lucent,___set_layer_lucent); \
				table:___irw('parallaxx',___get_layer_parallaxx,___set_layer_parallaxx); \
				table:___irw('parallaxy',___get_layer_parallaxy,___set_layer_parallaxy); \
				table:___ir('w',___get_layer_w); \
				table:___ir('h',___get_layer_h); \
			end);");

		//VII.i. Event Variables
		LUA_BIND_R(event_tx); LUA_BIND_R(event_ty); LUA_BIND_R(event_zone); 
		LUA_BIND_R(event_entity); LUA_BIND_R(event_param); 
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_table('event'); \
			xr(_G.event,'tx','event_tx'); xr(_G.event,'ty','event_ty'); \
			xr(_G.event,'zone','event_zone'); xr(_G.event,'entity','event_entity'); \
			xr(_G.event,'param','event_param'); \
		");

		//VII.j. Date/Time Variables
		LUA_BIND_R(sysdate_year); LUA_BIND_R(sysdate_month); 
		LUA_BIND_R(sysdate_day); LUA_BIND_R(sysdate_dayofweek); 
		LUA_BIND_R(sysdate_hour); LUA_BIND_R(sysdate_minute); LUA_BIND_R(sysdate_second); 
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_table('sysdate'); \
			xr(_G.sysdate,'year','sysdate_year'); xr(_G.sysdate,'month','sysdate_month'); \
			xr(_G.sysdate,'day','sysdate_day'); xr(_G.sysdate,'dayofweek','sysdate_dayofweek'); \
			xr(_G.sysdate,'hour','sysdate_hour'); xr(_G.sysdate,'minute','sysdate_minute'); xr(_G.sysdate,'second','sysdate_second'); \
		");

		//VII.k. Clipboard Variables
		LUA_BIND_RW(clipboard_text);
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_table('clipboard'); \
			xrw(_G.clipboard,'text','clipboard_text'); \
		");

		///VIII.a. Version
		LUA_BIND_R(version); LUA_BIND_R(build); LUA_BIND_R(os); 
		grr |= luaL_dostring(L,"\
			_G:___embed_magic_table('clipboard'); \
			r(_G,'_version','version'); \
			r(_G,'_build','build'); \
			r(_G,'_os','os'); \
		");

		grr |= luaL_dostring(L,"\
			_G:___r('SCAN_ESC',function() return 01 end); \
			_G:___r('SCAN_1',function() return 02 end); \
			_G:___r('SCAN_2',function() return 03 end); \
			_G:___r('SCAN_3',function() return 04 end); \
			_G:___r('SCAN_4',function() return 05 end); \
			_G:___r('SCAN_5',function() return 06 end); \
			_G:___r('SCAN_6',function() return 07 end); \
			_G:___r('SCAN_7',function() return 08 end); \
			_G:___r('SCAN_8',function() return 09 end); \
			_G:___r('SCAN_9',function() return 10 end); \
			_G:___r('SCAN_0',function() return 11 end); \
			_G:___r('SCAN_MINUS',function() return 12 end); \
			_G:___r('SCAN_EQUALS',function() return 13 end); \
			_G:___r('SCAN_BACKSP',function() return 14 end); \
			_G:___r('SCAN_TAB',function() return 15 end); \
			_G:___r('SCAN_Q',function() return 16 end); \
			_G:___r('SCAN_W',function() return 17 end); \
			_G:___r('SCAN_E',function() return 18 end); \
			_G:___r('SCAN_R',function() return 19 end); \
			_G:___r('SCAN_T',function() return 20 end); \
			_G:___r('SCAN_Y',function() return 21 end); \
			_G:___r('SCAN_U',function() return 22 end); \
			_G:___r('SCAN_I',function() return 23 end); \
			_G:___r('SCAN_O',function() return 24 end); \
			_G:___r('SCAN_P',function() return 25 end); \
			_G:___r('SCAN_LANGLE',function() return 26 end); \
			_G:___r('SCAN_RANGLE',function() return 27 end); \
			_G:___r('SCAN_ENTER',function() return 28 end); \
			_G:___r('SCAN_CTRL',function() return 29 end); \
			_G:___r('SCAN_A',function() return 30 end); \
			_G:___r('SCAN_S',function() return 31 end); \
			_G:___r('SCAN_D',function() return 32 end); \
			_G:___r('SCAN_F',function() return 33 end); \
			_G:___r('SCAN_G',function() return 34 end); \
			_G:___r('SCAN_H',function() return 35 end); \
			_G:___r('SCAN_J',function() return 36 end); \
			_G:___r('SCAN_K',function() return 37 end); \
			_G:___r('SCAN_L',function() return 38 end); \
			_G:___r('SCAN_SCOLON',function() return 39 end); \
			_G:___r('SCAN_QUOTA',function() return 40 end); \
			_G:___r('SCAN_RQUOTA',function() return 41 end); \
			_G:___r('SCAN_LSHIFT',function() return 42 end); \
			_G:___r('SCAN_BSLASH',function() return 43 end); \
			_G:___r('SCAN_Z',function() return 44 end); \
			_G:___r('SCAN_X',function() return 45 end); \
			_G:___r('SCAN_C',function() return 46 end); \
			_G:___r('SCAN_V',function() return 47 end); \
			_G:___r('SCAN_B',function() return 48 end); \
			_G:___r('SCAN_N',function() return 49 end); \
			_G:___r('SCAN_M',function() return 50 end); \
			_G:___r('SCAN_COMMA',function() return 51 end); \
			_G:___r('SCAN_DOT',function() return 52 end); \
			_G:___r('SCAN_SLASH',function() return 53 end); \
			_G:___r('SCAN_RSHIFT',function() return 54 end); \
			_G:___r('SCAN_STAR',function() return 55 end); \
			_G:___r('SCAN_ALT',function() return 56 end); \
			_G:___r('SCAN_SPACE',function() return 57 end); \
			_G:___r('SCAN_CAPS',function() return 58 end); \
			_G:___r('SCAN_F1',function() return 59 end); \
			_G:___r('SCAN_F2',function() return 60 end); \
			_G:___r('SCAN_F3',function() return 61 end); \
			_G:___r('SCAN_F4',function() return 62 end); \
			_G:___r('SCAN_F5',function() return 63 end); \
			_G:___r('SCAN_F6',function() return 64 end); \
			_G:___r('SCAN_F7',function() return 65 end); \
			_G:___r('SCAN_F8',function() return 66 end); \
			_G:___r('SCAN_F9',function() return 67 end); \
			_G:___r('SCAN_F10',function() return 68 end); \
			_G:___r('SCAN_NUMLOCK',function() return 69 end); \
			_G:___r('SCAN_SCRLOCK',function() return 70 end); \
			_G:___r('SCAN_HOME',function() return 71 end); \
			_G:___r('SCAN_UP',function() return 72 end); \
			_G:___r('SCAN_PGUP',function() return 73 end); \
			_G:___r('SCAN_GMINUS',function() return 74 end); \
			_G:___r('SCAN_LEFT',function() return 75 end); \
			_G:___r('SCAN_PAD_5',function() return 76 end); \
			_G:___r('SCAN_RIGHT',function() return 77 end); \
			_G:___r('SCAN_GPLUS',function() return 78 end); \
			_G:___r('SCAN_END',function() return 79 end); \
			_G:___r('SCAN_DOWN',function() return 80 end); \
			_G:___r('SCAN_PGDN',function() return 81 end); \
			_G:___r('SCAN_INSERT',function() return 82 end); \
			_G:___r('SCAN_DEL',function() return 83 end); \
			_G:___r('SCAN_F11',function() return 87 end); \
			_G:___r('SCAN_F12',function() return 88 end); \
			");

		//other misc defines
		grr |= luaL_dostring(L,"\
			_G:___r('screen',function() return 1 end);  \
			_G:___r('FILE_READ',function() return 1 end); \
			_G:___r('FILE_WRITE_APPEND',function() return 3 end); \
			_G:___r('SEEK_SET',function() return 0 end); \
			_G:___r('SEEK_CUR',function() return 1 end); \
			_G:___r('SEEK_END',function() return 2 end); \
			_G:___r('CF_NONE',function() return 0 end); \
			_G:___r('CF_GREY',function() return 1 end); \
			_G:___r('CF_GREYINV',function() return 2 end); \
			_G:___r('CF_INV',function() return 3 end); \
			_G:___r('CF_RED',function() return 4 end); \
			_G:___r('CF_GREEN',function() return 5 end); \
			_G:___r('CF_BLUE',function() return 6 end); \
			_G:___r('CF_CUSTOM',function() return 7 end); \
			_G:___r('FIXED_PI',function() return 205887 end); \
		");



	//			{"FILE_READ",	"1" },
	//{"FILE_WRITE",	"2" },
	//{"FILE_WRITE_APPEND",	"3" }, // Overkill (2006-07-05): Append mode added.
	//{"SEEK_SET",    "0" },
	//{"SEEK_CUR",    "1" },
	//{"SEEK_END",    "2" },

		/*grr |= luaL_dostring(L,"do \
			_G:___r('screen',___get_screen); \
			\
			_G:___embed_magic_table('mouse'); \
			_G.mouse:___rw('x',___get_mx,___set_mx); \
			_G.mouse:___rw('y',___get_my,___set_my); \
			\
			_G:___embed_magic_collection('entities',function(table) \
				table:___irw('tx',___get_ent_tx,___set_ent_tx);\
				table:___irw('yx',___get_ent_ty,___set_ent_ty);\
			end);\
			\
			___magic_table(_G:___embed('joy'));\
			_G.joy:___r('active',___get_joy_active); \
			_G.joy:___embed_array_collection('button',  function(idx) return ___get_joy_button(idx) end);\
			\
		end");*/

			if(grr) err("grr");


		//cleanup some functions we were using for convenience
		grr |= luaL_dostring(L,"_G.r = nil; _G.w = nil; _G.rw = nil");
		grr |= luaL_dostring(L,"_G.xr = nil; _G.xw = nil; _G.xrw = nil");


	if(grr) err("grr");

	//examples use cases:
	//timer
	//entities[10].tx
	//mouse.x
	//joy.buttons[0]

	int zzz=9;
	
	//-------------------------
	//funcs here
	//system things
	
	//gfxthings
	LUA_FUNC(LoadImage);
	LUA_FUNC(rectfill);
	LUA_FUNC(makecolor);
	LUA_FUNC(printstring);
	LUA_FUNC(tblit);
	LUA_FUNC(showpage);
	//-------------------------


		
}

//old crap

  //static void set(lua_State *L, int table_index, const char *key) {
  //  lua_pushstring(L, key);
  //  lua_insert(L, -2);  // swap value and key
  //  lua_settable(L, table_index);
  //}


//const char *msg = lua_tostring(L, -1); get last error
//printf(lua_tostring(L, -1));




//int __newindex(lua_State *L) {
//	const char *val = lua_tostring(L,-1);
//	const char *key = lua_tostring(L,-2);
//	//const void *tbl = lua_topointer(L,-3); //dont need this
//
//	if(!strcmp(key,"smack")) {
//		smack = string(val) + "SEX";
//		lua_pop(L,3); //pop all
//	} else {
//		lua_rawset(L,-3);
//		lua_pop(L,1); //pop table
//	}
//
//	return 0;
//}
//
//int __index(lua_State *L) {
//	const char *key = lua_tostring(L,-1);
//	//const void *tbl = lua_topointer(L,-2); //dont need this
//
//	if(!strcmp(key,"smack")) {
//		lua_pop(L,2);
//		lua_pushstring(L,smack.c_str());
//	} else {
//		//lua_getfield(L, LUA_GLOBALSINDEX, L);
//		lua_rawget(L,-2);
//		lua_remove(L,-2); //remove the table
//	}
//
//	
//
//	return 1;
//
//}

