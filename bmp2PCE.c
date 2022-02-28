/*
 * pceTOOL - to manipulate some attributes of pce files according to
 *           the special requirements of PC Engine developers
 */


#define	MAX_X   1024  //512
#define	MAX_Y   1024  //512
#define	MAX_PAL 256


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "bmp.c"
#include "pce.c"


//---------------------------------------
// 
//---------------------------------------
/* and now for the program */

void error(char *string, long pos)
{
	printf("\n");
	printf(string);
	printf("\n");
	printf("At position %ld in file", pos);
	printf("\n\n");
	exit(1);
}





static char szDrive[_MAX_DRIVE];     //c:
static char szDir[_MAX_DIR];         //￥mydocuments￥新しいﾌｫﾙﾀﾞ(7)￥test￥
static char szFileName[_MAX_FNAME];  //test
static char szExt[_MAX_EXT];         //.exe

//---------------------------------------
// フルパスから拡張子無しのファイル名を取り出す
//    この関数をまず呼び出し、あとはsz...を参照する
//---------------------------------------
char * splitFileName( char *szPath )
{
	//DWORD dwRet;

	//フルパス名を分割する
	_splitpath(szPath, szDrive, szDir, szFileName, szExt);

	return (szFileName);
}



//---------------------------------------
// 
//---------------------------------------

void init(void)
{
	int i;
	int j;

//	for (i = 0; i < MAX_PAL; i++)
//	{
//		palette_reference[i] = 0;
//
//		pal_r[i] = 0;
//		pal_g[i] = 0;
//		pal_b[i] = 0;
//	}
//
//	//※※※0xffで初期化します
//	//0xffというのは、圧縮形式で1バイトずつかならず出力されるパターンです。
//	//元画像になるべく含まれないように画像を作るべきですが、それとは別に
//	//圧縮のとき、左から1バイトずつ同じかどうか比較しています。
//	//左端に達したとき、あらかじめゼロクリアしていると、右隣がゼロのとき、ゼロが連続していると判断されます。
//	//0xffならそもそも連続しません。だから0xffで初期化しておいたほうがよい
//	for (i = 0; i < MAX_X; i++)
//	{
//		for (j = 0; j < MAX_X; j++)
//		{
//			//pixel[i*MAX_X+j] = 0; //×
//			pixel[i*MAX_X+j] = 0xff;
//		}
//	}
}


//---------------------------------------
// 
//---------------------------------------

void
usage(void)
{
	printf("pce2bmp: Convert pce Graphic image to BMP or BMP to pce.\n\n");
	printf("Usage:\n\n");
	printf("pcetool InputFileName OutputFileName\n\n");
	printf("The combination of input / output files is fixed.\n\n");
	printf("(ok)pcetool input.pce output.BMP\n");
	printf("(ok)pcetool input.BMP output.pce\n");
	printf("(no)pcetool input.pce output.pce\n");
	printf("(no)pcetool input.BMP output.BMP\n");
	printf("Input / output pce and BMP are 256-color palette images.\n");
	printf("\n");
}


//---------------------------------------
// 
//---------------------------------------

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


void Save_Palette( char *fname, int cnt )  //保存するパレットの個数
{
	int lp;

	if((cnt > 16)||(cnt<1 ))return;

	if( BitmapHedder.biBitCount == 8  ){  // 256パレット
		if( cnt > 16) cnt = 16;
	}
	else if ( BitmapHedder.biBitCount == 4 ){  //16色
		cnt = 1;
	}
	else {  //error
		printf("Pallete save error.");
		return;
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

	printf("Pallete 保存しました.");

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

	init();
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
			printf("HELP 出力\n");

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
			if(filecnt==0)  //最初のファイル名
			{
				strcpy( inputFilename , argv[argcnt] );

				//BMPファイルのとき
				if( ( NULL != strstr( inputFilename,".bmp" ))||
					( NULL != strstr( inputFilename,".BMP" )))
				{
					printf("ok... FirstFileName%s\n", inputFilename);
					inFileType=1;  //bmp
				}
				else{
					printf("err...input File Name err.\n");
					usage();
					exit(0);
				}
			}
			else if(filecnt==1)  //２個めのファイル名
			{
				strcpy( outputFilename, argv[argcnt] );

				//pceファイルのとき
				if( ( NULL != strstr( outputFilename,".bin" ))||
					( NULL != strstr( outputFilename,".BIN" )))
				{
					printf("ok... SecondtFileName%s\n", outputFilename);
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
		exit(0);
	}

	if( PalCnt == 0 )     // pal数指定無し
	{
		PalCnt = 1;  //一つ
	}


	//入力ファイル名から取り出す
	splitFileName( inputFilename );

	if( outFileType == 0 ){ //出力ファイル名無し
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

	//BMP_PCE(
	//	0,0,      // bmp内左上xy
	//	32,32 ); // 長さ、xy

	BMP_PCE(TopX,TopY, LengX,LengY );

	//write_PCE( outputFilename,
	//	0,0,      // bmp内左上xy
	//	32,32 ); // 長さ、xy
	//	//128,128 ); // 長さ、xy

	write_PCE( outputFilename,
	           TopX,TopY, LengX,LengY );

	Save_Palette( palettFilename, PalCnt );
}
