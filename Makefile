BUILDDIR = bin
ASM_FILES	= asm_func.s int_handler.s io.s loader.s taskSwitch.s
C_FILES	= console.c cpuinfo.c fb.c gdt.c interrupt.c keyboard.c kmain.c \
			kmalloc.c lib.c mem.c mmngr_phys.c mmngr_virtual.c ordered_array.c\
			pde.c pic.c pte.c serial.c task.c timer.c			

ASM_OBJ = $(addprefix $(BUILDDIR)/,$(ASM_FILES:.s=.o))
C_OBJ = $(addprefix $(BUILDDIR)/,$(C_FILES:.c=.o))


#OBJECTS = 	$(BUILDDIR)/asm_func.o $(BUILDDIR)/mem.o $(BUILDDIR)/io.o \
#			$(BUILDDIR)/interrupt.o $(BUILDDIR)/int_handler.o \
#			$(BUILDDIR)/pic.o $(BUILDDIR)/gdt.o $(BUILDDIR)/serial.o \
#			$(BUILDDIR)/fb.o $(BUILDDIR)/timer.o $(BUILDDIR)/keyboard.o \
#			$(BUILDDIR)/kmain.o $(BUILDDIR)/loader.o $(BUILDDIR)/lib.o \
#			$(BUILDDIR)/cpuinfo.o $(BUILDDIR)/mmngr_phys.o \
#			$(BUILDDIR)/mmngr_virtual.o $(BUILDDIR)/pde.o $(BUILDDIR)/pte.o \
#			$(BUILDDIR)/console.o $(BUILDDIR)/task.o $(BUILDDIR)/taskSwitch.o
CC = gcc
INC_DIR = headers
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -I$(INC_DIR) -Wall -Wextra -Werror -x c -c
LDFLAGS  = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf32
VPATH = src

all: os.iso

kernel.elf : $(ASM_OBJ) $(C_OBJ)
	ld $(LDFLAGS) $(ASM_OBJ) $(C_OBJ) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R								\
				-b boot/grub/stage2_eltorito	\
				-no-emul-boot					\
				-boot-load-size 4				\
				-A os 							\
				-input-charset utf8				\
				-quiet							\
				-boot-info-table				\
				-o os.iso						\
				iso

run : os.iso
	bochs -f bochsrc.txt -q

$(BUILDDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: src/%.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*.o kernel.elf os.iso