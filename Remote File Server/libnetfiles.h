#define INVALID_FILE_MODE 666
#define PORT_NO "42069"

int netserverinit(char * hostname, int mode);

ssize_t netwrite(int fildes, const void *buf, size_t nbytes);

ssize_t netread(int fildes, void * buf, size_t nbytes);

int netopen(const char * pathname, int flags);

int netclose(int fd);
