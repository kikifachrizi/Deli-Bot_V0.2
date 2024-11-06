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

#ifndef RPC_H_
#define RPC_H_

#include <stdint.h>

/** Identifiers of the signature supported by the RPC */
enum {
	SIG_TYPE_NONE = 1,
	SIG_TYPE_S,
	SIG_TYPE_P,
	SIG_TYPE_S_B,
	SIG_TYPE_B_B_P,
	SIG_TYPE_S_P,
	SIG_TYPE_S_B_P,
	SIG_TYPE_S_B_B_P
};

/**
 * RPC memory allocation function, must be implemented by the user of the RPC.
 *
 * This function is called by the RPC mechanism to allocate a buffer for transmission
 * of a serialized function.  The function should not fail.
 *
 * @param length Length of the buffer to allocate
 *
 * @return Pointer to the allocated buffer, the allocation shall not fail, error must
 * be handled internally
 */
uint8_t * rpc_alloc_cb(uint16_t length);

/**
 * RPC transmission function, must be implemented by the user of the RPC.
 *
 * @param p_buf Pointer to the buffer allocated for transmission by @ref rpc_alloc_cb
 * @param length Length of the buffer to transmit
 */
void rpc_transmit_cb(uint8_t * p_buf, uint16_t length);

/**
 * RPC serialization function to serialize a function that does not require any parameter.
 *
 * @param fn_index Index of the function
 */
void rpc_serialize_none(uint8_t fn_index);

/**
 * RPC serialization function to serialize a function that expects a structure as parameter.
 *
 * @param fn_index Index of the function
 * @param struct_data Pointer to the structure to serialize
 * @param struct_length Length of the structure to serialize
 */
void rpc_serialize_s(uint8_t fn_index, const void * struct_data, uint8_t struct_length);

/**
 * RPC serialization function to serialize a function that expects a structure as parameter.
 *
 * @param fn_index Index of the function
 * @param struct_data Pointer to the structure to serialize
 * @param struct_length Length of the structure to serialize
 * @param p_priv Pointer to serialize
 */
void rpc_serialize_s_p(uint8_t fn_index, const void * struct_data, uint8_t struct_length, void * p_priv);

/**
 * RPC serialization function to serialize a function that expects a pointer as parameter.
 *
 * @param fn_index Index of the function
 * @param p_priv Pointer to serialize
 */
void rpc_serialize_p(uint8_t fn_index, void * p_priv);

/**
 * RPC serialization function to serialize a function that expects a structure
 * and a buffer as parameters.
 *
 * @param fn_index Index of the function
 * @param struct_data Pointer to the structure to serialize
 * @param struct_length Length of the structure to serialize
 * @param vbuf Pointer to the buffer to serialize
 * @param vbuf_length Length of the buffer to serialize
 */
void rpc_serialize_s_b(uint8_t fn_index, const void * struct_data, uint8_t struct_length, const void * vbuf, uint16_t vbuf_length);

/**
 * RPC serialization function to serialize a function that expects a structure
 * and a buffer as parameters.
 *
 * @param fn_index Index of the function
 * @param vbuf1 Pointer to the buffer1 to serialize
 * @param vbuf1_length Length of the buffer1 to serialize
 * @param vbuf2 Pointer to the buffer2 to serialize
 * @param vbuf2_length Length of the buffer2 to serialize
 * @param p_priv Pointer to serialize
 */
void rpc_serialize_b_b_p(uint8_t fn_index, const void * vbuf1, uint16_t vbuf1_length,
		const void * vbuf2, uint16_t vbuf2_length, void * p_priv);

/**
 * RPC serialization function to serialize a function that expects a structure
 * and a buffer as parameters.
 *
 * @param fn_index Index of the function
 * @param struct_data Pointer to the structure to serialize
 * @param struct_length Length of the structure to serialize
 * @param vbuf Pointer to the buffer to serialize
 * @param vbuf_length Length of the buffer to serialize
 * @param p_priv Pointer to serialize
 */
void rpc_serialize_s_b_p(uint8_t fn_index, const void * struct_data, uint8_t struct_length,
		const void * vbuf, uint16_t vbuf_length, void * p_priv);

/**
 * RPC serialization function to serialize a function that expects a structure
 * and a buffer as parameters.
 *
 * @param fn_index Index of the function
 * @param struct_data Pointer to the structure to serialize
 * @param struct_length Length of the structure to serialize
 * @param vbuf1 Pointer to the buffer1 to serialize
 * @param vbuf1_length Length of the buffer1 to serialize
 * @param vbuf2 Pointer to the buffer2 to serialize
 * @param vbuf2_length2 Length of the buffer2 to serialize
 * @param p_priv Pointer to serialize
 */
void rpc_serialize_s_b_b_p(uint8_t fn_index, const void * struct_data, uint8_t struct_length,
		const void * vbuf1, uint16_t vbuf1_length, const void * vbuf2, uint16_t vbuf2_length, void * p_priv);

/** RPC deserialization function, shall be invoked when a buffer is received over the transport interface.
 *
 * @param p_buf Pointer to the received buffer
 * @param length Length of the received buffer
 */
void rpc_deserialize(const uint8_t * p_buf, uint16_t length);

#endif /* RPC_H_*/
