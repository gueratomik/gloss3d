/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GLOSS3D.  If not, see http://www.gnu.org/licenses/.            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <g3durmanager.h>

/******************************************************************************/
URMMOVEPOINT *urmmovepoint_new ( G3DSPLINE *spl, uint32_t save_type ) {
    uint32_t structsize = sizeof ( URMMOVEPOINT );

    URMMOVEPOINT *ump = ( URMMOVEPOINT * ) calloc ( 0x01, structsize );

    if ( ump == NULL ) {
        fprintf ( stderr, "urmmovepoint_new: memory allocation falied\n" );

        return NULL;
    }

    ump->spl       = spl;
    ump->lpt       = spl->curve->lselpt; /*** selected points         ***/
    ump->curhan    = spl->curve->curhan; /*** current selected handle ***/
    ump->save_type = save_type;

    return ump;
}

/******************************************************************************/
void urmmovepoint_free ( URMMOVEPOINT *ump ) {
    list_free ( &ump->lpt, NULL );

    if ( ump->oldpos ) free ( ump->oldpos );
    if ( ump->newpos ) free ( ump->newpos );

    free ( ump );
}

/******************************************************************************/
void movePoint_free ( void *data, uint32_t commit ) {
    URMMOVEPOINT *ump = ( URMMOVEPOINT * ) data;

    urmmovepoint_free ( ump );
}

/******************************************************************************/
void movePoint_undo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMMOVEPOINT *ump = ( URMMOVEPOINT * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) ump->spl;
    LIST *ltmppt = ump->lpt;
    uint32_t i = 0x00;

    if ( ump->save_type & UMPSAVESELECTEDPOINTS ) {
	while ( ltmppt ) {
            G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

            memcpy ( &pt->pos, &ump->oldpos[i++], sizeof ( G3DVECTOR3F ) );

            ltmppt = ltmppt->next;
	}
    }

    if ( ump->save_type & UMPSAVECURRENTHANDLE ) {
        memcpy ( &ump->curhan->pos, &ump->oldhanpos, sizeof ( G3DVECTOR3F ) );
    }

    /*** update faces and subdivided faces ***/
    g3dspline_update ( ump->spl, ump->lpt, UPDATEMODIFIERS, engine_flags );
}

/******************************************************************************/
void movePoint_redo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMMOVEPOINT *ump = ( URMMOVEPOINT * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) ump->spl;
    LIST *ltmppt = ump->lpt;
    uint32_t i = 0x00;

    if ( ump->save_type & UMPSAVESELECTEDPOINTS ) {
	while ( ltmppt ) {
            G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

            memcpy ( &pt->pos, &ump->newpos[i++], sizeof ( G3DVECTOR3F ) );

            ltmppt = ltmppt->next;
	}
    }

    if ( ump->save_type & UMPSAVECURRENTHANDLE ) {
        memcpy ( &ump->curhan->pos, &ump->newhanpos, sizeof ( G3DVECTOR3F ) );
    }

    /*** update faces and subdivided faces ***/
    g3dspline_update ( ump->spl, ump->lpt, UPDATEMODIFIERS, engine_flags );
}

/******************************************************************************/
void urmmovepoint_saveState ( URMMOVEPOINT *ump, uint32_t save_time ) {
    uint32_t vecsize = sizeof ( G3DVECTOR3F );
    uint32_t nbpt = list_count ( ump->lpt );
    LIST    *ltmppt = ump->lpt;
    uint32_t i = 0x00;

    if ( ump->save_type & UMPSAVESELECTEDPOINTS ) {
        ump->oldpos = ( G3DVECTOR3F * ) realloc ( ump->oldpos, vecsize * nbpt );
        ump->newpos = ( G3DVECTOR3F * ) realloc ( ump->newpos, vecsize * nbpt );
    }

    /*** only one handle position is saveable, no need to malloc anything ***/
    if ( save_time == UMPSAVESTATEBEFORE ) {
	    if ( ump->save_type & UMPSAVECURRENTHANDLE ) {
                if ( ump->curhan ) {
        	        memcpy ( &ump->oldhanpos, &ump->curhan->pos, sizeof ( G3DVECTOR3F ) );
                }
	    }
    }

    if ( save_time == UMPSAVESTATEAFTER ) {
	    if ( ump->save_type & UMPSAVECURRENTHANDLE ) {
                if ( ump->curhan ) {
        	        memcpy ( &ump->newhanpos, &ump->curhan->pos, sizeof ( G3DVECTOR3F ) );
                }
	    }
    }

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        if ( save_time == UMPSAVESTATEBEFORE ) {
            if ( ump->save_type & UMPSAVESELECTEDPOINTS ) {
                memcpy ( &ump->oldpos[i], &pt->pos, vecsize );
            }
        }

        if ( save_time == UMPSAVESTATEAFTER ) {
            if ( ump->save_type & UMPSAVESELECTEDPOINTS ) {
                memcpy ( &ump->newpos[i], &pt->pos, vecsize );
            }
        }

        i++;

        ltmppt = ltmppt->next;
    }
}

/******************************************************************************/
URMMOVEPOINT *g3durm_spline_movePoint ( G3DURMANAGER *urm,
                                        G3DSPLINE    *spl,
                                        uint32_t      save_type,
                                        uint32_t      return_flags ) {
    URMMOVEPOINT *ump;

    ump = urmmovepoint_new ( spl, save_type );

    g3durmanager_push ( urm, movePoint_undo,
                             movePoint_redo,
                             movePoint_free, ump, return_flags );

    return ump;
}
