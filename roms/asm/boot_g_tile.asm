SECTION "boot", ROM0[$0000]
Start:
    ;Set LCDC to use tile map 0 and 8000 addressing mode
    ld hl, $ff40
    ld [hl], $b3

    ;Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    ld hl, $ff47
    ld [hl], $e4

    ;Set map position 0 and 1 to tile 1
    ld hl, $9800
    ld [hl], 1
    ld hl, $9820
    ld [hl], 1

    ;Set data of tile 1 in 8000 addressing mode to gTile:
    ld hl, $8010
    ld bc, .gTile
.loop2:
    ld a, [bc]
    ld [hl], a
    inc hl
    inc bc
    ld a, c
    cp .gTileEnd
    jp nz, .loop2
      
    ;Stall
.infLoop:
    jr .infLoop


.gTile:
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
.gTileEnd:

    ;Add padding to file
SECTION "padding", ROM0[$ff]
    db 0
