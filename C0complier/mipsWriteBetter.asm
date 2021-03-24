.data
	Enter: .asciiz  "\n"
.text
ori	$fp	$sp	0
li	$t9	0x7fffeffc	#global stack bottom
li	$t8	0x10010000	#save word
j	__main
operation:
#Save Register
sw	$s1	4($t8)
sw	$s2	8($t8)
sw	$s3	12($t8)
sw	$s4	16($t8)
sw	$s5	20($t8)
sw	$s6	24($t8)
sw	$s7	28($t8)
sw	$s0	0($sp)
subi	$sp	$sp	4
sw	$fp	0($sp)
subi	$sp	$sp	4
add	$fp	$sp	$0
sw	$ra	($sp)
subi	$sp	$sp	4
#Save Register Done!
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
lw	$t0	12($fp)
li	$t1	1
bne	$t0	$t1	__tLABEL10
li	$t0	1
j	__tLABEL11
__tLABEL10:
li	$t0	0
__tLABEL11:
bne	$t0	1	_LABEL_1
lw	$t0	20($fp)
lw	$t1	16($fp)
add	$t0	$t0	$t1
sw	$t0	-4($fp)
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	112
syscall
li	$v0	11
li	$a0	101
syscall
li	$v0	11
li	$a0	114
syscall
li	$v0	11
li	$a0	97
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	58
syscall
li	$v0	1
lw	$a0	-4($fp)
syscall
li	$v0	4
la	$a0	Enter
syscall
lw	$t0	20($fp)
lw	$t1	16($fp)
add	$t0	$t0	$t1
sw	$t0	-8($fp)
lw	$v0	-8($fp)
j	__FEND_LAB_3
j	_LABEL_0
_LABEL_1:
lw	$t0	12($fp)
li	$t1	2
bne	$t0	$t1	__tLABEL12
li	$t0	1
j	__tLABEL13
__tLABEL12:
li	$t0	0
__tLABEL13:
bne	$t0	1	_LABEL_2
lw	$t0	20($fp)
lw	$t1	16($fp)
sub	$t0	$t0	$t1
sw	$t0	-12($fp)
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	112
syscall
li	$v0	11
li	$a0	101
syscall
li	$v0	11
li	$a0	114
syscall
li	$v0	11
li	$a0	97
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	58
syscall
li	$v0	1
lw	$a0	-12($fp)
syscall
li	$v0	4
la	$a0	Enter
syscall
lw	$t0	20($fp)
lw	$t1	16($fp)
sub	$t0	$t0	$t1
sw	$t0	-16($fp)
lw	$v0	-16($fp)
j	__FEND_LAB_3
j	_LABEL_0
_LABEL_2:
lw	$t0	12($fp)
li	$t1	3
bne	$t0	$t1	__tLABEL14
li	$t0	1
j	__tLABEL15
__tLABEL14:
li	$t0	0
__tLABEL15:
bne	$t0	1	_LABEL_3
lw	$t0	20($fp)
lw	$t1	16($fp)
mul	$t0	$t0	$t1
sw	$t0	-20($fp)
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	112
syscall
li	$v0	11
li	$a0	101
syscall
li	$v0	11
li	$a0	114
syscall
li	$v0	11
li	$a0	97
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	58
syscall
li	$v0	1
lw	$a0	-20($fp)
syscall
li	$v0	4
la	$a0	Enter
syscall
lw	$t0	20($fp)
lw	$t1	16($fp)
mul	$t0	$t0	$t1
sw	$t0	-24($fp)
lw	$v0	-24($fp)
j	__FEND_LAB_3
j	_LABEL_0
_LABEL_3:
lw	$t0	12($fp)
li	$t1	4
bne	$t0	$t1	__tLABEL16
li	$t0	1
j	__tLABEL17
__tLABEL16:
li	$t0	0
__tLABEL17:
bne	$t0	1	_LABEL_4
lw	$t0	16($fp)
li	$t1	0
beq	$t0	$t1	__tLABEL18
li	$t0	1
j	__tLABEL19
__tLABEL18:
li	$t0	0
__tLABEL19:
bne	$t0	1	_LABEL_5
lw	$t0	20($fp)
lw	$t1	16($fp)
div	$t0	$t0	$t1
sw	$t0	-28($fp)
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	112
syscall
li	$v0	11
li	$a0	101
syscall
li	$v0	11
li	$a0	114
syscall
li	$v0	11
li	$a0	97
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	58
syscall
li	$v0	1
lw	$a0	-28($fp)
syscall
li	$v0	4
la	$a0	Enter
syscall
lw	$t0	20($fp)
lw	$t1	16($fp)
div	$t0	$t0	$t1
sw	$t0	-32($fp)
lw	$v0	-32($fp)
j	__FEND_LAB_3
j	_LABEL_6
_LABEL_5:
li	$v0	11
li	$a0	99
syscall
li	$v0	11
li	$a0	97
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	100
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	118
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	48
syscall
li	$v0	11
li	$a0	33
syscall
li	$v0	11
li	$a0	33
syscall
li	$v0	11
li	$a0	33
syscall
li	$v0	0
j	__FEND_LAB_3
_LABEL_6:
j	_LABEL_0
_LABEL_4:
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	112
syscall
li	$v0	11
li	$a0	101
syscall
li	$v0	11
li	$a0	114
syscall
li	$v0	11
li	$a0	97
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	107
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	100
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	115
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	111
syscall
li	$v0	11
li	$a0	116
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	105
syscall
li	$v0	11
li	$a0	110
syscall
li	$v0	11
li	$a0	32
syscall
li	$v0	11
li	$a0	91
syscall
li	$v0	11
li	$a0	43
syscall
li	$v0	11
li	$a0	45
syscall
li	$v0	11
li	$a0	42
syscall
li	$v0	11
li	$a0	47
syscall
li	$v0	11
li	$a0	93
syscall
li	$v0	11
li	$a0	33
syscall
li	$v0	11
li	$a0	33
syscall
li	$v0	11
li	$a0	33
syscall
li	$t0	0 
li	$t1	1
sub	$t0	$t0	$t1
sw	$t0	-36($fp)
lw	$v0	-36($fp)
j	__FEND_LAB_3
_LABEL_0:
__FEND_LAB_3:
lw	$ra	0($fp)
lw	$sp	8($fp)
lw	$fp	4($fp)
lw	$s1	4($t8)
lw	$s2	8($t8)
lw	$s3	12($t8)
lw	$s4	16($t8)
lw	$s5	20($t8)
lw	$s6	24($t8)
lw	$s7	28($t8)
jr	$ra	
__main:
#Save Register
sw	$s1	4($t8)
sw	$s2	8($t8)
sw	$s3	12($t8)
sw	$s4	16($t8)
sw	$s5	20($t8)
sw	$s6	24($t8)
sw	$s7	28($t8)
sw	$s0	0($sp)
subi	$sp	$sp	4
sw	$fp	0($sp)
subi	$sp	$sp	4
add	$fp	$sp	$0
sw	$ra	($sp)
subi	$sp	$sp	4
#Save Register Done!
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#x
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#y
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#kind
sw	$t0	($sp)
subi	$sp	$sp	4
li	$t0	0	#ans
sw	$t0	($sp)
subi	$sp	$sp	4
lw	$s1	-20($fp)
lw	$s2	-4($fp)
lw	$s3	-16($fp)
lw	$s4	-12($fp)
lw	$s5	-8($fp)
li	$v0	5
syscall
add	$s5	$v0	$0
li	$v0	5
syscall
add	$s4	$v0	$0
li	$v0	5
syscall
add	$s3	$v0	$0
move	$s0	$sp
add	$t0	$s5	$0
sw	$t0	($sp)
subi	$sp	$sp	4
add	$t0	$s4	$0
sw	$t0	($sp)
subi	$sp	$sp	4
add	$t0	$s3	$0
sw	$t0	($sp)
subi	$sp	$sp	4
jal	operation
nop
add	$s2	$v0	$0
add	$t0	$s2	$0
add	$s1	$t0	$0
j	__FEND_LAB_4
__FEND_LAB_4:
lw	$ra	0($fp)
lw	$sp	8($fp)
lw	$fp	4($fp)
lw	$s1	4($t8)
lw	$s2	8($t8)
lw	$s3	12($t8)
lw	$s4	16($t8)
lw	$s5	20($t8)
lw	$s6	24($t8)
lw	$s7	28($t8)
li	$v0	10
syscall
