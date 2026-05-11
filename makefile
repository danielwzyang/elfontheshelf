payload:
	@nasm -f elf64 -o ${basename ${asm}}.o ${asm}
	@objcopy --only-section=.text -O binary ${basename ${asm}}.o ${basename ${notdir ${asm}}}.bin
	@rm ${basename ${asm}}.o

clean:
	@find . -type f -name "*.bin" -print -exec rm -rf {} +
