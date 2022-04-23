CC	= clang
CFLAGS	= -O2 -flto -F/System/Library/PrivateFrameworks
LDFLAGS	= -framework Carbon \
	  -framework CoreAudio \
	  -framework IOKit \
	  -framework SkyLight

BINDIR	= ./bin
BLDDIR	= ./build
SRCDIR	= ./src

SRCS	= $(filter-out $(SRCDIR)/soda.c,$(wildcard $(SRCDIR)/*.c))
DEPS	= $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
OBJS	= $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(SRCS))

all:	mkdir $(OBJS) $(BINDIR)/soda

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -pthread -MMD -c $< -o $@

$(BINDIR)/soda: $(SRCDIR)/soda.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -pthread $^ -o $@

mkdir:
	@mkdir -p $(BINDIR) $(BLDDIR)

iwyu:
	$(MAKE) clean
	$(MAKE) -k CC='include-what-you-use \
		-Xiwyu --no_fwd_decls \
		-Xiwyu --mapping_file=.soda.imp'

clean:
	rm -rf $(BINDIR) $(BLDDIR)

.PHONY:	all mkdir clean

-include $(DEPS)
