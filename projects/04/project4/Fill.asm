// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.


(LOOP)
	@KBD
	D=M
	@BLACK_SCREEN
	D;JGT
	@WHITE_SCREEN
	D;JEQ
	@LOOP
	0;JMP


(BLACK_SCREEN)

	@SCREEN
	D=A
	@addr
	M=D //addr = 16384
	@addr
	D=M
	@tempAddrB
	M=D //tempAddrB = 16384
	@24575
	D=A
	@addrLimit
	M=D

	@UPDATE_RAM_WITH_FF
	0;JMP


(UPDATE_RAM_WITH_FF)

	@tempAddrB
	A=M
	M=-1 			//RAM[tempAddrB] = -1, representing one pixel !

	@tempAddrB
	M=M+1 //tempAddrB = tempAddrB + 1

	@addrLimit
	D=M
	@addrSub
	M=D
	@tempAddrB
	D=M
	@addrSub
	M=M-D
	D=M
	@LOOP
	D;JLT

	@UPDATE_RAM_WITH_FF
	0;JMP



(WHITE_SCREEN)

	@SCREEN
	D=A
	@addr
	M=D //addr = 16384
	@addr
	D=M
	@tempAddrW
	M=D //tempAddrW = 16384
	@24575
	D=A
	@addrLimit
	M=D

	@UPDATE_RAM_WITH_ZERO
	0;JMP


(UPDATE_RAM_WITH_ZERO)

	@tempAddrW
	A=M
	M=0 			//RAM[tempAddrW] = 0, representing one pixel !

	@tempAddrW
	M=M+1 //tempAddrW = tempAddrW + 1

	@addrLimit
	D=M
	@addrSub
	M=D
	@tempAddrW
	D=M
	@addrSub
	M=M-D
	D=M
	@LOOP
	D;JLT

	@UPDATE_RAM_WITH_ZERO
	0;JMP


