/* 
   Unix SMB/CIFS implementation.
   HMAC MD5 tests
   Copyright (C) Stefan Metzmacher 2006
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "includes.h"
#include "lib/crypto/crypto.h"

struct torture_context;

static DATA_BLOB data_blob_repeat_byte(uint8_t byte, size_t length)
{
	DATA_BLOB b = data_blob(NULL, length);
	memset(b.data, byte, length);
	return b;
}

/*
 This uses the test values from rfc 2104, 2202
*/
BOOL torture_local_crypto_hmacmd5(struct torture_context *torture) 
{
	BOOL ret = True;
	uint32_t i;
	struct {
		DATA_BLOB key;
		DATA_BLOB data;
		DATA_BLOB md5;
	} testarray[8];

	testarray[0].key	= data_blob_repeat_byte(0x0b, 16);
	testarray[0].data	= data_blob_string_const("Hi There");
	testarray[0].md5	= strhex_to_data_blob("9294727a3638bb1c13f48ef8158bfc9d");

	testarray[1].key	= data_blob_string_const("Jefe");
	testarray[1].data	= data_blob_string_const("what do ya want for nothing?");
	testarray[1].md5	= strhex_to_data_blob("750c783e6ab0b503eaa86e310a5db738");

	testarray[2].key	= data_blob_repeat_byte(0xaa, 16);
	testarray[2].data	= data_blob_repeat_byte(0xdd, 50);
	testarray[2].md5	= strhex_to_data_blob("56be34521d144c88dbb8c733f0e8b3f6");

	testarray[3].key	= strhex_to_data_blob("0102030405060708090a0b0c0d0e0f10111213141516171819");
	testarray[3].data	= data_blob_repeat_byte(0xcd, 50);
	testarray[3].md5	= strhex_to_data_blob("697eaf0aca3a3aea3a75164746ffaa79");

	testarray[4].key	= data_blob_repeat_byte(0x0c, 16);
	testarray[4].data	= data_blob_string_const("Test With Truncation");
	testarray[4].md5	= strhex_to_data_blob("56461ef2342edc00f9bab995690efd4c");

	testarray[5].key	= data_blob_repeat_byte(0xaa, 80);
	testarray[5].data	= data_blob_string_const("Test Using Larger Than Block-Size Key - Hash Key First");
	testarray[5].md5	= strhex_to_data_blob("6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd");

	testarray[6].key	= data_blob_repeat_byte(0xaa, 80);
	testarray[6].data	= data_blob_string_const("Test Using Larger Than Block-Size Key "
							 "and Larger Than One Block-Size Data");
	testarray[6].md5	= strhex_to_data_blob("6f630fad67cda0ee1fb1f562db3aa53e");

	testarray[7].key        = data_blob(NULL, 0);

	for (i=0; testarray[i].key.data; i++) {
		HMACMD5Context ctx;
		uint8_t md5[16];
		int e;

		hmac_md5_init_rfc2104(testarray[i].key.data, testarray[i].key.length, &ctx);
		hmac_md5_update(testarray[i].data.data, testarray[i].data.length, &ctx);
		hmac_md5_final(md5, &ctx);

		e = memcmp(testarray[i].md5.data,
			   md5,
			   MIN(testarray[i].md5.length, sizeof(md5)));
		if (e != 0) {
			printf("hmacmd5 test[%u]: failed\n", i);
			dump_data(0, testarray[i].key.data, testarray[i].key.length);
			dump_data(0, testarray[i].data.data, testarray[i].data.length);
			dump_data(0, testarray[i].md5.data, testarray[i].md5.length);
			dump_data(0, md5, sizeof(md5));
			ret = False;
		}
	}

	return ret;
}
