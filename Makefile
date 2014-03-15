# *****************************************************************
# 02/21/2014
# Modified by David Ametsitsi Originally posted by Mark Yoder
# Starter makefile
#
# Contains rules for:
#
# - .x (arm executable target)
# - .o (arm object file)
# - .PHONY (all, clean and test)
#
#
# Uses the following variables (built-in and user-defined):
#
# CC = C compiler (gcc)
# CFLAGS = compiler options (e.g. -g)
# LINKER_FLAGS = specify libraries to link in
# $@ = target
# $^ = all dependencies
# $< = first dependency only (not used in this solution)
#
# Generic "rule" terminology:
#
# target:dependency
# [TAB] <CMD1>
# [TAB] <CMD2>
#
# GCC options used in this makefile:
#
# -c = "compile only"
# -o = "output filename"
# -g = "debug mode"
#
# Note1: if you use a filename other than "makefile", use the
# 	 -f option to enable make to use it:
# 	 e.g.  make -f my_make_file.mak
#
# Note2: .PHONY tells make that the rule's target is not 
#        actually a file (to be created or searched for).
#
# Note3: When make runs without any rules specified on the
#        command line, it will make (by default) the FIRST
#        rule found in the makefile. Hence, it is common
#        to have a rule named "all" listed first (as below).
# 
# ***************************************************************


# --------------------------------
# ------ user-defined vars -------
# --------------------------------
CC := gcc
CFLAGS := -g
LINKER_FLAGS := -lstdc++
SENDOBJECTS= joystickSend.o SimpleGPIO.o
#RECOBJECTS= XBee.o SimpleGPIO.o

# -----------------------
# ------ make all -------
# -----------------------

# The "all" rule commonly specifies all executable targets to
# be built. Note, the user has full control over what the "all"
# rule builds. In this solution, we only have one target.
# However, in the final Beagle makefile, you'll see more
# targets listed. 

#all:	joystickSend joystickReceive

all: motorController

motorController:  motorController.o SimpleGPIO.o
	$(CC) $(LDFLAGS) -pthread -o $@ $^ -lncurses

#commandReceive:   commandReceive.o XBee.o
#	$(CC) $(LDFLAGS) -o $@ $^

#
# Objects
#

%.o: %.c
	$(CC) $(CFLAGS) $(TOOLS_CFLAGS) -c $< -o $@


# ----------------------
# ----- clean all ------
# ----------------------

# The "clean" rule should remove all files created by
# the makefile (e.g. the executables and intermediate
# files).

clean:
	rm motorController motorController.o XBee.o SimpleGPIO.o

