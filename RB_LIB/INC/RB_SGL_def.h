//                                                                                
//    Copyright (c) 2004/2005 Thomas Fuchs / The Rockin'-B, www.rockin-b.de       
//                                                                                
//    Permission is granted to anyone to use this software for any purpose        
//    and to redistribute it freely, subject to the following restrictions:       
//                                                                                
//    1.  The origin of this software must not be misrepresented. You must not    
//        claim that you wrote the original software. If you use this software    
//        in a product, an acknowledgment in the product documentation would be   
//        appreciated but is not required.                                        
//                                                                                
//    2.  ANY COMMERCIAL USE IS PROHIBITED. This includes that you must not use   
//        this software in the production of a commercial product. Only an explicit
//        permission by the author can allow you to do that.                      
//                                                                                
//    3.  Any modification applied to the software must be marked as such clearly.
//                                                                                
//    4.  This license may not be removed or altered from any distribution.       
//                                                                                
//                                                                                
//    			    NO WARRANTY                                                 
//                                                                                
//    THERE IS NO WARRANTY FOR THE PROGRAM.                                       
//    THE PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,               
//    EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,                 
//    THE IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE.                 
//    THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.
//    SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY    
//    SERVICING, REPAIR OR CORRECTION.                                            
//                                                                                
//    IN NO EVENT WILL THE COPYRIGHT HOLDER BE LIABLE TO YOU FOR DAMAGES,         
//    INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING 
//    OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED   
//    TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY    
//    YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER  
//    PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE       
//    POSSIBILITY OF SUCH DAMAGES.                                                
//                                                                                

#ifndef HAVE_RB_SGL_XPDATA_H
#define HAVE_RB_SGL_XPDATA_H


// from SL_DEF.H
/*****************************************************************************/
/****************************** äÓñ{ÉfÅ[É^å^êÈåæ *****************************/
/*****************************************************************************/

typedef unsigned char	Uint8 ;		/* ïÑçÜÇ»ÇµÇPÉoÉCÉgêÆêî */
typedef signed   char	Sint8 ;		/* ïÑçÜÇ¬Ç´ÇPÉoÉCÉgêÆêî */
typedef unsigned short	Uint16 ;	/* ïÑçÜÇ»ÇµÇQÉoÉCÉgêÆêî */
typedef signed   short	Sint16 ;	/* ïÑçÜÇ¬Ç´ÇQÉoÉCÉgêÆêî */
typedef unsigned long	Uint32 ;	/* ïÑçÜÇ»ÇµÇSÉoÉCÉgêÆêî */
typedef signed   long	Sint32 ;	/* ïÑçÜÇ¬Ç´ÇSÉoÉCÉgêÆêî */
typedef float		Float32 ;	/* ÇSÉoÉCÉgé¿êî */
typedef double		Float64 ;	/* ÇWÉoÉCÉgé¿êî */

typedef int		Int ;		/* INTå^ÅiÉcÅ[ÉãópÅj		*/

typedef int		Bool ;		/* ò_óùå^Åiò_óùíËêîÇílÇ…Ç∆ÇÈÅj	*/

typedef	    Sint16	ANGLE ;		/* àÍé¸ÇRÇUÇOìxÇ ÇPÇUÉrÉbÉgÇ≈ï\Ç∑  */
typedef	    Sint32	FIXED ;		/* è„à ÇPÇUÉrÉbÉgÇ≈êÆêîïîÅAâ∫à ÇPÇUÉrÉbÉgÇ≈è¨êîïîÇï\Ç∑å≈íËè¨êîì_å^ */
#if 0
enum ps {X , Y , Z , XYZ , XYZS , XYZSS , XY = Z , S = XYZ , Sh = S , Sv = XYZS} ;
#endif
typedef     FIXED	VECTOR[XYZ] ;		/* ÉxÉNÉgÉãÉfÅ[É^ */
typedef     FIXED	POINT[XYZ] ;		/* í∏ì_ÇÃà íuÉfÅ[É^ */

#define	    toFIXED(a)		((FIXED)(65536.0 * (a)))
#define	    POStoFIXED(x,y,z)	{toFIXED(x),toFIXED(y),toFIXED(z)}
#define	    ATTRIBUTE(f,s,t,c,g,a,d,o)	{f,(s)|(((d)>>16)&0x1c)|(o),t,(a)|(((d)>>24)&0xc0),c,g,(d)&0x3f}
#define	    DEGtoANG(d)		((ANGLE)((65536.0 * (d)) / 360.0))

#define	    FUNC_Texture	2
#define	    FUNC_Polygon	4
#define	    FUNC_PolyLine	5

#define	    MSBon		(1 << 15)	/* ÉtÉåÅ[ÉÄÉoÉbÉtÉ@Ç…èëÇ´çûÇﬁÇlÇrÇaÇÇPÇ…Ç∑ÇÈ */
#define	    HSSon		(1 << 12)	/* ÉnÉCÉXÉsÅ[ÉhÉVÉÖÉäÉìÉNóLå¯ */
#define	    HSSoff		(0 << 12)	/* ÉnÉCÉXÉsÅ[ÉhÉVÉÖÉäÉìÉNñ≥å¯(default) */
#define	    No_Window		(0 << 9)	/* ÉEÉBÉìÉhÉEÇÃêßå¿ÇéÛÇØÇ»Ç¢(default)*/
#define	    Window_In		(2 << 9)	/* ÉEÉBÉìÉhÉEÇÃì‡ë§Ç…ï\é¶ */
#define	    Window_Out		(3 << 9)	/* ÉEÉBÉìÉhÉEÇÃäOë§Ç…ï\é¶ */
#define	    MESHoff		(0 << 8)	/* í èÌï\é¶(default) */
#define	    MESHon		(1 << 8)	/* ÉÅÉbÉVÉÖÇ≈ï\é¶ */
#define	    ECdis		(1 << 7)	/* ÉGÉìÉhÉRÅ[ÉhÇÉpÉåÉbÉgÇÃÇ–Ç∆Ç¬Ç∆ÇµÇƒégóp */
#define	    ECenb		(0 << 7)	/* ÉGÉìÉhÉRÅ[ÉhóLå¯ */
#define	    SPdis		(1 << 6)	/* ÉXÉyÅ[ÉXÉRÅ[ÉhÇÉpÉåÉbÉgÇÃÇ–Ç∆Ç¬Ç∆ÇµÇƒégóp */
#define	    SPenb		(0 << 6)	/* ÉXÉyÅ[ÉXÇÕï\é¶ÇµÇ»Ç¢(default) */
#define	    CL16Bnk		(0 << 3)	/* ÉJÉâÅ[ÉoÉìÉNÇPÇUêFÉÇÅ[Éh (default) */
#define	    CL16Look		(1 << 3)	/* ÉJÉâÅ[ÉãÉbÉNÉAÉbÉvÇPÇUêFÉÇÅ[Éh */
#define	    CL64Bnk		(2 << 3)	/* ÉJÉâÅ[ÉoÉìÉNÇUÇSêFÉÇÅ[Éh */
#define	    CL128Bnk		(3 << 3)	/* ÉJÉâÅ[ÉoÉìÉNÇPÇQÇWêFÉÇÅ[Éh */
#define	    CL256Bnk		(4 << 3)	/* ÉJÉâÅ[ÉoÉìÉNÇQÇTÇUêFÉÇÅ[Éh */
#define	    CL32KRGB		(5 << 3)	/* ÇqÇfÇaÇRÇQÇjêFÉÇÅ[Éh */
#define	    CL_Replace		0		/* èdÇÀèëÇ´(è„èëÇ´)ÉÇÅ[Éh */
#define	    CL_Shadow		1		/* âeÉÇÅ[Éh */
#define	    CL_Half		2		/* îºãPìxÉÇÅ[Éh */
#define	    CL_Trans		3		/* îºìßñæÉÇÅ[Éh */
#define	    CL_Gouraud		4		/* ÉOÅ[ÉçÅ[ÉVÉFÅ[ÉfÉBÉìÉOÉÇÅ[Éh */

#define	    UseTexture		(1 << 2)	/* ÉeÉNÉXÉ`ÉÉÇì\ÇÈÉ|ÉäÉSÉì */
#define	    UseLight		(1 << 3)	/* åıåπÇÃâeãøÇéÛÇØÇÈÉ|ÉäÉSÉì */
#define	    UsePalette		(1 << 5)	/* É|ÉäÉSÉìÇÃÉJÉâÅ[ */

/* SGL Ver3.0 Add */
#define	    UseNearClip		(1 << 6)	/* ÉjÉAÅEÉNÉäÉbÉsÉìÉOÇÇ∑ÇÈ */
#define	    UseGouraud		(1 << 7)	/* ÉäÉAÉãÉOÅ[ÉçÅ[ */
#define	    UseDepth		(1 << 4)	/* ÉfÉvÉXÉLÉÖÅ[ */
/* */

#if 0 /* SGL Ver3.0 Delete */
#define	    UseClip		(1 << 4)		/* âÊñ äOÇ…í∏ì_Ç™èoÇΩÇÁï\é¶ÇµÇ»Ç¢ */
#else
#define	    UseClip		UseNearClip		/* ÉjÉAÅEÉNÉäÉbÉsÉìÉOÇÇ∑ÇÈ */
#endif

#define	    sprHflip		((1 << 4) | FUNC_Texture | (UseTexture << 16))
#define	    sprVflip		((1 << 5) | FUNC_Texture | (UseTexture << 16))
#define	    sprHVflip		((3 << 4) | FUNC_Texture | (UseTexture << 16))
#define	    sprNoflip		((0) | FUNC_Texture | (UseTexture << 16))
#define	    sprPolygon		(FUNC_Polygon | ((ECdis | SPdis) << 24))
#define	    sprPolyLine		(FUNC_PolyLine | ((ECdis | SPdis) << 24))
#define	    sprLine		(FUNC_Line | ((ECdis | SPdis) << 24))

#define	    No_Texture		0		/* ÉeÉNÉXÉ`ÉÉÇégópÇµÇ»Ç¢éû */
#define	    No_Option		0		/* ÉIÉvÉVÉáÉìÇégópÇµÇ»Ç¢éû */
#define	    No_Gouraud		0		/* ÉOÅ[ÉçÅ[ÉVÉFÅ[ÉfÉBÉìÉOÇégópÇµÇ»Ç¢éû */
#define	    No_Palet	        0		/* ÉJÉâÅ[ÉpÉåÉbÉgÇÃéwíËÇ™Ç¢ÇÁÇ»Ç¢éû */
#define	    GouraudRAM		(0x00080000 - (32*8))	/* åıåπÇÃâeãøópÇÃÉOÅ[ÉçÅ[ÉoÉbÉtÉ@ */

#if 0
typedef struct {
    VECTOR	 norm ;			/* ñ@ê¸ÉxÉNÉgÉã */
    Uint16	 Vertices[4] ;		/* É|ÉäÉSÉìÇç\ê¨Ç∑ÇÈí∏ì_î‘çÜ */
} POLYGON ;

typedef struct {
    Uint8	 flag ;			/* ï–ñ Ç©óºñ Ç©ÇÃÉtÉâÉO */
    Uint8	 sort ;			/* É\Å[ÉgÇÃäÓèÄà íuÇ∆ÉIÉvÉVÉáÉìê›íË */
    Uint16	 texno ;		/* ÉeÉNÉXÉ`ÉÉî‘çÜ */
    Uint16	 atrb ;			/* ÉAÉgÉäÉrÉÖÅ[ÉgÉfÅ[É^(ï\é¶ÉÇÅ[Éh) */
    Uint16	 colno ;		/* ÉJÉâÅ[ÉiÉìÉoÅ[ */
    Uint16	 gstb ;			/* ÉOÅ[ÉçÅ[ÉVÉFÅ[ÉfÉBÉìÉOÉeÅ[ÉuÉã */
    Uint16	 dir ;			/* ÉeÉNÉXÉ`ÉÉÇÃîΩì]Ç∆ÉtÉ@ÉìÉNÉVÉáÉìÉiÉìÉoÅ[ */
} ATTR ;

typedef struct {
    POINT	*pntbl ;		/* í∏ì_ÇÃà íuÉfÅ[É^ÉeÅ[ÉuÉã */
    Uint32	 nbPoint ;		/* í∏ì_ÇÃêî */
    POLYGON	*pltbl ;		/* É|ÉäÉSÉìíËã`ÉeÅ[ÉuÉã */
    Uint32	 nbPolygon ;		/* É|ÉäÉSÉìÇÃêî */
    ATTR	*attbl ;		/* É|ÉäÉSÉìÇÃÉAÉgÉäÉrÉÖÅ[ÉgÉeÅ[ÉuÉã */
} PDATA ;
#endif
/* SGL Ver3.0 Add */
typedef struct {
    POINT	*pntbl ;		/* í∏ì_ÇÃà íuÉfÅ[É^ÉeÅ[ÉuÉã */
    Uint32	 nbPoint ;		/* í∏ì_ÇÃêî */
    POLYGON	*pltbl ;		/* É|ÉäÉSÉìíËã`ÉeÅ[ÉuÉã */
    Uint32	 nbPolygon ;		/* É|ÉäÉSÉìÇÃêî */
    ATTR	*attbl ;		/* É|ÉäÉSÉìÇÃÉAÉgÉäÉrÉÖÅ[ÉgÉeÅ[ÉuÉã */
    VECTOR	*vntbl;
} XPDATA ;
#if 0
typedef struct obj{
    PDATA	*pat ;			/* É|ÉäÉSÉìÉÇÉfÉãÉfÅ[É^ÉeÅ[ÉuÉã */
    FIXED	 pos[XYZ] ;		/* ëäëŒà íuÉfÅ[É^ */
    ANGLE	 ang[XYZ] ;		/* âÒì]äpÉfÅ[É^ */
    FIXED	 scl[XYZ] ;		/* ÉXÉPÅ[ÉäÉìÉOÉfÅ[É^ */
    struct obj	*child ;		/* éqÉIÉuÉWÉFÉNÉgÉfÅ[É^ */
    struct obj	*sibling ;		/* åZíÌÉIÉuÉWÉFÉNÉgÉfÅ[É^ */
} OBJECT ;

    enum base {
	SORT_BFR ,			/* íºëOÇ…ï\é¶ÇµÇΩÉ|ÉäÉSÉìÇÃà íuÇégÇ§ */
	SORT_MIN ,			/* ÇSì_ÇÃì‡ÅAàÍî‘éËëOÇÃì_ÇégÇ§ */
	SORT_MAX ,			/* ÇSì_ÇÃì‡àÍî‘âìÇ¢ì_ÇégÇ§ */
	SORT_CEN			/* ÇSì_ÇÃïΩãœà íuÇégÇ§ */
    } ;

    enum pln {
	Single_Plane ,			/* ï–ñ É|ÉäÉSÉì */
	Dual_Plane			/* óºñ É|ÉäÉSÉì(ï\ó†îªíËÇÃåãâ Çå©Ç»Ç¢) */
    } ;
#endif
// from SGL.H

#define	NORMAL(x,y,z)		{POStoFIXED(x,y,z)
#define	VERTICES(v0,v1,v2,v3)	{v0 , v1 , v2 , v3}}
#define	C_RGB(r,g,b)		(((b)&0x1f)<<10|((g)&0x1f)<<5|((r)&0x1f)|0x8000)

#define TRANSLATION(x,y,z)	{toFIXED(x),toFIXED(y),toFIXED(z)}
#define ROTATION(x,y,z)		{DEGtoANG(x),DEGtoANG(y),DEGtoANG(z)}
#define SCALING(x,y,z)		{toFIXED(x),toFIXED(y),toFIXED(z)}


#define slMulFX(a, b)	toFIXED(toFLOAT(a) * toFLOAT(b))
#define slDivFX(a, b)   toFIXED(toFLOAT(a) / toFLOAT(b))
#include <math.h>
#define slAtan(a, b)	DEGtoANG(atan2(toFLOAT(a), toFLOAT(b)) / (M_PI / 2.0));

#endif //HAVE_RB_SGL_XPDATA_H

