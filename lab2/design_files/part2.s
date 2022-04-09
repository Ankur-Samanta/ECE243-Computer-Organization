/* Program that counts consecutive 1's */

          .text                   // executable code follows
          .global _start                  
_start:                             
          MOV     R4, #TEST_NUM   // R4 gets the address of TEST_NUM
          MOV     R5, #0            // R5 will hold the result
		  
MAIN_LOOP:
		  LDR	R1, [R4], #4    //get next word of data and load into R1
                                // post-index mode, use R4 to read the data word, and then increment by 4; every time through the main loop,
                                //R4 will point to the next word in memory
		  CMP	R1, #0          //checks if we are done (if bottom is all 0s)
		  BEQ	END             //then branch to the end
		  BL	RESET_START     //if we aren't done, then we have to shift/and, so we call the subroutine to find
		  CMP	R5, R0          //compare the current best result in R5 with the best result found by the subroutine
		  MOVLT	R5, R0          //MOVLT is a conditional move instruction; move less than; if result better, put in R5
		  
		  B	MAIN_LOOP

END:      B       END           // R1 has the answer

RESET_START:     MOV     R0, #0          // R0 will hold the result
ONES:     CMP     R1, #0          // loop until the data contains no more 1's
          MOVEQ     PC, LR             
          LSR     R2, R1, #1      // perform SHIFT, followed by AND
          AND     R1, R1, R2      
          ADD     R0, #1          // count the string length so far
          B       ONES   

TEST_NUM: .word   0x103fe00f
			.word   0x103fe00f 
			.word   0x0000000f
			.word   0x000000ff
			.word   0x00000fff
			.word   0x0000ffff
			.word   0x000fffff
			.word   0x00ffffff
			.word   0x0fffffff
			.word   0xffffffff
			.word   0x00000009
			.word   0x0000000a
			.word   0x0000ffff
			.word 	0x0            //end of list, when program sees 0 it ends

          .end                            
