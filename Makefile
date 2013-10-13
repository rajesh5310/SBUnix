CC=gcc
AS=as
CFLAGS=-O1 -Wall -Werror -nostdinc -Iinclude -msoft-float -mno-sse -mno-red-zone -fno-builtin -fPIC -mtune=amdfam10 -g3
LD=ld
LDLAGS=-nostdlib
AR=ar

ROOTFS=rootfs
ROOTBIN=$(ROOTFS)/bin
ROOTLIB=$(ROOTFS)/lib
ROOTBOOT=$(ROOTFS)/boot

KERN_SRCS:=$(wildcard sys/*.c sys/*.s sys/*/*.c sys/*/*.s)
BIN_SRCS:=$(wildcard bin/*/*.c)
LIBC_SRCS:=$(wildcard libc/*.c libc/*/*.c)
LD_SRCS:=$(wildcard ld/*.c)
BINS:=$(addprefix $(ROOTFS)/,$(wildcard bin/*))

.PHONY: all binary

all: $(USER).iso $(USER).img

$(USER).iso: kernel
	cp kernel $(ROOTBOOT)/kernel/kernel
	mkisofs -r -no-emul-boot -input-charset utf-8 -b boot/cdboot -o $@ $(ROOTFS)/

$(USER).img:
	qemu-img create -f raw $@ 16M

kernel: $(patsubst %.s,obj/%.asm.o,$(KERN_SRCS:%.c=obj/%.o)) obj/tarfs.o
	$(LD) $(LDLAGS) -o $@ -T linker.script $^

obj/tarfs.o: $(BINS)
	tar --format=ustar -cvf tarfs --no-recursion -C $(ROOTFS) $(shell find $(ROOTFS)/ -name boot -prune -o ! -name .empty -printf "%P\n")
	objcopy --input binary --binary-architecture i386 --output elf64-x86-64 tarfs $@
	@rm tarfs

$(ROOTLIB)/libc.a: $(LIBC_SRCS:%.c=obj/%.o)
	$(AR) rcs $@ $^

$(ROOTLIB)/libc.so: $(LIBC_SRCS:%.c=obj/%.o) $(ROOTLIB)/ld.so
	$(LD) $(LDLAGS) -shared -soname=$@ --dynamic-linker=/lib/ld.so --rpath-link=/lib -o $@ $^

$(ROOTLIB)/ld.so: $(LD_SRCS:%.c=obj/%.o)
	$(LD) $(LDLAGS) -shared -o $@ $^

$(ROOTLIB)/crt1.o: obj/crt/crt1.o
	cp $^ $@

$(BINS): $(ROOTLIB)/crt1.o $(ROOTLIB)/libc.a $(ROOTLIB)/libc.so $(shell find bin/ -type f -name *.c)
	@$(MAKE) --no-print-directory BIN=$@ binary

binary: $(patsubst %.c,obj/%.o,$(wildcard $(BIN:rootfs/%=%)/*.c))
	$(LD) $(LDLAGS) -o $(BIN) $(ROOTLIB)/crt1.o $^ $(ROOTLIB)/libc.a

obj/%.o: %.c $(wildcard include/*.h include/*/*.h)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

obj/%.asm.o: %.s
	@mkdir -p $(dir $@)
	$(AS) -o $@ $<

.PHONY: submit clean

SUBMITTO:=~mferdman/cse506-submit/

submit: clean
	tar -czvf $(USER).tgz --exclude=$(ROOTLIB) --exclude=$(ROOTBIN) --exclude=.empty --exclude=.*.sw? --exclude=*~ LICENSE Makefile linker.script sys bin libc ld include $(ROOTFS)
	@gpg --quiet --import cse506-pubkey.txt
	gpg --yes --encrypt --recipient 'CSE506' $(USER).tgz
	rm -fv $(SUBMITTO)$(USER)=*.tgz.gpg
	cp -v $(USER).tgz.gpg $(SUBMITTO)$(USER)=`date +%F=%T`.tgz.gpg

clean:
	find $(ROOTLIB) $(ROOTBIN) -type f ! -name .empty -print -delete
	rm -rfv obj kernel $(ROOTBIN)/kernel/kernel
