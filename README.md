# ZipFS implementation in C as well as Python

#### Requirements for C :
**libzip-dev package :-** ``` $ sudo apt-get install libzip-dev ``` <br/>
**Fuse 2.9** <br/>
**Compile :-** ``` $  gcc -Wall -g zipfs.c `pkg-config fuse libzip --cflags --libs` -lzip -o zipfs ``` <br/>
**Run :-** ``` $ ./zipfs [zip-file.zip] [mount-point] ``` <br/>



#### Requirements for Python :
**fusepy :-** ```$ sudo pip install fusepy``` <br/>
**Run :-** ```$ python [zip-file.zip] [mount-point]``` <br/>


#### References :
<br/>
[FUSE Tutorial - Write a filesystem with FUSE](https://engineering.facile.it/blog/eng/write-filesystem-fuse/) <br/>
[FUSE Tutorial - Uncleaer FUSE Functions](https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/unclear.html)<br/>
[FUSE Callbacks](https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/callbacks.html)<br/>
[Read-Only Filesystem](https://github.com/cognusion/fuse-rofs/blob/master/rofs.c)<br/>
[fuse.h](https://github.com/libfuse/libfuse/blob/579c3b03f57856e369fd6db2226b77aba63b59ff/include/fuse.h#L102-L577) <br/>
[libzip doc](https://libzip.org/documentation) <br/>
[Fuse-Zip](https://github.com/Ninja3047/Fuse-Zip/)
[Writing a FUSE Filesystem in Python](https://medium.com/the-python-corner/writing-a-fuse-filesystem-in-python-5e0f2de3a813)<br/>
[Python Zipfile Docs](https://docs.python.org/2/library/zipfile.html) <br/>
[fusepy](https://github.com/fusepy/fusepy) <br/>
[fusepy-examples](https://github.com/fusepy/fusepy/tree/master/examples)
