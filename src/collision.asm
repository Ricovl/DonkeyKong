	.assume adl=1

	.ref _game
	.ref _barrel
	.ref _num_barrels
	.ref _firefox
	.ref _num_firefoxes
	.ref _bouncer
	.ref _num_bouncers
	.ref _oilcan
	.ref _pie
	.ref _num_pies
	.ref _jumpman

	.def _check_collision_jumpman
	
	; link to collision code from original DonkeyKong:
	; https://github.com/furrykef/dkdasm/blob/master/dkong.asm#L8114
	; something about the hitboxes: http://donkeykongforum.com/index.php?topic=493.0


_check_collision_jumpman:
	push	iy
	push	ix
	push	bc
    ld  iy,_jumpman
	ld	hl,0
    ld	bc,0
	ld  hl,0406h			; width = 4, height = 6(might be 7?)
    call    check_collision_entities
	pop	bc
	pop	ix
	pop	iy
	ret	z					; return if there was no collision
	xor	a,a
	ld	(_jumpman+31),a		; else jumpman is dead
	ret



check_collision_entities:
; check collision barrels if stage is barrels
	ld	a,(_game)			; a = game.stage
	cp	1
	jr	nz,skip_barrels

    ld  a,(_num_barrels)
	cp	0
	jr	z,skip_barrels		; skip if there are no barrels
    ld  b,a					; b = num_barrels

    ld	de,(iy+05h)			; de = x item 1
	ld	a,(iy+01h)
	sub	a,3					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+01h),de			; x item1
	ld	(iy+00h),a			; y item1

    ld  de,187				; barrel struct size
    ld  ix,_barrel			; ix = &barrel

	push	hl
	ld	a,h
	add a,3
	ld	h,a					; h = width += 2 + 1
	ld	a,l
	add	a,3
	ld	l,a					; l = height += 2 + 1

    call check_collision
	pop	hl
	pop	iy
skip_barrels:

; check collision fireballs/firefoxes
    ld  a,(_num_firefoxes)
	cp	0
	jr	z,skip_firefoxes	; skip if there are no fireballs/firefoxes
    ld  b,a					; b = num_firefoxes

    ld	de,(iy+#05)			; de = x item 1
	ld	a,(iy+#01)			; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+00h),a			; y item1
    ld  de,279				; firefox struct size
    ld  ix,_firefox			; ix = &firefox

	push	hl
	ld	a,(_game)			; a = game.stage
	cp	4
	jr	nz,isFireball		; if (stage != rivets) fireball else firefox
	ld	a,h
	add a,5
	ld	h,a					; h = width += 4 + 1
	ld	a,l
	add	a,2
	ld	l,a					; l = height += 1 + 1
	jr	skipFireball
isFireball:
	ld	a,h
	add a,3
	ld	h,a					; h = width += 3 + 1
	ld	a,l
	add	a,3
	ld	l,a					; l = height += 2 + 1
skipFireball:

    call check_collision
	pop	hl
	pop	iy
skip_firefoxes:

; check collision pies if stage is conveyors
	ld	a,(_game)			; a = game.stage
	cp	2
	jr	nz,skip_pies

    ld  a,(_num_pies)
	cp	0
	jr	z,skip_pies			; skip if there are no pies
    ld  b,a					; b = num_pies

	push	hl
    ld	hl,(iy+05h)
	ld	de,7
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,(iy+01h)
	sub	a,11				; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+01h),hl			; x item1
	ld	(iy+00h),a			; y item1

    ld  de,138				; pie struct size
    ld  ix,_pie				; ix = &pie

	push	hl
	ld	a,h
	add a,4
	ld	h,a					; h = width += 3 + 1
	ld	a,l
	add	a,9
	ld	l,a					; l = height += 8 + 1

    call check_collision
	pop	hl
	pop	iy
skip_pies:

	xor	a,a					; A := 0 - code for no collision
	ret



check_collision:
; Checks for a collision between two objects
; Arguments:
;  iy	= struct location  item 1
;  ix	= struct location  item 2
;  de	= struct size      item 2
;  hl	= width and height item 1 and item 2 combined
;  b	= ammount of times to loop
; Returns:
;  none
    push    ix          ; push IX to stack
; start of loop
loop:
; check for y in range
    ld  a,(iy+00h)		; no, load A with C = y of item 1
    sub a,(ix+01h)		; subtract the Y value of item 2
    jr  nc,+skip_neg_0	; if no carry, skip next step

    neg					; a = 0 - a
skip_neg_0:
    sub a,l				; subtract l = width
    jr  nc,end_loop		; if no carry, loop again

; check for x in range
	push	hl
	push	de
	ld	hl,(iy+01h)		; hl = item 1 x
	ld	de,(ix+05h)		; de = item 2 x
	or	a,a				; reset flags(Don't know if I need this here?)
	sbc	hl,de			; substract item 1 x from item 2 x
	ld	a,l				; put in a
	jr	nc,+skip_neg_1	; if no carry, skip next step

	neg					; a = 0 - a
skip_neg_1:
	pop	de
	pop	hl
	sub	h				; substract h = height
	jr	nc,end_loop		; if no carry, loop again else handle collision

; else a collision
    ld  a,#01			; A := 1 - code for collision
    pop ix              ; restore IX
	pop	hl
	pop	iy
    inc sp              ; adjust SP for higher level subroutine
    inc sp
	inc sp
	ret                 ; return to higher subroutine

end_loop:
; no collision
    add ix,de           ; add offset for next sprite
    djnz	loop		; next item

    pop ix              ; restore IX
    ret                 ; return



data_stuff:
	DL 0, 0, 0, 0