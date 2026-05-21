.PHONY: default injector target test_payload clean

INJECTOR_FLAGS := 

ASM := payloads/exec.asm

NASM_FLAGS :=
ifdef IP
# replace periods with commas and change to proper endian
NASM_FLAGS += -DIPADDR=$(shell echo $(IP) | awk -F. '{printf "0x%02X%02X%02X%02X", $$4, $$3, $$2, $$1}') 
endif
ifdef PORT
NASM_FLAGS += -DPORT=$(shell printf '0x%02X%02X' $$(($(PORT) & 0xFF)) $$(($(PORT) >> 8))) # change to proper endian
endif

BINARY := target

default: injector $(BINARY) payload
ifdef VERBOSE
	@./injector -v -t $(BINARY) -p $(basename $(notdir $(ASM))).bin
else
	@./injector -t $(BINARY) -p $(basename $(notdir $(ASM))).bin
endif
	@echo "Injection complete. Run ./$(BINARY)"

injector:
	@gcc -Wall src/main.c src/helpers.c -o injector

target:
	@gcc -Wall -fPIE -pie src/test-bin.c -o target

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
	@find . -type f -name "*.bin" -exec rm -vrf {} +

