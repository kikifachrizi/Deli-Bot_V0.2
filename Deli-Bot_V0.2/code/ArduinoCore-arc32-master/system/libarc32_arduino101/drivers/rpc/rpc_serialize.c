/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>

#include "rpc.h"

/* Include the functions called */
#if defined(CONFIG_QUARK_SE_BLE_CORE)
#include "rpc_functions_to_quark.h"
#elif defined(CONFIG_SOC_QUARK_SE)
#include "rpc_functions_to_ble_core.h"
#elif defined(LINUX_HOST_RUNTIME)
// for the host compilation (to simulate connection to BLE controller)
#include "rpc_functions_to_ble_core.h"
#else
#error "File is compiled but should not"
#endif

/* Build the functions exposed */
/* Define the functions identifiers per signature */
#define FN_SIG_NONE(__fn)                                                          fn_index_##__fn,
#define FN_SIG_S(__fn, __s)                                                        FN_SIG_NONE(__fn)
#define FN_SIG_P(__fn, __type)                                                     FN_SIG_NONE(__fn)
#define FN_SIG_S_B(__fn, __s, __type, __length)                                    FN_SIG_NONE(__fn)
#define FN_SIG_B_B_P(__fn, __type1, __length1, __type2, __length2,__type3)         FN_SIG_NONE(__fn)
#define FN_SIG_S_P(__fn, __s, __type)                                              FN_SIG_NONE(__fn)
#define FN_SIG_S_B_P(__fn, __s, __type, __length, __type_ptr)                      FN_SIG_NONE(__fn)
#define FN_SIG_S_B_B_P(__fn, __s, __type1, __length1, __type2, __length2,__type3)  FN_SIG_NONE(__fn)

/* Build the list of function indexes -> this should match the array at deserialization */
enum { LIST_FN_SIG_NONE fn_none_index_max };
enum { LIST_FN_SIG_S fn_s_index_max };
enum { LIST_FN_SIG_P fn_p_index_max };
enum { LIST_FN_SIG_S_B fn_s_b_index_max };
enum { LIST_FN_SIG_B_B_P fn_b_b_p_index_max };
enum { LIST_FN_SIG_S_P fn_s_p_index_max };
enum { LIST_FN_SIG_S_B_P fn_s_b_p_index_max };
enum { LIST_FN_SIG_S_B_B_P fn_s_b_b_p_index_max };

/* Implement the functions using serialization API */
#undef FN_SIG_NONE
#undef FN_SIG_S
#undef FN_SIG_P
#undef FN_SIG_S_B
#undef FN_SIG_B_B_P
#undef FN_SIG_S_P
#undef FN_SIG_S_B_P
#undef FN_SIG_S_B_B_P

#define FN_SIG_NONE(__fn)                     \
    void __fn(void) {                         \
        rpc_serialize_none(fn_index_##__fn);  \
    }                                         \

#define FN_SIG_S(__fn, __s)                                   \
    void __fn(__s p_s) {                                      \
        rpc_serialize_s(fn_index_##__fn, p_s, sizeof(*p_s));  \
    }                                                         \

#define FN_SIG_P(__fn, __type)                                  \
    void __fn(__type p_priv) {                                  \
        rpc_serialize_p(fn_index_##__fn, p_priv);               \
    }                                                           \

#define FN_SIG_S_B(__fn, __s, __type, __length)                               \
    void __fn(__s p_s, __type p_buf, __length length) {                       \
        rpc_serialize_s_b(fn_index_##__fn, p_s, sizeof(*p_s), p_buf, length); \
    }                                                                         \

#define FN_SIG_B_B_P(__fn, __type1, __length1, __type2, __length2, __type3)                             \
    void __fn(__type1 p_buf1, __length1 length1, __type2 p_buf2, __length2 length2, __type3 p_priv) {   \
        rpc_serialize_b_b_p(fn_index_##__fn, p_buf1, length1, p_buf2, length2, p_priv);            \
    }                                                                                              \

#define FN_SIG_S_P(__fn, __s, __type)                                         \
    void __fn(__s p_s, __type p_priv) {                                       \
        rpc_serialize_s_p(fn_index_##__fn, p_s, sizeof(*p_s), p_priv);        \
    }                                                                         \

#define FN_SIG_S_B_P(__fn, __s, __type, __length, __type_ptr)                                   \
    void __fn(__s p_s, __type p_buf, __length length, __type_ptr p_priv) {                      \
        rpc_serialize_s_b_p(fn_index_##__fn, p_s, sizeof(*p_s), p_buf, length, p_priv);     \
    }

#define FN_SIG_S_B_B_P(__fn, __s, __type1, __length1, __type2, __length2, __type3)                                \
    void __fn(__s p_s, __type1 p_buf1, __length1 length1, __type2 p_buf2, __length2 length2, __type3 p_priv) {    \
        rpc_serialize_s_b_b_p(fn_index_##__fn, p_s, sizeof(*p_s), p_buf1, length1, p_buf2, length2, p_priv);      \
    }                                                                                                             \


/* Build the functions */
LIST_FN_SIG_NONE
LIST_FN_SIG_S
LIST_FN_SIG_P
LIST_FN_SIG_S_B
LIST_FN_SIG_B_B_P
LIST_FN_SIG_S_P
LIST_FN_SIG_S_B_P
LIST_FN_SIG_S_B_B_P

#define SIG_TYPE_SIZE 1
#define FN_INDEX_SIZE 1
#define POINTER_SIZE 4

static void _send(uint8_t *buf, uint16_t length) {
	rpc_transmit_cb(buf, length);
}

static uint16_t encoded_structlen(uint8_t structlen) {
	return 1 + structlen;
}

static uint8_t *serialize_struct(uint8_t *p, const uint8_t *struct_data, uint8_t struct_length) {
	*p++ = struct_length;
	memcpy(p, struct_data, struct_length);
	p += struct_length;
	return p;
}

static uint16_t encoded_buflen(const uint8_t *buf, uint16_t buflen) {
	if (NULL == buf)
		return 1;
	else {
		if (buflen < (1 << 7)) {
			return 1 + buflen;
		}
		else
			return 2 + buflen;
	}
}

static uint8_t *serialize_buf(uint8_t *p, const uint8_t *buf, uint16_t buflen) {
	uint16_t varint;

	if (NULL == buf)
		buflen = 0;

	varint = buflen;

	*p = varint & 0x7F;
	if (varint >= (1 << 7)) {
		*p |= 0x80;
		p++;
		*p = varint >> 7;
	}
	p++;
	memcpy(p, buf, buflen);
	p += buflen;
	return p;
}

static uint8_t *serialize_p(uint8_t *p, uintptr_t priv) {
	*p++ = priv;
	*p++ = (priv >> 8);
	*p++ = (priv >> 16);
	*p++ = (priv >> 24);
	return p;
}

void rpc_serialize_none(uint8_t fn_index) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE;

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_NONE;
	*p   = fn_index;

	_send(buf, length);
}

void rpc_serialize_s(uint8_t fn_index, const void * struct_data, uint8_t struct_length) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + encoded_structlen(struct_length);

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_S;
	*p++ = fn_index;
	p = serialize_struct(p, struct_data, struct_length);

	_send(buf, length);
}


void rpc_serialize_p(uint8_t fn_index, void * p_priv) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;
	uintptr_t priv = (uintptr_t) p_priv;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + POINTER_SIZE;

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_P;
	*p++ = fn_index;
	p = serialize_p(p, priv);

	_send(buf, length);
}

void rpc_serialize_s_b(uint8_t fn_index, const void * struct_data, uint8_t struct_length, const void * vbuf, uint16_t vbuf_length) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + encoded_structlen(struct_length) +
			encoded_buflen(vbuf, vbuf_length);

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_S_B;
	*p++ = fn_index;
	p = serialize_struct(p, struct_data, struct_length);
	p = serialize_buf(p, vbuf, vbuf_length);

	_send(buf, length);
}

void rpc_serialize_b_b_p(uint8_t fn_index, const void * vbuf1, uint16_t vbuf1_length, const void * vbuf2, uint16_t vbuf2_length, void * p_priv) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;
	uintptr_t priv = (uintptr_t) p_priv;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + encoded_buflen(vbuf1, vbuf1_length) +
			encoded_buflen(vbuf2, vbuf2_length) + POINTER_SIZE;

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_B_B_P;
	*p++ = fn_index;
	p = serialize_buf(p, vbuf1, vbuf1_length);
	p = serialize_buf(p, vbuf2, vbuf2_length);
	p = serialize_p(p, priv);

	_send(buf, length);
}

void rpc_serialize_s_p(uint8_t fn_index, const void * struct_data, uint8_t struct_length, void * p_priv) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;
	uintptr_t priv = (uintptr_t) p_priv;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + encoded_structlen(struct_length)
			+ POINTER_SIZE;

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_S_P;
	*p++ = fn_index;
	p = serialize_struct(p, struct_data, struct_length);
	p = serialize_p(p, priv);

	_send(buf, length);
}

void rpc_serialize_s_b_p(uint8_t fn_index, const void * struct_data, uint8_t struct_length,
		const void * vbuf, uint16_t vbuf_length, void * p_priv) {
	uint16_t length;
	uint8_t * buf;
	uint8_t * p;
	uintptr_t priv = (uintptr_t) p_priv;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + encoded_structlen(struct_length) +
			encoded_buflen(vbuf, vbuf_length) + POINTER_SIZE;

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_S_B_P;
	*p++ = fn_index;
	p = serialize_struct(p, struct_data, struct_length);
	p = serialize_buf(p, vbuf, vbuf_length);
	p = serialize_p(p, priv);

	_send(buf, length);
}

void rpc_serialize_s_b_b_p(uint8_t fn_index, const void * struct_data, uint8_t struct_length,
		const void * vbuf1, uint16_t vbuf1_length, const void * vbuf2, uint16_t vbuf2_length, void * p_priv) {

	uint16_t length;
	uint8_t * buf;
	uint8_t * p;
	uintptr_t priv = (uintptr_t) p_priv;

	length = SIG_TYPE_SIZE + FN_INDEX_SIZE + encoded_structlen(struct_length) +
			encoded_buflen(vbuf1, vbuf1_length) +
			encoded_buflen(vbuf2, vbuf2_length) + POINTER_SIZE;

	p = buf = rpc_alloc_cb(length);

	*p++ = SIG_TYPE_S_B_B_P;
	*p++ = fn_index;
	p = serialize_struct(p, struct_data, struct_length);
	p = serialize_buf(p, vbuf1, vbuf1_length);
	p = serialize_buf(p, vbuf2, vbuf2_length);
	p = serialize_p(p, priv);

	_send(buf, length);
}
