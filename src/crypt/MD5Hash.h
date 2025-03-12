/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #pragma once

 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #include <stdint.h>

 /**
  * The MD5 functions calculate a 128-bit cryptographic digest for any number of input bytes.
  */
 #define ESP_ROM_MD5_DIGEST_LEN 16
 

 /**
  * @brief Type defined for MD5 context
  *
  */
 typedef struct MD5Context {
     uint32_t buf[4];
     uint32_t bits[2];
     uint8_t in[64];
 } md5_context_t;
 
 /**
  * @brief Initialize the MD5 context
  *
  * @param context Context object allocated by user
  */
 void md5_init(md5_context_t *context);
 
 /**
  * @brief Running MD5 algorithm over input data
  *
  * @param context MD5 context which has been initialized by `MD5Init`
  * @param buf Input buffer
  * @param len Buffer length in bytes
  */
 void md5_update(md5_context_t *context, const void *buf, uint32_t len);
 
 /**
  * @brief Extract the MD5 result, and erase the context
  *
  * @param digest Where to store the 128-bit digest value
  * @param context MD5 context
  */
 void md5_final(uint8_t *digest, md5_context_t *context);
 

 #ifdef __cplusplus
 }
 #endif