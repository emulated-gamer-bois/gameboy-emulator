SECTION "boot", ROM0[$0000]
Start:
    ;Disable interrupts
    di
    ;Set stack pointer
    ld sp, $fffe

    ;Set LCDC to use tile map 0 and 8000 addressing mode
    ;Window disabled, Sprites disabled
    ld hl, $ff40
    ld [hl], %10010001

    ;Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    ld hl, $ff47
    ld [hl], $e4


    ;Initializes sound
    ld  a, $80              ; '€'
    ld  [$FF26], a          ; Enable sound system, all channels OFF
    ld  [$FF11], a          ; Sound wave duty 50%
    ld  a, $F3              ; 'ó'
    ld  [$FF12], a          ; Init Envelope
    ld  [$FF25], a          ; Set enabled sound channels
    ld  a, $77              ; 'w'
    ld  [$FF24], a          ; Set full volume

    ;Set map positions
SetMap:
    ld de, $10
    ld b, 29
    ld hl, $9800
    ld a, 1
.loop:
    ld [hl+], a
    inc a
    ld [hl+], a
    inc a
    push hl     ; put top row on stack
    ld de, $1e
    add hl, de  ; hl = bottom row
    ld [hl+], a
    inc a
    ld [hl+], a
    inc a
    pop hl      ; hl = top row
    cp b
    jr nz, .loop


LoadTiles:
    ld b, 48        ; set byte-counter
    ld c, 4         ; set tile-counter
    push bc         ; push counter to stack

    ld hl, TileData ; set src address

    ld bc, $8010    ; set dst address

    ;   assmune counter on stack
    ;   assume BC = dst addr
    ;   assume HL = src addr
.loop_byte:
    call ParseByte  ; DE = parsed
    push hl         ; push hl
    push bc         ; push bc
    ld hl, 16
    add hl, bc
    ld [hl], e      ; store second byte
    inc hl
    ld [hl], e
    inc hl
    ld [hl], e
    inc hl
    ld [hl], e
    pop hl          ; hl = bc
    ld [hl], d      ; store first byte
    inc hl
    ld [hl], d
    inc hl
    ld [hl], d
    inc hl
    ld [hl], d
    pop hl          ; restore hl

    inc hl
    inc bc
    inc bc
    inc bc
    inc bc

    pop de          ; pop counter from stack
    dec d           ; decrement byte-counter
    jr z, .end      ; if zero go to end
    dec e           ; decrement tile-counter
    push de         ; push counter to stack
    jr nz, .loop_byte   ; if not zero loop

    ; tile count is zero
    pop de          ; reset tile count
    ld e, 4
    push de
    push hl         ; push src-addr
    push bc         ; switch dst-addr from bc to hl
    pop hl
    ld bc, $10       ; increment dst-addr 16 bytes
    add hl, bc
    push hl         ; switch back dst-addr from hl to bc
    pop bc
    pop hl          ; restore hl (src-addr)
    jr .loop_byte

.end:

Animation:
    ld hl, $ff42    ; addr for camera pos

    ld e, $4f       ; animation duration
.loop:
    dec [hl]        ; movement per frame time
    dec [hl]
    inc hl
    dec [hl]
    dec hl
    ld c, $14       ; frame time
.frame:
    ld a,[$FF00+$44]    ; wait for next frame
    cp $90
    jr nz, .frame
    dec c
    jr nz, .frame
    dec e
    jr nz, .loop

    jr DisableBoot


; -------------------------------------
; --------- Help functions ------------
; -------------------------------------

; ParseByte
;in:  HL - src addr, value preserved
;out: DE - parsed
ParseByte:
    push af
    push bc
    ld a, [hl]          ; load src-byte
    ld b, 2             ; set counter
    push bc             ; push counter

.loop_byte:
    ld e, 0             ; zero e
    ld b, 4             ; set counter

.loop_4bits:
    sla a               ; check most significant bit
    jr nc, .skip_set    ; skip set bits

    push af
    ld a, b             ; get mask
    call MakeMask
    or e                ; set bits
    ld e, a             ; move result to e
    pop af
.skip_set:
    dec b               ; decrement counter, loop if not zero
    jr nz, .loop_4bits

    pop bc              ; get outer loop counter
    dec b               ; decrement counter
    jr z, .end          ; if zero go to .end
    push bc             ; push outer loop counter to stack
    ld d, e             ; move first result to d
    jr .loop_byte
.end:
    pop bc
    pop af
    ret


; MakeMask
;in:  A - pos
;out: A - mask
MakeMask:
    push bc
    ld b, 3
    dec a
    jr z, .done
.rot:
    rlc b
    rlc b
    dec a
    jr nz, .rot
.done:
    ld a, b
    pop bc
    ret


; -------------------------------------
; -------------- Data -----------------
; -------------------------------------

TileData:
    DB $60,$40,$40,$c0
    DB $80,$81,$fb,$00

    DB $00,$60,$60,$d1
    DB $f1,$9b,$08,$00

    DB $00,$88,$dc,$d4
    DB $74,$25,$05,$04

    DB $00,$f1,$81,$e2
    DB $83,$82,$fb,$00

    DB $00,$e1,$23,$22
    DB $f2,$33,$e0,$00

    DB $00,$99,$4d,$47
    DB $42,$c6,$04,$04


;Add padding to file
;Disable boot rom
SECTION "padding", ROM0[$ff-3]
DisableBoot:
    ld a, 1
    ld [$ff00+$50], a
