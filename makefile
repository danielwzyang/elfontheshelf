.PHONY: default injector target test_payload clean

ASM := payloads/exec.asm

NASM_FLAGS :=

ifdef IP
NASM_FLAGS += -DIPADDR=$(shell printf '0x%02X%02X%02X%02X' $(subst ., ,$(IP))) # replace periods with commas and change to proper endian
endif
ifdef PORT
NASM_FLAGS += -DPORT=$(shell printf '0x%02X%02X' $$(($(PORT) & 0xFF)) $$(($(PORT) >> 8))) # change to proper endian
endif

default: injector target payload
	@./injector target $(basename $(notdir $(ASM))).bin
	@echo "Injection complete. Run ./target"

injector:
	@gcc -Wall src/main.c src/helpers.c -o injector

target:
	@gcc -Wall -no-pie src/test-bin.c -o target

payload:
	@nasm -f elf64 $(NASM_FLAGS) -o $(basename $(ASM)).o $(ASM)
	@objcopy --only-section=.text -O binary $(basename $(ASM)).o $(basename $(notdir $(ASM))).bin
	@rm $(basename $(ASM)).o

test_payload:
	@nasm -f elf64 $(NASM_FLAGS) -o temp.o $(ASM) 
	@ld temp.o
	@./a.out <&0 ; rm -f ./a.out temp.o

clean:
	@rm -vf injector target
	@find . -type f -name "*.bin" -print -exec rm -rf {} +

