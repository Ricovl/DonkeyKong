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
	
	.ref _hammer
	.ref _hammerActive

	.ref _wasItemHit
	.ref _hitItemType
	.ref _hitItemNum

	.def _check_collision_jumpman
	.def _check_collision_hammer
	.def _check_jump_over_item
	
	; link to collision code from original DonkeyKong:
	; something about the hitboxes: http://donkeykongforum.com/index.php?topic=493.0

	; 	type	  width	height
	; jumpman	:	4	8
	; barrels	:	2	2
	; fireballs	:	3	2
	; firefoxes	:	4	1
	; bouncers	:	2	2
	; pies		:	8	3
	; oilfire	:	2	0		(2, 2 if large fire)


; Check for collision between jumpman and a hostile
_check_collision_jumpman:
	push	iy
	push	ix
    ld  iy,_jumpman
	ld	a,(iy+#01)			; a = jumpman.y_old
	ld	c,a					; c = jumpman.y_old
	ld  hl,0407h			; width = 4, height = 7
    call    check_collision_entities
	pop	ix
	pop	iy
	ret	z					; return if there was no collision
	xor	a,a
	ld	(_jumpman+31),a		; else jumpman is dead
	ret

; Check for collision between hammer and a hostile
_check_collision_hammer:
	ld	a,(_hammerActive)
	cp	0
	ret	z

	push	iy
	push	ix
	ld  iy,_hammer
	cp	1
	jr	z,firstHammer
	ld	iy,_hammer+173
firstHammer:
	ld	a,(iy+#01)			; a = jumpman.y_old
	ld	c,a					; c = jumpman.y_old
	ld	hl,0605h
	call	check_collision_entities
	pop	ix
	pop	iy

	ret	z				; return if there was no collision
	ld	a,(_hitItemNum)
	sub	a,b
	ld	(_hitItemNum),a
	ld	a,#01
	ld	(_wasItemHit),a
	ret

; Check if jumpman jumped over a hostile
_check_jump_over_item:
	push	iy
	push	ix
	ld	iy,_jumpman
	ld	a,(iy+#01)			; a = jumpman.y_old
	add	a,09h				; 0C-7?
	ld	c,a					; c = jumpman.y_old + 5
	ld	a,(iy+0Eh)			; a = jumpDirIndicator
	or	a,a
	ld	hl,0508h			; hitbox when jumping straight up
	jr	z,jumpingUp
	ld	hl,1208h			; hitbox when jumping right/left
jumpingUp:
	ld	a,(_game)
	cp	1
	jr	nz,skip_barrels_check
	call 	check_jump_barrels
	jr	end_check_over
skip_barrels_check:
	call	skip_barrels
end_check_over:
	pop	ix
	pop	iy
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

	ld	(_hitItemNum),a
	ld	a,0
	ld	(_hitItemType),a

    ld	de,(iy+05h)			; de = x item 1
	ld	a,c
	sub	a,3					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+01h),de			; x item1
	ld	(iy+00h),a			; y item1

	push	hl
	ld	de,0303h
	add	hl,de				; width += 2 + 1, height += 2 + 1

    ld  de,187				; barrel struct size
    ld  ix,_barrel			; ix = &barrel

    call check_collision
	pop	hl
	pop	iy
skip_barrels:

; check collision fireballs/firefoxes if stage != conveyors
    ld	a,(_game)			; a = game.stage
	cp	2
	jr	z,skip_firefoxes
	
	ld  a,(_num_firefoxes)
	cp	0
	jr	z,skip_firefoxes	; skip if there are no fireballs/firefoxes
    ld  b,a					; b = num_firefoxes

	ld	(_hitItemNum),a
	ld	a,1
	ld	(_hitItemType),a

    ld	de,(iy+#05)			; de = x item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+00h),c			; y item1
	
	push	hl
	ld	a,(_game)			; a = game.stage
	cp	4
	ld 	de,0403h			; width += 3 + 1, height += 2 + 1
	jr	nz,skipFirefox		; if (stage != rivets) fireball else firefox
	ld	de,0502h			; width += 4 + 1, height += 1 + 1
skipFirefox:
	add	hl,de

    ld  de,279				; firefox struct size
    ld  ix,_firefox			; ix = &firefox

    call check_collision
	pop	hl
	pop	iy
	jr	skip_pies
skip_firefoxes:

; check collision pies if stage is conveyors
    ld  a,(_num_pies)
	cp	0
	jr	z,skip_pies			; skip if there are no pies
    ld  b,a					; b = num_pies

	ld	(_hitItemNum),a
	ld	a,2
	ld	(_hitItemType),a

	push	hl
    ld	hl,(iy+05h)
	ld	de,7
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	sub	a,11				; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+01h),hl			; x item1
	ld	(iy+00h),a			; y item1

	push	hl
	ld	de,0409h
	add	hl,de				; width += 3 + 1, height += 8 + 1

    ld  de,138				; pie struct size
    ld  ix,_pie				; ix = &pie

    call check_collision
	pop	hl
	pop	iy
skip_pies:

; check collision bouncers if stage is elevators
	ld	a,(_game)			; a = game.stage
	cp	3
	jr	nz,skip_bouncers

    ld  a,(_num_bouncers)
	cp	0
	jr	z,skip_bouncers		; skip if there are no bouncers
    ld  b,a					; b = num_bouncers

	push	hl
    ld	hl,(iy+05h)
	ld	de,7
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	sub	a,7					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+01h),de			; x item1
	ld	(iy+00h),a			; y item1

	push	hl
	ld	de,0303h
	add	hl,de				; width += 2 + 1; height += 2 + 1

    ld  de,255				; bouncer struct size
    ld  ix,_bouncer			; ix = &bouncer

    call check_collision
	pop	hl
	pop	iy
skip_bouncers:

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
    push    ix          ; push IX to stack (don't need this right?)<<<<<<<<<<<<<<<
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




check_jump_barrels:
	xor	a,a
	ld	(NumObstaclesJumped),a

    ld  a,(_num_barrels)
	cp	0
	jr	z,skip_barrels_		; skip if there are no barrels
    ld  b,a					; b = num_barrels

    ld	de,(iy+05h)			; de = x item 1
	ld	a,c
	sub	a,3					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+01h),de			; x item1
	ld	(iy+00h),a			; y item1

	push	hl
	ld	de,0303h
	add	hl,de				; width += 2 + 1, height += 2 + 1

    ld  de,187				; barrel struct size
    ld  ix,_barrel			; ix = &barrel

    call count_items_collided
	pop	hl
	pop	iy
skip_barrels_:

; check collision fireballs/firefoxes
    ld  a,(_num_firefoxes)
	cp	0
	jr	z,skip_firefoxes_	; skip if there are no fireballs/firefoxes
    ld  b,a					; b = num_firefoxes

    ld	de,(iy+#05)			; de = x item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+00h),c			; y item1
	
	push	hl
	ld	a,(_game)			; a = game.stage
	cp	4
	ld 	de,0403h			; width += 3 + 1, height += 2 + 1
	jr	nz,skipFirefox_		; if (stage != rivets) fireball else firefox
	ld	de,0502h			; width += 4 + 1, height += 1 + 1
skipFirefox_:
	add	hl,de

    ld  de,279				; firefox struct size
    ld  ix,_firefox			; ix = &firefox

    call count_items_collided
	pop	hl
	pop	iy
skip_firefoxes_:

	ld	a,(NumObstaclesJumped)
	and	a,a

	ret	z					; return if no items jumped
	cp	#01					; 1 item jumped?
	ret	z					; yes, 1; 100 pt

	cp	#03					; less than 3 items jumped?
	ld	a,#03				; 2; 300
	ret	c					; yes, return

	LD      A,#07           ; else 3+ items jumped, 7; 800 pt
	ret


count_items_collided:
; Checks for a collision between two objects
; Arguments:
;  iy	= struct location  item 1
;  ix	= struct location  item 2
;  de	= struct size      item 2
;  hl	= width and height item 1 and item 2 combined
;  b	= ammount of times to loop
; Returns:
;  none

; start of loop
loop_:
; check for y in range
    ld  a,(iy+00h)		; no, load A with C = y of item 1
    sub a,(ix+01h)		; subtract the Y value of item 2
    jr  nc,+skip_neg_0_	; if no carry, skip next step

    neg					; a = 0 - a
skip_neg_0_:
    sub a,l				; subtract l = width
    jr  nc,end_loop_	; if no carry, loop again

; check for x in range
	push	hl
	push	de
	ld	hl,(iy+01h)		; hl = item 1 x
	ld	de,(ix+05h)		; de = item 2 x
	or	a,a				; reset flags(Don't know if I need this here?)
	sbc	hl,de			; substract item 1 x from item 2 x
	ld	a,l				; put in a
	jr	nc,+skip_neg_1_	; if no carry, skip next step

	neg					; a = 0 - a
skip_neg_1_:
	pop	de
	pop	hl
	sub	h				; substract h = height
	jr	nc,end_loop_	; if no carry, loop again else handle collision

; else a collision
    ld	a,(NumObstaclesJumped)
	inc	a
	ld	(NumObstaclesJumped),a

end_loop_:
; no collision
    add ix,de           ; add offset for next sprite
    djnz	loop_		; next item

    ret                 ; return


data_stuff:
	DL 0, 0, 0, 0

NumObstaclesJumped:
	DL 0