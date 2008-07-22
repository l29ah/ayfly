# ============================================================================
#  Name	 : build_help.mk
#  Part of  : ayfly_s60
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : ayfly_s60
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : ayfly_s60_0xEA0B66F6.hlp
ayfly_s60_0xEA0B66F6.hlp : ayfly_s60.xml ayfly_s60.cshlp Custom.xml
	cshlpcmp ayfly_s60.cshlp
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
	copy ayfly_s60_0xEA0B66F6.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del ayfly_s60_0xEA0B66F6.hlp
	del ayfly_s60_0xEA0B66F6.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo ayfly_s60_0xEA0B66F6.hlp

FINAL : do_nothing
