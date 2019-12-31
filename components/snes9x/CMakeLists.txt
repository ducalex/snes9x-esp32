set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_REQUIRES "odroid-go")
register_component()
component_compile_options(
    -fomit-frame-pointer -fno-exceptions -fno-stack-protector
    -finline-limit=255
    -fdata-sections -ffunction-sections -Wl,--gc-sections
	-fno-stack-protector -fno-ident -fomit-frame-pointer
	-falign-functions=1 -falign-jumps=1 -falign-loops=1
	-fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops
	-fmerge-all-constants -fno-math-errno
	-fno-exceptions -fno-rtti
)
