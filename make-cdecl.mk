# (c) Corto Inc, 2012
#
# ========================================================================
# declaration
# ========================================================================
#
SHELL     = /bin/sh
RM        = /bin/rm
MAKE      = /usr/bin/make
STRIP     = /usr/bin/strip
FIND      = /usr/bin/find

MYNAME    = cdecl
MAKEFILE  = $(.CURDIR)/make-$(MYNAME).mk
VERBOSE   = 1

INCDRS    = -I$(.CURDIR)/inc -I/usr/local/include/glib-2.0
LIBDRS    = -L/usr/local/lib -L$(.CURDIR)/lib -L$(.CURDIR)/src

# -lc for rand, srand, ... -lm for sqrt, ...
LIBS      = -lglib-2.0 

CC        = /usr/bin/clang
LL        = $(CC)
#
.if defined(DEBUG)
CFLAGS     = -std=c99 -O0 -g -pg -Wall -pipe
CFLAGS_L   = -std=c99 -O0 -Wall -pipe
.else
CFLAGS     = -std=c99 -O2 -Wall -pipe
CFLAGS_L   = $(CFLAGS)
.endif
# 

## deps
SRC1      = 
OBJ1      = 
INC      += 
OBJS     += 


## main
DSTFILE   = $(.CURDIR)/bin/$(MYNAME)
SRC       = $(.CURDIR)/src/$(MYNAME).c
_OBJ      = $(SRC:.c=.o)
OBJ       = ${_OBJ:C/src/obj/}
OBJS     += $(OBJ)


# ========================================================================
# rules
# ========================================================================
#

# here we use the basename as an alias on the following targets 
# $(DSTFILE)
#

$(DSTFILE): $(OBJS) $(INC) $(SRC)
	@echo "++ Linking stage for [$@]"
	$(LL) $(CFLAGS_L) -o $@ $(OBJS) $(LIBDRS) $(LIBS)


$(OBJ1): $(SRC1)
	@echo "-- object stage with [$(OBJ1) // [$@]]"
	$(CC) $(CFLAGS) $(INCDRS) -c $(SRC1) -o $@

$(OBJ2): $(SRC2)
	@echo "-- object stage with [$(OBJ2) // [$@]]"
	$(CC) $(CFLAGS) $(INCDRS) -c $(SRC2) -o $@


$(OBJ): $(SRC)
	@echo " - object stage with [$(OBJ)]"
	$(CC) $(CFLAGS) $(INCDRS) -c $(SRC) -o ${OBJ}

# build the whole project and stripe the executable 
#
install: 
	$(MAKE) -f $(MAKEFILE) all
	$(STRIP) $(DSTFILE)

#
all:
	$(MAKE) -f $(MAKEFILE) clean
#	$(MAKE) -f $(MAKEFILE) depend
	$(MAKE) -f $(MAKEFILE) $(DSTFILE)

# generate the object files necessary to the project
#
depend:
.for _name in $(ALLSRCFILE)
	makedepend $(INCDRS) -f $(MAKEFILE) ${_name}
	$(MAKE) -f $(MAKEFILE) ${_name}.o
.endfor


# do some vacuum cleaning
#
clean:
	$(RM) -f $(OBJ) $(OBJ1) $(OBJ2)
	$(RM) -f $(DSTFILE)
	$(FIND) $(.CURDIR) -type f -name "*~" -delete
