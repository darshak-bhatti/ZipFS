'''
Darshak Harisinh Bhatti : dbhatti
References :-   hello.c
                https://medium.com/the-python-corner/writing-a-fuse-filesystem-in-python-5e0f2de3a813
                https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/unclear.html
                https://github.com/Ninja3047/Fuse-Zip/
                https://docs.python.org/2/library/zipfile.html
    fusepy :-   https://github.com/fusepy/fusepy
    fusepy-eg:- https://github.com/fusepy/fusepy/tree/master/examples


Packages Required:-
fusepy
os
sys
zipfile
datetime
errno
time
fuse
'''


from __future__ import with_statement

import os
import sys
import zipfile
import datetime

from collections import defaultdict
from errno import *
from stat import S_IFDIR, S_IFLNK, S_IFREG
from time import time


from fuse import FUSE, FuseOSError, Operations


class zipfs(Operations):
    
    def __init__(self, root):
        print 'zip is : ', root
        self.files = {}
        self.epoch = datetime.datetime.utcfromtimestamp(0)
        #self.root = root
        if zipfile.is_zipfile(root):
            self.zArchive = zipfile.ZipFile(root, 'r')
        else:
            print "Invalid zip"
            exit(1)


    #### HELPERS #####
    def unix_time_millis(self, x):
        dt = datetime.datetime(year=x[0], month=x[1], day=x[2], hour=x[3], minute=x[4], second=x[5])
        return (dt - self.epoch).total_seconds() 
        
    ##################

    def getattr(self, path, fh=None):
        print "get attr path is : ", path
        if path == "/": # simple root condition
            self.files['/'] = dict(st_mode=(S_IFDIR | 0o755), st_nlink=2, st_size=1)
            return self.files['/']    

        pathWoSlash = path[1:]  # remove the initial slash 
        try:
            zInfo = self.zArchive.getinfo(pathWoSlash)  # Check for file
            self.files[pathWoSlash] = dict(st_mode=(S_IFREG | 0o777), st_nlink=1, st_size=zInfo.file_size,st_mtime=self.unix_time_millis(zInfo.date_time))
            return self.files[pathWoSlash]
        except KeyError:
            try:
                zInfo = self.zArchive.getinfo(pathWoSlash + '/')  # Check for directory
                self.files[pathWoSlash + '/'] = dict(st_mode=(S_IFDIR | 0o755), st_nlink=2, st_size=zInfo.file_size, st_mtime=self.unix_time_millis(zInfo.date_time))
                return self.files[pathWoSlash + '/']
            except KeyError:
                print "Except 2" # Doest not exist retrun error
                raise FuseOSError(ENOENT)
                return -ENOENT;
        
        raise FuseOSError(ENOENT)
        return -ENOENT

    def readdir(self, path, fh):
        dirents = ['.', '..']
        # traverse through the list and check what all entries have the parent dir as the 'path'
        for i in self.zArchive.namelist():
            print "READDIR LOOP 1 -- i is :", i
            if i[-1] == '/':
                i = i[:-1]
            print "os dirname is : ",os.path.dirname('/' + i)
            if os.path.dirname('/' + i) == path:
                if os.path.basename(i) != '':
                    dirents.append(os.path.basename(i)) # append to dirent if entry is not empty

        print "Dirents are"
        print dirents
        for i in dirents:
            yield i


    def open(self, path, flags):
        if path[1:] not in self.zArchive.namelist():
            raise FuseOSError(ENOENT)
            return -ENOENT;

        return 0


    def read(self, path, length, offset, fh):
        # check if path exists in the archive or not
        if path[1:] in self.zArchive.namelist() and path[-1] != '/':
            read_res = self.zArchive.read(path[1:])
            # offset greater than length can not be tolerated
            if offset > len(read_res):
                return_res = 0
            if offset + length < len(read_res):
                return_res = read_res[offset:offset+length]
            else:
                return_res = read_res[offset:]
        else:
            # does not exist; return error
            raise FuseOSError(ENOENT)
            return -ENOENT;
            
        return return_res

    
    def flush(self, path, fh):
        return 0

    # Graceful Failures
    def write(self, path, buf, offset, fh):
        raise FuseOSError(EROFS)
        return -EROFS;

    def truncate(self, path, length, fh=None):
        raise FuseOSError(EROFS)
        return -EROFS;

    def create(self, path, mode, fi=None):
        raise FuseOSError(EROFS)
        return -EROFS;

    def mkdir(self, path, mode):
        raise FuseOSError(EROFS)
        return -EROFS;

    def unlink(self, path):
        raise FuseOSError(EROFS)
        return -EROFS;

    def rename(self, old, new):
        raise FuseOSError(EROFS)
        return -EROFS;

    def rmdir(self, path):
        raise FuseOSError(EROFS)
        return -EROFS;


def main(mountpoint, root):
    FUSE(zipfs(root), mountpoint, nothreads=True, foreground=True)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage zipfs.py [zip-file.zip] [mount-point]"
        exit(1)
    main(sys.argv[2], sys.argv[1])
