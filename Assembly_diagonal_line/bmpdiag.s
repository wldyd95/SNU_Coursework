
#------------------------------------------------------------------
# 
#  4190.308 Computer Architecture (Spring 2019)
#
#  Project #4: Optimizing Program for a Pipelined Y86-64 Processor
#
#  May 28, 2019
#
#  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#------------------------------------------------------------------

.text
	.align 4

.global bmp_diag
	.type bmp_diag, @function

bmp_diag:
	# imgptr is in %rdi
	# width  is in %rsi
	# height is in %rdx
	# gap 	 is in %rcx
	#-----------------------------------------------------------------
	
	# Example: Initially, the %rdi register points to the first 
	# pixel in the last row of the image.  The following three 
	# instructions change its color to red.

	# movb 	$0x00, (%rdi)			# blue
	# movb	$0x00, 1(%rdi)			# green
	# movb	$0xff, 2(%rdi)			# red

	# FILL HERE
	# You are allowed to use any register supported by Y86-64 architecture
	movq $0x00, %r13
	movq $3, %rbx # store number 3
	pushq %rax
	pushq %rbp

	# multiply 3 to gap for pointer move
	imulq %rbx, %rcx
	
	# multiply 3 to original width for pointer move
	imulq %rbx, %rsi

	# padding process
	padding:
		# r8 is used for fixed width (considering padding)
		movq %rsi, %r8 # r8 = 3 * rsi 
		addq $3, %r8 
		sar $2, %r8
		sal $2, %r8

		movq %rdx, %r9 

		addq $-1, %r9
		imulq %r8 ,%r9 

	# should move pointer 
	initial:
	addq %r9, %rdi # move pointer up

	start:
	movq $0, %r10 # used for offset value
	movq %rdi, %r11 # used for base imgptr for each horizontal line
	movq $-1, %r12 # use %r12 for gap number

	# gap number calculation
	pushq %rsi
	cal:
	addq $1, %r12
	subq %rcx, %rsi
	jg cal # ge or g ??????
	popq %rsi

	# %r12 even or odd? if odd, we must move one turn in advance
	movq $2, %r9
	movq %r12, %rax
	sarq $1, %rax
	salq $1, %rax
	movq %rdx, %r9 # used for vertical index	
	subq %r12, %rax 
	jne draw_odd 	# if rax is odd, jmp to draw_odd

	draw_even:
		movq %r12, %rbx # rbx used for r12
		movq %r11, %rdi
		addq %r10, %rdi # start from offset
		
		movq %rcx, %rbp
		subq %r10, %rbp
		addq %r11, %rbp # base pointer + ( gap - offset)
		
		loop_even:
		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)
		
		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		addq %rcx, %rdi
		addq %rcx, %rbp

		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)
		
		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		addq %rcx, %rdi
		addq %rcx, %rbp

		addq $-2, %rbx
		jg loop_even
		
		# last loop (near boundary)
		last_loop_even:	
		# boundary test
		L_l_even:
		movq %r11, %rbx
		addq %rsi, %rbx
		subq %rbp ,%rbx
		jle L_r_even
		
		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		L_r_even:
		movq %r11, %rbx
		addq %rsi, %rbx
		subq %rdi ,%rbx
		jle loop_end_even
		
		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)
		
		loop_end_even:
			addq $-1, %r9 # decrease vertical index
			movq %rcx, %rbp
			addq $3, %r10 # increase offset
			subq %r8, %r11 # update base pointer

			# compare offset with gap
			subq %r10, %rbp
			cmove %r13, %r10
			
			# out of hegith? test
			movq %r9, %rbx
			addq $-1, %rbx # compare with 1
			jge draw_even
		
		end_even:
			popq %rbp
			popq %rax
			ret

		draw_odd:
		movq %r12, %rbx # rbx used for r12
		movq %r11, %rdi
		addq %r10, %rdi # start from offset
		
		movq %rcx, %rbp
		subq %r10, %rbp
		addq %r11, %rbp # base pointer + ( gap - offset)
		
		# in odd case, we must move once more
		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)

		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		addq %rcx, %rdi
		addq %rcx, %rbp

		addq $-1, %rbx

		loop_odd:
		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)
		
		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		addq %rcx, %rdi
		addq %rcx, %rbp

		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)
		
		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		addq %rcx, %rdi
		addq %rcx, %rbp

		addq $-2, %rbx
		jg loop_odd
		
		# last loop (near boundary)
		last_loop_odd:	
		# boundary test
		L_l_odd:
		movq %r11, %rbx
		addq %rsi, %rbx
		subq %rbp ,%rbx
		jle L_r_odd
		
		movb $0x00, (%rbp)
		movb $0x00, 1(%rbp)
		movb $0xff, 2(%rbp)
		
		L_r_odd:
		movq %r11, %rbx
		addq %rsi, %rbx
		subq %rdi ,%rbx
		jle loop_end_odd
		
		movb $0x00, (%rdi)
		movb $0x00, 1(%rdi)
		movb $0xff, 2(%rdi)
		
		loop_end_odd:
			addq $-1, %r9 # decrease vertical index
			movq %rcx, %rbp
			addq $3, %r10 # increase offset
			subq %r8, %r11 # update base pointer

			# compare offset with gap
			subq %r10, %rbp
			cmove %r13, %r10
			
			# out of hegith? test
			movq %r9, %rbx
			addq $-1, %rbx # compare with 1
			jge draw_odd
		
		end_odd:
			popq %rbp
			popq %rax
		
	#-----------------------------------------------------------------

	ret
