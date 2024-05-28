/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for font functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//------------------------------------------------------------------ TTF STRUCTS

typedef struct _tagTT_OFFSET_TABLE {
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
} TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY {
	char	szTag[4];
	ULONG	uCheckSum;
	ULONG	uOffset;
	ULONG	uLength;
} TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER {
	USHORT	uFSelector;
	USHORT	uNRCount;
	USHORT	uStorageOffset;
} TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD {
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;
} TT_NAME_RECORD;

//----------------------------------------------------------- EXPORTED FUNCTIONS

// Creates a font and stores it in the font cache

ZEND_FUNCTION(wb_create_font)
{
	zend_long height = 10, color = 0x000000, flags = 0;
	char *name;
	size_t name_len;
	TCHAR *wcs = 0;
	zend_long color_len;
	zend_long flags_len;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|ll", &name, &name_len, &height, &color, &flags) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(name,name_len)
		Z_PARAM_LONG(height)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(color, color_len)
		Z_PARAM_LONG_OR_NULL(flags, flags_len)
	ZEND_PARSE_PARAMETERS_END();

	wcs = Utf82WideChar(name, name_len);
	RETURN_LONG(wbCreateFont(wcs, height, color, flags));
}

// Destroys a font or all created fonts

ZEND_FUNCTION(wb_destroy_font)
{
	zend_long nfont = 0;
	zend_long nfont_len = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &nfont) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(nfont, nfont_len)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbDestroyFont(nfont));
}

// Applies a font to a control

ZEND_FUNCTION(wb_set_font)
{
	zend_long pwbo, nfont = 0, redraw = 1;
	zend_long nfont_len, redraw_len;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pwbo, &nfont, &redraw) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(nfont, nfont_len)
		Z_PARAM_LONG_OR_NULL(redraw, redraw_len)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbSetControlFont((PWBOBJ)pwbo, nfont, redraw));
}


ZEND_FUNCTION(wb_get_ttf_info)
{
	char *file;
	size_t file_size;
	php_stream *fhnd = NULL;
	char real_path[MAXPATHLEN];

	TT_OFFSET_TABLE ttOffsetTable;
	TT_TABLE_DIRECTORY tblDir;
	TT_NAME_TABLE_HEADER ttNTHeader;
	TT_NAME_RECORD ttRecord[20];
	
	BOOL bFound = FALSE;
	char csTemp[4096];
	int i,j,k=0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING_OR_NULL(file, file_size)
	ZEND_PARSE_PARAMETERS_END();

	if(!realpath(file, real_path)) RETURN_BOOL(FALSE);
	
	fhnd = php_stream_fopen(real_path, "rb", NULL);
	if(!fhnd) RETURN_BOOL(FALSE);

	if(!php_stream_read(fhnd, (char *)&ttOffsetTable, sizeof(TT_OFFSET_TABLE))) {
		php_stream_close(fhnd);
		RETURN_BOOL(FALSE);
	}

	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

	// Check is this is a true type font and the version is 1.0
	if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
		RETURN_BOOL(FALSE);

	for(i = 0; i< ttOffsetTable.uNumOfTables; i++) {
		php_stream_read(fhnd, (char *)&tblDir, sizeof(TT_TABLE_DIRECTORY));
		ZeroMemory(csTemp, sizeof(csTemp));
		strncpy(csTemp, tblDir.szTag, 4);
		if(!strcasecmp(csTemp,"name")){
			tblDir.uLength = SWAPLONG(tblDir.uLength);
			tblDir.uOffset = SWAPLONG(tblDir.uOffset);
			bFound = TRUE;
			break;
		}
	}

	if(bFound){
		php_stream_seek(fhnd, tblDir.uOffset, SEEK_SET);
		php_stream_read(fhnd, (char *)&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));
		ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
		ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);

		for(i=0; i<ttNTHeader.uNRCount && i < 20; i++){
			php_stream_read(fhnd, (char *)&ttRecord[i], sizeof(TT_NAME_RECORD));
			ttRecord[i].uNameID = SWAPWORD(ttRecord[i].uNameID);
			ttRecord[i].uStringLength = SWAPWORD(ttRecord[i].uStringLength);
			ttRecord[i].uStringOffset = SWAPWORD(ttRecord[i].uStringOffset);
		}

		array_init(return_value);
		for(j = 0; j < i; j++) {
			if(ttRecord[j].uNameID < k) continue;
			if(ttRecord[j].uStringLength > 4095) ttRecord[j].uStringLength = 4095;
			php_stream_seek(fhnd, tblDir.uOffset + ttNTHeader.uStorageOffset + ttRecord[j].uStringOffset, SEEK_SET);
			ZeroMemory(csTemp, sizeof(csTemp));
			php_stream_read(fhnd, csTemp, ttRecord[j].uStringLength);
			
			if(!strlen(csTemp)) continue;
			k = ttRecord[j].uNameID;
			switch(ttRecord[j].uNameID){
				case  0: add_assoc_string(return_value, "copyright", csTemp); break;
				case  1: add_assoc_string(return_value, "name", csTemp); break;
				case  2: add_assoc_string(return_value, "subfamily", csTemp); break;
				case  3: add_assoc_string(return_value, "subfamily_id", csTemp); break;
				case  4: add_assoc_string(return_value, "full_name", csTemp); break;
				case  5: add_assoc_string(return_value, "version", csTemp); break;
				case  6: add_assoc_string(return_value, "postscript_name", csTemp); break;
				case  7: add_assoc_string(return_value, "trademark", csTemp); break;
				case  8: add_assoc_string(return_value, "manufacturer", csTemp); break;
				case  9: add_assoc_string(return_value, "designer", csTemp); break;
				case 10: add_assoc_string(return_value, "description", csTemp); break;
				case 11: add_assoc_string(return_value, "vendor_url", csTemp); break;
				case 12: add_assoc_string(return_value, "designer_url", csTemp); break;
				case 13: add_assoc_string(return_value, "license", csTemp); break;
				case 14: add_assoc_string(return_value, "license_url", csTemp); break;
				case 16: add_assoc_string(return_value, "preferre_family", csTemp); break;
				case 17: add_assoc_string(return_value, "preferre_subfamily", csTemp); break;
				case 18: add_assoc_string(return_value, "compat_full_name", csTemp); break;
				case 19: add_assoc_string(return_value, "sample_text", csTemp); break;
			}
		}
	}
	php_stream_close(fhnd);
	if(!bFound) RETURN_BOOL(FALSE);
}
//------------------------------------------------------------------ END OF FILE