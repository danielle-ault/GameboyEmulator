INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header

EntryPoint:
	jp TestLoads1

TestLoads1:
	ld b,$30
	ld c,b
	ld HL,$C001
	ld [HL],$25
	ld [HL],c
	ld d,[HL]
	ld BC,$C002
	ld DE,$C003
	ld a,$31
	ld [BC],a
	ld a,$32
	ld [DE],a

TestLoads2:
	