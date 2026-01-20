/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #ifndef __RANDOM_H__
 #define __RANDOM_H__
 
 #include <stddef.h>
 #include <stdint.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif

 int get_entropy(uint32_t*, size_t);
 
 /**
  * @brief  Get one random 32-bit word
  *
  * @return Random value between 0 and UINT32_MAX
  */
 uint32_t get_random(void);
 
 /**
  * @brief Fill a buffer with random bytes from hardware RNG
  *
  * @note This function is implemented via calls to get_random(), so the same constraints apply.
  *
  * @param buf Pointer to buffer to fill with random numbers.
  * @param len Length of buffer in bytes
  */
 void fill_random(void *buf, size_t len);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* _RANDOM_H__ */