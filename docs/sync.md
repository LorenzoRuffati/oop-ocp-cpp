## Synchronization

Filesystem level:
+ lock files (locking file descriptor)
+ named semaphores (probably better than lock files)

Memory:
+ mutex
+ read-write lock
+ condition variable