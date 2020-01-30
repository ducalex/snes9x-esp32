#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# -finstrument-functions
CXXFLAGS += -O3 -fomit-frame-pointer -fno-exceptions -fno-stack-protector -mlongcalls
	-finline-limit=255 -fdata-sections -ffunction-sections -Wl,--gc-sections \
	-fno-ident -falign-functions=1 -falign-jumps=1 -falign-loops=1 \
	-fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops \
	-fmerge-all-constants -fno-math-errno -fno-rtti
