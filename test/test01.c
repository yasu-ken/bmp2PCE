#include "huc.h"

#incspr(spr_exp,  "explos.pcx",0, 0, 2,8)  /*(0,0)  2x8sprite 2chank */
#incspr(spr_exp_0,  "explos.pcx",32,0, 2,8)  /*(32,0) 2x8sprite 2chank */
#incspr(spr_exp_1,  "explos.pcx",64,0, 2,8)  /*(64,0) 2x8sprite 2chank */
#incspr(spr_exp_2,  "explos.pcx",96,0, 2,8)  /*(96,0) 2x8sprite 2chank */

#incpal(spritepalett,  "explos.pcx", 0)   /* palette */

const int xy_tbl[]={0,0,0,64,32,0,32,64,64,0,64,64,96,0,96,64 };

setSprite(xx,yy)
int xx;
int yy;
{
	int lp;

	for( lp=0 ; lp<8 ; lp++ )  /* 8chank loop */
	{
		/***********************/
		spr_set(lp);

		spr_pal(16);

		spr_pattern(0x4000+(lp*0x200));  /* 1chank $200 word */

		spr_ctrl( SIZE_MAS|FLIP_MAS, SZ_32x64|NO_FLIP );  /* 2x4 sprite = 1chank */

		spr_x(xx + xy_tbl[lp*2]);
		spr_y(yy + xy_tbl[lp*2+1]);

		/***********************/
	}

}




/*****************************************************/

main()
{
	int lp;
	int xx,yy;

	disp_off();    /* stop screen flash.( vram control command cls() vsync().... ) */

	set_screen_size(SCR_SIZE_64x32);  /* 2x4sprite */
	cls();
	vsync();

	/* sprite inz.  */
	for( lp=0 ; lp<64 ; lp++)
	{
		spr_set(lp);   /* set No. */
		spr_x(500);    /* out of screen */
		spr_y(500);
	}


	load_sprites(0x4000,  spr_exp, 8);  /* 8chank = 8x8 = 64sprite = 1bank */
	load_palette(16, spritepalett,1);    /* spritePalette No.0 = palette 16 */


	xx= 20;
	yy= 40;

	vsync();
	disp_on();

	/*  main loop */
	for (;;)
	{
		if (joy(0) & JOY_A)
		{
		}

		if (joy(0) & JOY_UP)
		{
			yy-= 1;
		}
		if (joy(0) & JOY_DOWN)
		{
			yy+= 1;
		}
		if (joy(0) & JOY_LEFT)
		{
			xx -= 1;
		}
		if (joy(0) & JOY_RGHT)
		{
			xx += 1;
		}

		setSprite( xx, yy );

		put_string("incspr test.", 2,1);
		put_string("up dn lt rt  =  Move", 5,2);

		vsync();
		satb_update();
	}
}
