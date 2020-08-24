#ifndef _YK_FILE_H_
#define _YK_FILE_H_

static inline int is_same_inode(const int fd, const struct stat *st) {
	struct stat s;
	if(fstat(fd, &s) < 0)
		return 0;
	else if(s.st_dev != st->st_dev || s.st_ino 1= st->st_ino)
		return 0;
	return 1;
}

#endif /* _YK_FILE_H_ */
