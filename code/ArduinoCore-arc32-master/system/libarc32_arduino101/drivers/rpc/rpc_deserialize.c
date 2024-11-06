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

#include <stdlib.h>
#include <string.h>

#include "rpc.h"

extern void panic(int err);

/* Include the functions offered */
#if defined(CONFIG_QUARK_SE_BLE_CORE)
#include "rpc_functions_to_ble_core.h"
#elif defined(CONFIG_SOC_QUARK_SE)
#include "rpc_functions_to_quark.h"
#elif defined(LINUX_HOST_RUNTIME)
// for the host compilation (to simulate connection to BLE controller)
#include "rpc_functions_to_ble_core.h"
#else
#error "File is compiled but should not"
#endif

/* Build the list of prototypes and check that list are made only of matching signatures */
#define FN_SIG_NONE(__fn) void __fn(void);
LIST_FN_SIG_NONE
#undef FN_SIG_NONE

#define FN_SIG_S(__fn, __s) void __fn(__s p_s);
LIST_FN_SIG_S
#undef FN_SIG_S

#define FN_SIG_P(__fn, __type) void __fn(__type p_priv);
LIST_FN_SIG_P
#undef FN_SIG_P

#define FN_SIG_S_B(__fn, __s, __type, __length) void __fn(__s p_s, __type p_buf, __length length);
LIST_FN_SIG_S_B
#undef FN_SIG_S_B

#define FN_SIG_B_B_P(__fn, __type1, __length1, __type2, __length2, __type3) \
	void __fn(__type1 p_buf1, __length1 length1, __type2 p_buf2, __length2 length2, __type3 p_priv);
LIST_FN_SIG_B_B_P
#undef FN_SIG_B_B_P

#define FN_SIG_S_P(__fn, __s, __type) void __fn(__s p_s, __type p_priv);
LIST_FN_SIG_S_P
#undef FN_SIG_S_P

#define FN_SIG_S_B_P(__fn, __s,  __type, __length, __type_ptr) \
	void __fn(__s p_s, __type p_buf, __length length, __type_ptr p_priv);
LIST_FN_SIG_S_B_P
#undef FN_SIG_S_B_P

#define FN_SIG_S_B_B_P(__fn, __s,  __type1, __length1, __type2, __length2, __type_ptr) \
	void __fn(__s p_s, __type1 p_buf1, __length1 length1, __type2 p_buf2, __length2 length2, __type_ptr p_priv);
LIST_FN_SIG_S_B_B_P
#undef FN_SIG_S_B_B_P



/* 1 - define the size check arrays */
#define FN_SIG_NONE(__fn)

#define FN_SIG_S(__fn, __s) sizeof(*((__s)0)),

#define FN_SIG_P(__fn, __type)

#define FN_SIG_S_B(__fn, __s, __type, __length) sizeof(*((__s)0)),

#define FN_SIG_B_B_P(__fn, __type1, __length1, __type2, __length2, __type3) sizeof(*((__s)0)),

#define FN_SIG_S_P(__fn, __s, __type) sizeof(*((__s)0)),

#define FN_SIG_S_B_P(__fn, __s, __type, __length, __type_ptr) sizeof(*((__s)0)),

#define FN_SIG_S_B_B_P(__fn, __s, __type1, __length1, __type2, __length2, __type3) sizeof(*((__s)0)),

static uint8_t m_size_s[] = { LIST_FN_SIG_S };
static uint8_t m_size_s_b[] = { LIST_FN_SIG_S_B };
static uint8_t m_size_s_p[] = { LIST_FN_SIG_S_P };
static uint8_t m_size_s_b_p[] = { LIST_FN_SIG_S_B_P };
static uint8_t m_size_s_b_b_p[] = { LIST_FN_SIG_S_B_B_P };

#undef FN_SIG_NONE
#undef FN_SIG_S
#undef FN_SIG_P
#undef FN_SIG_S_B
#undef FN_SIG_B_B_P
#undef FN_SIG_S_P
#undef FN_SIG_S_B_P
#undef FN_SIG_S_B_B_P

/* 2- build the enumerations list */
#define FN_SIG_NONE(__fn)                                                          fn_index_##__fn,
#define FN_SIG_S(__fn, __s)                                                        FN_SIG_NONE(__fn)
#define FN_SIG_P(__fn, __type)                                                     FN_SIG_NONE(__fn)
#define FN_SIG_S_B(__fn, __s, __type, __length)                                    FN_SIG_NONE(__fn)
#define FN_SIG_B_B_P(__fn, __type1, __length1, __type2, __length2, __type3)        FN_SIG_NONE(__fn)
#define FN_SIG_S_P(__fn, __s, __type)                                              FN_SIG_NONE(__fn)
#define FN_SIG_S_B_P(__fn, __s, __type, __length, __type_ptr)                      FN_SIG_NONE(__fn)
#define FN_SIG_S_B_B_P(__fn, __s, __type1, __length1, __type2, __length2, __type3) FN_SIG_NONE(__fn)

/* Build the list of function indexes in the deserialization array */
enum { LIST_FN_SIG_NONE fn_none_index_max };
enum { LIST_FN_SIG_S fn_s_index_max };
enum { LIST_FN_SIG_P fn_p_index_max };
enum { LIST_FN_SIG_S_B fn_s_b_index_max };
enum { LIST_FN_SIG_B_B_P fn_b_b_p_index_max };
enum { LIST_FN_SIG_S_P fn_s_p_index_max };
enum { LIST_FN_SIG_S_B_P fn_s_b_p_index_max };
enum { LIST_FN_SIG_S_B_B_P fn_s_b_b_p_index_max };

#undef FN_SIG_NONE
#undef FN_SIG_S
#undef FN_SIG_P
#undef FN_SIG_S_B
#undef FN_SIG_B_B_P
#undef FN_SIG_S_P
#undef FN_SIG_S_B_P
#undef FN_SIG_S_B_B_P

/* 3- build the array */
#define FN_SIG_NONE(__fn)                                                             [fn_index_##__fn] = (void*)__fn,
#define FN_SIG_S(__fn, __s)                                                           FN_SIG_NONE(__fn)
#define FN_SIG_P(__fn, __type)                                                        FN_SIG_NONE(__fn)
#define FN_SIG_S_B(__fn, __s, __type, __length)                                       FN_SIG_NONE(__fn)
#define FN_SIG_B_B_P(__fn, __type1, __length1, __type2, __length2, __type3)           FN_SIG_NONE(__fn)
#define FN_SIG_S_P(__fn, __s, __type)                                                 FN_SIG_NONE(__fn)
#define FN_SIG_S_B_P(__fn, __s, __type, __length, __type_ptr)                         FN_SIG_NONE(__fn)
#define FN_SIG_S_B_B_P(__fn, __s, __type1, __length1, __type2, __length2, __type3)    FN_SIG_NONE(__fn)

static void (*m_fct_none[])(void) = { LIST_FN_SIG_NONE };
static void (*m_fct_s[])(void * structure) = { LIST_FN_SIG_S };
static void (*m_fct_p[])(void * pointer) = { LIST_FN_SIG_P };
static void (*m_fct_s_b[])(void * structure, void * buffer, uint8_t length) = { LIST_FN_SIG_S_B };
static void (*m_fct_b_b_p[])(void * buffer1, uint8_t length1, void * buffer2, uint8_t length2, void * pointer) = { LIST_FN_SIG_B_B_P };
static void (*m_fct_s_p[])(void * structure, void * pointer) = { LIST_FN_SIG_S_P };
static void (*m_fct_s_b_p[])(void * structure, void * buffer, uint8_t length, void * pointer) = { LIST_FN_SIG_S_B_P };
static void (*m_fct_s_b_b_p[])(void * structure, void * buffer1, uint8_t length1, void * buffer2, uint8_t length2, void * pointer) = { LIST_FN_SIG_S_B_B_P };

static const uint8_t * deserialize_struct(const uint8_t *p, const uint8_t **pp_struct, uint8_t *p_struct_length) {
	uint8_t struct_length;

	struct_length = *p++;
	*pp_struct = p;
	*p_struct_length = struct_length;

	return p + struct_length;
}

static const uint8_t * deserialize_buf(const uint8_t *p, const uint8_t **pp_buf, uint16_t *p_buflen) {
	uint8_t b;
	uint16_t buflen;

	/* Get the current byte */
	b = *p++;
	buflen = b & 0x7F;
	if (b & 0x80) {
		/* Get the current byte */
		b = *p++;
		buflen += (uint16_t)b << 7;
	}

	/* Return the values */
	*pp_buf = p;
	*p_buflen = buflen;
	p += buflen;
	return p;
}

static void deserialize_none(uint8_t fn_index, const uint8_t * p_buf, uint16_t length) {
	(void)p_buf;
	if (length != 0)
		panic(-1);
	m_fct_none[fn_index]();
}

static void deserialize_s(uint8_t fn_index, const uint8_t * p_buf, uint16_t length) {
	const uint8_t *p_struct_data;
	uint8_t struct_length;
	const uint8_t *p;

	p = deserialize_struct(p_buf, &p_struct_data, &struct_length);

	if ((length != (p - p_buf)) ||
	   (struct_length != m_size_s[fn_index]))
		panic(-1);

	{
		/* Always align structures on word boundary */
		uintptr_t struct_data[(struct_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];

		memcpy(struct_data, p_struct_data, struct_length);

		m_fct_s[fn_index](struct_data);
	}
}

static void deserialize_p(uint8_t fn_index, const uint8_t * p_buf, uint16_t length) {
	uintptr_t p_priv;

	if (length != 4)
		panic(-1);

	/* little endian conversion */
	p_priv = p_buf[0] | (p_buf[1] << 8) | (p_buf[2] << 16) | (p_buf[3] << 24);

	m_fct_p[fn_index]((void *)p_priv);
}

static void deserialize_s_b(uint8_t fn_index, const uint8_t * p_buf, uint16_t length) {
	const uint8_t *p_struct_data;
	uint8_t struct_length;
	const uint8_t *p_vbuf;
	uint16_t vbuf_length;
	const uint8_t *p;

	p = deserialize_struct(p_buf, &p_struct_data, &struct_length);
	p = deserialize_buf(p, &p_vbuf, &vbuf_length);

	if ((length != (p - p_buf)) ||
	    (struct_length != m_size_s_b[fn_index]))
		panic(-1);

	{
		/* Always align structures on word boundary */
		uintptr_t struct_data[(struct_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		uintptr_t vbuf[(vbuf_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		void * buf = NULL;

		memcpy(struct_data, p_struct_data, struct_length);

		if (vbuf_length) {
			memcpy(vbuf, p_vbuf, vbuf_length);
			buf = vbuf;
		}

		m_fct_s_b[fn_index](struct_data, buf, vbuf_length);
	}
}

static void deserialize_b_b_p(uint8_t fn_index, const uint8_t * p_buf, uint16_t length) {
	const uint8_t *p_vbuf1;
	uint16_t vbuf1_length;
	const uint8_t *p_vbuf2;
	uint16_t vbuf2_length;
	uintptr_t p_priv;
	const uint8_t *p;

	p = deserialize_buf(p_buf, &p_vbuf1, &vbuf1_length);
	p = deserialize_buf(p, &p_vbuf2, &vbuf2_length);
	p += 4;

	if (length != (p - p_buf))
		panic(-1);

	{
		/* Always align structures on word boundary */
		uintptr_t vbuf1[(vbuf1_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		uintptr_t vbuf2[(vbuf2_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		void * buf1 = NULL;
		void * buf2 = NULL;

		if (vbuf1_length) {
			memcpy(vbuf1, p_vbuf1, vbuf1_length);
			buf1 = vbuf1;
		}

		if (vbuf2_length) {
			memcpy(vbuf2, p_vbuf2, vbuf2_length);
			buf2 = vbuf2;
		}
		p = p_vbuf2 + vbuf2_length;

		/* little endian conversion */
		p_priv = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);

		m_fct_b_b_p[fn_index](buf1, vbuf1_length, buf2, vbuf2_length, (void *)p_priv);
	}
}

static void deserialize_s_p(uint8_t fn_index, const uint8_t * p_buf, uint16_t length)
{
	const uint8_t *p_struct_data;
	uint8_t struct_length;
	uintptr_t p_priv;
	const uint8_t *p;

	p = deserialize_struct(p_buf, &p_struct_data, &struct_length);
	p += 4;

	if ((length != (p - p_buf)) ||
	    (struct_length != m_size_s_p[fn_index]))
		panic(-1);

	{
		/* Always align structures on word boundary */
		uintptr_t struct_data[(struct_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];

		memcpy(struct_data, p_struct_data, struct_length);
		p = p_struct_data + struct_length;

		/* little endian conversion */
		p_priv = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);

		m_fct_s_p[fn_index](struct_data, (void *)p_priv);
	}
}

static void deserialize_s_b_p(uint8_t fn_index, const uint8_t * p_buf, uint16_t length)
{
	const uint8_t *p_struct_data;
	uint8_t struct_length;
	const uint8_t *p_vbuf;
	uint16_t vbuf_length;
	uintptr_t p_priv;
	const uint8_t *p;

	p = deserialize_struct(p_buf, &p_struct_data, &struct_length);
	p = deserialize_buf(p, &p_vbuf, &vbuf_length);
	p += 4;

	if ((length != (p - p_buf)) ||
	    (struct_length != m_size_s_b_p[fn_index]))
		panic(-1);

	{
		/* Always align structures on word boundary */
		uintptr_t struct_data[(struct_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		uintptr_t vbuf[(vbuf_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		void * buf = NULL;

		memcpy(struct_data, p_struct_data, struct_length);

		if (vbuf_length) {
			memcpy(vbuf, p_vbuf, vbuf_length);
			buf = vbuf;
		}
		p = p_vbuf + vbuf_length;

		/* little endian conversion */
		p_priv = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);

		m_fct_s_b_p[fn_index](struct_data, buf, vbuf_length, (void *)p_priv);
	}
}

static void deserialize_s_b_b_p(uint8_t fn_index, const uint8_t * p_buf, uint16_t length) {
	const uint8_t *p_struct_data;
	uint8_t struct_length;
	const uint8_t *p_vbuf1;
	uint16_t vbuf1_length;
	const uint8_t *p_vbuf2;
	uint16_t vbuf2_length;
	uintptr_t p_priv;
	const uint8_t *p;

	p = deserialize_struct(p_buf, &p_struct_data, &struct_length);
	p = deserialize_buf(p, &p_vbuf1, &vbuf1_length);
	p = deserialize_buf(p, &p_vbuf2, &vbuf2_length);
	p += 4;
	if ((length != (p - p_buf)) ||
		 (struct_length != m_size_s_b_b_p[fn_index]))
		panic(-1);

	{
		/* Always align structures on word boundary */
		uintptr_t struct_data[(struct_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		uintptr_t vbuf1[(vbuf1_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		uintptr_t vbuf2[(vbuf2_length + (sizeof(uintptr_t) - 1))/(sizeof(uintptr_t))];
		void * buf1 = NULL;
		void * buf2 = NULL;

		memcpy(struct_data, p_struct_data, struct_length);

		if (vbuf1_length) {
			memcpy(vbuf1, p_vbuf1, vbuf1_length);
			buf1 = vbuf1;
		}
		if (vbuf2_length) {
			memcpy(vbuf2, p_vbuf2, vbuf2_length);
			buf2 = vbuf2;
		}

		p = p_vbuf2 + vbuf2_length;

		/* little endian conversion */
		p_priv = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);

		m_fct_s_b_b_p[fn_index](struct_data, buf1, vbuf1_length, buf2, vbuf2_length, (void *)p_priv);
	}
}

void rpc_deserialize(const uint8_t * p_buf, uint16_t length) {

	uint8_t fn_index;
	uint8_t sig_type;

	if (NULL != p_buf) {
		sig_type = p_buf[0];
		fn_index = p_buf[1];

		p_buf += 2;
		length -= 2;

		switch(sig_type) {
		case SIG_TYPE_NONE:
			if (sizeof(m_fct_none))
				deserialize_none(fn_index, p_buf, length);
			break;
		case SIG_TYPE_S:
			if (sizeof(m_fct_s))
				deserialize_s(fn_index, p_buf, length);
			break;
		case SIG_TYPE_P:
			if (sizeof(m_fct_p))
				deserialize_p(fn_index, p_buf, length);
			break;
		case SIG_TYPE_S_B:
			if (sizeof(m_fct_s_b))
				deserialize_s_b(fn_index, p_buf, length);
			break;
		case SIG_TYPE_B_B_P:
			if (sizeof(m_fct_b_b_p))
				deserialize_b_b_p(fn_index, p_buf, length);
			break;
		case SIG_TYPE_S_P:
			if (sizeof(m_fct_s_p))
				deserialize_s_p(fn_index, p_buf, length);
			break;
		case SIG_TYPE_S_B_P:
			if (sizeof(m_fct_s_b_p))
				deserialize_s_b_p(fn_index, p_buf, length);
			break;
		case SIG_TYPE_S_B_B_P:
			if (sizeof(m_fct_s_b_b_p))
				deserialize_s_b_b_p(fn_index, p_buf, length);
			break;
		default:
			panic(-1);
			break;
		}
	}
}
