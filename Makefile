CROSS_COMPILE ?= aarch64-linux-gnu-
CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy

MKSUNXIBOOT ?= tools/mksunxiboot

BUILDID := $(shell git rev-parse --short HEAD 2>/dev/null || echo unknown)

CFLAGS := -march=armv8-a -mgeneral-regs-only -Os -g \
          -ffreestanding -fno-builtin -nostdlib -nostartfiles \
          -fno-stack-protector -fomit-frame-pointer \
          -Wall -Wextra -Iinclude -DBUILDID=\"$(BUILDID)\"
LDFLAGS = -nostdlib
ASFLAGS := -march=armv8-a

OBJS = arch/$(ARCH)/start.o arch/$(ARCH)/helper.o psci.o psci_trampoline.o

# helpers for Kbuild, taken from Linux
include Makefile.kbuild
ifndef mixed-build
ifndef config-build
# end of helpers for Kbuild

export CC AS
export KBUILD_CFLAGS := $(CFLAGS)
export KBUILD_AFLAGS := $(ASFLAGS)

all: spl.bin

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

start.o: arch/arm64/start.S
	$(CC) $(ASFLAGS) -c -o $@ $<

spl-raw.elf: start.o main.o arch/arm64/linker.lds built-in.a
	$(CC) $(CFLAGS) $(LDFLAGS) -T arch/arm64/linker.lds -o $@ start.o main.o built-in.a

spl-raw.bin: spl-raw.elf
	$(OBJCOPY) -O binary $< $@

spl.bin: spl-raw.bin
	$(MKSUNXIBOOT) $< $@

PHONY += built-in.a
built-in.a: $(build-dir)

PHONY += $(build-dir)
$(build-dir):
	$(Q)$(MAKE) $(build)=$@ need-builtin=1 need-modorder=1 $(single-goals)

CLEAN_FILES += spl.bin spl-raw.bin spl-raw.elf

MRPROPER_FILES += include/config include/generated \
		  .config .config.old

mrproper-dirs      := $(addprefix _mrproper_,scripts)

PHONY += $(mrproper-dirs) mrproper
$(mrproper-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean $(mrproper-dirs)
	$(call cmd,rmfiles)

clean: private rm-files := $(CLEAN_FILES)
mrproper: private rm-files := $(MRPROPER_FILES)

clean-dirs := $(addprefix _clean_, $(clean-dirs))
PHONY += $(clean-dirs) clean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)
	$(call cmd,rmfiles)
	@find . $(RCS_FIND_IGNORE) \
		\( -name '*.[oad]' -o -name '.*.cmd' \
		   -o -name '*.lex.c' -o -name '*.tab.[ch]' \
		\) -type f -print \
		| xargs rm -rf

.PHONY: clean

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -rf $(rm-files)

endif # config-build
endif # mixed-build

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
