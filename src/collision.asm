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
	; jumpman	:	4	7
	; hammer	: 	3	9			(6,	3 if above head; 5, 6 if in front)
	; barrels	:	2	2
	; fireballs	:	3	2
	; firefoxes	:	4	1
	; bouncers	:	2	2
	; pies		:	8	3
	; oilfire	:	2	0			(2, 2 if large fire)

;-------------------------------------------------------------------------------
_check_collision_jumpman:
; Check for collision between jumpman and a hostile
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
	ld	(_jumpman+32),a		; else jumpman is dead
	ret

;-------------------------------------------------------------------------------
_check_collision_hammer:
; Check for collision between hammer and a hostile
	ld	a,(_hammerActive)
	dec	a
	ret	m					; return if not holding hammer

	push	iy
	push	ix
	ld  iy,_hammer			; iy = address first hammer
	jr	z,firstHammer
	ld	iy,_hammer+173		; iy = address second hammer
firstHammer:
	ld	a,(iy+#01)			; a = hammer.y_old
	ld	c,a					; c = hammer.y_old

	ld	a,(_hammer+0Ah)		; a = hammer.sprite
	and	a,1
	ld	hl,0603h			; width = 6; height = 3
	jr	z,aboveHead
	ld	hl,0506h			; width = 6; height = 5
aboveHead:
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

;-------------------------------------------------------------------------------
_check_jump_over_item:
; Check if jumpman jumped over a hostile
; Returns:
;  Number of hostiles jumped
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
	call 	check_jump_stage_barrels
	jr	end_check_over
skip_barrels_check:
	call	skip_barrels
end_check_over:
	pop	ix
	pop	iy
	ret



;-------------------------------------------------------------------------------
check_collision_entities:
; check collision barrels if stage is barrels
	ld	a,(_game)			; a = game.stage
	dec	a
	jr	nz,skip_barrels

    ld  a,(_num_barrels)
	and	a,a
	jr	z,skip_barrels		; skip if there are no barrels
    ld  b,a					; b = num_barrels

	ld	(_hitItemNum),a
	ld	a,#00
	ld	(_hitItemType),a

    ld	de,(iy+05h)			; de = x item 1
	ld	a,c
	sub	a,#03				; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

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
	ld  a,(_num_firefoxes)
	and	a,a
	jr	z,skip_firefoxes	; skip if there are no fireballs/firefoxes
    ld  b,a					; b = num_firefoxes

	ld	(_hitItemNum),a
	ld	a,#01
	ld	(_hitItemType),a

    ld	de,(iy+#05)			; de = x item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),c			; y item1
	
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
skip_firefoxes:

; check collision pies if stage is conveyors
    ld	a,(_game)			; a = game.stage
	cp	#02
	jr	nz,skip_pies

    ld  a,(_num_pies)
	and	a,a
	jr	z,skip_pies			; skip if there are no pies
    ld  b,a					; b = num_pies

	ld	(_hitItemNum),a
	ld	a,#02
	ld	(_hitItemType),a

	push	hl
    ld	hl,(iy+#05)
	ld	de,7
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	sub	a,11				; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	de,0803h
	add	hl,de				; width += 3 + 1, height += 8 + 1

    ld  de,138				; pie struct size
    ld  ix,_pie				; ix = &pie
    call check_collision
	pop	hl
	pop	iy
skip_pies:

; check collision bouncers if stage is elevators
	ld	a,(_game)			; a = game.stage
	cp	#03
	jr	nz,skip_bouncers

    ld  a,(_num_bouncers)
	and	a,a
	jr	z,skip_bouncers		; skip if there are no bouncers
    ld  b,a					; b = num_bouncers

	push	hl
    ld	hl,(iy+#05)
	ld	de,7
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	sub	a,7					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	de,0303h
	add	hl,de				; width += 2 + 1; height += 2 + 1

    ld  de,255				; bouncer struct size
    ld  ix,_bouncer			; ix = &bouncer
    call check_collision
	pop	hl
	pop	iy
skip_bouncers:

; check collision oilcan flame if stage is barrels or conveyors
	ld	a,(_game)			; a = game.stage
	cp	#03
	jr	nc,skip_oilcan_fire	; skip if stage >= 3

	ld	ix,_oilcan

	ld	a,(ix+#08)			; a = oilcan.onFire
	and	a,a
	jr	z,skip_oilcan_fire	; skip if oilcan is not on fire
	ld 	b,a					; b = 1 oilcan fire

	push	hl
    ld	hl,(iy+#05)
	ld	de,6
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	add	a,4					; a  = y item 1				; need to figure this number out
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	a,(ix+#00)			; a = oilcan.sprite
	cp	#02
	ld 	de,0100h			; width += 2 + 1, height += 0 + 1							; It seems like i shouldn't do the + 1 here(and probably also not everywhere else)
	jr	c,smallFire			; jump if sprite is small fire
	ld	de,0101h			; width += 2 + 1, height += 2 + 1
smallFire:
	add	hl,de

    call check_collision
	pop	hl
	pop	iy
skip_oilcan_fire:

	xor	a,a					; A := 0 - code for no collision
	ret




;-------------------------------------------------------------------------------
check_collision:
; Checks for a collision between two objects
; Arguments:
;  iy	= struct location  item 1
;  ix	= struct location  item 2
;  de	= struct size      item 2
;  hl	= width and height item 1 and item 2 combined
;  b	= amount of times to loop
; Returns:
;  none
loop:
; check for y in range
    ld  a,(iy+00h)		; load a with c = y of item 1
    sub a,(ix+01h)		; subtract the y-pos of item 2
    jr  nc,L__0			; if no carry, skip next step

    neg					; a = 0 - a
L__0:
    sub a,l				; subtract l = width
    jr  nc,end_loop		; if no carry, loop again

; check for x in range
	push	hl
	push	de
	ld	hl,(iy+01h)		; hl = item_1_x
	ld	de,(ix+05h)		; de = item_2_x
	or	a,a				; reset flags
	sbc	hl,de			; substract item_1_x from item_2_x
	ld	a,l				; put in a
	jr	nc,L__1			; if no carry, skip negate

	neg					; a = 0 - a
L__1:
	pop	de
	pop	hl
	sub	h				; substract h = height
	jr	nc,end_loop		; if no carry, loop again

; else a collision
    ld  a,#01			; a = 1 (collision occurred)
	pop	hl
	pop	iy
    inc sp              ; adjust SP for higher level subroutine
    inc sp
	inc sp
	ret                 ; return to higher subroutine

; no collision
end_loop:
    add ix,de           ; add offset for next sprite
    djnz	loop		; next item

    ret                 ; return



;-------------------------------------------------------------------------------
check_jump_stage_barrels:
; Checks and counts the number of barrels and fireballs jumped by jumpman
	xor	a,a
	ld	(NumObstaclesJumped),a

    ld  a,(_num_barrels)
	and	a,a
	jr	z,skip_barrels_		; skip if there are no barrels
    ld  b,a					; b = num_barrels

    ld	de,(iy+05h)			; de = x item 1
	ld	a,c
	sub	a,3					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	de,0303h
	add	hl,de				; width += 2 + 1, height += 2 + 1

    ld  de,187				; barrel struct size
    ld  ix,_barrel			; ix = &barrel
    call count_items_colliding
	pop	hl
	pop	iy
skip_barrels_:

; check collision fireballs/firefoxes
    ld  a,(_num_firefoxes)
	and	a,a
	jr	z,skip_firefoxes_	; skip if there are no fireballs/firefoxes
    ld  b,a					; b = num_firefoxes

    ld	de,(iy+#05)			; de = x item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),c			; y item1
	
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
    call count_items_colliding
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

;-------------------------------------------------------------------------------
count_items_colliding:
; Checks for a collision between two objects
; Arguments:
;  iy	= struct location  item 1
;  ix	= struct location  item 2
;  de	= struct size      item 2
;  hl	= width and height item 1 and item 2 combined
;  b	= ammount of times to loop
; Returns:
;  a	= obstacles jumped
loop1:
; check for y in range
    ld  a,(iy+00h)		; no, load A with c = y of item 1
    sub a,(ix+01h)		; subtract the y-pos of item 2
    jr  nc,L__2			; if no carry, skip next step

    neg					; a = 0 - a
L__2:
    sub a,l				; subtract l = width
    jr  nc,end_loop1	; if no carry, loop again

; check for x in range
	push	hl
	push	de
	ld	hl,(iy+01h)		; hl = item_1_x
	ld	de,(ix+05h)		; de = item_2_x
	or	a,a				; reset flags
	sbc	hl,de			; substract item_1_x from item_2_x
	ld	a,l				; put in a
	jr	nc,L__3			; if no carry, skip next step

	neg					; a = 0 - a
L__3:
	pop	de
	pop	hl
	sub	h				; substract h = height
	jr	nc,end_loop1	; if no carry, loop again

; else a collision
    ld	a,(NumObstaclesJumped)
	inc	a
	ld	(NumObstaclesJumped),a

; no collision
end_loop1:
    add ix,de           ; add offset for next sprite
    djnz	loop1		; next item

    ret                 ; return












	; Check for collision with kong on stage rivets
	ld	a,(_game)			; a = game.stage
	cp	4
	jr	nz,skip_kong
	
	ld	a,c
	sub	a,72
	jr	nc,skip_kong
	ld	hl,(iy+#01)
	ld	de,159
	sbc	hl,de
	ld	a,h				; or l
	and	a,a
	jr	nc,L__4
	neg	a
L__4:
	sub	a,22
	jr	nc,skip_kong
	ld	a,1
	ret
skip_kong:







data_stuff:
	DL 0, 0, 0, 0

NumObstaclesJumped:
	DL 0