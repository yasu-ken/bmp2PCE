
bmp2PCE

This is converter .
BMP Graphic File (256color or 16colr) ---> PCEngine Graphic format.

********************************************************************************
A working version for the time being...
Please be careful not to overwrite the file by the operation.

********************************************************************************
command line

bmp2PCE -LXxx -LYxx infile.bmp <outfile.bin>
    -LXxx  Sprite aeria X Length. default=16
    -LYxx  Sprite aeria Y Length. default=16
    infile.bmp   BMP Filename.
    outfile.bin  Optional.
                 In that case, the name of "infile.bin" will be the file name.


Cut out and save the image by the X and Y length from the upper left (0,0) of the bmp image.
The output file is a binary file with no header.


bmp2PCE -LX32 -LY64 test04.bmp
  ->
test04.bin   (0,0)-(32,64) = 16x16dotSprite 2x4(1chank)
test04.pal   Palette data. Colors from 0 to 15 in bmp file.

bmp2PCE -LX128 -LY128 test03.bmp test__03.bin
  ->
test__03.bin  (0,0)-(128,128) = sprite 8x8(4chank)
test03.pal    Palette data. Colors from 0 to 15 in bmp file.

********************************************************************************
how to use for HuC source code.(pceas)

  #incbin(rom_sprite4x4, "test_sprite.bin")
(Do not use #incspr...)

file_load()
{
    :
    :
   load_sprites( 0x4000, rom_sprite4x4, 6); //6chank = 2x4x6 = 48sprites
}


********************************************************************************
etc.

Be aware of chunks when drawing sprites. Chunk is a unit of 
2x4 sprite size. Chunks are units related to the xy extension of sprites.
PC-Engine provides an XY extended display of sprites within chunks.

XY extended type
  1x1 1x2 2x1 2x2 1x4 2x4

1chank
( 2x4 sprites)   1x2        2x1        2x2     1x4    
    [0][1]     [0]  [5]   [0][1]    [0][1]    [0]  [1]
    [2][3]     [2]  [7]             [2][3]    [2]  [3]
    [4][5]                [4][5]              [4]  [5]
    [6][7]                          [4][5]    [6]  [7]
                                    [6][7]            

8chank 128x128dot
    [00][01]|[10][11]|[20][21]|[30][31]
    [02][03]|[12][13]|[22][23]|[32][33]
    [04][05]|[14][15]|[24][25]|[34][35]
    [06][07]|[16][17]|[26][27]|[36][37]
   ---------+--------+--------+--------
    [08][09]|[18][19]|[28][29]|[38][39]
    [0A][0B]|[1A][1B]|[2A][2B]|[3A][3B]
    [0C][0D]|[1C][1D]|[2C][2D]|[3C][3D]
    [0E][0F]|[1E][1F]|[2E][2F]|[3E][3F]

  1 Sprite = 16x16dot = 16word x 16Line x 4Plane = 64($40)word


-----------------------------------

BMP File
	Hedder .... 256bytes
	palette
	256 x 4 bytes ( B,G,R,0  0-255Level)
	graphicdata.....
		16x16dot = 256bytes
		128x128dot = 16384($4000)bytes

-----------------------------------
PC Engine(Binary)
	Hedder ....none.

	Graphic data (Sprite)
	[0000 0000 0000 0000] Plane 0  16dot=16bit(1word)
		[0000 0000 0000 0000] Plane 1  16dot=16bit(1word)
			[0000 0000 0000 0000] Plane 2  16dot=16bit(1word)
				[0000 0000 0000 0000] Plane 3  16dot=16bit(1word)
		:
	16x16dots = 1Sprite = 128bytes = 64words ($40 words)

	spritex1 = 128bytes 64word $40word
	sprite2x2 = $200bytes $100word
	sprite2x4 = 1chank = $200word
    sprite8x8 = 128x128dot = 8chank = $1000word

	-----------------------------------
	Graphic data (BackGround Charactor)
	[0000 0000] Plane 0 (8bit = 8dot)
       [0000 0000] Plane 1 (8bit= 8dot)
         [0000 0000] Plane 2 (8bit= 8dot)
             [0000 0000] Plane 3 (8bit= 8dot)
	:
	8x8dot = 1Charactor = 32bytes = 16word ($10 words)


PC Engine VideoRam Adress
$0000--------------
       BAT
     --------------
$7e00  SAT  ($100 word)
$7fff  SAT END

$ffff
	1adress = 1word = 2bytes
	$10000 = 128Kbytes = 64Kword($0000-$ffff adress)

********************************************************************************
License

The MIT License.
