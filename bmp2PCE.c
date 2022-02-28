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
static char szDir[_MAX_DIR];         //��mydocuments���V����̫���(7)��test��
static char szFileName[_MAX_FNAME];  //test
static char szExt[_MAX_EXT];         //.exe

//---------------------------------------
// �t���p�X����g���q�����̃t�@�C���������o��
//    ���̊֐����܂��Ăяo���A���Ƃ�sz...���Q�Ƃ���
//---------------------------------------
char * splitFileName( char *szPath )
{
	//DWORD dwRet;

	//�t���p�X���𕪊�����
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
//	//������0xff�ŏ��������܂�
//	//0xff�Ƃ����̂́A���k�`����1�o�C�g�����Ȃ炸�o�͂����p�^�[���ł��B
//	//���摜�ɂȂ�ׂ��܂܂�Ȃ��悤�ɉ摜�����ׂ��ł����A����Ƃ͕ʂ�
//	//���k�̂Ƃ��A������1�o�C�g���������ǂ�����r���Ă��܂��B
//	//���[�ɒB�����Ƃ��A���炩���߃[���N���A���Ă���ƁA�E�ׂ��[���̂Ƃ��A�[�����A�����Ă���Ɣ��f����܂��B
//	//0xff�Ȃ炻�������A�����܂���B������0xff�ŏ��������Ă������ق����悢
//	for (i = 0; i < MAX_X; i++)
//	{
//		for (j = 0; j < MAX_X; j++)
//		{
//			//pixel[i*MAX_X+j] = 0; //�~
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



//bmp��ǂݍ���ł�����s�ł��܂�
//256�F�p���b�g�̂����A�ŏ���16�F��pce�̃t�H�[�}�b�g�ɂ��ďo�͂��܂��B

	//pce�W�K���̃p���b�g��256�K���ɕϊ�
	//int palett_HenkanTbl[8]={0,63,95,127,159,191,223,255};
	//BMP�p���b�g 256x4 data �A�΁A�ԁA�\��(0)�̏���
	//PCE�p���b�g (FEDCBA9876543210) H ---- ---G GGRR RBBB L
	//int palett_HenkanTbl[8]={0,56,88,120,152,184,220,255};    //21/12/08 �t���J���[�Ή�

unsigned char PalWork[256][3];  // 1024 256�F�p���b�g
unsigned short int PalWokW[256];  // word 256�F�p���b�g
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


void Save_Palette( char *fname, int cnt )  //�ۑ�����p���b�g�̌�
{
	int lp;

	if((cnt > 16)||(cnt<1 ))return;

	if( BitmapHedder.biBitCount == 8  ){  // 256�p���b�g
		if( cnt > 16) cnt = 16;
	}
	else if ( BitmapHedder.biBitCount == 4 ){  //16�F
		cnt = 1;
	}
	else {  //error
		printf("Pallete save error.");
		return;
	}

	//BGR(level256)--->BRG(level8)
	for( lp=0; lp<(cnt*16); lp++ )
	{
		//0-255��0-7��
		PalWork[lp][0] = palchg0_7( BitmapHedder.palet[lp][0]); //B
		PalWork[lp][1] = palchg0_7( BitmapHedder.palet[lp][1]); //G
		PalWork[lp][2] = palchg0_7( BitmapHedder.palet[lp][2]); //R
	}
	for( lp=0; lp<(cnt*16); lp++ )
	{
		PalWokW[lp] =    // word 256�F�p���b�g
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

	printf("Pallete �ۑ����܂���.");

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
		//strncasecmp() ? ��/����������ʂ��Ȃ��X�g�����O�̔�r

		if (strncasecmp(argv[argcnt], "-h", 2) == 0)
		{
			printf("HELP �o��\n");

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
			if(filecnt==0)  //�ŏ��̃t�@�C����
			{
				strcpy( inputFilename , argv[argcnt] );

				//BMP�t�@�C���̂Ƃ�
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
			else if(filecnt==1)  //�Q�߂̃t�@�C����
			{
				strcpy( outputFilename, argv[argcnt] );

				//pce�t�@�C���̂Ƃ�
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

	//�������`�F�b�N
	if( inFileType == 0 )   //  ���̓t�@�C��������
	{
		printf("err... input File Name none.\n");
		usage();
		exit(0);
	}

	if( PalCnt == 0 )     // pal���w�薳��
	{
		PalCnt = 1;  //���
	}


	//���̓t�@�C����������o��
	splitFileName( inputFilename );

	if( outFileType == 0 ){ //�o�̓t�@�C��������
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
	//	0,0,      // bmp������xy
	//	32,32 ); // �����Axy

	BMP_PCE(TopX,TopY, LengX,LengY );

	//write_PCE( outputFilename,
	//	0,0,      // bmp������xy
	//	32,32 ); // �����Axy
	//	//128,128 ); // �����Axy

	write_PCE( outputFilename,
	           TopX,TopY, LengX,LengY );

	Save_Palette( palettFilename, PalCnt );
}
