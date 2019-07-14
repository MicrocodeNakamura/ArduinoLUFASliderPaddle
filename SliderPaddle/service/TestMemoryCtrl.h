/*
 * TestMemoryCtrl.h
 *
 *  Created on: 2018/08/16
 *      Author: ya_nakamura
 */

#ifndef TESTMEMORYCTRL_H_
#define TESTMEMORYCTRL_H_

#ifdef FEATURE_TESTCODE
#include "typedef.h"
#include "memoryCtrl.h"

#define MAX_LINKLIST 20

linkListGroup_t ListGroup;
linkList_t lists[ MAX_LINKLIST ];

uint16_t contents;

void testInit( void );
void addtTest( void );
int main ( void );

void testInit(void){
	initList (&ListGroup, lists, MAX_LINKLIST);
}

void addtTest( void ){
	hlist_t handle,handle2,handle3;
	uint16_t err;
	uint8_t i;

	err = 0;
	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		handle = getEmptyList( &ListGroup );
		printf ( "addTest=%04x\n", handle );
		if ( IS_ERR_LIST_HANDLE(handle) != FALSE ){
			printf ( "add Error 1=%04x\n", handle );
			err = handle;
			break;
		}
	}

	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		if ( addListToHead( &ListGroup, i, (void *)(&contents) ) == TRUE ){
			printf ( "handle[%04x] is registered.\n",i );
		} else {
			printf ( "handle[%04x] is not registered.\n",i );
		}
	}

	printf ( "err=%04x\n", err );

	testInit();
	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		handle = getEmptyList( &ListGroup );
		printf ( "addTest=%04x\n", handle );
		if ( IS_ERR_LIST_HANDLE(handle) != FALSE ){
			printf ( "add Error 1=%04x\n", handle );
			err = handle;
			break;
		}
	}

	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		if ( addListToLast( &ListGroup, i, (void *)(&contents) ) == TRUE ){
			printf ( "last-handle[%04x] is registered.\n",i );
		} else {
			printf ( "last-handle[%04x] is not registered.\n",i );
		}
	}

	testInit();
	handle2 = getEmptyList( &ListGroup );
	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		handle = getEmptyList( &ListGroup );
		printf ( "addTest=%04x\n", handle );
		if ( IS_ERR_LIST_HANDLE(handle) != FALSE ){
			printf ( "add Error 1=%04x\n", handle );
			err = handle;
			break;
		}
	}

	addListToHead( &ListGroup, handle2, NULL);
	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		if ( addListToNext( &ListGroup, i, 0, (void *)(&contents) ) == TRUE ){
			printf ( "addListToNex-handle[%04x] is registered.\n",i );
		} else {
			printf ( "addListToNex-handle[%04x] is not registered.\n",i );
		}
	}

	testInit();
	handle2 = getEmptyList( &ListGroup );
	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		handle = getEmptyList( &ListGroup );
		printf ( "addTest=%04x\n", handle );
		if ( IS_ERR_LIST_HANDLE(handle) != FALSE ){
			printf ( "add Error 1=%04x\n", handle );
			err = handle;
			break;
		}
	}

	addListToHead( &ListGroup, handle2, NULL);
	for ( i = 0 ; i < MAX_LINKLIST ; i++ ) {
		if ( addListToPrev( &ListGroup, i, 0, (void *)(&contents) ) == TRUE ){
			printf ( "addListToNex-handle[%04x] is registered.\n",i );
		} else {
			printf ( "addListToNex-handle[%04x] is not registered.\n",i );
		}
	}

	testInit();
	handle2 = getEmptyList( &ListGroup );
	(void)delList(&ListGroup, handle2 );

	handle2 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle2, NULL );
	(void)delList(&ListGroup, handle2 );

	handle = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle, NULL );
	handle2 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle2, NULL );
	(void)delList(&ListGroup, handle );
	(void)delList(&ListGroup, handle2 );

	handle = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle, NULL );
	handle2 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle2, NULL );
	(void)delList(&ListGroup, handle2 );
	(void)delList(&ListGroup, handle );

	handle = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle, NULL );
	handle2 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle2, NULL );
	handle3 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle3, NULL );
	// now debugging
	(void)delList(&ListGroup, handle );
	(void)delList(&ListGroup, handle2 );
	(void)delList(&ListGroup, handle3 );

	handle = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle, NULL );
	handle2 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle2, NULL );
	handle3 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle3, NULL );
	(void)delList(&ListGroup, handle3 );
	(void)delList(&ListGroup, handle2 );
	(void)delList(&ListGroup, handle );

	handle = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle, NULL );
	handle2 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle2, NULL );
	handle3 = getEmptyList( &ListGroup );
	addListToHead ( &ListGroup, handle3, NULL );
	(void)delList(&ListGroup, handle2 );
	(void)delList(&ListGroup, handle3 );
	(void)delList(&ListGroup, handle );


}

int main ( void ) {
	testInit();
	addtTest();
}
#else /* FEATURE_TESTCODE */
#endif /* FEATURE_TESTCODE */
#endif /* TESTMEMORYCTRL_H_ */
