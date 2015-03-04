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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#ifndef __LIST_H__
#define __LIST_H__

typedef struct _LIST {
    struct _LIST *prev;
    void         *data;
    struct _LIST *next;
} LIST;

void   list_exec         ( LIST *, void (*) (void *) );
void   list_execdata1arg ( LIST *, void (*) (void *, void *), void * );
void   list_execdata2arg ( LIST *, void (*) (void *, void *, void *), void *, void * );
void   list_execargdata  ( LIST *, void (*) (void *, void *), void * );
int    list_remove       ( LIST **, void * );
LIST  *list_insert       ( LIST **, void * );
LIST  *list_append       ( LIST **, void * );
LIST  *list_new          ( void * );
void   list_free         ( LIST **, void (*)( void * ) );
int    list_count        ( LIST *ltmp );
LIST  *list_seek         ( LIST *, void * );
LIST  *list_forward      ( LIST *, unsigned long );
LIST  *list_copy         ( LIST * );
void   list_freeprev     ( LIST *, void (*)(void *) );
void   list_freenext     ( LIST *, void (*)(void *) );
void **list_to_array     ( LIST * );
void  *list_seekbyrank   ( LIST *, int );

#endif
