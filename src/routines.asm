	.assume adl=1

	.def _handle_jumping
	.def _handle_bouncing

_handle_jumping
	pop hl			; get return address in de
	ex (sp),ix		; store the given struct address in ix
	push hl			; push the return address back in the stack

	; These lines change the barrels/jumpmans x-position
	ld a,(ix+10h)	; load a with jumpcounterX
	add a,(ix+0Eh)	; add jumpDirIndicator #80 for left or right, 0 for up, this way left en right jumping is slower than walking		
	ld (ix+10h),A	; save in jumpcounterX

	ld bc,-1
	ld a,(ix+0Dh)	; load a with jumpDir
	bit 7,a
	jr nz,right		; jump if a is negative
	inc bc			; else increase bc to 0
right:
	ld c,a
	
	ld hl,(ix+2)	; load object's X position
	adc hl,bc		; add jumpDir. Note this is add with carry
	ld (ix+2),hl	; store into X position


	ld hl, 0
	ld bc, 0
	; These lines change the barrels/jumpmans y-position
	ld a,(ix+11h)	; load a with jumpCounterY
	sub (ix+12h)	; subtract velocityY
	ld l,a			; store into l
	ld a,(ix+00)	; load a with barrel/jumpman Y position
	sbc a,(ix+13h)	; subtract vertical speed == movingUp
	ld h,a			; store into h
	ld a,(ix+0Fh)	; load jumpcounter
	and a			; clear flags
	rla				; rotate left (mult by 2)
	inc a			; add 1
	ld b,0			; b := 0
	rl b			;
	sla a
	rl b
	sla a
	rl b
	sla a
	rl b
	ld c,a			; copy answer (a) to c. bc now has ???
	add hl,bc		; add to hl
	ld (ix+00),h	; update Y position
	ld (ix+11h),l	; update jumpCounterY
	inc (ix+0Fh)	; increase jumpcounter


	pop hl			; pop return address in hl	
	pop ix			; restore ix
	push bc			; forced to do this by the compiler :(
	jp (hl)



_handle_bouncing
	pop hl			; get return address in de
	ex (sp),ix		; store the given struct address in ix
	push hl			; push the return address back in the stack

	ld hl, 0
	ld bc, 0

	ld a,(IX+15)	; load a with +15 status
	rlca
	rlca
	rlca
	rlca			; rotate left 4 times
	ld c,a			; save to C for use next 2 steps
	and a,15		; mask with #0F.  now between #00 and #0F
	ld h,a			; store into H
	ld a,c			; restore a to value saved above
	and a,240		; mask with #F0
	ld l,a			; store into L
	ld c,(ix+18)	; load C with +18
	ld b,(ix+19)	; load B with +19
	sbc hl,bc		; hl := hl - BC

	pop de			; pop return address in de
	pop ix			; restore ix
	push bc			; forced to do this by the compiler :(
	push de
	ret