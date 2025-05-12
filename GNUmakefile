CFLAGS ?= -Wall -O3 -DNDEBUG

OUTFILE := bin/xxh_rand
SRCFILE := $(wildcard src/*.c)

# --------------------------------------------------------------
# Build options
# --------------------------------------------------------------

ifneq ($(EXTRA_CFLAGS),)
  CFLAGS += $(EXTRA_CFLAGS)
endif

ifneq ($(EXTRA_LDFLAGS),)
  LDFLAGS += $(EXTRA_LDFLAGS)
endif

ifneq ($(EXTRA_LIBS),)
  LDLIBS += $(EXTRA_LIBS)
endif

ifneq ($(SUFFIX),)
  OUTFILE := $(addsuffix .$(SUFFIX),$(OUTFILE))
endif

# --------------------------------------------------------------
# Targets
# --------------------------------------------------------------

.PHONY: all bin clean

all: clean bin

bin: $(OUTFILE)

$(OUTFILE): $(SRCFILE)
	test -d bin || mkdir bin
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LDLIBS)
	strip $@

clean:
	rm -rf $(dir $(OUTFILE))
