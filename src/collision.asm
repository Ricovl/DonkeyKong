	.assume adl=1

	.ref _game
	.ref _barrel
	.ref _firefox
	.ref _bouncer
	.ref _oilcan
	.ref _pie

	.def _check_collision_hostiles
	

	; link to collision code from original DonkeyKong:
	; https://github.com/furrykef/dkdasm/blob/master/dkong.asm#L8114
	; something about the hitboxes: http://donkeykongforum.com/index.php?topic=493.0

	; jumpman has a hitbox of 7 x 13

	; Change barrels and firefoxes x and y position to not have an offset to the middle of the sprite.
	; Now the x and y positions off all the entities point to the left corner of the sprite.
	; Move the background_data to the same offset in every entitie struct.
	; Add 1/2*background_data[0 or 1] to the x and y to get the middle.

	; if (abs(jumpmanY - (item2Y + background_data[1] >> 1)) - jumpmanHeight - item2Height <= 0) { <= or <?
	;	 if (abs(jumpmanX - (item2X + background_data[0] >> 1)) - jumpmanHeight - item2Height <= 0) {
	;
	;	 }
	; }
	
	; iy = pointer to struct
	; c  = y-position
	; h  = width
	; l  = height
_check_collision_hostiles:
	
	ld a, (_game)
	ld de,0

	cp a,0
	jr nz,s_2
	; barrels - check for collisions with barrels, firefoxes and oilcan fire

	ld b,10
	ld e,50
	ld iy,_barrel
	; check for collision with barrels

	ld b,5
	ld e,50
	ld iy,_firefox
	; check for collision with firefoxes

	ld b,1
	ld e,10
	ld iy,_oilcan ; this won't work; need seperate fire struct
	; check for collision with oilcan fire

	ret
s_2: cp a,1
	jr nz,s_3
	; conveyors - check for collisions with firefoxes, pies and oilcan fire

	ld b,5
	ld e,50
	ld iy,_firefox
	; check for collision with firefoxes

	ld b,6
	ld e,20
	ld iy,_pie
	; check for collision with pies

	ld b,1
	ld e,10
	ld iy,_oilcan ; this won't work; need seperate fire struct
	; check for collision with oilcan fire

	ret
s_3: cp a,2
	jr nz,s_4
	; elevators - check for collisions with firefoxes and bouncers

	ld b,5
	ld e,50
	ld iy,_firefox
	; check for collision with firefoxes

	ld b,5
	ld e,50
	ld iy,_bouncer
	; check for collision with firefoxes

	ret
s_4:
	; rivets - check for collisions with firefoxes and squares next to kong?
	
	ld b,5
	ld e,50
	ld iy,_firefox
	; check for collision with firefoxes

	ret