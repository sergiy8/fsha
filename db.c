#include "malloc_file.c"

static db_t dbsize; 

static inline db_t db_alloc(void){
        if(dbsize==DBSIZE) error("DBASE full, %u max",DBSIZE);
        return dbsize++; // we relay that item will be zero-inited
}
static inline void db_free(void){
        dbsize--;
}

static db_t  tsearch(db_t key, db_t * rootp)
{
    while (*rootp ) {   /* Knuth's T1: */
        int r;

        if ((r = db_compar(key, *rootp)) == 0)  /* T2: */
            return *rootp;           /* we found it! */
        rootp = (r < 0) ?
            &dbase[*rootp].left :               /* T3: follow left branch */
            &dbase[*rootp].right;               /* T4: follow right branch */
    }
    *rootp = key;
    return key;
}

