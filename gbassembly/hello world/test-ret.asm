INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0

EntryPoint:
	ld b, 0

.loop
	ld a, [rLY]
	call TestFunc
	jr .loop
	


TestFunc:
	inc b
	ld a, b
	ret