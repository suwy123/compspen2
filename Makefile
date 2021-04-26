#---------------------------------------
# FLAGS
#---------------------------------------
include Make.properties
#---------------------------------------
# FILES
#---------------------------------------
SUBDIRS=component parser scanner solver_slid_set solver_slid_int solver_slah
SRCS=$(foreach sub, $(SUBDIRS), $(wildcard $(SRCDIR)$(sub)/*.cpp))
OBJS=$(foreach sub, $(SUBDIRS), $(addprefix $(OBJSDIR)$(sub)/, $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SRCDIR)$(sub)/*.cpp)))))
TPOBJ=$(OBJSDIR)TestParser.o

#---------------------------------------
# RULES
#---------------------------------------
all: pre main
	echo "build all"

pre:
	@if [ ! -d $(OBJSDIR) ]; then \
	mkdir -p $(OBJSDIR); \
	fi
	@for sub in $(SUBDIRS); do \
		tmp="$(OBJSDIR)$$sub"; \
		echo "test $$tmp"; \
		if [ ! -d $$tmp ]; then \
			mkdir -p $$tmp; \
		fi; \
	done

main: $(OBJS)
	echo "building main"

tp: testparser
	mv $(BASEDIR)/testparser $(OBJSDIR)

testparser: $(TPOBJ) $(OBJS)
	$(CC) -o $@ $^ $(F_LIB)

$(TPOBJ):$(SRCDIR)test/parser/TestParser.cpp
	$(CC) -o $@ -c $^ $(CPPFLAGS)

clean:
	rm $(OBJS)
	(cd $(OBJSDIR) && if [ -f $(TPOBJ) ]; then rm $(TPOBJ); fi; \
	if [ -f testparser.exe ]; then rm testparser; fi;) || exit 1
#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------
$(OBJSDIR)%.o:$(SRCDIR)%.cpp
	$(CC) -o $@ -c $^ $(CPPFLAGS)
