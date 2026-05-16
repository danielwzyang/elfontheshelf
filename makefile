NASM_FLAGS :=

ifdef IP
NASM_FLAGS += -DIPADDR=$(shell printf '0x%02X%02X%02X%02X' $(subst ., ,$(IP))) # replace periods with commas and change to proper endian
endif
ifdef PORT
NASM_FLAGS += -DPORT=$(shell printf '0x%02X%02X' $$(($(PORT) & 0xFF)) $$(($(PORT) >> 8))) # change to proper endian
endif

payload:
	@nasm -f elf64 $(NASM_FLAGS) -o $(basename $(ASM)).o $(ASM)
	@objcopy --only-section=.text -O binary $(basename $(ASM)).o $(basename $(notdir $(ASM))).bin
	@rm $(basename $(ASM)).o

test_payload:
	@nasm -f elf64 $(NASM_FLAGS) -o temp.o $(ASM) 
	@ld temp.o
	@./a.out <&0 ; rm -f ./a.out temp.o

clean:
	@find . -type f -name "*.bin" -print -exec rm -rf {} +

