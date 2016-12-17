return strdup(
"=PC	eip\n"
"=SP	esp\n"
"=BP	ebp\n"
"=A0	eax\n"
"=A1	ebx\n"
"=A2	ecx\n"
"=A3	edi\n"
"drx	dr0	.32	4	0\n"
"drx	dr1	.32	8	0\n"
"drx	dr2	.32	12	0\n"
"drx	dr3	.32	16	0\n"
"drx	dr6	.32	20	0\n"
"drx	dr7	.32	24	0\n"
/* floating save area 4+4+4+4+4+4+4+80+4 = 112 */
"seg	gs	.32	140	0\n"
"seg	fs	.32	144	0\n"
"seg	es	.32	148	0\n"
"seg	ds	.32	152	0\n"
"gpr	edi	.32	156	0\n"
"gpr	esi	.32	160	0\n"
"gpr	ebx	.32	164	0\n"
"gpr	edx	.32	168	0\n"
"gpr	ecx	.32	172	0\n"
"gpr	eax	.32	176	0\n"
"gpr	ebp	.32	180	0\n"
"gpr	eip	.32	184	0\n"
"seg	cs	.32	188	0\n"
"gpr	eflags	.32	192	0	c1p.a.zstido.n.rv\n" // XXX must be flg
"gpr	esp	.32	196	0\n"
"seg	ss	.32	200	0\n"
"gpr	cf	.1	.1536	0	carry\n"
"gpr	pf	.1	.1538	0	parity\n"
"gpr	af	.1	.1540	0	adjust\n"
"gpr	zf	.1	.1542	0	zero\n"
"gpr	sf	.1	.1543	0	sign\n"
"gpr	tf	.1	.1544	0	trap\n"
"gpr	if	.1	.1545	0	interrupt\n"
"gpr	df	.1	.1546	0	direction\n"
"gpr	of	.1	.1547	0	overflow\n"
/* +512 bytes for maximum supoprted extension extended registers */
);