ZipFS implementation in C as well as Python

Requirements for C :
libzip-dev package :- $ sudo apt-get install libzip-dev
Fuse 2.9
Compile :- $ gcc -Wall -g zipfs.c `pkg-config fuse libzip --cflags --libs` -lzip -o zipfs
Run :- $ ./zipfs [zip-file.zip] [mount-point]



Requirements for Python :
fusepy :- $ sudo pip install fusepy
All other pakcages are present in VCL
Run :- $ python [zip-file.zip] [mount-point]


References :-
                hello.c
                https://engineering.facile.it/blog/eng/write-filesystem-fuse/
                https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/unclear.html
                https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/callbacks.html
                https://github.com/cognusion/fuse-rofs/blob/master/rofs.c
    fuse.h  -   https://github.com/libfuse/libfuse/blob/579c3b03f57856e369fd6db2226b77aba63b59ff/include/fuse.h#L102-L577
    libzip doc- https://libzip.org/documentation
                https://github.com/Ninja3047/Fuse-Zip/


                https://medium.com/the-python-corner/writing-a-fuse-filesystem-in-python-5e0f2de3a813
                https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/unclear.html
                https://github.com/Ninja3047/Fuse-Zip/
                https://docs.python.org/2/library/zipfile.html
    fusepy :-   https://github.com/fusepy/fusepy
    fusepy-eg:- https://github.com/fusepy/fusepy/tree/master/examples


