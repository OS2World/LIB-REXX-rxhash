/* array routines for rexx -- this provides an associative array which can
 * be passed to procedures
 *
 * Distributed under the Mozilla Public Licence, version 1.1
 * The Original Code is rxHash
 *
 * The Initial Developer of the Original Code is Patrick TJ McPhee.
 * Portions created by Patrick McPhee are Copyright © 1999, 2003
 * Patrick TJ McPhee. All Rights Reserved.
 *
 * Contributors:
 *
 * $Header: C:/ptjm/rexx/rxhash/RCS/rxhash.c 1.9 2003/05/26 17:04:03 ptjm Rel $
 */ 

#include "rxproto.h"
#include "rxhash.h"

/* number of hash buckets. Everybody says this should be prime */
#define hash_Size 1013

typedef struct hashentry_T {
   struct hashentry_T * N;
   RXSTRING key;
   RXSTRING val;
   int hv;
} hashentry_t;

/* a pointer to this is typedeffed as rxhash_t in rxhash.h */
struct hash_T {
   hashentry_t * tbl[hash_Size];
   rxhash_t props;    /* properties are stored in a hash table */
};

/* property names for rxhash -- the rxhash_ prefix is reserved */
static const char prppk[] = "rxhash_pk",   /* previous key for doover */
                 prpdft[] = "rxhash_dft";  /* default value */

static PRXSTRING ppk, pdft;

static PRXSTRING setpstr(const char * s, const int l)
{
   PRXSTRING p;

   p = malloc(sizeof(*p)+l);

   if (!p)
      return 0;

   p->strptr = (unsigned char *)(p+1);
   memcpy(p->strptr, s, l);
   p->strlength = l;

   return p;
}


/* the hash algorithm is ripped-off from cweb */
static int hash_val(const PRXSTRING r)
{
   register int i, h = 0;

   for (i = 0 ; i < r->strlength; i++) {
      h = ((h + h) + r->strptr[i]) % hash_Size;
   }

   return h;
}


rxhash_t rxhash_new()
{
   return (rxhash_t) calloc(sizeof(struct hash_T), 1);
}

rxfunc(arrnew)
{
   rxhash_t tbl = rxhash_new();

   checkparam(0, 1);

   if (tbl) {
      if (argc > 0) {
         if (!pdft)
            pdft = setpstr(prpdft, sizeof(prpdft)-1);
         rxhash_setprop(tbl, pdft, argv);
      }

      RXHASH_TO_RXSTRING(result, tbl);
   }
   else {
      result_zero();
   }

   return 0;
}

/* add a value to the array */
int rxhash_set(rxhash_t tbl, PRXSTRING key, PRXSTRING val)
{
   int hv;
   register hashentry_t * h;

   hv = hash_val(key);

   for (h = tbl->tbl[hv];
        h && (key->strlength != h->key.strlength || memcmp(key->strptr, h->key.strptr, key->strlength));
        h = h->N)
      ;

   if (!h) {
      h = malloc(sizeof(*h)+key->strlength);

      if (!h)
         return 0;

      else {
         h->key.strlength = key->strlength;
         h->key.strptr = (unsigned char *)(h+1);
         memcpy(h->key.strptr, key->strptr, key->strlength);

         h->val.strlength = 0;
         h->val.strptr = NULL;

         h->hv = hv;

         h->N = tbl->tbl[hv];
         tbl->tbl[hv] = h;
      }
   }

   if (val->strlength) {
      h->val.strptr = realloc(h->val.strptr, val->strlength);
      if (!h->val.strptr)
         return 0;

      memcpy(h->val.strptr, val->strptr, val->strlength);
   }
   else if (h->val.strptr) {
      free(h->val.strptr);
      h->val.strptr = NULL;
   }
   h->val.strlength = val->strlength;

   return 1;
}

/* add a value to the array */
rxfunc(arrset)
{
   register hashentry_t * h;
   rxhash_t tbl;

   checkparam(3, 3);

   if (argv[0].strlength != sizeof(tbl) || !argv[1].strptr)
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   if (rxhash_set(tbl, argv+1, argv+2)) {
      result_zero();
      return 0;
   }
   else {
      return NOMEMORY;
   }

   return 0;
}

/* look an element up in the hash table. If it's found, return a pointer to
 * its val member. Otherwise, return NULL */
PRXSTRING rxhash_get(rxhash_t tbl, PRXSTRING key)
{
   int hv;
   register hashentry_t * h;

   hv = hash_val(key);

   for (h = tbl->tbl[hv];
        h && (key->strlength != h->key.strlength || memcmp(key->strptr, h->key.strptr, h->key.strlength));
        h = h->N)
      ;

   if (!h)
      return NULL;
   else {
      return &h->val;
   }
}

/* look an element up in the hash table and return either its value of the null
 * string. */
rxfunc(arrget)
{
   PRXSTRING val;
   rxhash_t tbl;

   checkparam(2, 2);

   if (argv[0].strlength != sizeof(tbl) || !argv[1].strptr)
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   val = rxhash_get(tbl, argv+1);

   if (!val) {
      PRXSTRING dft;
      
      if (!pdft)
         pdft = setpstr(prpdft, sizeof(prpdft)-1);

      dft = rxhash_getprop(tbl, pdft);

      if (dft) {
         rxresize(result, dft->strlength);
         memcpy(result->strptr, dft->strptr, dft->strlength);
      }
      else
         result->strlength = 0;
   }
   else if (!val->strlength)
      result->strlength = 0;
   else {
      rxresize(result, val->strlength);
      memcpy(result->strptr, val->strptr, val->strlength);
   }

   return 0;
}

/* returns 1 if all of the arguments are elements of the array, or 0
 * otherwise */
rxfunc(arrin)
{
   register int i;
   rxhash_t tbl;

   checkparam(2, -1);

   if (argv[0].strlength != sizeof(tbl) || !argv[1].strptr)
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   /* assume success */
   result_one();

   for (i = 1; i < argc; i++) {
      if (!rxhash_get(tbl, argv+i)) {
         result_zero();
         break;
      }
   }

   return 0;
}

/* drop an element from the array */
void rxhash_drop(rxhash_t tbl, PRXSTRING key)
{
   int hv;
   register hashentry_t * h, *p;

   hv = hash_val(key);

   for (h = tbl->tbl[hv], p = NULL;
        h && (key->strlength != h->key.strlength || memcmp(key->strptr, h->key.strptr, h->key.strlength));
        p = h, h = h->N)
      ;

   /* it's not an error to drop something that isn't there! */
   if (h) {
      if (p)
         p->N = h->N;
      else
         tbl->tbl[hv] = h->N;

      if (h->val.strptr)
         free(h->val.strptr);
      free(h);
   }
}

/* drop everything and free the table */
void rxhash_delete(rxhash_t tbl)
{
   register hashentry_t * h, *n;
   register int i;

   for (i = 0; i < hash_Size; i++) {
      for (h = tbl->tbl[i]; h; h = n) {
         n = h->N;
         if (h->val.strptr)
            free(h->val.strptr);
         free(h);
      }
   }

   if (tbl->props) {
      rxhash_delete(tbl->props);
   }

   free(tbl);
}

/* drop an element from the array, or drop the entire array */
rxfunc(arrdrop)
{
   register int i;
   rxhash_t tbl;

   checkparam(1, -1);


   if (argv[0].strlength != sizeof(tbl))
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   /* if there were additional arguments, drop those arguments from the array */
   if (argc > 1) {
      for (i = 1; i < argc; i++) {
         rxhash_drop(tbl, argv+i);
      }
   }

   /* if only the array was given, drop the entire thing */
   else {
      rxhash_delete(tbl);
   }

   result_zero();

   return 0;
}

int rxhash_iterate(rxhash_t tbl, PRXSTRING prevkey, PRXSTRING * pkey, PRXSTRING * pval)
{
   hashentry_t *ph, *h;
   register int i;

   /* new hash, so find the first entry */
   if (!prevkey) {
      for (i = 0; i < hash_Size && !tbl->tbl[i]; i++)
         ;

      if (i < hash_Size) {
         h = tbl->tbl[i];
      }
      else {
         h = NULL;
      }
   }

   /* otherwise set ph based on prevkey, then set h based on ph */
   else {
      ph = (hashentry_t *)((char *)prevkey - ((char *)&h->key - (char *)h));

      h = ph->N;

      if (!h) {
         /* end of that bucket, so walk through the buckets until we find one */
         for (i = ph->hv + 1; i < hash_Size && !tbl->tbl[i]; i++)
            ;

         if (i < hash_Size) {
            h = tbl->tbl[i];
         }
      }
   }

   if (h) {
      *pkey = &h->key;
      *pval = &h->val;
      return 1;
   }
   else {
      return 0;
   }
}

/* return a table property */
PRXSTRING rxhash_getprop(rxhash_t tbl, PRXSTRING key)
{
   if (!tbl->props)
      return NULL;
   else
      return rxhash_get(tbl->props, key);
}

/* set a table property */
int rxhash_setprop(rxhash_t tbl, PRXSTRING key, PRXSTRING val)
{
   if (!tbl->props)
      tbl->props = rxhash_new();

   if (!tbl->props)
      return 0;

   return rxhash_set(tbl->props, key, val);
}

/* return the `next' key for this array */
rxfunc(arrdoover)
{
   rxhash_t tbl;
   register int i;
   PRXSTRING key, val, pk, vpk;

   checkparam(1, 2);
   if (argv[0].strlength != sizeof(tbl))
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   if (!ppk)
      ppk = setpstr(prppk, sizeof(prppk)-1);

   vpk = rxhash_getprop(tbl, ppk);
   if (vpk)
      RXSTRING_TO_RXHASH(pk, vpk);
   else {
      /* we want there to be an entry in the properties table because it
       * allows us to copy rather than looking up twice each time */
      vpk = alloca(sizeof(*vpk) + sizeof(vpk));
      vpk->strptr = (unsigned char *)(vpk + 1);
      RXHASH_TO_RXSTRING(vpk, pk);
      rxhash_setprop(tbl, ppk, vpk);
      vpk = rxhash_getprop(tbl, ppk);
      pk = NULL;
   }

   if (argc > 1)
      pk = NULL;

   if (rxhash_iterate(tbl, pk, &key, &val)) {
      rxresize(result, key->strlength);
      memcpy(result->strptr, key->strptr, key->strlength);
      RXHASH_TO_RXSTRING(vpk, key);
   }
   else {
      result->strlength = 0;
      pk = NULL;
      RXHASH_TO_RXSTRING(vpk, pk);
   }

   return 0;
}

/* copy one array to another and return a pointer to the new one */
rxfunc(arrcopy)
{
   rxhash_t tbl, tbl2;
   register int i;
   PRXSTRING key, val;
   int rc;

   checkparam(1, 1);
   if (argv[0].strlength != sizeof(tbl))
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   tbl2 = rxhash_new();

   if (!tbl2) {
      return NOMEMORY;
   }

   for (rc = rxhash_iterate(tbl, NULL, &key, &val);
        rc;
        rc = rxhash_iterate(tbl, key, &key, &val)) {
      
      if (!rxhash_set(tbl2, key, val)) {
         rxhash_delete(tbl2);
         return NOMEMORY;
      }
   }

   if (tbl->props) {
      for (rc = rxhash_iterate(tbl->props, NULL, &key, &val);
           rc;
           rc = rxhash_iterate(tbl->props, key, &key, &val)) {

         if (!rxhash_setprop(tbl2, key, val)) {
            rxhash_delete(tbl2);
            return NOMEMORY;
         }
      }
   }

   RXHASH_TO_RXSTRING(result, tbl2);
   return 0;
}

/* convert an array to a stem */
rxfunc(arrtostem)
{
   rxhash_t tbl;
   register int i;
   PRXSTRING key, val, dft;
   SHVBLOCK shv;
   int rc, namel, bufl;

   checkparam(2, 2);

   if (argv[0].strlength != sizeof(tbl))
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   memset(&shv, 0, sizeof(shv));

   shv.shvnext = NULL;
   shv.shvnamelen = shv.shvvaluelen = 0;
   shv.shvcode = RXSHV_DROPV;
   shv.shvret = 0;
   namel = argv[1].strlength;
   bufl = 1000;
   if (!(shv.shvname.strptr = malloc(namel+bufl))) {
      return NOMEMORY;
   }

   for (i = 0; i < namel; i++) {
      shv.shvname.strptr[i] = toupper(argv[1].strptr[i]);
   }

   if (shv.shvname.strptr[namel-1] != '.') {
      shv.shvname.strptr[namel++] = '.';
      bufl--;
   }

   shv.shvname.strlength = namel;
   RexxVariablePool(&shv);

   shv.shvcode = RXSHV_SET;

   if (!pdft)
      pdft = setpstr(prpdft, sizeof(prpdft)-1);

   dft = rxhash_getprop(tbl, pdft);
   if (dft) {
      shv.shvvalue = *dft;
      RexxVariablePool(&shv);
   }

   for (rc = rxhash_iterate(tbl, NULL, &key, &val);
        rc;
        rc = rxhash_iterate(tbl, key, &key, &val)) {
      if (key->strlength > bufl) {
         bufl = key->strlength;
         shv.shvname.strptr = realloc(shv.shvname.strptr, namel+bufl);
         if (!(shv.shvname.strptr = malloc(namel+bufl))) {
            return NOMEMORY;
         }
      }
      memcpy(shv.shvname.strptr+namel, key->strptr, key->strlength);
      shv.shvname.strlength = namel+key->strlength;
      shv.shvvalue = *val;
      RexxVariablePool(&shv);
   }

   return 0;
}

/* convert a stem to an array */
rxfunc(arrfromstem)
{
   rxhash_t tbl;
   register int i;
   RXSTRING key;
   SHVBLOCK shv;
   int rc, namel;

   checkparam(1, 1);

   tbl = rxhash_new();

   if (!tbl) {
      return NOMEMORY;
   }

   memset(&shv, 0, sizeof(shv));

   shv.shvcode = RXSHV_NEXTV;

   namel = argv[0].strptr[argv[0].strlength-1] == '.' ? argv[0].strlength-1 : argv[0].strlength;

   for (RexxVariablePool(&shv); shv.shvret != RXSHV_LVAR; RexxVariablePool(&shv)) {
      if (shv.shvname.strlength > namel && shv.shvname.strptr[namel] == '.' &&
          !strncasecmp(shv.shvname.strptr, argv[0].strptr, namel)) {
          key.strptr = shv.shvname.strptr + namel + 1;
          key.strlength = shv.shvname.strlength - namel - 1;
          if (key.strlength)
             rxhash_set(tbl, &key, &shv.shvvalue);
          else {
             if (!pdft)
                pdft = setpstr(prpdft, sizeof(prpdft)-1);

             rxhash_setprop(tbl, pdft, &shv.shvvalue);
          }
       }
       REXXFREEMEMORY(shv.shvname.strptr);
       REXXFREEMEMORY(shv.shvvalue.strptr);

       shv.shvname.strptr = NULL;
       shv.shvvalue.strptr = NULL;
   }

   RXHASH_TO_RXSTRING(result, tbl);

   return 0;
}

rxfunc(arrdefault)
{
   rxhash_t tbl;

   checkparam(2, 2);

   if (argv[0].strlength != sizeof(tbl))
      return BADARGS;

   RXSTRING_TO_RXHASH(tbl, argv);

   if (!pdft)
      pdft = setpstr(prpdft, sizeof(prpdft)-1);

   if (argv[1].strlength) {
      rxhash_setprop(tbl, pdft, argv+1);
   }
   else {
      if (tbl->props)
         rxhash_drop(tbl->props, pdft);
   }

   result_zero();

   return 0;
}


rxfunc(arrversion)
{
   static const char ver[] = "1.0.0";
   memcpy(result->strptr, ver, sizeof(ver)-1);
   result->strlength = sizeof(ver) - 1;
   return 0;
}



rxfunc(arrloadfuncs);
rxfunc(arrdropfuncs);

struct {
    char * name;
    RexxFunctionHandler *funcptr;
} funclist[] = {
  "ARRNEW", (RexxFunctionHandler*)arrnew,
  "ARRSET", (RexxFunctionHandler*)arrset,
  "ARRGET", (RexxFunctionHandler*)arrget,
  "ARRIN", (RexxFunctionHandler*)arrin,
  "ARRDROP", (RexxFunctionHandler*)arrdrop,
  "ARRDOOVER", (RexxFunctionHandler*)arrdoover,
  "ARRCOPY", (RexxFunctionHandler*)arrcopy,
  "ARRTOSTEM", (RexxFunctionHandler*)arrtostem,
  "ARRFROMSTEM", (RexxFunctionHandler*)arrfromstem,
  "ARRDEFAULT", (RexxFunctionHandler*)arrdefault,
  "ARRVERSION", (RexxFunctionHandler*)arrversion,
  "ARRLOADFUNCS", (RexxFunctionHandler*)arrloadfuncs,
  "ARRDROPFUNCS", (RexxFunctionHandler*)arrdropfuncs
};

rxfunc(arrloadfuncs)
{
   register int i;

   checkparam(0,0);

   for (i = 0; i < DIM(funclist); i++) {
      RexxRegisterFunctionExe(funclist[i].name, funclist[i].funcptr);
   }

   result_zero();

   return 0;
}

rxfunc(arrdropfuncs)
{
   register int i;

   checkparam(0,0);

   for (i = 0; i < DIM(funclist); i++) {
      RexxDeregisterFunction(funclist[i].name);
   }

   result_zero();

   return 0;
}
