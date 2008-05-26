#include <UnitTest++.h>

typedef unsigned int   quad;
typedef unsigned short word;
typedef unsigned char  byte;

#include "a_vfile.h"
#include "vid_manager.h"
#include "g_script.h"
#include "vc_compiler.h"

SUITE(when_creating_a_vc_compiler_instance) {

    TEST(vcerror_is_false) {
		VCCompiler vcc;
		CHECK_EQUAL(vcc.vcerror, false);
    }
}
