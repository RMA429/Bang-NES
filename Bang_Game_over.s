FT_DPCM_PTR=$c000
	.export _Bang_Game_over_music_data
;this file for FamiTone2 library generated by text2data tool

_Bang_Game_over_music_data:
	.byte 1
	.word @instruments
	.word @samples-3
	.word @song0ch0,@song0ch1,@song0ch2,@song0ch3,@song0ch4,307,256

@instruments:
	.byte $30 ;instrument $00
	.word @env1,@env0,@env0
	.byte $00
	.byte $30 ;instrument $01
	.word @env1,@env0,@env0
	.byte $00
	.byte $30 ;instrument $02
	.word @env2,@env4,@env0
	.byte $00
	.byte $30 ;instrument $03
	.word @env3,@env0,@env0
	.byte $00

@samples:
@env0:
	.byte $c0,$00,$00
@env1:
	.byte $cd,$c8,$c5,$c3,$c1,$00,$04
@env2:
	.byte $cc,$c9,$c6,$c4,$c3,$c3,$c4,$ca,$cd,$00,$08
@env3:
	.byte $cd,$ca,$c9,$c8,$c6,$c5,$c5,$c4,$c4,$c4,$00,$09
@env4:
	.byte $c0,$04,$c0,$00,$02


@song0ch0:
	.byte $fb,$06
@song0ch0loop:
@ref0:
	.byte $86,$0c,$95,$0c,$85,$0c,$85,$10,$95,$10,$85,$10,$85,$0a,$95,$0a
	.byte $85,$0a,$85,$06,$85
@ref1:
	.byte $97,$06,$8d,$02,$b5,$00,$9d
	.byte $fd
	.word @song0ch0loop

@song0ch1:
@song0ch1loop:
@ref2:
	.byte $00,$f5,$86,$14,$85
@ref3:
	.byte $97,$14,$8d,$10,$b5,$00,$9d
	.byte $fd
	.word @song0ch1loop

@song0ch2:
@song0ch2loop:
@ref4:
	.byte $f9,$85
@ref5:
	.byte $f9,$85
	.byte $fd
	.word @song0ch2loop

@song0ch3:
@song0ch3loop:
@ref6:
	.byte $f9,$85
@ref7:
	.byte $f9,$85
	.byte $fd
	.word @song0ch3loop

@song0ch4:
@song0ch4loop:
@ref8:
	.byte $f9,$85
@ref9:
	.byte $f9,$85
	.byte $fd
	.word @song0ch4loop