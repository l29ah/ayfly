@ Generated by gcc 2.9-psion-98r2 (Symbian build 539) for ARM/pe
	.file	"ayfly_s60Application.cpp"
.gcc2_compiled.:
	.section .rdata
	.align	0
.LC0:
	.ascii	"%1c%1c%1c%1c\000"
	.align	0
applicationName.3874:
	.word	9
	.ascii	"a\000y\000f\000l\000y\000_\000s\0006\0000\000\000\000"
.text
	.align	0
	.global	__21Cayfly_s60Application
__21Cayfly_s60Application:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0
	stmfd	sp!, {r4, lr}
	mov	r4, r0
	bl	__15CEikApplication
	ldr	r3, .L3933
	str	r3, [r4, #0]
	mov	r0, r4
	b	.L3932
.L3934:
	.align	0
.L3933:
	.word	__vt_21Cayfly_s60Application
.L3932:
	ldmfd	sp!, {r4, lr}
	bx	lr
	.align	0
	.global	_._21Cayfly_s60Application
_._21Cayfly_s60Application:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0
	@ I don't think this function clobbers lr
	ldr	r3, .L3939
	str	r3, [r0, #0]
	b	_._15CEikApplication
.L3940:
	.align	0
.L3939:
	.word	__vt_21Cayfly_s60Application
	.align	0
	.global	CreateDocumentL__21Cayfly_s60Application
CreateDocumentL__21Cayfly_s60Application:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0
	@ I don't think this function clobbers lr
	b	NewL__18Cayfly_s60DocumentR15CEikApplication
	.align	0
	.global	AppDllUid__C21Cayfly_s60Application
AppDllUid__C21Cayfly_s60Application:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0
	ldr	r3, .L3943
	ldr	r0, [r3, #0]
	bx	lr
.L3944:
	.align	0
.L3943:
	.word	KUidayfly_s60App
	.section .text$Delete__t13CleanupDelete1Z11MDataSourcePv,"x"
	.linkonce discard
	.align	0
	.global	Delete__t13CleanupDelete1Z11MDataSourcePv
Delete__t13CleanupDelete1Z11MDataSourcePv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0
	stmfd	sp!, {lr}
	cmp	r0, #0
	ldmeqfd	sp!, {ip}
	bxeq	ip
	ldr	r3, [r0, #8]
	mov	r1, #3
	ldr	ip, [r3, #8]
	mov	lr, pc
	bx	ip
	ldmfd	sp!, {ip}
	bx	ip
	.section .text$Delete__t13CleanupDelete1Z9MDataSinkPv,"x"
	.linkonce discard
	.align	0
	.global	Delete__t13CleanupDelete1Z9MDataSinkPv
Delete__t13CleanupDelete1Z9MDataSinkPv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0
	stmfd	sp!, {lr}
	cmp	r0, #0
	ldmeqfd	sp!, {ip}
	bxeq	ip
	ldr	r3, [r0, #8]
	mov	r1, #3
	ldr	ip, [r3, #8]
	mov	lr, pc
	bx	ip
	ldmfd	sp!, {ip}
	bx	ip
	.global	__vt_21Cayfly_s60Application
	.section .rdata$__vt_21Cayfly_s60Application,"w"
	.linkonce same_size
	.align	0
__vt_21Cayfly_s60Application:
	.word	0
	.word	0
	.word	_._21Cayfly_s60Application
	.word	PreDocConstructL__15CAknApplication
	.word	CreateDocumentL__15CEikApplicationP11CApaProcess
	.word	AppDllUid__C21Cayfly_s60Application
	.word	OpenIniFileLC__C15CAknApplicationR3RFs
	.word	OpenAppInfoFileLC__C15CEikApplication
	.word	AppFullName__C15CApaApplication
	.word	Capability__C15CEikApplicationR5TDes8
	.word	Reserved_1__15CEikApplication
	.word	GetDefaultDocumentFileName__C15CEikApplicationRt4TBuf1i256
	.word	BitmapStoreName__C15CEikApplication
	.word	ResourceFileName__C15CEikApplication
	.word	CreateDocumentL__21Cayfly_s60Application
	.section .rdata
	.align	0
KUidayfly_s60App:
	.word	-368351498
