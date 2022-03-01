
//*******************************************************************************
//bmp2PCE
//
//*******************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "bmp.c"
#include "pce.c"


//***************************************
// get parameter from FullPath.
// Call this function first, then refer to sz ...
//***************************************
static char szDrive[_MAX_DRIVE];     //c:
static char szDir[_MAX_DIR];         //￥mydocuments￥新しいﾌｫﾙﾀﾞ(7)￥test￥
static char szFileName[_MAX_FNAME];  //test
static char szExt[_MAX_EXT];         //.exe

char * splitFileName( char *szPath )
{
	//DWORD dwRet;

	//フルパス名を分割する
	_splitpath(szPath, szDrive, szDir, szFileName, szExt);

	return (szFileName);
}

char * spritNext_GetFileName(){  return szFileName;  }
char * spritNext_GetExt()     {  return szExt;  }


//***************************************
// 
//***************************************

void
usage(void)
{
	printf("bmp2PCE: Convert BMP Graphic image to PCE ( or PCE to BMP).\n\n");
	printf("Usage:\n\n");
	printf("bmp2PCE -LXxx -LYxx infile.bmp <outfile.bin>\n\n");
	printf("-LXxx  Sprite aeria X Length. default=16\n");
	printf("-LYxx  Sprite aeria Y Length. default=16\n");
	printf("infile.bmp   BMP Filename.\n");
	printf("outfile.bin  Optional.\n");
	printf("             In that case, the name of infile will be the file name.\n");
	printf("\n");
}


//***************************************
// Convert strings to numbers
//***************************************

int get_val(char *val)
{
	int i;
	int base, startidx, cumul_val, temp;

	cumul_val = 0;

	if (val[0] == '$')
	{
		base = 16;
		startidx = 1;
	}
	else if  ((val[0] == '0') && (val[1] == 'x'))
	{
		base = 16;
		startidx = 2;
	}
	else
	{
		base = 10;
		startidx = 0;
	}

	cumul_val = 0;

	for (i = startidx; i < strlen(val); i++)
	{
		if ((val[i] >= '0') && (val[i] <= '9'))
		{
			temp = val[i] - '0';
			cumul_val = cumul_val * base + temp;
		}
		else if ((val[i] >= 'A') && (val[i] <= 'F') && (base == 16))
		{
			temp = val[i] - 'A' + 10;
			cumul_val = cumul_val * base + temp;
		}
		else if ((val[i] >= 'a') && (val[i] <= 'f') && (base == 16))
		{
			temp = val[i] - 'a' + 10;
			cumul_val = cumul_val * base + temp;
		}
		else break;
	}

	return(cumul_val);
}


//***************************************
//***************************************
//bmpを読み込んでから実行できます
//256色パレットのうち、最初の16色をpceのフォーマットにして出力します。

	//pce８階調のパレットを256階調に変換
	//int palett_HenkanTbl[8]={0,63,95,127,159,191,223,255};
	//BMPパレット 256x4 data 青、緑、赤、予約(0)の順番
	//PCEパレット (FEDCBA9876543210) H ---- ---G GGRR RBBB L
	//int palett_HenkanTbl[8]={0,56,88,120,152,184,220,255};    //21/12/08 フルカラー対応

unsigned char PalWork[256][3];  // 1024 256色パレット
unsigned short int PalWokW[256];  // word 256色パレット
int PalCnt = 0;

int palchg0_7(unsigned short int val )
{

	val &= 0x0ff;

	if( val<56) return 0;
	if( val<88) return 1;
	if( val<120) return 2;
	if( val<152) return 3;
	if( val<184) return 4;
	if( val<220) return 5;
	if( val<255) return 6;
	else return 7;  //val == 255
}


//***************************************
// Palette save.
// cnt=1    0-15(16 pieces) pallet save from BMP 256 palette.
// cnt=4    0-63(16x4 pieces) pallet save from BMP 256 palette.
// cnt=16   0-255(16x16 pieces) pallet save from BMP 256 palette.
//***************************************

void Save_Palette( char *fname, int cnt )  //保存するパレットの個数
{
	int lp;

	if((cnt > 16)||(cnt<1 ))return;

	if( BitmapHedder.biBitCount == 8  ){  // 256パレット
		if( cnt > 16) cnt =16;
	}
	else if ( BitmapHedder.biBitCount == 4 ){  //16色
		cnt = 1;
	}
	else {  //error
		printf("Pallete save error.");
		exit(1);
	}

	//BGR(level256)--->BRG(level8)
	for( lp=0; lp<(cnt*16); lp++ )
	{
		//0-255を0-7に
		PalWork[lp][0] = palchg0_7( BitmapHedder.palet[lp][0]); //B
		PalWork[lp][1] = palchg0_7( BitmapHedder.palet[lp][1]); //G
		PalWork[lp][2] = palchg0_7( BitmapHedder.palet[lp][2]); //R
	}

	for( lp=0; lp<(cnt*16); lp++ )
	{
		PalWokW[lp] =    // word 256色パレット
			//        B                 G               R
			(unsigned short int )PalWork[lp][0] | ( PalWork[lp][1] << 6 ) | ( PalWork[lp][2] << 3 );
	}

	//data save.
	FILE *fp;

	fp = fopen( fname, "wb" );
	if( fp == NULL )
	{
		printf("Pallete file save error.");
		exit(1);
	}

	fwrite( PalWokW, (sizeof (short int)), cnt*16 , fp );

	fclose(fp);

}


//*******************************************************************************
// MAIN
//*******************************************************************************
char inputFilename[1024];
char outputFilename[1024];
char palettFilename[1024];
int inFileType   = 0;
int outFileType  = 0;

int TopX =0;
int TopY =0;
int LengX =32;
int LengY =32;

int main( int argc, char ** argv )
{
	FILE *fp;
	int rtn;
	int argcnt=0;
	int filecnt=0;

	printf("argc = %d\n", argc);

	for(int i=0; i<argc; i++ ){
		printf("argv[%d] = %s\n",i, argv[i]);
	}

	//----------------------------------
	if( argc <= 1 ){ usage(); exit(0); }

	argcnt = 1;

	filecnt = 0;

	for( argcnt=1; argcnt<argc ; argcnt++ )
	{
		//strncasecmp() ? 大/小文字を区別しないストリングの比較

		if (strncasecmp(argv[argcnt], "-h", 2) == 0)
		{
			printf("HELP\n");

			usage();
			exit(0);
		}
		else if (strncasecmp(argv[argcnt], "-TX", 3) == 0)  //TOP X
		{
			TopX = get_val( &argv[argcnt][3] );
		}
		else if (strncasecmp(argv[argcnt], "-TY", 3) == 0)
		{
			TopY = get_val( &argv[argcnt][3] );
		}
		else if (strncasecmp(argv[argcnt], "-LX", 3) == 0)  //Length X
		{
			LengX = get_val( &argv[argcnt][3] );
		}
		else if (strncasecmp(argv[argcnt], "-LY", 3) == 0)
		{
			LengY = get_val( &argv[argcnt][3] );
		}
		else if (strncasecmp(argv[argcnt], "-PAL", 4) == 0)
		{
			PalCnt = get_val( &argv[argcnt][4] );
		}
		else
		{
			if(filecnt==0)  //first Filename
			{
				strcpy( inputFilename , argv[argcnt] );

				//BMP file then
				if( ( NULL != strstr( inputFilename,".bmp" ))||
					( NULL != strstr( inputFilename,".BMP" )))
				{
					inFileType=1;  //bmp
				}
				else{
					printf("err...input File Name err.\n");
					usage();
					exit(0);
				}
			}
			else if(filecnt==1)  //second file name
			{
				strcpy( outputFilename, argv[argcnt] );

				//pce file then
				if( ( NULL != strstr( outputFilename,".bin" ))||
					( NULL != strstr( outputFilename,".BIN" )))
				{
					outFileType=2;  //pce
				}
				else{
					printf("err...output File Name err.\n");
					usage();
					exit(0);
				}

			}
			else{
				printf("err...UnKnown Parameter.\n");
				usage();
				exit(0);
			}

			filecnt++;  //0->1 1->2
		}
	}

	//-----------------------------------

	//正当性チェック
	if( inFileType == 0 )   //  入力ファイル名無し
	{
		printf("err... input File Name none.\n");
		usage();
		exit(1);
	}

	if( PalCnt == 0 )     // pal数指定無し
	{
		PalCnt = 1;  //一つ
	}


	//input(first)file name full path --> split
	splitFileName( inputFilename );

	if( outFileType == 0 ){ //output file name does not exist.
		strcpy( outputFilename, szFileName );
		strcat( outputFilename,".bin");
	}
	strcpy( palettFilename, szFileName );
	strcat( palettFilename,".pal");


	//--------------------------------------------------
	fp = fopen( inputFilename, "rb" );
	if( fp == NULL )
	{
		printf("BMPfile not found. \n");
		exit(1);
	}

	rtn = Read_BMP( fp );
	if( rtn == -1 )
	{
		printf("BMPfile not found. \n");
		exit(1);
	}
	fclose(fp);

	//--------------------------------------------------


	BMP_PCE(TopX,TopY,      // UpLeft XY(in BMP)
			LengX,LengY );  //Length XY(in BMP)

	write_PCE( outputFilename,
	           TopX,TopY, LengX,LengY );

	Save_Palette( palettFilename, PalCnt );
}
