#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# -Ofast makes our binary 50% bigger but 5% more performant...
#CXXFLAGS  += -fomit-frame-pointer -fno-exceptions -fno-rtti -pedantic -Wall -W -Wno-unused-parameter # -Ofast

CPPFLAGS = -finstrument-functions -fomit-frame-pointer -fno-exceptions -fno-stack-protector #\
	-finline-limit=255 \
	-fdata-sections -ffunction-sections -Wl,--gc-sections \
	-fno-stack-protector -fno-ident -fomit-frame-pointer \
	-falign-functions=1 -falign-jumps=1 -falign-loops=1 \
	-fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops \
	-fmerge-all-constants -fno-math-errno \
	-fno-exceptions -fno-rtti
