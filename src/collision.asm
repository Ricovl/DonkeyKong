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
	ld	a,(iy+#00)			; a = jumpman.y
	ld	c,a					; c = jumpman.y
	ld  hl,0406h			; width = 4, height = 7 - 1 (all item1 heights are decreased by 1)
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
	ld	a,(iy+#00)			; a = hammer.y
	ld	c,a					; c = hammer.y

	ld	a,(_hammer+0Ah)		; a = hammer.sprite
	and	a,1
	ld	hl,0602h			; width = 6; height = 3 - 1
	jr	z,aboveHead
	ld	hl,0604h			; width = 6; height = 5 - 1
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
	ld	a,(iy+#00)			; a = jumpman.y
	add	a,09h				; 0C-7?
	ld	c,a					; c = jumpman.y + 5
	ld	a,(iy+0Eh)			; a = jumpDirIndicator
	or	a,a
	ld	hl,0507h			; hitbox when jumping straight up: width = 5; height = 8 - 1
	jr	z,jumpingUp
	ld	hl,1207h			; hitbox when jumping right/left: width = 12; height = 8 - 1
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

    ld	de,(iy+#02)			; de = x item 1
	ld	a,c
	sub	a,#03				; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	de,0202h
	add	hl,de				; width += 2, height += 2

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

    ld	de,(iy+#02)			; de = x item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),c			; y item1
	
	push	hl
	ld	a,(_game)			; a = game.stage
	cp	#04
	ld 	de,0302h			; width += 3, height += 2
	jr	nz,skipFirefox		; if (stage != rivets) fireball else firefox
	ld	de,0401h			; width += 4, height += 1
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
    ld	hl,(iy+#02)
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
	add	hl,de				; width += 8, height += 3

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
    ld	hl,(iy+#02)
	ld	de,8
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	sub a,12
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	de,0202h
	add	hl,de				; width += 2, height += 2

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
    ld	hl,(iy+#02)
	ld	de,8
	sbc	hl,de				; de = x item 1
	ex	hl,de
	pop	hl
	ld	a,c
	inc	a					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	a,(ix+#01)			; a = oilcan.sprite
	cp	#02
	ld 	de,0200h			; small fire: width += 2, height += 0
	jr	c,smallFire			; jump if sprite is small fire
	ld	de,0202h			; big fire  : width += 2, height += 2
smallFire:
	add	hl,de

    call check_collision
	pop	hl
	pop	iy
skip_oilcan_fire:

; Check for collision with kong on stage rivets
	ld	a,(_game)			; a = game.stage
	cp	#04
	jr	nz,skip_kong
	
	ld	a,c					; a = jumpman.y
	sub	a,74				; a = jumpman.y - 72
	jr	nc,skip_kong		; if y >= 72: skip_kong
	ld	hl,(iy+#02)

	ld	de,132
	sbc	hl,de				; hl = jumpman.x - 136
	jr	c,skip_kong
	ld	a,l					; a = hl
	sub a,50
	jr	nc,skip_kong
	ld	a,1
	ret
skip_kong:


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
    ld  a,(iy+#00)		; load a with y-pos of item 1
    sub a,(ix+#00)		; subtract the y-pos of item 2
    jr  nc,L__0			; if no carry, skip next step

    neg					; a = 0 - a
L__0:
    sub a,l				; subtract l = height
    jr  nc,end_loop		; if no carry, loop again

; check for x in range
	push	hl
	push	de
	ld	hl,(iy+#01)		; hl = item_1_x
	ld	de,(ix+#02)		; de = item_2_x
	or	a,a				; reset flags
	sbc	hl,de			; substract item_1_x from item_2_x
	ld	a,l				; put in a
	jr	nc,L__1			; if no carry, skip negate

	neg					; a = 0 - a
L__1:
	pop	de
	pop	hl
	sub	h				; substract h = width
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

    ld	de,(iy+#02)			; de = x item 1
	ld	a,c
	sub	a,3					; a  = y item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),a			; y item1

	push	hl
	ld	de,0202h
	add	hl,de				; width += 2, height += 2

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

    ld	de,(iy+#02)			; de = x item 1
	push	iy
	ld	iy,data_stuff
	ld	(iy+#01),de			; x item1
	ld	(iy+#00),c			; y item1
	
	push	hl
	ld	a,(_game)			; a = game.stage
	cp	#04
	ld 	de,0302h			; width += 3, height += 2
	jr	nz,skipFirefox_		; if (stage != rivets) fireball else firefox
	ld	de,0401h			; width += 4, height += 1
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
    ld  a,(iy+#00)		; load A with y-pos of item 1
    sub a,(ix+#00)		; subtract the y-pos of item 2
    jr  nc,L__2			; if no carry, skip next step

    neg					; a = 0 - a
L__2:
    sub a,l				; subtract l = width
    jr  nc,end_loop1	; if no carry, loop again

; check for x in range
	push	hl
	push	de
	ld	hl,(iy+#01)		; hl = item_1_x
	ld	de,(ix+#02)		; de = item_2_x
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



data_stuff:
	DL 0, 0, 0, 0

NumObstaclesJumped:
	DL 0