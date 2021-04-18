SECTION "boot", ROM0[$0000]
Start:
    ;Set LCDC to use tile map 0 and 8000 addressing mode
    ld hl, $ff40
    ld [hl], $b3

    ;Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    ld hl, $ff47
    ld [hl], $e4

    call Setup_Sound

    ;Set map positions
    ld l, 7
    ld h, $98
.loop:
    ld a, d
    ld [hl], l
    dec l
    jp nz, .loop

    ;Set data of tile 1 in 8000 addressing mode to GTile:
    ld hl, $8010
    ld bc, Tiles
    call LoadTiles

;     ;Stall
; .infLoop:
;     jr .infLoop
Animation:
    ld hl, $ff42

    ld e, $4f
.loop:
    dec [hl]
    inc hl
    dec [hl]
    dec hl
    ld c, $14
.frame:
    ld a,[$FF00+$44]
    cp $90
    jr nz, .frame
    dec c
    jr nz, .frame
    dec e
    jr nz, .loop

    jp DisableBoot

; Initializes sound
Setup_Sound:                    ; CODE XREF: sub_0000+82p
        ld  a, $80              ; '€'
        ld  [$FF26], a          ; Enable sound system, all channels OFF
        ld  [$FF11], a          ; Sound wave duty 50%
        ld  a, $F3              ; 'ó'
        ld  [$FF12], a          ; Init Envelope
        ld  [$FF25], a          ; Set enabled soudn channels
        ld  a, $77              ; 'w'
        ld  [$FF24], a          ; Set full volume
        ld  hl, $FF30           ; Setup 00 FF repeating waveform in wave RAM
        xor a
        ld  c, $10

_loop_clear_wave_data:          ; CODE XREF: Setup_Sound+19j
        ldi [hl], a
        cpl
        dec c
        jr  nz, _loop_clear_wave_data
        ret
; End of function Setup_Sound

; LoadTile
;HL - dst addr
;BC - src addr
LoadTiles:
    ld d, 56
.loop1:
    ld a, [bc]
    ld [hl+], a
    ld [hl+], a
    inc bc
    dec d
    jp nz, .loop1
    ret

;Tile data
Tiles:
.l:
    DB $30,$20,$20,$60
    DB $40,$40,$7e,$00
.a:
    DB $00,$18,$18,$34
    DB $3c,$66,$c2,$00
.m:
    DB $00,$22,$37,$75
    DB $5d,$c9,$01,$01
.e:
    DB $00,$3c,$20,$38
    DB $20,$60,$7e,$00
.b:
    DB $00,$3c,$24,$44
    DB $7e,$46,$7c,$00
.o:
    DB $00,$18,$34,$24
    DB $24,$3c,$00,$00
.y:
    DB $00,$64,$34,$1c
    DB $08,$18,$10,$10

;Add padding to file
SECTION "padding", ROM0[$ff-3]
DisableBoot:
    ld a, 1
    ld [$ff00+$50], a
