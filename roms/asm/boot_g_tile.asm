SECTION "boot", ROM0[$0000]
Start:
    ;Set LCDC to use tile map 0 and 8000 addressing mode
    ld hl, $ff40
    ld [hl], $b3

    ;Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    ld hl, $ff47
    ld [hl], $e4

    call Setup_Sound

    ;Set map position 0 to tile 3
    ;        position 16 to tile 2
    ;        position 2 to tile 1
    ld hl, $9800
    ld [hl], 3
    ld hl, $9820
    ld [hl], 2
    ld hl, $9802
    ld [hl], 1

    ;Set data of tile 1 in 8000 addressing mode to GTile:
    ld hl, $8010
    ld bc, GTile
    call LoadTile
    
    ;Set data of tile 2 in 8000 addressing mode to BlockTile
    ld hl, $8020
    ld bc, BlockTile
    call LoadTile
    
    ;Set data of tile 3 in 8000 addressing mode to BeaverTile
    ld hl, $8030
    ld bc, BeaverTile
    call LoadTile

    ;Stall
.infLoop:
    jr .infLoop

; Initializes sound
Setup_Sound:				; CODE XREF: sub_0000+82p
		ld	a, $80 ; '€'
		ld	[unk_FF26], a	; Enable sound system, all channels OFF
		ld	[unk_FF11], a	; Sound	wave duty 50%
		ld	a, $F3 ; 'ó'
		ld	[unk_FF12], a	; Init Envelope
		ld	[unk_FF25], a	; Set enabled sound channels
		ld	a, $77 ; 'w'
		ld	[unk_FF24], a	; Set full volume
		ld	hl, $FF30	; Setup	00 FF repeating	waveform in wave RAM
		xor	a
		ld	c, $10

_loop_clear_wave_data:			; CODE XREF: Setup_Sound+19j
		ldi	[hl], a
		cpl
		dec	c
		jr	nz, _loop_clear_wave_data
		ret
; End of function Setup_Sound

; LoadTile
;HL - dst addr
;BC - src addr
LoadTile:
    ld d, 16
.loop1:
    ld a, [bc]
    ld [hl], a
    inc hl
    inc bc
    dec d
    jp nz, .loop1
    ret
      
;Tile data
GTile:
    db %00111000
    db %00111000
    db %01000100
    db %01000000
    db %10000000
    db %10000000
    db %10000000
    db %10000000
    db %10000000
    db %10000000
    db %10000000
    db %10011110
    db %01000000
    db %01000010
    db %00111000
    db %00111010

BlockTile:
	DB $ff,$ff,$81,$ff,$bd,$c3,$a5,$c3
	DB $a5,$c3,$bd,$c3,$81,$ff,$ff,$ff

BeaverTile:
	DB $00,$0f,$00,$0f,$0a,$0f,$e0,$ff
	DB $e0,$ff,$e0,$ff,$f8,$ff,$00,$6f

;Add padding to file
SECTION "padding", ROM0[$ff]
    db 0
