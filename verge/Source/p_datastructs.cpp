/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

//This file contains plugins for manipulating the primary verge datastructures (map,vsp,chr),
//as well as more normal simple datastructure things (dynamic arrays, hash tables, etc).
//It also serves as an example of the plugin api.

#include "xerxes.h"
#include <string>
#include <vector>

//see bottom of file for symbol registration code
//all of this stuff needs validation!

//datastructure stuff----
#define INTARR (*((std::vector<int>*)context.ints[0]))
void intarr_new(VCPlugins::FunctionContext &context) { context.setReturnInt((int)new std::vector<int>()); }
void intarr_get(VCPlugins::FunctionContext &context) { context.setReturnInt(INTARR[context.ints[1]]); }
void intarr_add(VCPlugins::FunctionContext &context) { INTARR.push_back(context.ints[1]); }
void intarr_count(VCPlugins::FunctionContext &context) { context.setReturnInt(INTARR.size()); }
void intarr_clear(VCPlugins::FunctionContext &context) { INTARR.clear(); }
void intarr_size(VCPlugins::FunctionContext &context) { INTARR.reserve(context.ints[1]); }
void intarr_free(VCPlugins::FunctionContext &context) { delete &INTARR; }
//-----------

//map stuff----
void map_new(VCPlugins::FunctionContext &context) { context.setReturnInt((int)new MAP()); }
//-----------

void p_datastructs() {
	VCPlugins::registerFunction("1","intarr_new","",intarr_new);
	VCPlugins::registerFunction("1","intarr_get","11",intarr_get);
	VCPlugins::registerFunction("5","intarr_add","11",intarr_add);
	VCPlugins::registerFunction("1","intarr_count","1",intarr_free);
	VCPlugins::registerFunction("1","intarr_clear","1",intarr_free);
	VCPlugins::registerFunction("5","intarr_size","11",intarr_free);
	VCPlugins::registerFunction("5","intarr_free","1",intarr_free);
	VCPlugins::registerFunction("1","map_new","",map_new);
}