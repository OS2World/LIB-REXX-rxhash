/* C interfaces for array routines for rexx
 *  prerequisite: RXSTRING and PRXSTRING (i.e., from rexxsaa.h)
 *
 * Distributed under the Mozilla Public Licence, version 1.1
 * The Original Code is rxHash
 *
 * The Initial Developer of the Original Code is Patrick TJ McPhee.
 * Portions created by Patrick McPhee are Copyright © 1998, 2001
 * Patrick TJ McPhee. All Rights Reserved.
 *
 * Contributors:
 *
 * $Header: C:/ptjm/rexx/rxhash/RCS/rxhash.h 1.2 2003/05/26 02:57:27 ptjm Rel $
 */ 

typedef struct hash_T * rxhash_t;

/* initialise an empty array -- returns NULL for failure */
rxhash_t rxhash_new(void);

/* free memory for an array */
void rxhash_delete(rxhash_t tbl);

/*  convert a rxhash_t to an RXSTRING and vice-versa */ 
#define RXHASH_TO_RXSTRING(str, hsh) (memcpy((str)->strptr, &(hsh), sizeof(hsh)), (str)->strlength = sizeof(hsh))
#define RXSTRING_TO_RXHASH(hsh, str) memcpy(&(hsh), (str)->strptr, sizeof(hsh))

/* add a value to an array -- returns 0 for failure, 1 for success */
int rxhash_set(rxhash_t tbl, PRXSTRING key, PRXSTRING val);

/* retrieve a value from an array -- returns NULL when not found.
 * the return code is a pointer into the hash table. It will persist
 * as long as that hash entry is not deleted, although the strptr value
 * may change. */
PRXSTRING rxhash_get(rxhash_t tbl, PRXSTRING key);

/* drop an element from the array */
void rxhash_drop(rxhash_t tbl, PRXSTRING key);

/* walk through the array. returns 1 if we haven't reached the end of the
 * table, or 0 if we have. prevkey should be NULL to start from the beginning.
 * the key and value are returned in pkey and pval, respectively.
 * eg: key = NULL; while (rxhash_iterate(tbl, key, &key, &val)) ... ; */
int rxhash_iterate(rxhash_t tbl, PRXSTRING prevkey, PRXSTRING * pkey, PRXSTRING * pval);

/* get and set table properties. These can be whatever's needed by a
 * particular application. Property names starting with rxhash_ are
 * reserved for use by the rxhash package */
PRXSTRING rxhash_getprop(rxhash_t tbl, PRXSTRING key);
int rxhash_setprop(rxhash_t tbl, PRXSTRING key, PRXSTRING val);
