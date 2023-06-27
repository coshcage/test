#ifndef _SVIMRDB_H_
#define _SVIMRDB_H_

#include "StoneValley/src/svstring.h"
#include "StoneValley/src/svtree.h"
#include "StoneValley/src/svset.h"

typedef enum en_CellType
{
	CT_NONE,
	CT_CHAR,
	CT_SHORT,
	CT_INTEGER,
	CT_LONG,
	CT_STRING
} CellType;

typedef struct st_CELL
{
	CellType ct;
	void * pdata;
} CELL, * P_CELL;

typedef CBF_TRAVERSE SICBF_CONDITION;

typedef struct st_TABLE
{
	ARRAY_Z header;
	MATRIX tbldata;
} TABLE, * P_TABLE;

#endif