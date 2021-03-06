
/* 
 * Projeto Mezza Library 0.3.0
 * Copyright 2018, 2019, 2020 Danilo Marcos Pontes de Almeida
 * <paioniu@gmail.com>
 * 
 * collections/Map.h
 *
 *  Created on: 21 de dez de 2018
 *      Author: Danilo Almeida
 */

#define TMAP_IMPLEMENT
#include "Map.h"
#include "../base/TMemMgr.h"
#include "../base/TObject.h"
#include <assert.h>

#define    TMapEntry_Create(MEMMGR, KEY, VAL) \
                TMapEntry_Create__Backend(MEMMGR, KEY, VAL, FALSE)
#define    TMapEntry_CreateObj(MEMMGR, KEY, VAL) \
                TMapEntry_Create__Backend(MEMMGR, KEY, VAL, TRUE)

static void TMapEntry_dtor(TObject obj, void *userdata);

TMapEntry *TMapEntry_Create__Backend(TMemMgr *memmgr, TString key, void *value, TBool valueIsObj) {
    TMapEntry *ret;

    #ifdef DEBUG
        assert(memmgr);
        assert(key);
    #endif

    ret = TObject_Create(memmgr, sizeof(TMapEntry), TMapEntry_dtor);
    onerror(ret) 
        throw(ExceptionTMapEntryCreation, NULL) 
        
    ret->key = key;
    ret->value = value;
		ret->valueIsObj = valueIsObj;
    
    return ret;
}

void *Map_SetEntry__Backend(TList *map, TString key, void *value, TBool valueIsObj, void *userdata) {
	TNode *nodeKey;
	TMapEntry *entry;

	#ifdef DEBUG
		assert(map);
		assert(key);
	#endif

	nodeKey = Map_GetEntryNode(map, key);
	if (nodeKey) {
		entry = (TMapEntry *)nodeKey->item;
		if(entry->valueIsObj == TRUE)
			TObject_Destroy(entry->value, userdata);
		entry->value = value;
		entry->valueIsObj = valueIsObj;
	} else {
		entry = TMapEntry_Create__Backend(TObject_ManagerOf(map), key, value, valueIsObj);
		onerror(entry) {
			throw_note(ExceptionTMapSetEntry, NULL, "Criacao de objeto de entrada")
		}
		onerror(TList_AddObj(map, entry)) {
			TObject_Destroy(entry, NULL);
			throw_note(ExceptionTMapSetEntry, NULL, "Adicao de entrada à lista")
		}
	}

	return value;
}

void Map_UnsetEntry__Backend(TList *map, char *key, void *userdata) {
  TNode *nodeKey;

	#ifdef DEBUG
		assert(map);
		assert(key);
	#endif

  nodeKey = Map_GetEntryNode(map, key);
  if (nodeKey)
    TList_Rem__Backend(map, nodeKey, userdata);
}

int map_tlistcallback(TNode *node, void *key) {
	if (0 == strcmp(((TMapEntry *)node->item)->key, key)) 
			return 1;
	return 0;
}

void *Map_GetEntry(TList *map, char *key) {
  TNode *nodeKey;

	#ifdef DEBUG
		assert(map);
		assert(key);
	#endif

  nodeKey = Map_GetEntryNode(map, key);
  if (nodeKey)
    return ((TMapEntry *)nodeKey->item)->value;
		
  return NULL;
}

static void TMapEntry_dtor(TObject obj, void *userdata) {
  TMapEntry *entry = obj;

  if (entry->key)
    TObject_Destroy(entry->key, userdata);
	if(entry->valueIsObj == TRUE) {
		TObject_Destroy(entry->value, userdata);
	}
}
