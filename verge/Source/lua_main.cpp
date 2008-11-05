#include "xerxes.h"

#ifdef ENABLE_LUA

#include "lua_main.h"
#include <luabind/object.hpp>
#include <stdio.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


//http://www.codeproject.com/cpp/luaincpp.asp

bool LUA::CompileMap(const char *f) {

	VFILE *si = vopen(va("%s.lua", f));
	if (!si) ::err("unable to open %s.lua", f);
	int scriptlen = filesize(si);
	std::auto_ptr<char> temp(new char[scriptlen+1]);
	vread(temp.get(),scriptlen,si);
	vclose(si);

	VFILE *mi = vopen(va("%s.map", f));
	if (!mi) ::err("unable to open %s.map", f);
	vseek(mi, 10, 0);

	int mapcoresize;
	vread(&mapcoresize, 4, mi);
	log("DESTINATION POS: %d", mapcoresize);
	vseek(mi, 0, 0);
	byte* buf = new byte[mapcoresize];
	vread(buf, mapcoresize, mi);
	vclose(mi);

	FILE *mo = fopen(va("%s.map", f), "wb");
	if (!mo)
		::err("couldn't open map for writing");
	fwrite(buf, 1, mapcoresize, mo);
	delete[] buf;

	// we need to endian flip this so that it gets read properly
	flip(&scriptlen,4);
	
	fwrite(&scriptlen,4,1,mo);
	fwrite(temp.get(),1,scriptlen,mo);
	fclose(mo);

	return true;
}


void LUA::compileSystem() {
	loadFile("system.lua");
}

void LUA::LoadMapScript(VFILE *f) {
	int scriptsize;
	vread(&scriptsize,4,f);
	std::auto_ptr<char> temp(new char[scriptsize+1]);
	vread(temp.get(),scriptsize,f);
	temp.get()[scriptsize] = 0;
	luaL_dostring(L,temp.get());
}

void LUA::loadFile(const char *fname) {

	boost::shared_array<byte> buf = vreadfile(fname);
	if(!buf.get())
		err("Error loading " + std::string(fname));

	if(luaL_loadbuffer(L, (char*)buf.get()+4, *(int*)buf.get(), fname))
		err("Error loading " + std::string(fname));

	if(lua_pcall(L, 0,0,0))
		err("Error compiling " + std::string(fname));
}

void LUA::ExecAutoexec() {
	ExecuteFunctionString("autoexec");
	//lua_getglobal(L, "autoexec");
	//if(lua_pcall(L, 0,0,0))
	//	err("Error calling autoexec");
}


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

#define GLOBAL_NAMESPACE "v3"
#define LUA_FUNC(name) { module(L) [ def(#name, ___##name) ]; }
#define LUA_BIND(name) { module(L) [ def(#name, ##name)]; }
#define LUA_BIND_R(name)  { module(L) [ def("___get_"#name,___get_##name) ]; }
#define LUA_BIND_W(name)  { module(L) [ def("___set_"#name,___set_##name) ]; }
#define LUA_BIND_RW(name) { LUA_BIND_R(name); LUA_BIND_W(name); }

//VII.a. General System Variables
int ___get_systemtime() { return systemtime; }
int ___get_timer() { return timer; }
void ___set_timer(int val) { timer = val; }
int ___get_lastpressed() { return lastpressed; }
void ___set_lastpressed(int val) { lastpressed = val; }
int ___get_lastkey() { return lastkey; }
void ___set_lastkey(int val) { lastkey = val; }
bool ___get_key(int ofs) { if (ofs>=0 && ofs<256) return keys[ofs]!=0; else return false; }
void ___set_key(int ofs, bool val) { if (ofs>=0 && ofs<256) keys[ofs] = val; }

//VII.b. Mouse Variables
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

//VII.c. Joystick Variables
//todo - bounds
int ___get_joy_active(int idx) { if (idx>=0 && idx<4) return sticks[idx].active; else return 0; }
int ___get_joy_up(int idx) { if (idx>=0 && idx<4)return sticks[idx].up; else return 0; }
int ___get_joy_down(int idx) { if (idx>=0 && idx<4) return sticks[idx].down; else return 0; }
int ___get_joy_left(int idx) { if (idx>=0 && idx<4) return sticks[idx].left; else return 0; }
int ___get_joy_right(int idx) { if (idx>=0 && idx<4) return sticks[idx].right; else return 0; }
int ___get_joy_analogx(int idx) { if (idx>=0 && idx<4) return sticks[idx].analog_x; else return 0; }
int ___get_joy_analogy(int idx) { if (idx>=0 && idx<4) return sticks[idx].analog_y; else return 0; }
int ___get_joy_button(int idx, int btn) { if (idx>=0 && idx<4) return sticks[idx].button[btn]; else return 0; }

//-----------------------------
//VII.d. Entity Variables
//todo - bounds
int ___get_entities() { return entities; }

int ___get_ent_x(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->getx(); else return 0; }
void ___set_ent_x(int ofs, int value) { if (ofs>=0 && ofs<entities) entity[ofs]->setxy(value, entity[ofs]->gety()); }
int ___get_ent_y(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->gety(); else return 0;}
void ___set_ent_y(int ofs, int value) { if (ofs>=0 && ofs<entities) entity[ofs]->setxy(entity[ofs]->getx(), value); }
int ___get_ent_specframe(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->specframe; else return 0; }
void ___set_ent_specframe(int ofs, int value) { if (ofs>=0 && ofs<entities) entity[ofs]->specframe = value; }

int ___get_ent_frame(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->frame; else return 0; }
int ___get_ent_hotx(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hx; else return 0; }
int ___get_ent_hoty(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hy; else return 0; }
int ___get_ent_hotw(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hw; else return 0; }
int ___get_ent_hoth(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hh; else return 0; }
int ___get_ent_movecode(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->movecode; else return 0; }

int ___get_ent_face(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->face; else return 0; }
void ___set_ent_face(int ofs, int value) { if (ofs>=0 && ofs<entities) entity[ofs]->setface(value); }
int ___get_ent_speed(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->speed; else return 0; }
void ___set_ent_speed(int ofs, int value) { if (ofs>=0 && ofs<entities) entity[ofs]->setspeed(value); }
bool ___get_ent_visible(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->visible; else return 0; }
void ___set_ent_visible(int ofs, bool value) { if (ofs>=0 && ofs<entities) entity[ofs]->visible = value; }
bool ___get_ent_obstruct(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->obstruction; else return 0; }
void ___set_ent_obstruct(int ofs, bool value) { if (ofs>=0 && ofs<entities) entity[ofs]->obstruction = value; }
bool ___get_ent_obstructable(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->obstructable; else return 0; }
void ___set_ent_obstructable(int ofs, bool value) { if (ofs>=0 && ofs<entities) entity[ofs]->obstructable = value; }
int ___get_ent_lucent(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->lucent; else return 0; }
void ___set_ent_lucent(int ofs, int value) { if (ofs>=0 && ofs<entities) entity[ofs]->lucent = value; }

std::string ___get_ent_script(int ofs) { if (ofs>=0 && ofs<entities) return entity[ofs]->script; else return ""; }
void ___set_ent_script(int ofs,std::string val) { if (ofs>=0 && ofs<entities) se->Set_EntityActivateScript(ofs, val); }

std::string ___get_ent_chr(int ofs) { if (ofs>=0 && ofs<entities) return se->Get_EntityChr(ofs); else return ""; }
void ___set_ent_chr(int ofs, std::string chr) { if (ofs>=0 && ofs<entities) se->Set_EntityChr(ofs,chr); }
int ___get_ent_framew(int ofs) { if (ofs>=0 && ofs<entities) return se->Get_EntityFrameW(ofs); else return 0; }
int ___get_ent_frameh(int ofs) { if (ofs>=0 && ofs<entities) return se->Get_EntityFrameH(ofs); else return 0; }
std::string ___get_ent_description(int ofs) { if (ofs>=0 && ofs<entities) return se->Get_EntityDescription(ofs); else return 0; }
void ___set_ent_description(int ofs, std::string val) { if (ofs>=0 && ofs<entities) se->Set_EntityDescription(ofs, val); }


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
int ___get_layer_lucent(int ofs) { if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->lucent; else return 0; }
void ___set_layer_lucent(int ofs, int value) { if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->lucent = value; }
double ___get_layer_parallaxx(int ofs) { if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->parallax_x; else return 0; }
void ___set_layer_parallaxx(int ofs, double value) { if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->SetParallaxX(value); }
double ___get_layer_parallaxy(int ofs) { if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->parallax_y; else return 0; }
void ___set_layer_parallaxy(int ofs, double value) { if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->SetParallaxY(value); }
int ___get_layer_w(int ofs) { if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->width; else return 0; }
int ___get_layer_h(int ofs) { if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->height; else return 0; }

//VII.i. Zone Variables -- Overkill(2007-09-04)
std::string ___get_zone_name(int ofs) { if (current_map && ofs >= 0 && ofs < current_map->numzones) return current_map->zones[ofs]->name; else return ""; }
std::string ___get_zone_event(int ofs) { if (current_map && ofs >= 0 && ofs < current_map->numzones) return current_map->zones[ofs]->script; else return ""; }

//VII.j. Event Variables
int ___get_event_tx() { return event_tx; }
int ___get_event_ty() { return event_ty; }
int ___get_event_zone() { return event_zone; }
int ___get_event_entity() { return event_entity; }
int ___get_event_param() { return event_param; }

//VII.k. Date/Time Variables
int ___get_sysdate_year() { return getYear(); }
int ___get_sysdate_month() { return getMonth(); }
int ___get_sysdate_day() { return getDay(); }
int ___get_sysdate_dayofweek() { return getDayOfWeek(); }
int ___get_sysdate_hour() { return getHour(); }
int ___get_sysdate_minute() { return getMinute(); }
int ___get_sysdate_second() { return getSecond(); }

//VII.l. Clipboard Variables
std::string ___get_clipboard_text() { return clipboard_getText(); }
void ___set_clipboard_text(std::string str) { clipboard_setText(str.c_str()); }

///VIII.a. Version
std::string ___get_version() { return DEF_VERSION; }
std::string ___get_build() { return DEF_BUILD; }
std::string ___get_os() { return DEF_OS; }

// Error handler. Don't shove this in a package, since this is important to the Lua environment.
int InitErrorHandler(lua_State *L)
{
	return luaL_dostring(L,"function __err(e)\n\
		local level = 1 \n\
		local s = \"\" \n\
		\n\
		local function explode(div, str) \n\
			if (div=='') then return false end \n\
			if (not div or not str) then return false end \n\
			local pos,arr = 0,{} \n\
			-- for each divider found \n\
			for st,sp in function() return string.find(str,div,pos,true) end do \n\
				table.insert(arr,string.sub(str,pos,st-1)) -- Attach chars left of current divider \n\
				pos = sp + 1 -- Jump past current divider \n\
			end \n\
			table.insert(arr,string.sub(str,pos)) -- Attach chars right of last divider \n\
			return arr \n\
		end \n\
		\n\
		-- Start printing all trace after the offending line. We don't want to trace the error handler or the error() call if there was level modifiers. \n\
		local match = false \n\
		local t = explode(\":\", e) \n\
		if not t then return \"Error was so bad the error handler failed too.\" end \n\
		local filename, line, message = unpack(t) \n\
		line = tonumber(line) \n\
		if not message then \n\
			filename = \"?\" \n\
			line = 0 \n\
			message = e \n\
			match = true \n\
		end \n\
		\n\
		s = s .. message .. \"\\n\" \n\
		s = s .. \"  Traceback (most recent call first):\\n\" \n\
		while true do \n\
			local info = debug.getinfo(level, \"Sln\") \n\
			if not info then \n\
				break \n\
			end \n\
			\n\
			s = s .. \"    File '\" .. info.short_src .. \"', line \" .. tostring(info.currentline) \n\
			if info.name or info.namewhat then \n\
				s = s .. \", in\" \n\
				if info.namewhat then \n\
					if info.namewhat ~= \"\" then \n\
						s = s .. \" \" .. info.namewhat \n\
					else \n\
						s = s .. \" (function)\" \n\
					end \n\
				end \n\
				if info.name and info.name ~= \"\" then \n\
					s = s .. \" \" .. info.name \n\
				end \n\
			end \n\
			s = s .. \"\\n\" \n\
			level = level + 1 \n\
		end \n\
		return s \n\
	end\n");
}

void LUA::bindApi() {
	using namespace luabind;
	open(L);



	//TODO -- add logic to lua global arrays to perform bounds checking


	//install the metatable and utility functions
	int grr = 0;

	// Error handler. Don't shove this in a package, since this is important to the Lua environment.
	grr |= InitErrorHandler(L);

	grr |= luaL_dostring(L,"function ___getmetatable(tbl) \
				local meta = getmetatable(tbl) \
				if(meta) then return meta else return {} end \
			end");
	grr |= luaL_dostring(L,"function ___set_fail_readonly(k,v) \
			error('builtin global variable `'..k..'` is readonly', 2)\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_readonly_blah() \
			error('builtin global variable is readonly', 2)\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_needmember(k,v) \
			error('builtin global variable `'..k..'` must be accessed by member, i.e. `'..k..'`.something', 2)\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_systemvar(k,v) \
			error('builtin global variable `'..k..'` cannot be set directly. Please drilldown or index as appropriate.', 2)\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_array(k,v) \
			error('builtin global array `'..k..'` cannot be accessed directly. Please index it.', 2)\
			end");
	grr |= luaL_dostring(L,"function ___get_fail_array(k) \
			error('builtin global array `'..k..'` cannot be accessed directly. Please index it.', 2)\
			end");
	grr |= luaL_dostring(L,"function ___set_fail_func(k) \
			error('`'..k..'` is a builtin global function. You are trying to overwrite it. Please do not.', 2)\
			end");
	grr |= luaL_dostring(L," function ___getname(self) \
			if(self.___parent) then return ___getname(self.___parent)..self.___name; \
			else return '::'; end \
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
				else error(k..' 1 does not exist error tbd', 2); end \
			end \
			function meta.__newindex(t, k, v) \
				local tk = string.lower(k);\
				if table.___setters[tk] then table.___setters[tk](v) \
				elseif allowdefault then rawset(t, k, v) \
				elseif(string.sub(k,1,3) == '___') then rawset(t, k, v)\
				else error(k..' 2 does not exist error tbd', 2); end \
			end \
			function table:___rw(name,getter,setter) \
				name = string.lower(name);\
				if(getter) then self.___getters[name] = getter; else self.___getters[name] = function() error('3 \\'' .. name .. '\\' is writeonly error tbd', 2) end; end \
				if(setter) then self.___setters[name] = setter; else self.___setters[name] = function(v) error('4 \\'' .. name .. '\\' is readonly error tbd', 2) end; end \
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
				function table:___ir(name,rfunc) table:___rw(name, function() if rfunc then return rfunc(index) else error('system array cannot be read. tbd better error message', 2) end end, nil) end; \
				function table:___iw(name,wfunc) table:___rw(name, nil, function(val) if wfunc then return wfunc(index, val) else error('system array cannot be written to. tbd better error message', 2) end end) end; \
				function table:___irw(name,rfunc,wfunc) table:___rw(name, function() if rfunc then return rfunc(index) else error('system array cannot be read. tbd better error message', 2) end end, function(val) if wfunc then return wfunc(index, val) else error('system array cannot be written. tbd better error message', 2) end end) end; \
				makeitem(table); \
				return table; \
			end \
			function ___make_magic_collection(table, makeitem) \
				\
				local meta = ___getmetatable(table); \
				function meta.__index(t,k) return ___magic_collection_getindex(table, k, makeitem) end \
				function meta.__newindex(t,k,v) error('invalid attempt to set system array index. tbd better error message', 2) end \
				setmetatable(table,meta); \
			end  \
			function ___make_array_collection(table, getitem, setitem) \
				\
				local meta = ___getmetatable(table); \
				if(getitem) then function meta.__index(t,k) return getitem(k) end \
				else function meta.__index(t,k) error('invalid attempt to get system array index. tbd better error message', 2) end end \
				if(setitem) then function meta.__newindex(t,k,v) setitem(k,v) end \
				else function meta.__newindex(t,k,v) error('invalid attempt to set system array index. tbd better error message', 2) end end \
				setmetatable(table,meta); \
			end");

		if(grr) err("grr");

		//OVERKILL: use this instead of the _G one below to put things in the other namespace
		grr |= luaL_dostring(L,"\
		v3 = {}; \
		___xmagic_table(v3,true); \
		");
		
		//special setup for _G
		//grr |= luaL_dostring(L,"\
		//___xmagic_table(_G,true); \
		//");
		//#define GLOBAL_NAMESPACE "_G"


		#define SEFUNC(name) { { module(L) [ def("___"#name, ScriptEngine:: name) ]; }  grr |= luaL_dostring(L,GLOBAL_NAMESPACE":___rw('"#name"',function() return ___"#name" end,function(val) ___set_fail_func('"#name"') end);"); }
		#define FUNC(name) { { module(L) [ def("___"#name, ___##name) ]; }  grr |= luaL_dostring(L,GLOBAL_NAMESPACE":___rw('"#name"',function() return ___"#name" end,function(val) ___set_fail_func('"#name"') end);"); }
		#define TODO(name) {  grr |= luaL_dostring(L,GLOBAL_NAMESPACE":___rw('"#name"',function() error('`"#name"` is not implemented yet in lua. Post a bug or consider whether you need it', 2) end,function(val) ___set_fail_func('"#name"') end);"); }

		//VI.a. General Utility Functions
		TODO(CallFunction);
		SEFUNC(Exit);
		TODO(FunctionExists);
		TODO(GetInt);
		TODO(GetIntArray);
		TODO(GetString);
		TODO(GetStringArray);
		SEFUNC(HookButton);
		SEFUNC(HookKey);
		SEFUNC(HookRetrace);
		SEFUNC(HookTimer);
		SEFUNC(Log);
		SEFUNC(MessageBox);
		SEFUNC(Random);
		SEFUNC(SetAppName);
		SEFUNC(SetButtonJB);
		SEFUNC(SetButtonKey);
		TODO(SetInt);
		TODO(SetIntArray);
		SEFUNC(SetRandSeed);
		SEFUNC(SetResolution);
		TODO(SetString);
		TODO(SetStringArray);
		SEFUNC(Unpress);
		SEFUNC(UpdateControls);
		//VI.b. String Functions
		SEFUNC(Asc);
		SEFUNC(Chr);
		SEFUNC(GetToken);
		SEFUNC(Left);
		SEFUNC(Len);
		SEFUNC(Mid);
		SEFUNC(Right);
		SEFUNC(Str);
		SEFUNC(Strcmp);
		SEFUNC(Strdup);
		SEFUNC(TokenCount);
		SEFUNC(ToLower);
		SEFUNC(ToUpper);
		SEFUNC(Val);
		//VI.c. Dictionary Functions
		TODO(DictContains);
		TODO(DictFree);
		TODO(DictGetInt);
		TODO(DictGetString);
		TODO(DictNew);
		TODO(DictRemove);
		TODO(DictSetInt);
		TODO(DictSetString);
		TODO(DictSize);
		//VI.d. Map Functions
		SEFUNC(GetObs);
		SEFUNC(GetObsPixel);
		SEFUNC(GetTile);
		SEFUNC(GetZone);
		SEFUNC(Map);
		SEFUNC(Render);
		SEFUNC(RenderMap);
		SEFUNC(SetObs);
		SEFUNC(SetTile);
		SEFUNC(SetZone);
		//VI.e. Entity Functions
		SEFUNC(ChangeCHR);
		SEFUNC(EntityMove);
		SEFUNC(EntitySetWanderDelay);
		SEFUNC(EntitySetWanderRect);
		SEFUNC(EntitySetWanderZone);
		SEFUNC(EntitySpawn);
		SEFUNC(EntityStalk);
		SEFUNC(EntityStop);
		SEFUNC(Get_EntityFrameW);
		SEFUNC(Get_EntityFrameH);
		SEFUNC(HookEntityRender);
		SEFUNC(PlayerMove);
		SEFUNC(SetEntitiesPaused);
		SEFUNC(SetPlayer);
		SEFUNC(GetPlayer);  // Kildorf (2007-10-12)
		//VI.f. Graphics Functions
		SEFUNC(AdditiveBlit);
		SEFUNC(AlphaBlit);
		SEFUNC(Blit);
		SEFUNC(BlitEntityFrame);
		SEFUNC(BlitLucent);
		SEFUNC(BlitTile);
		SEFUNC(BlitWrap);
		SEFUNC(Circle);
		SEFUNC(CircleFill);
		SEFUNC(ColorFilter);
		SEFUNC(CopyImageToClipboard);
		SEFUNC(DuplicateImage);
		SEFUNC(FlipBlit);
		SEFUNC(FreeImage);
		SEFUNC(GetB);
		SEFUNC(GetG);
		SEFUNC(GetImageFromClipboard);
		SEFUNC(GetPixel);
		SEFUNC(GetR);
		SEFUNC(GrabRegion);
		SEFUNC(ImageHeight);
		SEFUNC(ImageShell);
		SEFUNC(ImageValid);
		SEFUNC(ImageWidth);
		SEFUNC(Line);
		SEFUNC(LoadImage);
		SEFUNC(LoadImage0);
		SEFUNC(MakeColor);
		SEFUNC(MixColor);
		SEFUNC(Mosaic);
		SEFUNC(NewImage);
		SEFUNC(Rect);
		SEFUNC(RectFill);
		SEFUNC(rgb);
		SEFUNC(RotScale);
		SEFUNC(ScaleBlit);
		SEFUNC(SetClip);
		SEFUNC(SetCustomColorFilter);
		SEFUNC(SetLucent);
		SEFUNC(SetPixel);
		SEFUNC(ShowPage);
		SEFUNC(Silhouette);
		SEFUNC(SubtractiveBlit);
		SEFUNC(SuperSecretThingy);
		SEFUNC(TAdditiveBlit);
		SEFUNC(TBlit);
		SEFUNC(TBlitLucent);
		SEFUNC(TBlitTile);
		SEFUNC(TGrabRegion);
		SEFUNC(Triangle);
		SEFUNC(TScaleBlit);
		SEFUNC(TSubtractiveBlit);
		SEFUNC(TWrapBlit);
		SEFUNC(WrapBlit);
		//VI.g. Sprite Functions
		SEFUNC(GetSprite);
		SEFUNC(ResetSprites);
		//VI.h. Sound/Music Functions
		SEFUNC(FreeSong);
		SEFUNC(FreeSound);
		SEFUNC(GetSongPos);
		SEFUNC(GetSongVolume);
		SEFUNC(LoadSong);
		SEFUNC(LoadSound);
		SEFUNC(PlayMusic);
		SEFUNC(PlaySong);
		SEFUNC(PlaySound);
		SEFUNC(SetMusicVolume);
		SEFUNC(SetSongPaused);
		SEFUNC(SetSongPos);
		SEFUNC(SetSongVolume);
		SEFUNC(StopMusic);
		SEFUNC(StopSong);
		SEFUNC(StopSound);
		//VI.i. Font Functions
		SEFUNC(EnableVariableWidth);
		SEFUNC(FontHeight);
		SEFUNC(FreeFont);
		SEFUNC(LoadFont);
		SEFUNC(LoadFontEx);
		SEFUNC(PrintCenter);
		SEFUNC(PrintRight);
		SEFUNC(PrintString);
		SEFUNC(TextWidth);
		//VI.j. Math Functions
		SEFUNC(acos);
		SEFUNC(facos);
		SEFUNC(asin);
		SEFUNC(fasin);
		SEFUNC(atan);
		SEFUNC(fatan);
		SEFUNC(atan2);
		SEFUNC(fatan2);
		SEFUNC(cos);
		SEFUNC(fcos);
		SEFUNC(pow);
		SEFUNC(sin);
		SEFUNC(fsin);
		SEFUNC(sqrt);
		SEFUNC(tan);
		SEFUNC(ftan);
		//VI.k. File Functions
		SEFUNC(FileClose);
		SEFUNC(FileCurrentPos);
		SEFUNC(FileEOF);
		SEFUNC(FileOpen);
		SEFUNC(FileReadByte);
		SEFUNC(FileReadln);
		SEFUNC(FileReadQuad);
		SEFUNC(FileReadString);
		SEFUNC(FileReadToken);
		SEFUNC(FileReadWord);
		SEFUNC(FileSeekLine);
		SEFUNC(FileSeekPos);
		SEFUNC(FileWrite);
		SEFUNC(FileWriteByte);
		SEFUNC(FileWriteln);
		SEFUNC(FileWriteQuad);
		SEFUNC(FileWriteString);
		SEFUNC(FileWriteWord);
		SEFUNC(ListFilePattern);
		SEFUNC(FileWriteCHR);
		SEFUNC(FileWriteMAP);
		SEFUNC(FileWriteVSP);
		//VI.l. Window Managment Functions
		SEFUNC(WindowClose);
		SEFUNC(WindowCreate);
		SEFUNC(WindowGetHeight);
		SEFUNC(WindowGetImage);
		SEFUNC(WindowGetWidth);
		SEFUNC(WindowGetXRes);
		SEFUNC(WindowGetYRes);
		SEFUNC(WindowHide);
		SEFUNC(WindowSetPosition);
		SEFUNC(WindowSetResolution);
		SEFUNC(WindowSetSize);
		SEFUNC(WindowSetTitle);
		SEFUNC(WindowShow);
		//VI.m. Movie Playback Functions
		SEFUNC(AbortMovie);
		SEFUNC(MovieClose);
		SEFUNC(MovieGetCurrFrame);
		SEFUNC(MovieGetFramerate);
		SEFUNC(MovieGetImage);
		SEFUNC(MovieLoad);
		SEFUNC(MovieNextFrame);
		SEFUNC(MoviePlay);
		SEFUNC(MovieRender);
		SEFUNC(MovieSetFrame);
		SEFUNC(PlayMovie);
		//VI.n. Netcode Functions
		SEFUNC(Connect);
		SEFUNC(GetConnection);
		SEFUNC(GetUrlImage);
		SEFUNC(GetUrlText);
		SEFUNC(SocketClose);
		SEFUNC(SocketConnected);
		SEFUNC(SocketGetFile);
		SEFUNC(SocketGetInt);
		SEFUNC(SocketGetString);
		SEFUNC(SocketHasData);
		SEFUNC(SocketSendFile);
		SEFUNC(SocketSendInt);
		SEFUNC(SocketSendString);
		SEFUNC(SetConnectionPort); // Overkill (2008-04-17): Socket port can be switched to something besides 45150.
		SEFUNC(SocketGetRaw); // Overkill (2008-04-17): Sockets can send and receive raw length-delimited strings
		SEFUNC(SocketSendRaw);
		SEFUNC(SocketByteCount);
		//XX: unsorted functions and variables, mostly newly added and undocumented
		SEFUNC(SoundIsPlaying);
		SEFUNC(RectVGrad);
		SEFUNC(RectHGrad);
		SEFUNC(RectRGrad);
		SEFUNC(Rect4Grad);
		SEFUNC(strovr);
		SEFUNC(WrapText);
		SEFUNC(strpos);
		SEFUNC(HSV);
		SEFUNC(GetH);
		SEFUNC(GetS);
		SEFUNC(GetV);
		SEFUNC(HueReplace);
		SEFUNC(ColorReplace);
		SEFUNC(GetKeyBuffer);
		SEFUNC(FlushKeyBuffer);
		SEFUNC(SetKeyDelay);


		//some handy functions
		grr |= luaL_dostring(L,"\
			function xr(table,name,funcname) table:___r(name,_G['___get_'..funcname]) end\
			function xw(table,name,funcname) table:___w(name,_G['___set_'..funcname]) end\
			function xrw(table,name,funcname)table:___rw(name,_G['___get_'..funcname],_G['___set_'..funcname]) end\
			function r(table,name) table:___r(name,_G['___get_'..name]) end\
			function w(table,name) table:___w(name,_G['___set_'..name]) end\
			function rw(table,name) table:___rw(name,_G['___get_'..name],_G['___set_'..name]) end\
		");

		//VII.a. General System Variables
		LUA_BIND_R(systemtime);
		LUA_BIND_RW(timer);
		LUA_BIND_RW(lastpressed);
		LUA_BIND_RW(lastkey)
		LUA_BIND_RW(key);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___r('systemtime',___get_systemtime);  \
			"GLOBAL_NAMESPACE":___rw('timer',___get_timer,___set_timer);  \
			"GLOBAL_NAMESPACE":___rw('lastpressed',___get_lastpressed,___set_lastpressed);  \
			"GLOBAL_NAMESPACE":___rw('lastkey',___get_lastkey,___set_lastkey);  \
			"GLOBAL_NAMESPACE":___embed_array_collection('key',  ___get_key, ___set_key);\
		");
		//VII.b. Mouse Variables
		LUA_BIND_RW(mx); LUA_BIND_RW(my);
		LUA_BIND_RW(ml); LUA_BIND_RW(mm); LUA_BIND_RW(mr);
		LUA_BIND_RW(mw);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_table('mouse'); \
			xrw("GLOBAL_NAMESPACE".mouse,'x','mx'); xrw("GLOBAL_NAMESPACE".mouse,'y','my'); \
			xrw("GLOBAL_NAMESPACE".mouse,'l','ml'); xrw("GLOBAL_NAMESPACE".mouse,'m','mm'); xrw("GLOBAL_NAMESPACE".mouse,'r','mr'); \
			xrw("GLOBAL_NAMESPACE".mouse,'w','mw'); \
		");

		//VII.c. Joystick Variables
		LUA_BIND_R(joy_active);
		LUA_BIND_R(joy_up); LUA_BIND_R(joy_down); LUA_BIND_R(joy_left); LUA_BIND_R(joy_right);
		LUA_BIND_R(joy_analogx); LUA_BIND_R(joy_analogy);
		LUA_BIND_R(joy_button);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_collection('joy', function(table) \
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
		LUA_BIND_RW(ent_script);
		LUA_BIND_RW(ent_chr);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___r('entities',___get_entities); \
			"GLOBAL_NAMESPACE":___embed_magic_collection('entity', function(table) \
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
				table:___irw('speed',___get_ent_speed,___set_ent_speed); \
				table:___irw('visible',___get_ent_visible,___set_ent_visible); \
				table:___irw('obstruct',___get_ent_obstruct,___set_ent_obstruct); \
				table:___irw('obstructable',___get_ent_obstructable,___set_ent_obstructable); \
				table:___irw('script',___get_ent_script,___set_ent_script); \
				table:___irw('chr',___get_ent_chr, ___set_ent_chr); \
				table:___irw('lucent',___get_ent_lucent,___set_ent_lucent); \
				table:___ir('framew',Get_EntityFrameW); \
				table:___ir('frameh',Get_EntityFrameH); \
				table:___irw('description',___get_ent_description,___set_ent_description); \
			end);");


		//VII.e. Sprite Variables
		LUA_BIND_RW(sprite_x); LUA_BIND_RW(sprite_y);
		LUA_BIND_RW(sprite_sc);
		LUA_BIND_RW(sprite_image); LUA_BIND_RW(sprite_lucent);
		LUA_BIND_RW(sprite_addsub); LUA_BIND_RW(sprite_alphamap);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_collection('sprite', function(table) \
				table:___irw('x',___get_sprite_x,___set_sprite_x); \
				table:___irw('y',___get_sprite_y,___set_sprite_y); \
				table:___irw('sc',___get_sprite_sc,___set_sprite_sc); \
				table:___irw('image',___get_sprite_image,___set_sprite_image); \
				table:___irw('lucent',___get_sprite_lucent,___set_sprite_lucent); \
				table:___irw('addsub',___get_sprite_addsub,___set_sprite_addsub); \
				table:___irw('alphamap',___get_sprite_alphamap,___set_sprite_alphamap); \
			end);");

		//VII.f. Camera Variables
		LUA_BIND_RW(xwin); LUA_BIND_RW(ywin);
		LUA_BIND_RW(cameratracking); LUA_BIND_RW(cameratracker);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___rw('xwin',___get_xwin,___set_xwin); \
			"GLOBAL_NAMESPACE":___rw('ywin',___get_ywin,___set_ywin); \
			"GLOBAL_NAMESPACE":___rw('cameratracking',___get_cameratracking,___set_cameratracking);  \
			"GLOBAL_NAMESPACE":___rw('cameratracker',___get_cameratracker,___set_cameratracker); \
		");

		//VII.g. Map Variables
		LUA_BIND_R(curmap_w); LUA_BIND_R(curmap_h);
		LUA_BIND_R(curmap_startx); LUA_BIND_R(curmap_starty);
		LUA_BIND_R(curmap_tileset);
		LUA_BIND_R(curmap_name);  LUA_BIND_R(curmap_rstring);
		LUA_BIND_R(curmap_music);  LUA_BIND_R(curmap_path);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_table('curmap'); \
			xr("GLOBAL_NAMESPACE".curmap,'w','curmap_w'); xr("GLOBAL_NAMESPACE".curmap,'h','curmap_h'); \
			xr("GLOBAL_NAMESPACE".curmap,'startx','curmap_startx'); xr("GLOBAL_NAMESPACE".curmap,'starty','curmap_starty'); \
			xr("GLOBAL_NAMESPACE".curmap,'tileset','curmap_tileset'); \
			xr("GLOBAL_NAMESPACE".curmap,'name','curmap_name'); xr("GLOBAL_NAMESPACE".curmap,'rstring','curmap_rstring'); \
			xr("GLOBAL_NAMESPACE".curmap,'music','curmap_music'); xr("GLOBAL_NAMESPACE".curmap,'path','curmap_path'); \
		");

		//VII.h. Layer Variables
		LUA_BIND_RW(layer_visible);
		LUA_BIND_RW(layer_lucent);
		LUA_BIND_RW(layer_parallaxx); LUA_BIND_RW(layer_parallaxy);
		LUA_BIND_R(layer_w); LUA_BIND_R(layer_h);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_collection('layer', function(table) \
				table:___irw('visible',___get_layer_visible,___set_layer_visible); \
				table:___irw('lucent',___get_layer_lucent,___set_layer_lucent); \
				table:___irw('parallaxx',___get_layer_parallaxx,___set_layer_parallaxx); \
				table:___irw('parallaxy',___get_layer_parallaxy,___set_layer_parallaxy); \
				table:___ir('w',___get_layer_w); \
				table:___ir('h',___get_layer_h); \
			end);");

		//VII.i. Zone Variables -- Overkill (2007-09-04)
		LUA_BIND_R(zone_name);
		LUA_BIND_R(zone_event);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_collection('zone', function(table) \
				table:___ir('name',___get_zone_name); \
				table:___ir('event',___get_zone_event); \
			end);");

		//VII.j. Event Variables
		LUA_BIND_R(event_tx); LUA_BIND_R(event_ty); LUA_BIND_R(event_zone);
		LUA_BIND_R(event_entity); LUA_BIND_R(event_param);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_table('event'); \
			xr("GLOBAL_NAMESPACE".event,'tx','event_tx'); xr("GLOBAL_NAMESPACE".event,'ty','event_ty'); \
			xr("GLOBAL_NAMESPACE".event,'zone','event_zone'); xr("GLOBAL_NAMESPACE".event,'entity','event_entity'); \
			xr("GLOBAL_NAMESPACE".event,'param','event_param'); \
		");

		//VII.k. Date/Time Variables
		LUA_BIND_R(sysdate_year); LUA_BIND_R(sysdate_month);
		LUA_BIND_R(sysdate_day); LUA_BIND_R(sysdate_dayofweek);
		LUA_BIND_R(sysdate_hour); LUA_BIND_R(sysdate_minute); LUA_BIND_R(sysdate_second);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_table('sysdate'); \
			xr("GLOBAL_NAMESPACE".sysdate,'year','sysdate_year'); xr("GLOBAL_NAMESPACE".sysdate,'month','sysdate_month'); \
			xr("GLOBAL_NAMESPACE".sysdate,'day','sysdate_day'); xr("GLOBAL_NAMESPACE".sysdate,'dayofweek','sysdate_dayofweek'); \
			xr("GLOBAL_NAMESPACE".sysdate,'hour','sysdate_hour'); xr("GLOBAL_NAMESPACE".sysdate,'minute','sysdate_minute'); xr("GLOBAL_NAMESPACE".sysdate,'second','sysdate_second'); \
		");

		//VII.l. Clipboard Variables
		LUA_BIND_RW(clipboard_text);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_table('clipboard'); \
			xrw("GLOBAL_NAMESPACE".clipboard,'text','clipboard_text'); \
		");

		///VIII.a. Version
		LUA_BIND_R(version); LUA_BIND_R(build); LUA_BIND_R(os);
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___embed_magic_table('clipboard'); \
			r("GLOBAL_NAMESPACE",'_version','version'); \
			r("GLOBAL_NAMESPACE",'_build','build'); \
			r("GLOBAL_NAMESPACE",'_os','os'); \
		");

		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___r('SCAN_ESC',function() return 01 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_1',function() return 02 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_2',function() return 03 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_3',function() return 04 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_4',function() return 05 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_5',function() return 06 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_6',function() return 07 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_7',function() return 08 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_8',function() return 09 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_9',function() return 10 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_0',function() return 11 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_MINUS',function() return 12 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_EQUALS',function() return 13 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_BACKSP',function() return 14 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_TAB',function() return 15 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_Q',function() return 16 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_W',function() return 17 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_E',function() return 18 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_R',function() return 19 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_T',function() return 20 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_Y',function() return 21 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_U',function() return 22 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_I',function() return 23 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_O',function() return 24 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_P',function() return 25 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_LANGLE',function() return 26 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_RANGLE',function() return 27 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_ENTER',function() return 28 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_CTRL',function() return 29 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_A',function() return 30 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_S',function() return 31 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_D',function() return 32 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F',function() return 33 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_G',function() return 34 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_H',function() return 35 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_J',function() return 36 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_K',function() return 37 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_L',function() return 38 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_SCOLON',function() return 39 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_QUOTA',function() return 40 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_RQUOTA',function() return 41 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_LSHIFT',function() return 42 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_BSLASH',function() return 43 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_Z',function() return 44 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_X',function() return 45 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_C',function() return 46 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_V',function() return 47 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_B',function() return 48 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_N',function() return 49 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_M',function() return 50 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_COMMA',function() return 51 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_DOT',function() return 52 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_SLASH',function() return 53 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_RSHIFT',function() return 54 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_STAR',function() return 55 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_ALT',function() return 56 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_SPACE',function() return 57 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_CAPS',function() return 58 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F1',function() return 59 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F2',function() return 60 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F3',function() return 61 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F4',function() return 62 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F5',function() return 63 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F6',function() return 64 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F7',function() return 65 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F8',function() return 66 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F9',function() return 67 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F10',function() return 68 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_NUMLOCK',function() return 69 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_SCRLOCK',function() return 70 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_HOME',function() return 71 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_UP',function() return 72 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_PGUP',function() return 73 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_GMINUS',function() return 74 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_LEFT',function() return 75 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_PAD_5',function() return 76 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_RIGHT',function() return 77 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_GPLUS',function() return 78 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_END',function() return 79 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_DOWN',function() return 80 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_PGDN',function() return 81 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_INSERT',function() return 82 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_DEL',function() return 83 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F11',function() return 87 end); \
			"GLOBAL_NAMESPACE":___r('SCAN_F12',function() return 88 end); \
			");

		//other misc defines
		grr |= luaL_dostring(L,"\
			"GLOBAL_NAMESPACE":___r('screen',function() return 1 end);  \
			"GLOBAL_NAMESPACE":___r('FILE_READ',function() return 1 end); \
			"GLOBAL_NAMESPACE":___r('FILE_WRITE_APPEND',function() return 3 end); \
			"GLOBAL_NAMESPACE":___r('SEEK_SET',function() return 0 end); \
			"GLOBAL_NAMESPACE":___r('SEEK_CUR',function() return 1 end); \
			"GLOBAL_NAMESPACE":___r('SEEK_END',function() return 2 end); \
			"GLOBAL_NAMESPACE":___r('CF_NONE',function() return 0 end); \
			"GLOBAL_NAMESPACE":___r('CF_GREY',function() return 1 end); \
			"GLOBAL_NAMESPACE":___r('CF_GREYINV',function() return 2 end); \
			"GLOBAL_NAMESPACE":___r('CF_INV',function() return 3 end); \
			"GLOBAL_NAMESPACE":___r('CF_RED',function() return 4 end); \
			"GLOBAL_NAMESPACE":___r('CF_GREEN',function() return 5 end); \
			"GLOBAL_NAMESPACE":___r('CF_BLUE',function() return 6 end); \
			"GLOBAL_NAMESPACE":___r('CF_CUSTOM',function() return 7 end); \
			"GLOBAL_NAMESPACE":___r('FIXED_PI',function() return 205887 end); \
		");

		if(grr) err("grr");


		//cleanup some functions we were using for convenience
		grr |= luaL_dostring(L,""GLOBAL_NAMESPACE".r = nil; "GLOBAL_NAMESPACE".w = nil; "GLOBAL_NAMESPACE".rw = nil");
		grr |= luaL_dostring(L,""GLOBAL_NAMESPACE".xr = nil; "GLOBAL_NAMESPACE".xw = nil; "GLOBAL_NAMESPACE".xrw = nil");


	if(grr) err("grr");

	//example use cases:
	//timer
	//entities[10].tx
	//mouse.x
	//joy.buttons[0]

	int zzz=9;

}

#endif
