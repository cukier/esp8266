CC = ccsc
PK2 = pk2cmd

DEVICE = PIC18F25K22

SRC = src
OUT = Debug
REL = Release

UNIT = ESP
UINT_FILE = esp

OBJS += *.ccspjt *.cof *.err *.esym *.hex *.lst *.xsym
MOBJ = $(OBJS:%=$(SRC)/%)

ifeq ($(DEVICE), PIC24FJ256GB206)
CFLAGS += +FD
DFLAGS += +GM_UC=""
endif
ifeq ($(DEVICE), PIC24FJ1024GB606)
CFLAGS += +FD
DFLAGS += +GM_UC="M_PIC24FJ1024GB606"
endif
ifeq ($(DEVICE), PIC24FJ64GB002)
CFLAGS += +FD
DFLAGS += +GM_UC=""
endif
ifeq ($(DEVICE), PIC18F46K22)
CFLAGS += +FH
DFLAGS += +GM_UC="M_PIC18F46K22"
endif
ifeq ($(DEVICE), PIC18F25K22)
CFLAGS += +FH
DFLAGS += +GM_UC="M_PIC18F25K22"
endif
ifeq ($(DEVICE), PIC18F252)
CFLAGS += +FH
DFLAGS += +GM_UC=""
endif


DFLAGS += +GDEBUG=""
CFLAGS += +LN -T -A -M -Z +DF +Y=9 +STDOUT +EA
TFLAGS += +FB +LN -T -A -M -Z +DF +Y=9 +STDOUT +EA
PK2DELFLAGS += -E -P$(DEVICE)
PK2FLAGS +=$(PK2DELFLAGS) -M -R -J -F

all: $(UNIT)

$(UNIT): $(SRC)/$(UINT_FILE).c
	$(CC) $(CFLAGS) $(DFLAGS) $<
	[[ -d $(OUT) ]] || mkdir $(OUT)
	mv $(MOBJ) $(OUT)
	
Release: $(SRC)/$(UINT_FILE).c
	$(CC) $(CFLAGS) $(RDFLAGS) $<
	[[ -d $(REL) ]] || mkdir $(REL)
	mv $(MOBJ) $(REL)
	
burn: $(OUT)/$(UINT_FILE).hex
	$(PK2) $(PK2FLAGS) $<
	
erase:
	$(PK2) $(PK2DELFLAGS)
	
clean:
	rm -Rvf $(OUT)/*
	
clean_release:
	rm -Rvf $(REL)
