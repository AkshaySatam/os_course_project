#ifndef _FILEDESCRIPTOR_H_
#define _FILEDESCRIPTOR_H_

#include <sys/defs.h>
#include <sys/cs.h>
#include <sys/memoryallocator.h>
#include <sys/processLoader.h>


int open(const char *pathname, int flags);
int close(int fd);
ssize_t fileRead(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

#endif
