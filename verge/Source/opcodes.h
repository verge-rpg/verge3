/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef _OPCODES_H_
#define _OPCODES_H_

// Opcode values.
#define ERROR				0
#define opRETURN			1
#define opASSIGN			2
#define opIF				3
#define opGOTO				4
#define opSWITCH			5
#define opCASE				6
#define opDEFAULT			7
#define opLIBFUNC			8
#define opUSERFUNC			9
#define opRETVALUE			10
#define opRETSTRING			11

// Overkill: Variadic functions
#define opVARARG_START		12 // Start of argument list
#define opVARARG_END		13 // End of argument list

#define ifZERO				20
#define	ifNONZERO			21
#define ifEQUAL				22
#define ifNOTEQUAL			23
#define ifGREATER			24
#define ifGREATEROREQUAL	25
#define ifLESS				26
#define ifLESSOREQUAL		27
#define ifAND				28
#define ifOR				29
#define ifUNGROUP			30

#define iopADD				31
#define iopSUB				32
#define iopDIV				33
#define iopMULT				34
#define iopMOD				35
#define iopSHL				36
#define iopSHR				37
#define iopAND				38
#define iopOR				39
#define iopXOR				40
#define iopEND				41
#define iopNOT				42
#define iopNEGATE			43

#define intLITERAL			50
#define intHVAR0			51
#define intHVAR1			52
#define intGLOBAL			53
#define intARRAY			54
#define intLOCAL			55
#define intLIBFUNC			56
#define intUSERFUNC			57
#define intGROUP			58

#define sADD				60
#define sEND				61

#define strLITERAL			70
#define strGLOBAL			71
#define strARRAY			72
#define strLOCAL			73
#define strLIBFUNC			74
#define strUSERFUNC			75
#define strHSTR0            76
#define strHSTR1			77
#define strINT				78
#define strLEFT				79
#define strRIGHT			80
#define strMID				81

#define aSET				90
#define aINC				91
#define aDEC				92
#define aINCSET				93
#define aDECSET				94

//plugin API

#define	intPLUGINVAR		100
#define strPLUGINVAR		101
#define	intPLUGINFUNC		102
#define strPLUGINFUNC		103
#define opPLUGINFUNC		104

// types are 1 = int, 3 = string, 4 = variable number of ints, 5 = void, 6 = struct

#define t_INT				1
#define t_STRING			3
#define t_VARARG			4
#define t_VOID				5
#define t_STRUCT			6

#define NUM_LIBFUNCS		275
#define NUM_HVARS			125
#define NUM_HDEFS			104

extern char *libfuncs[NUM_LIBFUNCS][3];
extern char *libvars[NUM_HVARS][3];
extern char *hdefs[NUM_HDEFS][2];

#endif  /* OPCODES_H */
