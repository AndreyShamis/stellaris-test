#
# Defines the part type that this project uses.
#
PART=LM4F120H5QR

#
# Set the processor variant.
#
VARIANT=cm4f

#
# The base directory for StellarisWare.
#
ROOT=./

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=../../..

#
# The default rule, which causes the blinky example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/stellaris-test.afx

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the blinky example.
#
${COMPILER}/stellaris-test.axf: ${COMPILER}/stellaris-test.o
${COMPILER}/stellaris-test.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/stellaris-test.axf: stellaris-test.ld
SCATTERgcc_blinky=stellaris-test.ld
ENTRY_blinky=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif

flash:
	lm4flash gcc/stellaris-test.bin
