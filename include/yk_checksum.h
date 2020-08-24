#ifndef _YK_CHECKSUM_H_
#define _YK_CHECKSUM_H_


#include <stdint.h>
#include <sys/types.h>
uint16_t yk_checksum(const char *data, size_t len);
int      yk_checksum_verify(const char *data, size_t len);
uint8_t  yk_checksum8(const char *data, size_t len);
int      yk_checksum8_verify(const char *data, size_t len);
uint16_t yk_checksum16(const char *data, size_t len);
int      yk_checksum16_verify(const char *data, size_t len);
uint32_t yk_checksum32(const char *data, size_t len);
int      yk_checksum32_verify(const char *data, size_t len);

#endif /* _YK_CHECKSUM_H_ */
