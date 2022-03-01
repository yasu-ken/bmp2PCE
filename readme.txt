bmp2PCE

This is converter .
BMP Graphic File (256 or 16colr) ---> PCEngine Graphic format.

********************************************************************************
A working version for the time being...
Please be careful not to overwrite the file by the operation.

********************************************************************************
command line

bmp2PCE <-TXxx> <-TYxx> -LXxx -LYxx infile.bmp <outfile.bin>
    -TXxx  Upper left coordinates X. default = 0
    -TYxx  Upper left coordinates Y. default = 0
    -LXxx  Sprite aeria X Length. default=16
    -LYxx  Sprite aeria Y Length. default=16
    infile.bmp   BMP Filename.
    outfile.bin  Optional.
                 In that case, the name of "infile.bin" will be the file name.


Cut out and save the image by the X and Y length from the upper left (0,0) of the bmp image.
The output file is a binary file with no header.

bmp2PCE -LX32 -LY64 test04.bmp
    test04.bin   (0,0)-(32,64) = 16x16dotSprite 2x4(1chank)
    test04.pal   Palette data. Colors from 0 to 15 in bmp file.

bmp2PCE -LX128 -LY128 test03.bmp test__03.bin
    test__03.bin  (0,0)-(128,128) = sprite 8x8(4chank)
    test03.pal    Palette data. Colors from 0 to 15 in bmp file.

bmp2PCE -TX128 -TY0 -LX32 -LY32 -PAL1 test01.bmp
    test01.bmp(96,64)-(96+32,64+64)-->test01.bin
    palet0-15 (1palett set = 16color)-->test01.pal

bmp2PCE -TX96 -TY64 -LX32 -LY64 -PAL16 test01.bmp
    test01.bmp(96,64)-(96+32,64+64)-->test01.bin
    palet0-255 (1palett set = 16color)-->test01.pal

********************************************************************************
how to use for HuC source code.(pceas)

  #incbin(rom_sprite4x4, "test_sprite.bin")
(Do not use #incspr...)
  #incbin(sprpal_00, "test_sprite.pal")

file_load()
{
    :
    :
   load_sprites( 0x4000, rom_sprite4x4, 6); /* 6 chank = 2x4x6 = 48sprites */

   load_palette( 16, sprpal_00, 1);  /* Sprite palette #0( = palette #16 ) */

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
	hedder .... ??bytes(variable)
	palette(16 or 256)
		16 x 4 bytes ( B,G,R,0  0-255Level)
		256 x 4 bytes ( B,G,R,0  0-255Level)
	graphicdata( width x height bytes. 1pixel = 1byte )
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

    16x16dots = 1Sprite = 128bytes = 64words ($40 words)

    spritex1  = 128bytes = 64word = $40word
    sprite2x2 = $200bytes = $100word
    sprite2x4 = $200word = 1chank
    sprite8x8 = $1000word = 8chank = 128x128dot

	-----------------------------------
	Graphic data (BackGround Charactor)

	[0000 0000] Plane 0 (8bit = 8dot)
		[0000 0000] Plane 1 (8bit= 8dot)
			[0000 0000] Plane 2 (8bit= 8dot)
				[0000 0000] Plane 3 (8bit= 8dot)

	8x8dot = 1Charactor = 32bytes = 16word ($10 words)


PC Engine VideoRam Adress
$0000--------------
       BAT
     --------------
$0800  charactor font
     --------------
#1000   :
      free
     --------------
$7e00  SAT  ($100 word)
$7fff  SAT END
     --------------
$8000   :
      free
$ffff
     --------------
	1adress = 1word = 2bytes
	$10000 = 128Kbytes = 64Kword($0000-$ffff adress)

********************************************************************************
License

The MIT License.
