OPT := # -O3 -nodebug
MIMPL := CFOR
TYPE := int
BIMPL := NOBUSY
BCHECK := NOBARGINGCHECK

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wextra -multi ${OPT} -MMD -D"${MIMPL}" -DTYPE="${TYPE}" \
		-D"${BIMPL}" -D"${BCHECK}"	# compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = q2main.o
EXEC1 = mergesort				# 1st executable name

OBJECTS2 = q3main.o
EXEC2 = buffer					# 2nd executable name

OBJECTS = ${OBJECTS1} ${OBJECTS2}		# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}			# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables
.ONESHELL :

-include MergeImpl

ifeq (${shell if [ "${MERGEIMPL}" = "${MIMPL}" -a "${MTYPE}" = "${TYPE}" ] ; \
		then echo true ; fi },true) # same implementation type as last time ?
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed => rebuilt
.PHONY : ${EXEC1}
${EXEC1} :
	rm -f MergeImpl
	touch q2mergesort.h
	${MAKE} ${EXEC1} MIMPL="${MIMPL}" TYPE="${TYPE}"
endif

MergeImpl :
	echo "MERGEIMPL=${MIMPL}\nMTYPE=${TYPE}" > MergeImpl
	sleep 1

-include BufImpl

ifeq (${shell if [ "${BUFIMPL}" = "${BIMPL}" -a "${BCHECKIMPL}" = "${BCHECK}" ] ; \
		then echo true ; fi },true)
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed => rebuilt
.PHONY : ${EXEC2}
${EXEC2} :
	rm -f BufImpl
	touch q3buffer.h
	sleep 1
	${MAKE} ${EXEC2} BIMPL="${BIMPL}" BCHECK="${BCHECK}"
endif

BufImpl :
	echo "BUFIMPL=${BIMPL}\nBCHECKIMPL=${BCHECK}" > BufImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} MergeImpl BufImpl
