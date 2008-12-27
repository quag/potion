//
// internal.c
// memory allocation and innards
//
// (c) 2008 why the lucky stiff, the freelance professor
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "potion.h"

#define TYPE_BATCH_SIZE 64

static void potion_init(Potion *P) {
  PN vtable = potion_type_new(P, PN_TVTABLE, 0);
  PN obj_vt = potion_type_new(P, PN_TOBJECT, vtable);
  potion_type_new(P, PN_TNIL, obj_vt);
  potion_type_new(P, PN_TNUMBER, obj_vt);
  potion_type_new(P, PN_TBOOLEAN, obj_vt);
  potion_type_new(P, PN_TSTRING, obj_vt);
  potion_type_new(P, PN_TTABLE, obj_vt);
  potion_type_new(P, PN_TCLOSURE, obj_vt);
  potion_type_new(P, PN_TTUPLE, obj_vt);
  potion_type_new(P, PN_TSTATE, obj_vt);
  potion_type_new(P, PN_TSOURCE, obj_vt);
  potion_type_new(P, PN_TBYTES, obj_vt);
  potion_str_hash_init(P);
  potion_lobby_init(P);

  PN_allocate = potion_str(P, "allocate");
  PN_def = potion_str(P, "def");
  PN_delegated = potion_str(P, "delegated");
  PN_lookup = potion_str(P, "lookup");
  PN_inspect = potion_str(P, "inspect");

  potion_def_method(P, 0, vtable, PN_lookup, PN_FUNC(potion_lookup));
  potion_def_method(P, 0, vtable, PN_def, PN_FUNC(potion_def_method));

  potion_send(vtable, PN_def, PN_allocate, PN_FUNC(potion_allocate));
  potion_send(vtable, PN_def, PN_delegated, PN_FUNC(potion_delegated));

  potion_primitive_init(P);
  potion_num_init(P);
  potion_str_init(P);
  potion_table_init(P);
  potion_source_init(P);
}

Potion *potion_create() {
  Potion *P = PN_ALLOC(Potion);
  PN_MEMZERO(P, Potion);
  PN_GB(P->gb, NULL, 0);
  P->vt = PN_TSTATE;
  P->typea = TYPE_BATCH_SIZE;
  P->typen = PN_TUSER;
  P->vts = PN_ALLOC_N(PN, P->typea);
  potion_init(P);
  return P;
}

PN potion_delegated(Potion *P, PN closure, PN self) {
  PNType t = P->typen++;
  PN vt = potion_type_new(P, t, self);

  // TODO: expand the main vtable if full
  if (P->typea == P->typen)
    printf("Vtable out of room!\n");
  return vt;
}

void potion_destroy(Potion *P) {
  // TODO: free the state through GC
  PN_FREE(P);
}