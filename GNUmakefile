CFLAGS ?= -Wall -O3 -DNDEBUG
STRIP ?= strip
OUTFILE := bin/xxh_rand

# --------------------------------------------------------------
# Build options
# --------------------------------------------------------------

ifneq ($(EXTRA_CFLAGS),)
  CFLAGS += $(EXTRA_CFLAGS)
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

.PHONY: all test clean

all: clean $(OUTFILE)

$(OUTFILE): $(wildcard src/*.c)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	$(STRIP) $@

clean:
	rm -rf $(dir $(OUTFILE))
