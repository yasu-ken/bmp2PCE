SET PATH=c:\huc\bin;c:\huc\include\pce
SET PCE_INCLUDE=c:\huc\include\pce

bmp2PCE -LX128 -LY128 explos.bmp explos.bin

huc.exe -v test01.c
huc.exe -v test02.c

