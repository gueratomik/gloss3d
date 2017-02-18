/********************* Standard Librabry Headers ******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************* Application's Headers ******************************/
#include <list.h>

/******************************************************************************/
void *list_seekbyrank ( LIST *lis, int rank ) {
    unsigned long i = 0x00;
    LIST *lcur = NULL;

    while ( i++ <= rank ) {
        lcur = lis;

        lis  = lis->next;
    }

    if ( lcur ) return lcur->data;


    return NULL;
}

/******************************************************************************/
void list_exec ( LIST *lis, void (*func) (void *) ) {
    while ( lis ) {
        func ( lis->data );

        lis = lis->next;
    }
}

/******************************************************************************/
void list_execdata1arg ( LIST *lis, void (*func) (void *, void *),
                                    void *arg ) {
    while ( lis ) {
        func ( lis->data, arg );

        lis = lis->next;
    }
}

/******************************************************************************/
void list_execdata2arg ( LIST *lis, void (*func) (void *, void *, void *),
                                    void *arg1,
                                    void *arg2 ) {
    while ( lis ) {
        func ( lis->data, arg1, arg2 );

        lis = lis->next;
    }
}

/******************************************************************************/
void list_execargdata ( LIST *lis, void (*func) (void *, void *), void *arg ) {
    while ( lis ) {
        func ( arg, lis->data );

        lis = lis->next;
    }
}

/******************************************************************************/
void **list_to_array ( LIST *lis ) {
    int nbnod = list_count ( lis );
    void **ptr = ( void ** ) calloc ( nbnod, sizeof ( void * ) );
    int i = 0x00;

    if ( ptr == NULL ) {
        fprintf ( stderr, "list_to_array: memory allocation failed\n" );
    }

    while ( lis ) {
        ptr[i++] = lis->data;

        lis = lis->next;
    }

    return ptr;   
}

/******************************************************************************/
LIST *list_copy ( LIST *lis ) {
    LIST *lcp = NULL;

    while ( lis ) {
        list_append ( &lcp, lis->data );

        lis = lis->next;
    }

    return lcp;
}

/******************************************************************************/
LIST *list_forward ( LIST *lis, unsigned long n ) {
    while ( lis && (n--) ) {

        lis = lis->next;
    }

    return lis;
}

/******************************************************************************/
int list_count ( LIST *ltmp ) {
    int i = 0x00;

    while ( ltmp ) {
        i++;

        ltmp = ltmp->next;
    }

    return i;
}

/*****************************************************************************/
LIST *list_seek ( LIST *lis, void *data ) {
    LIST *ltmp = lis;

    while ( ltmp ) {
        if ( data == ltmp->data ) {

            return ltmp;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/*****************************************************************************/
void list_freeprev ( LIST *lis, void (*func)(void *data) ) {
    if ( lis ) {
        LIST *ltmp = lis->prev;

        while ( ltmp ) {
            LIST *lprev = ltmp->prev;

            if ( func ) func ( ltmp->data );

            free ( ltmp );

            ltmp = lprev;
        }

        lis->prev = NULL;
    }
}

/*****************************************************************************/
void list_freenext ( LIST *lis, void (*func)(void *data) ) {
    if ( lis ) {
        LIST *ltmp = lis->next;

        while ( ltmp ) {
            LIST *lnext = ltmp->next;

            if ( func ) func ( ltmp->data );

            free ( ltmp );

            ltmp = lnext;
        }

        lis->next = NULL;
    }
}

/*****************************************************************************/
void list_free ( LIST **lis, void (*func)(void *data) ) {
    LIST *ltmp = (*lis);

    while ( ltmp ) {
        LIST *lnext = ltmp->next;

        if ( func ) func ( ltmp->data );

        free ( ltmp );

        ltmp = lnext;
    }

    (*lis) = NULL;
}

/**************************** LIST FUNCTIONS *********************************/
LIST *list_new ( void *data ) {
    LIST *lis = ( LIST * ) calloc ( 0x01, sizeof ( LIST ) );

    if ( lis == NULL ) {
        fprintf ( stderr, "LIST_NEW:ERR\n" );

        return NULL;
    }

    lis->data = data;

    return lis;
}

/*****************************************************************************/
int list_remove ( LIST **lis, void *data ) {
    LIST *ltmp = (*lis);
    int n = 0x00;

    while ( ltmp ) {
        LIST *lprev = ltmp->prev,
             *lnext = ltmp->next;

        if ( ltmp->data == data ) {
            if ( ltmp == (*lis) ) (*lis) = lnext;

            if ( lprev ) {
                lprev->next = lnext;
            }

            if ( lnext ) {
                lnext->prev = lprev;
            }

            free ( ltmp ); n++;
        }

        ltmp = lnext;
    }

    return n;
}


/*****************************************************************************/
LIST *list_insert ( LIST **lis, void *data ) {
    LIST *lnew = list_new ( data );
    LIST *ltmp = (*lis);

    if ( lnew == NULL ) {
        fprintf ( stderr, "LIST_INSERT:ERR\n" );

        return NULL;
    }

    if ( ltmp ) ltmp->prev = lnew;

    lnew->next = ltmp;

    (*lis) = lnew;

    return lnew;
}

/*****************************************************************************/
LIST *list_append ( LIST **lis, void *data ) {
    LIST *lnew = list_new ( data );

    if ( lnew == NULL ) {
        fprintf ( stderr, "LIST_APPEND:ERR\n" );

        return NULL;
    }

    if ( (*lis) == NULL ) {
        (*lis) = lnew;
    } else {
        LIST *ltmp = (*lis);

        while ( ltmp->next ) {
            ltmp = ltmp->next;
        }

        ltmp->next = lnew;
        lnew->prev = ltmp;
    }

    return lnew;
}
