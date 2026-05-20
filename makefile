.PHONY: default injector target test_payload clean

INJECTOR_FLAGS := 

ASM := payloads/exec.asm

NASM_FLAGS :=
ifdef IP
NASM_FLAGS += -DIPADDR=$(shell printf '0x%02X%02X%02X%02X' $(subst ., ,$(IP))) # replace periods with commas and change to proper endian
endif
ifdef PORT
NASM_FLAGS += -DPORT=$(shell printf '0x%02X%02X' $$(($(PORT) & 0xFF)) $$(($(PORT) >> 8))) # change to proper endian
endif

BINARY := target

default: injector $(BINARY) payload
ifdef VERBOSE
	@./injector -v $(BINARY) $(basename $(notdir $(ASM))).bin
else
	@./injector $(BINARY) $(basename $(notdir $(ASM))).bin
endif
	@echo "Injection complete. Run ./$(BINARY)"

injector:
	@gcc -Wall src/main.c src/helpers.c -o injector

target:
ifdef PIE
	@gcc -Wall -fPIE -pie src/test-bin.c -o target
else
	@gcc -Wall -no-pie src/test-bin.c -o target
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
	@rm -vf injector target
	@find . -type f -name "*.bin" -exec rm -vrf {} +

