#ifndef __MEMORY_CTRL_H__
#define __MEMORY_CTRL_H__
#include "stdio.h"
#include "typedef.h"

typedef uint16_t hlist_t;

#define ERR_LIST_OK    (0x0000)
#define ERR_LIST_EMPTY (0xFFFF)
#define ERR_LIST_FULL  (0xFFFE)
#define ERR_LIST_NOT_FOUND  (0xFFFD)
#define ERR_LIST_STRUCT_ERROR  (0xFFFC)

#define NO_LIST_DATA 0xFFFF

/* ListHandle�̃G���[����}�N�� */
#define IS_ERR_LIST_HANDLE(_x_) ((((_x_)&0x8000)!=0)?TRUE:FALSE)

typedef struct {
	void *prev;
	void *next;
	void *contents;
	hlist_t handle;
	bool_t isUsed;
} linkList_t;

typedef struct {
	linkList_t *plist;
	hlist_t head;
	hlist_t last;
	uint16_t num_of_list;
} linkListGroup_t;

void my_memcpy( void *dest, void *src, uint16_t size );

void initList( linkListGroup_t *group, linkList_t *pt, uint16_t num );
hlist_t refListHead( linkListGroup_t *group );
hlist_t refListLast( linkListGroup_t *group );
/* ���X�g�̏��Ԃ���Handle���l������ */
hlist_t refListWithNO( linkListGroup_t *group, uint16_t order );
uint16_t refListPosition ( linkListGroup_t *group, hlist_t handle );
hlist_t refListNext ( linkListGroup_t *group, hlist_t handle );
hlist_t refListPrev ( linkListGroup_t *group, hlist_t handle );

hlist_t getEmptyList( linkListGroup_t *group );
bool_t isUsedList( linkListGroup_t *group, hlist_t handle );
void *getContentsPointer ( linkListGroup_t *group, hlist_t handle );

bool_t addListToHead( linkListGroup_t *group, hlist_t handle , void *contents );
bool_t addListToLast( linkListGroup_t *group, hlist_t handle , void *contents );
/* ref_handle�̎��̃G���g����add_handle��List�C���X�^���X��ǉ����� */
bool_t addListToNext( linkListGroup_t *group, hlist_t add_handle, hlist_t ref_handle, void *contents );
/* ref_handle�̑O�̃G���g����add_handle��List�C���X�^���X��ǉ����� */
bool_t addListToPrev( linkListGroup_t *group, hlist_t add_handle, hlist_t ref_handle, void *contents );

bool_t delList( linkListGroup_t *group, hlist_t del_handle );
#endif /* __MEMORY_CTRL_H__ */
