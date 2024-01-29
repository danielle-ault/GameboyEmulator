rgbasm -L -o test_instructions.o test_instructions.asm
rgblink -o test_instructions.gb test_instructions.o
rgbfix -v -p 0xFF test_instructions.gb