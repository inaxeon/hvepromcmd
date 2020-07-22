SRCS       = main.c pgm.c test_descriptions.c serial_posix.c util.c
OBJS       = $(SRCS:.c=.o)
DEPDIR     = deps
DEPFLAGS   = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
RM         = rm
MV         = mv
MKDIR      = mkdir

POSTCOMPILE = $(MV) $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
COMPILE = gcc -Wall -Os $(DEPFLAGS)

all: hvepromcmd

.c.o: $(DEPDIR)/%.d
	@$(MKDIR) -p $(DEPDIR)
	$(COMPILE) -c $< -o $@
	@$(POSTCOMPILE)

clean:
	$(RM) -f hvepromcmd $(OBJS)
	$(RM) -rf deps

hvepromcmd: $(OBJS)
	$(COMPILE) -o $@ $(OBJS)

cpp:
	$(COMPILE) -E $(SRCS)

$(DEPDIR)/%.d:
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
