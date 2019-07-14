#include "stdio.h"
#include "typedef.h"
#include "memoryCtrl.h"

/* ・ｽ・ｽ・ｽX・ｽg・ｽn・ｽ・ｽ・ｽh・ｽ・ｽ・ｽ・ｽ・ｽ・ｽA・ｽﾇ暦ｿｽ・ｽ\・ｽ・ｽ・ｽﾌゑｿｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ */
static linkList_t *searchList ( linkListGroup_t *group, hlist_t handle );
/* ・ｽ・ｽ・ｽX・ｽg・ｽﾌ擾ｿｽ・ｽﾔゑｿｽ・ｽ・ｽA・ｽ・ｽ・ｽX・ｽg・ｽn・ｽ・ｽ・ｽh・ｽ・ｽ・ｽ・ｽ・ｽ謫ｾ・ｽ・ｽ・ｽ・ｽ aaa*/
static uint16_t searchListTree ( linkListGroup_t *group, uint16_t order, uint16_t *no);
static linkList_t *searchListNo ( linkListGroup_t *group, hlist_t handle, uint16_t *no );

void my_memcpy( void *dest, void *src, uint16_t size ) {
	uint16_t i;
	for ( i = 0 ; i < size ; i++ ){
		*((uint8_t *)dest) = *((uint8_t *)src);
		dest++;
		src++;
	}
}

static linkList_t *searchList ( linkListGroup_t *group, hlist_t handle ) {
	uint8_t i;
	for ( i = 0 ; i < group->num_of_list ; i++ ) {
		if ( ( group->plist[i].handle == handle ) && ( group->plist[i].isUsed == TRUE ) ){
			return &(group->plist[i]);
		}
	}
	return NULL;
}

static linkList_t *searchListNo ( linkListGroup_t *group, hlist_t handle, uint16_t *no ) {
	uint16_t i;
	for ( i = 0 ; i < group->num_of_list ; i++ ) {
		if ( ( group->plist[i].handle == handle ) && ( group->plist[i].isUsed == TRUE ) ){
			*no = i;
			return &(group->plist[i]);
		}
	}
	*no = ERR_LIST_NOT_FOUND;
	return NULL;
}

/* ・ｽ・ｽ・ｽX・ｽg・ｽﾌ擾ｿｽ・ｽﾔゑｿｽ・ｽ・ｽA・ｽ・ｽ・ｽX・ｽg・ｽn・ｽ・ｽ・ｽh・ｽ・ｽ・ｽ・ｽ・ｽ謫ｾ・ｽ・ｽ・ｽ・ｽ */
static uint16_t searchListTree (linkListGroup_t *group, uint16_t order, uint16_t *no) {
	hlist_t handle;
	linkList_t *pl;
	uint16_t ct;

	*no = ERR_LIST_NOT_FOUND;
	if ( group->num_of_list == 0 ) {
		return ERR_LIST_EMPTY;
	}

	ct = 0;
	handle = refListHead( group );
	pl = searchList (group, handle );
	if ( pl == NULL ) {
		/* ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽA・ｽ謫ｪ・ｽﾌデ・ｽ[・ｽ^・ｽ・ｽ・ｽ・ｽ・ｽﾂゑｿｽ・ｽ・ｽﾈゑｿｽ */
		return ERR_LIST_NOT_FOUND;
	}

	/* node・ｽﾌ本・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽﾜで・ｿｽ・ｽ[・ｽv・ｽ・ｽ・ｽ・ｽ */
	while ( ct != order ){
		/* ・ｽ・ｽ・ｽ・ｽ・ｽN・ｽ・ｽ・ｽX・ｽg・ｽﾉ不・ｽ・ｽ・ｽﾈ厄ｿｽ・ｽg・ｽp・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽﾄゑｿｽ・ｽ・ｽ */
		if ( pl->isUsed != TRUE ) {
			return ERR_LIST_STRUCT_ERROR;
		}

		if ( pl->next != NULL )
		{
			ct++;
			pl = pl->next;
		} else {
			return ERR_LIST_NOT_FOUND;
		}
	}

	/* ・ｽ・ｽ・ｽﾂゑｿｽ・ｽ・ｽ・ｽ・ｽ・ｽﾆゑｿｽ・ｽﾄゑｿｽ・ｽA・ｽ・ｽ・ｽg・ｽp・ｽﾅゑｿｽ・ｽ・ｽﾎエ・ｽ・ｽ・ｽ[・ｽ・ｽﾔゑｿｽ */
	if ( pl->isUsed != TRUE ) {
		return ERR_LIST_NOT_FOUND;
	}

	*no = ct;
	return pl->handle;
}

/* Link・ｽ・ｽ・ｽX・ｽg・ｽﾌ擾ｿｽ・ｽ・ｽ・ｽ・ｽ */
void initList( linkListGroup_t *group, linkList_t *pt, uint16_t num ) {
	uint8_t i;
	group->num_of_list = num;
	group->head = NO_LIST_DATA;
	group->last = NO_LIST_DATA;
	group->plist = pt;

	/* Link・ｽ・ｽ・ｽX・ｽg・ｽﾌ・ｿｽ・ｽ・ｽ・ｽo・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ */
	group->plist->isUsed = FALSE;
	for ( i = 0 ; i < num ; i++ ){
		(&(group->plist[i]))->isUsed = FALSE;
		(&(group->plist[i]))->prev = NULL;
		(&(group->plist[i]))->next = NULL;
		(&(group->plist[i]))->contents = NULL;
		(&(group->plist[i]))->handle = i;
	}
}

hlist_t refListHead( linkListGroup_t *group )
{
	if ( group->num_of_list == 0 ) {
		return ERR_LIST_EMPTY;
	}

	return (&(group->plist[group->head]))->handle;
}

hlist_t refListLast( linkListGroup_t *group )
{
	if ( group->num_of_list == 0 ) {
		return ERR_LIST_EMPTY;
	}

	return (&(group->plist[group->last]))->handle;
}

uint16_t refListNext ( linkListGroup_t *group, hlist_t handle )
{
	uint16_t no;
	linkList_t *pl;

	if ( group->num_of_list == 0 ) {
		return ERR_LIST_EMPTY;
	}

	pl = searchListNo ( group, handle, &no );
	if ( ( pl == NULL ) || ( pl->next == NULL ) ){
		return ERR_LIST_NOT_FOUND;
	}
	return no;
}

uint16_t refListPrev ( linkListGroup_t *group, hlist_t handle )
{
	uint16_t no;
	linkList_t *pl;

	if ( group->num_of_list == 0 ) {
		return ERR_LIST_EMPTY;
	}

	pl = searchListNo ( group, handle, &no );
	if ( ( pl == NULL ) || ( pl->prev == NULL ) ){
		return ERR_LIST_NOT_FOUND;
	}
	return no;
}

hlist_t refListWithNO( linkListGroup_t *group, uint16_t order )
{
	hlist_t hlist;
	uint16_t no;

	hlist = refListHead( group );
	if ( hlist == ERR_LIST_EMPTY ) {
		return ERR_LIST_EMPTY;
	}

	/* ・ｽ・ｽ・ｽX・ｽg・ｽﾌ抵ｿｽ・ｽﾌ擾ｿｽ・ｽﾔゑｿｽ・ｽ・ｽA・ｽ・ｽ・ｽX・ｽg・ｽn・ｽ・ｽ・ｽh・ｽ・ｽ・ｽ鰹o・ｽ・ｽ・ｽ・ｽ */
	return searchListTree( group, order , &no );
}

/* ・ｽw・ｽ閧ｳ・ｽ黷ｽ・ｽ・ｽ・ｽX・ｽg・ｽn・ｽ・ｽ・ｽh・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽX・ｽg・ｽﾌ会ｿｽ・ｽﾔ目に位置・ｽ・ｽ・ｽ驍ｩ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ */
uint16_t refListPosition ( linkListGroup_t *group, hlist_t handle )
{
	uint16_t no;

	if ( group->num_of_list == 0 ) {
		return ERR_LIST_EMPTY;
	}

	(void)searchListTree( group, handle, &no );
	return no;
}


hlist_t getEmptyList( linkListGroup_t *group ) {
	uint8_t i;

	for ( i = 0 ; i < group->num_of_list ; i++ ) {
		if ( group->plist[i].isUsed == FALSE ){
			(&(group->plist[i]))->next = NULL;
			(&(group->plist[i]))->prev = NULL;
			(&(group->plist[i]))->isUsed = TRUE;
			return (&(group->plist[i]))->handle;
		}
	}
	return ERR_LIST_NOT_FOUND;
}

bool_t isUsedList( linkListGroup_t *group, hlist_t handle ){
	if ( handle >= group->num_of_list ) {
		return FALSE;
	}

	return group->plist[handle].isUsed;
}
void *getContentsPointer ( linkListGroup_t *group, hlist_t handle ){
	if ( handle >= group->num_of_list ) {
		return NULL;
	}

	return group->plist[handle].contents;
}

bool_t addListToHead( linkListGroup_t *group, hlist_t handle, void *contents )
{
	hlist_t head;
	if ( ( (&(group->plist[handle]))->isUsed == TRUE )
			&& ( (&(group->plist[handle]))->prev == NULL )
			&& ( (&(group->plist[handle]))->next == NULL )
			&& ( handle != group->head ) ){
		if ( group->head == NO_LIST_DATA ){
			group->head   = handle;
			group->last   = handle;
			(&(group->plist[handle]))->prev = NULL;
			(&(group->plist[handle]))->next = NULL;
			(&(group->plist[handle]))->contents = contents;
		} else {
			head = group->head;
			group->head = handle;
			if ( ((&(group->plist[head]))->next) == NULL ) {
				group->last = head;
			}
			(&(group->plist[head]))->prev   = &(group->plist[handle]);
			(&(group->plist[handle]))->next = &(group->plist[head]);
			(&(group->plist[handle]))->prev = NULL;
			(&(group->plist[handle]))->contents = contents;
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

bool_t addListToLast( linkListGroup_t *group, hlist_t handle, void *contents )
{
	hlist_t last;
	if ( ( (&(group->plist[handle]))->isUsed == TRUE )
			&& ( (&(group->plist[handle]))->prev == NULL )
			&& ( (&(group->plist[handle]))->next == NULL )
			&& ( handle != group->head ) ){
		if ( group->head == NO_LIST_DATA ){
			group->head   = handle;
			group->last   = handle;
			(&(group->plist[handle]))->prev = NULL;
			(&(group->plist[handle]))->next = NULL;
			(&(group->plist[handle]))->contents = contents;
		} else {
			last = group->last;
			group->last = handle;
			if ( ((&(group->plist[last]))->prev) == NULL ) {
				group->head = last;
			}
			(&(group->plist[last]))->next   = &(group->plist[handle]);
			(&(group->plist[handle]))->next = NULL;
			(&(group->plist[handle]))->prev = &(group->plist[last]);
			(&(group->plist[handle]))->contents = contents;
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

/* ref_handle・ｽﾌ趣ｿｽ・ｽﾌエ・ｽ・ｽ・ｽg・ｽ・ｽ・ｽ・ｽadd_handle・ｽ・ｽList・ｽC・ｽ・ｽ・ｽX・ｽ^・ｽ・ｽ・ｽX・ｽ・ｽﾇ会ｿｽ・ｽ・ｽ・ｽ・ｽ */
bool_t addListToNext( linkListGroup_t *group, hlist_t add_handle, hlist_t ref_handle,
	void *contents )
{
	if ( group->head == NO_LIST_DATA ){
		return addListToLast( group, add_handle, contents );
	}

	if ( ( (&(group->plist[add_handle]))->isUsed == TRUE )
			&& ( (&(group->plist[add_handle]))->prev == NULL )
			&& ( (&(group->plist[add_handle]))->next == NULL )
			&& ( (&(group->plist[ref_handle]))->isUsed == TRUE )
			&& ( add_handle != group->head ) ){

		if ( ((&(group->plist[ref_handle]))->next) == NULL ) {
				group->last = add_handle;
		} else {
			((linkList_t *)((&(group->plist[ref_handle]))->next))->prev = &(group->plist[add_handle]);
			(&(group->plist[add_handle]))->next = (&(group->plist[ref_handle]))->next;
		}
		(&(group->plist[ref_handle]))->next =  &(group->plist[add_handle]);
		(&(group->plist[add_handle]))->prev = &(group->plist[ref_handle]);
		(&(group->plist[add_handle]))->contents = contents;
		return TRUE;
	} else {
		return FALSE;
	}
}
/* ref_handle・ｽﾌ前・ｽﾌエ・ｽ・ｽ・ｽg・ｽ・ｽ・ｽ・ｽadd_handle・ｽ・ｽList・ｽC・ｽ・ｽ・ｽX・ｽ^・ｽ・ｽ・ｽX・ｽ・ｽﾇ会ｿｽ・ｽ・ｽ・ｽ・ｽ */
bool_t addListToPrev( linkListGroup_t *group, hlist_t add_handle, hlist_t ref_handle,
		void *contents){
	if ( group->head == NO_LIST_DATA ){
		return addListToLast( group, add_handle, contents );
	}

	if ( ( (&(group->plist[add_handle]))->isUsed == TRUE )
			&& ( (&(group->plist[add_handle]))->prev == NULL )
			&& ( (&(group->plist[add_handle]))->next == NULL )
			&& ( (&(group->plist[ref_handle]))->isUsed == TRUE )
			&& ( add_handle != group->head ) ){

		if ( ((&(group->plist[ref_handle]))->prev) == NULL ) {
				group->head = add_handle;
		} else {
			((linkList_t *)((&(group->plist[ref_handle]))->prev))->next = &(group->plist[add_handle]);
			(&(group->plist[add_handle]))->prev = (&(group->plist[ref_handle]))->prev;
		}
		(&(group->plist[ref_handle]))->prev = &(group->plist[add_handle]);
		(&(group->plist[add_handle]))->next = &(group->plist[ref_handle]);
		(&(group->plist[add_handle]))->contents = contents;
		return TRUE;
	} else {
		return FALSE;
	}
}

bool_t delList( linkListGroup_t *group, hlist_t del_handle ){
	linkList_t *prevAddr;
	linkList_t *nextAddr;

	if ( (&(group->plist[del_handle]))->isUsed == FALSE ) {
		return FALSE;
	}

	nextAddr = (((linkList_t *)(&(group->plist[del_handle]))->next));
	prevAddr = (((linkList_t *)(&(group->plist[del_handle]))->prev));

	/* 蜑企勁譎ゅ・head縺ｨlast縺ｮ菫ｮ豁｣ */
	if ( ( nextAddr == NULL ) && ( prevAddr == NULL ) && ( del_handle != group->head ) ) {
		(&(group->plist[del_handle]))->prev = NULL;
		(&(group->plist[del_handle]))->next = NULL;
		(&(group->plist[del_handle]))->contents = NULL;
		(&(group->plist[del_handle]))->isUsed = FALSE;
		return TRUE;
	} else if ( ( (&(group->plist[del_handle]))->next == NULL )
			&& (   (&(group->plist[del_handle]))->prev == NULL ) ){
		group->head = NO_LIST_DATA;
		group->last = NO_LIST_DATA;
	} else if ( group->last == del_handle ) {
		/*  */
		if ( (prevAddr->prev) != NULL ) {
			group->last = ((linkList_t *)(prevAddr->prev))->handle;
		} else {
			group->last = prevAddr->handle;
		}
	} else if ( group->head == del_handle ) {
		if ( (nextAddr->next) != NULL ) {
			group->head = ((linkList_t *)(nextAddr->next))->handle;
		} else {
			group->head = nextAddr->handle;
		}
	} else {
		prevAddr->next = nextAddr;
		nextAddr->prev = prevAddr;
	}

	if ( ( group->head != NO_LIST_DATA ) && ( group->last != NO_LIST_DATA ) ) {
		if ( group->head == group->last ) {
			(&(group->plist[group->head]))->prev = NULL;
			(&(group->plist[group->head]))->next = NULL;
		}
	}

	(&(group->plist[del_handle]))->prev = NULL;
	(&(group->plist[del_handle]))->next = NULL;
	(&(group->plist[del_handle]))->contents = NULL;
	(&(group->plist[del_handle]))->isUsed = FALSE;
	return TRUE;
}


/* TestCode */
#include "stdio.h"
#include "TestMemoryCtrl.h"
