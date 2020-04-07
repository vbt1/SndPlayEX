
#include "sl_def.h"

#include "textnum.h"
#include "colornum.h"

 static POINT point_cube[] = {
	POStoFIXED(-80,-80,-80) ,
	POStoFIXED( 80,-80,-80) ,
	POStoFIXED( 80, 80,-80) ,
	POStoFIXED(-80, 80,-80) ,

	POStoFIXED( 80,-80, 80) ,
	POStoFIXED(-80,-80, 80) ,
	POStoFIXED(-80, 80, 80) ,
	POStoFIXED( 80, 80, 80) ,

	POStoFIXED( 70,  0, 80) ,
	POStoFIXED(  0,  0, 80) ,
	POStoFIXED(  0, 70, 80) ,
	POStoFIXED( 70, 70, 80)
    } ;

 static POINT point_head[] = {
	POStoFIXED(-10,-25,-10) ,
	POStoFIXED( 10,-25,-10) ,
	POStoFIXED( 10,  0,-10) ,
	POStoFIXED(-10,  0,-10) ,
	POStoFIXED( 10,-25, 10) ,
	POStoFIXED(-10,-25, 10) ,
	POStoFIXED(-10,  0, 10) ,
	POStoFIXED( 10,  0, 10)
    } ;

 static POINT point_bodyH[] = {
	POStoFIXED(-25,-40,-15) ,
	POStoFIXED( 25,-40,-15) ,
	POStoFIXED( 25, 20,-15) ,
	POStoFIXED(-25, 20,-15) ,
	POStoFIXED( 25,-40, 15) ,
	POStoFIXED(-25,-40, 15) ,
	POStoFIXED(-25, 20, 15) ,
	POStoFIXED( 25, 20, 15)
    } ;

 static POINT point_bodyL[] = {
	POStoFIXED(-25,-10,-15) ,
	POStoFIXED( 25,-10,-15) ,
	POStoFIXED( 25, 15,-15) ,
	POStoFIXED(-25, 15,-15) ,
	POStoFIXED( 25,-10, 15) ,
	POStoFIXED(-25,-10, 15) ,
	POStoFIXED(-25, 15, 15) ,
	POStoFIXED( 25, 15, 15)
    } ;

 static POINT point_arm[] = {
	POStoFIXED(-10, -5,-10) ,
	POStoFIXED( 10, -5,-10) ,
	POStoFIXED( 10, 35,-10) ,
	POStoFIXED(-10, 35,-10) ,
	POStoFIXED( 10, -5, 10) ,
	POStoFIXED(-10, -5, 10) ,
	POStoFIXED(-10, 35, 10) ,
	POStoFIXED( 10, 35, 10)
    } ;

 static POINT point_hand[] = {
	POStoFIXED(-10, -5,-10) ,
	POStoFIXED( 10, -5,-10) ,
	POStoFIXED( 10, 15,-10) ,
	POStoFIXED(-10, 15,-10) ,
	POStoFIXED( 10, -5, 10) ,
	POStoFIXED(-10, -5, 10) ,
	POStoFIXED(-10, 15, 10) ,
	POStoFIXED( 10, 15, 10)
    } ;

 static POINT point_foot[] = {
	POStoFIXED(-15, -5,-15) ,
	POStoFIXED( 15, -5,-15) ,
	POStoFIXED( 15, 40,-15) ,
	POStoFIXED(-15, 40,-15) ,
	POStoFIXED( 15, -5, 15) ,
	POStoFIXED(-15, -5, 15) ,
	POStoFIXED(-15, 40, 15) ,
	POStoFIXED( 15, 40, 15)
    } ;

 static POINT point_shoe[] = {
	POStoFIXED(-15, -5,-20) ,
	POStoFIXED( 15, -5,-20) ,
	POStoFIXED( 15, 15,-20) ,
	POStoFIXED(-15, 15,-20) ,
	POStoFIXED( 15, -5, 15) ,
	POStoFIXED(-15, -5, 15) ,
	POStoFIXED(-15, 15, 15) ,
	POStoFIXED( 15, 15, 15)
    } ;

 static POINT point_ground[] = {
	POStoFIXED(-512, 0,  512) ,	/*  0 */
	POStoFIXED(-256, 0,  512) ,	/*  1 */
	POStoFIXED(   0, 0,  512) ,	/*  2 */
	POStoFIXED( 256, 0,  512) ,	/*  3 */
	POStoFIXED( 512, 0,  512) ,	/*  4 */

	POStoFIXED(-512, 0,  256) ,	/*  5 */
	POStoFIXED(-256, 0,  256) ,	/*  6 */
	POStoFIXED(   0, 0,  256) ,	/*  7 */
	POStoFIXED( 256, 0,  256) ,	/*  8 */
	POStoFIXED( 512, 0,  256) ,	/*  9 */

	POStoFIXED(-512, 0,    0) ,	/* 10 */
	POStoFIXED(-256, 0,    0) ,	/* 11 */
	POStoFIXED(   0, 0,    0) ,	/* 12 */
	POStoFIXED( 256, 0,    0) ,	/* 13 */
	POStoFIXED( 512, 0,    0) ,	/* 14 */

	POStoFIXED(-512, 0, -256) ,	/* 15 */
	POStoFIXED(-256, 0, -256) ,	/* 16 */
	POStoFIXED(   0, 0, -256) ,	/* 17 */
	POStoFIXED( 256, 0, -256) ,	/* 18 */
	POStoFIXED( 512, 0, -256) ,	/* 19 */

	POStoFIXED(-512, 0, -512) ,	/* 20 */
	POStoFIXED(-256, 0, -512) ,	/* 21 */
	POStoFIXED(   0, 0, -512) ,	/* 22 */
	POStoFIXED( 256, 0, -512) ,	/* 23 */
	POStoFIXED( 512, 0, -512) ,	/* 24 */
    } ;

 static POINT point_flag1[] = {
	POStoFIXED(   0,  0,  0) ,
	POStoFIXED(  16,  0,  0) ,
	POStoFIXED(   0, 16,  0) ,
	POStoFIXED( -16, 32,  0) ,
	POStoFIXED( -32, 32,  0) ,
	POStoFIXED( -32, 48,  0) ,
	POStoFIXED( -48, 64,  0) ,
	POStoFIXED( -64, 64,  0) ,
	POStoFIXED( -64, 80,  0) ,
	POStoFIXED( -80, 96,  0) ,
	POStoFIXED( -96, 96,  0)
    } ;

 static POINT point_flag2[] = {
	POStoFIXED(   0,  0,  0) ,
	POStoFIXED(  16,  0,  0) ,
	POStoFIXED( -16, 32,  0) ,
	POStoFIXED( -16, 16,  0) ,
	POStoFIXED( -32, 32,  0) ,
	POStoFIXED( -48, 64,  0) ,
	POStoFIXED( -48, 48,  0) ,
	POStoFIXED( -64, 64,  0) ,
	POStoFIXED( -80, 96,  0) ,
	POStoFIXED( -80, 80,  0) ,
	POStoFIXED( -96, 96,  0)
} ;

 static POLYGON polygon_cube[] = {
	{POStoFIXED( 0 , 0 ,-1) , { 0 , 1 , 2 , 3}} ,
	{POStoFIXED( 1 , 0 , 0) , { 1 , 4 , 7 , 2}} ,
	{POStoFIXED(-1 , 0 , 0) , { 5 , 0 , 3 , 6}} ,
	{POStoFIXED( 0 ,-1 , 0) , { 4 , 1 , 0 , 5}} ,
	{POStoFIXED( 0 , 1 , 0) , { 2 , 7 , 6 , 3}} ,
	{POStoFIXED( 0 , 0 , 1) , { 4 , 5 , 6 , 7}} ,
	{POStoFIXED( 0 , 0 , 1) , { 8 , 9 , 10 , 11}}
    } ;

 static POLYGON polygon_ground[] = {
	{POStoFIXED(0 , -1 , 0) , { 0 , 1 , 6 , 5}} ,
	{POStoFIXED(0 , -1 , 0) , { 1 , 2 , 7 , 6}} ,
	{POStoFIXED(0 , -1 , 0) , { 2 , 3 , 8 , 7}} ,
	{POStoFIXED(0 , -1 , 0) , { 3 , 4 , 9 , 8}} ,
	{POStoFIXED(0 , -1 , 0) , { 5 , 6 ,11 ,10}} ,
	{POStoFIXED(0 , -1 , 0) , { 6 , 7 ,12 ,11}} ,
	{POStoFIXED(0 , -1 , 0) , { 7 , 8 ,13 ,12}} ,
	{POStoFIXED(0 , -1 , 0) , { 8 , 9 ,14 ,13}} ,
	{POStoFIXED(0 , -1 , 0) , {10 ,11 ,16 ,15}} ,
	{POStoFIXED(0 , -1 , 0) , {11 ,12 ,17 ,16}} ,
	{POStoFIXED(0 , -1 , 0) , {12 ,13 ,18 ,17}} ,
	{POStoFIXED(0 , -1 , 0) , {13 ,14 ,19 ,18}} ,
	{POStoFIXED(0 , -1 , 0) , {15 ,16 ,21 ,20}} ,
	{POStoFIXED(0 , -1 , 0) , {16 ,17 ,22 ,21}} ,
	{POStoFIXED(0 , -1 , 0) , {17 ,18 ,23 ,22}} ,
	{POStoFIXED(0 , -1 , 0) , {18 ,19 ,24 ,23}}
    } ;

 static POLYGON polygon_flag1[] = {
	{POStoFIXED(0 ,  0 , -1) , {0 , 1 , 2 , 2}}
    } ;

 static POLYGON polygon_flag1_1[] = {
	{POStoFIXED(0 ,  0 , -1) , {0 , 2 , 3 , 4}} ,
	{POStoFIXED(0 ,  0 , -1) , {3 , 5 , 4 , 4}} ,
	{POStoFIXED(0 ,  0 , -1) , {4 , 5 , 6 , 7}} ,
	{POStoFIXED(0 ,  0 , -1) , {7 , 6 , 8 , 8}} ,
	{POStoFIXED(0 ,  0 , -1) , {7 , 8 , 9 , 10}}
    } ;

 static POLYGON polygon_flag2[] = {
	{POStoFIXED(0 ,  0 , -1) , {0 , 1 , 2 , 3}}
    } ;

 static POLYGON polygon_flag2_1[] = {
	{POStoFIXED(0 ,  0 , -1) , {3 , 2 , 4 , 4}} ,
	{POStoFIXED(0 ,  0 , -1) , {4 , 2 , 5 , 6}} ,
	{POStoFIXED(0 ,  0 , -1) , {6 , 5 , 7 , 7}} ,
	{POStoFIXED(0 ,  0 , -1) , {7 , 5 , 8 , 9}} ,
	{POStoFIXED(0 ,  0 , -1) , {9 , 8 , 10 , 10}}
    } ;

 static ATTR attribute_cube[] = {
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumGreen  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumBlue   ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumMagenta,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumWhite  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumYellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumRed    ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_BFR,No_Texture,CD_MediumYellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight)
/*
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Green  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Blue   ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Magenta,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Yellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Red    ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_BFR,No_Texture,PL_Yellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette)
*/
    } ;

 static ATTR attribute_cubeLC[] = {
/*
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumGreen  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumBlue   ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumMagenta,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumWhite  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumYellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumRed    ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip) ,
	ATTRIBUTE(Single_Plane,SORT_BFR,No_Texture,CD_MediumYellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip)
*/
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Green  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Blue   ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Magenta,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Yellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Red    ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_BFR,No_Texture,PL_Yellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UseClip|UsePalette)
    } ;

 static ATTR attribute_cube2[] = {
/*
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumMagenta,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,PN_AM2_LogoH, CL_AM2_Logo,GouraudRAM/8+16,Window_In|CL16Bnk|CL_Replace|SPdis|ECdis,sprNoflip,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumWhite  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumYellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,PN_AM2_LogoL, CL_AM2_Logo,GouraudRAM/8+16,Window_In|CL16Bnk|CL_Replace|SPdis|ECdis,sprNoflip,UseLight) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,CD_MediumRed    ,GouraudRAM/8|16,Window_In|CL_Replace,sprPolygon,UseLight) ,

	ATTRIBUTE(Single_Plane,SORT_BFR,PN_AM2_RGB,CL_AM2_Logo ,No_Gouraud,Window_In|CL32KRGB|CL_Replace|SPdis|ECdis,sprNoflip,UseLight)
*/
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Magenta,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,PN_AM2_LogoH, CL_AM2_Logo,GouraudRAM/8+16,Window_In|CL16Bnk|CL_Replace|SPdis|ECdis,sprNoflip,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White  ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Yellow ,GouraudRAM/8+16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,PN_AM2_LogoL, CL_AM2_Logo,GouraudRAM/8+16,Window_In|CL16Bnk|CL_Replace|SPdis|ECdis,sprNoflip,UseLight|UsePalette) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Red    ,GouraudRAM/8|16,Window_In|CL_Replace,sprPolygon,UseLight|UsePalette) ,

	ATTRIBUTE(Single_Plane,SORT_BFR,PN_AM2_Logo,CL_AM2_Logo ,No_Gouraud,Window_In|CL16Bnk|CL_Replace|SPdis|ECdis,sprNoflip,UseLight|UsePalette)
    } ;

 static ATTR attribute_cube3[] = {
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_Green,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option) ,
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,PL_White,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option) ,

	ATTRIBUTE(Single_Plane,SORT_BFR,No_Texture,PL_White,No_Gouraud,Window_In|CL_Replace,sprPolyLine,No_Option)
    } ;

 static ATTR attribute_ground[] = {
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,

	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,

	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,

	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip,No_Texture,CD_DarkGreen,No_Gouraud,Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MAX|UseClip|UseTexture,PN_AM2_Logo,CL_AM2_Logo,No_Gouraud,Window_In|CL_Replace|ECdis|SPdis,sprNoflip,No_Option)
    } ;

 static ATTR attribute_flag1[] = {
/*
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_White,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option)
*/
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_White+7,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option)
    } ;

 static ATTR attribute_flag2[] = {
/*
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_DarkWhite,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_White,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_DarkWhite,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_White,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_DarkWhite,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,CD_White,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option)
*/
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_Black,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_White+7,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_Black,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_White+7,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_Black,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option) ,
	ATTRIBUTE(Dual_Plane,SORT_MIN,No_Texture,PL_White+7,No_Gouraud,MESHoff|Window_In|CL_Replace,sprPolygon,No_Option)
    } ;

    PDATA PD_Cube = {
	point_cube , sizeof(point_cube)/sizeof(POINT) ,
	polygon_cube , sizeof(polygon_cube)/sizeof(POLYGON) ,
	attribute_cube
    } ;

    PDATA PD_CubeLC = {
	point_cube , sizeof(point_cube)/sizeof(POINT) ,
	polygon_cube , sizeof(polygon_cube)/sizeof(POLYGON) ,
	attribute_cubeLC
    } ;

    PDATA PD_Cube2 = {
	point_cube , sizeof(point_cube)/sizeof(POINT) ,
	polygon_cube , sizeof(polygon_cube)/sizeof(POLYGON) ,
	attribute_cube2
    } ;

    PDATA PD_Cube3 = {
	point_cube , sizeof(point_cube)/sizeof(POINT) ,
	polygon_cube , sizeof(polygon_cube)/sizeof(POLYGON) ,
	attribute_cube3
    } ;

    PDATA PD_HMHead = {
	point_head , sizeof(point_head)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMBodyH = {
	point_bodyH , sizeof(point_bodyH)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMBodyL = {
	point_bodyL , sizeof(point_bodyL)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMArmH = {
	point_arm , sizeof(point_arm)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMArmL = {
	point_arm , sizeof(point_arm)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMHand = {
	point_hand , sizeof(point_hand)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMFootH = {
	point_foot , sizeof(point_foot)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMFootL = {
	point_foot , sizeof(point_foot)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_HMShoe = {
	point_shoe , sizeof(point_shoe)/sizeof(POINT) ,
	polygon_cube , 6 ,
	attribute_cube
    } ;

    PDATA PD_Ground = {
	point_ground , sizeof(point_ground)/sizeof(POINT) ,
	polygon_ground , 16 ,
	attribute_ground
    } ;

    PDATA PD_Flag1 = {
	point_flag1 , 3 ,
	polygon_flag1 , 1 ,
	attribute_flag1
    } ;

    PDATA PD_Flag2 = {
	point_flag2 , 4 ,
	polygon_flag2 , 1 ,
	attribute_flag1
    } ;

    PDATA PD_Flag3 = {
	point_flag1 , 6 ,
	polygon_flag1 , 3 ,
	attribute_flag2
    } ;

    PDATA PD_Flag4 = {
	point_flag2 , 7 ,
	polygon_flag2 , 3 ,
	attribute_flag2
    } ;

    PDATA PD_Flag5 = {
	point_flag1 , 9 ,
	polygon_flag1 , 5 ,
	attribute_flag1
    } ;

    PDATA PD_Flag6 = {
	point_flag2 , 10 ,
	polygon_flag2 , 5 ,
	attribute_flag1
    } ;

    PDATA PD_Flag7 = {
	point_flag1 , 11 ,
	polygon_flag1_1 , 5 ,
	attribute_flag1
    } ;

    PDATA PD_Flag8 = {
	point_flag2 , 11 ,
	polygon_flag2_1 , 5 ,
	attribute_flag1
    } ;

    PDATA PD_Flag9 = {
	point_flag1 , 8 ,
	polygon_flag1_1 , 3 ,
	attribute_flag2
    } ;

    PDATA PD_Flag10 = {
	point_flag2 , 8 ,
	polygon_flag2_1 , 3 ,
	attribute_flag2
    } ;

    PDATA PD_Flag11 = {
	point_flag1 , 5 ,
	polygon_flag1_1 , 1 ,
	attribute_flag1
    } ;

    PDATA PD_Flag12 = {
	point_flag2 , 5 ,
	polygon_flag2_1 , 1 ,
	attribute_flag1
    } ;

