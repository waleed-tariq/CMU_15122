15-122 Principles of Imperative Computation
Generic Queues

==========================================================

Files you won't change
   queue.h       - C interface of queues

Code to fill in:
   queue.c1      - Implementation of generic queues
   queue-test.c1 - Test functions for C1 queues (the contents are optional, but highly recommended)

Files that don't exist yet:
   queue.c       - Implementation of generic queues in C
   queue-use.c1  - Functions that can be passed to queue_all or queue_iterate
   queue-test.c  - Test functions for C queues (you must supply the file, but contents are optional, highly recommended)

==========================================================

Compiling C0 code with contracts:
   % cc0 -d queue.c1 queue-use.c1 queue-test.c1
   % ./a.out

Compiling C code with contracts:
   % gcc -DDEBUG -g -Wall -Wextra -Werror -Wshadow -std=c99 -pedantic lib/*.c queue.c queue-test.c
   % ./a.out

Compiling C code without contracts:
   % gcc -g -Wall -Wextra -Werror -Wshadow -std=c99 -pedantic lib/*.c queue.c queue-test.c

==========================================================

Submitting with Andrew handin script:
   % handin queues queue.c1 queue-use.c1 queue-test.c1 queue.c queue-test.c
OR
   % handin queues queue.c1 queue-use.c1 queue.c

Creating a tarball to submit with autolab.andrew.cmu.edu web interface:
   % tar -czvf handin.tgz queue.c1 queue-use.c1 queue-test.c1 queue.c queue-test.c
OR
   % tar -czvf handin.tgz queue.c1 queue-use.c1 queue.c
