CFLAGS ?= -Wall -O3 -DNDEBUG
STRIP ?= strip

INFILES := $(wildcard src/*.c)
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

ifneq ($(RESCOMP),)
  INFILES += obj/xxh_rand.res
endif

# --------------------------------------------------------------
# Targets
# --------------------------------------------------------------

.PHONY: all clean

all: clean $(OUTFILE)

obj/%.res: etc/resources/%.rc
	mkdir -p obj
	$(RESCOMP) --output-format=coff -o $@ $^

$(OUTFILE): $(INFILES)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	$(STRIP) $@

clean:
	rm -rf bin obj
