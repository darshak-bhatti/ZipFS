/*
Darshak Harisinh Bhatti : dbhatti
References :-   hello.c
                https://engineering.facile.it/blog/eng/write-filesystem-fuse/
                https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/unclear.html
                https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/callbacks.html
                https://github.com/cognusion/fuse-rofs/blob/master/rofs.c
    fuse.h  -   https://github.com/libfuse/libfuse/blob/579c3b03f57856e369fd6db2226b77aba63b59ff/include/fuse.h#L102-L577
    libzip doc- https://libzip.org/documentation
                https://github.com/Ninja3047/Fuse-Zip/

 *     gcc -Wall -g zipfs.c `pkg-config fuse libzip --cflags --libs` -lzip -o zipfs
*/

#define FUSE_USE_VERSION 29

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>

#include <zip.h>

static zip_t* zArchive;
static int num_entries;

/** Get file attributes.
	 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
	 * ignored.	 The 'st_ino' field is ignored except if the 'use_ino'
	 * mount option is given.
*/

static int zipfs_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	(void) fi; // unused

	printf("\n In zipfs_getattr");
	printf("\n \t Path is : %s", path);

    memset(stbuf, 0, sizeof(struct stat));

    // simple root logic
    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        stbuf->st_size = 1;
        return 0;
    }

    zip_stat_t sb;
    zip_stat_init(&sb);

    // getattr could get a path with slash in the end (dir) or without it i.e. file.
    // so we need to check for both type of entries in the archive
    char* slash_appended = malloc(strlen(path) + 1);
    strcpy(slash_appended, path+1);
    slash_appended[strlen(path) - 1] = '/',
    slash_appended[strlen(path)] = 0;

    if( zip_name_locate(zArchive, slash_appended, 0) != -1 ){ // Directory Check
        zip_stat(zArchive, slash_appended, 0, &sb);
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        stbuf->st_size = 0;
        stbuf->st_mtime = sb.mtime;

    } else if ( zip_name_locate(zArchive, path+1, 0) != -1 ){ // File Check
        zip_stat(zArchive, path + 1, 0, &sb);
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = sb.size;
        stbuf->st_mtime = sb.mtime;

    } else { // Invalid
        return -ENOENT;
    }

	return 0;
}

static int zipfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
    int i;

	printf("\n In zipfs_readdir");
	printf("\n Path is : %s", path);

    // Default entries
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

    // traverse through the list and check what all entries have the parent dir as the 'path'
    for (i=0; i < num_entries; i++)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        zip_stat_t sb;
        zip_stat_index(zArchive, i, 0, &sb);

        char* zPath = malloc(strlen(sb.name) + 2);
        *zPath = '/';
        strcpy(zPath + 1, sb.name);

        char* dpath = strdup(zPath); // Because dirname removes everything after directory name

        if (strcmp(path, dirname(dpath)) == 0)
        {
            if (zPath[strlen(zPath) - 1] == '/') {
                zPath[strlen(zPath) - 1] = 0; // getattr has the logic for trailing slash
            }
            zipfs_getattr(zPath, &st, fi);
            char* name = basename(zPath);
            if (filler(buf, name, &st, 0))
                break;
        }

        free(zPath);
        free(dpath);
    }

    return 0;
}

static int zipfs_open(const char *path, struct fuse_file_info *fi)
{
	//(void) fi;
    
    printf("\n In hello_open");
	printf("\n Path is : %s", path);
    printf("\n Flags are : %d", fi);
    
    if(zip_name_locate(zArchive, path+1, 0) == -1){
        return -ENOENT;
    }

    return 0;
}

static int zipfs_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
    int res;

	printf("\n In hello_read");
	printf("\n Path is : %s", path);
    if(zip_name_locate(zArchive, path+1, 0) == -1){
        return -ENOENT;
    }

    zip_stat_t sb;
    zip_stat(zArchive, path+1, 0, &sb);

    zip_file_t* zFile = zip_fopen(zArchive, path + 1, 0);
    char tempBuffer[sb.size];

    memset(tempBuffer, 0, sb.size); // initialize temporary buffer


    // read from file inside zip
    if(zip_fread(zFile, tempBuffer, sb.size) == -1){
        printf("\n zip_fread failed");
        return -ENOENT;
    }

    // check for offset limits
    if(offset > sb.size){
        printf("\n offset larger than the content size");
        res = 0;
        //return -ENOENT;
    }

    // if offset+size is less than the content size we return only content of size(size)
    if(offset+size < sb.size){
        memcpy(buf, tempBuffer+offset, size);
        res = size;
    } else { // else if offset+size is less than the content size we return whatever is left
        memcpy(buf, tempBuffer+offset, sb.size - offset);
        res = sb.size - offset;
    }
    
    zip_fclose(zFile);

    return res;
}

static int zipfs_flush(const char* path, struct fuse_file_info* fi){

    return 0;
}

static int zipfs_write(const char* path, const char *buf, size_t size, off_t offset, struct fuse_file_info* fi){

    return -EROFS;
}

static int zipfs_mknod(const char* path, mode_t mode, dev_t rdev){

    return -EROFS;
}

static int zipfs_rmdir(const char *path){

    return -EROFS;
}

static int zipfs_mkdir(const char *path, mode_t mode){

    return -EROFS;
}

static int zipfs_unlink(const char *path){

    return -EROFS;
}

static int zipfs_symlink(const char *path, const char *lnPath){

    return -EROFS;
}
static int zipfs_rename(const char *path, const char *newPath, unsigned int i){

    return -EROFS;
}



static struct fuse_operations zipfs_ops = {
	//.init       = zipfs_init,
	.getattr	= zipfs_getattr,
	.readdir	= zipfs_readdir,
	.open		= zipfs_open,
	.read		= zipfs_read,
    .flush      = zipfs_flush,
    .write      = zipfs_write,
    .mknod      = zipfs_mknod,
    .rmdir      = zipfs_rmdir,
    .mkdir      = zipfs_mkdir,
    .unlink      = zipfs_unlink,
    .symlink      = zipfs_symlink,
    .rename      = zipfs_rename,
};

// To do Write


int main(int argc, char *argv[])
{
    int err;

    zArchive = zip_open(argv[1], 0, &err);
    if(zArchive == NULL){
        printf("\n  Zip could not be opened ");
        return -1;
    }

    num_entries = zip_get_num_entries(zArchive, 0);
	
	printf("\n Hello World!");

    // because second argument is the zip file and all others belong to fuse
    char* fuseArgv[argc - 1];
    fuseArgv[0] = argv[0];
    for (int i = 1; i < argc - 1; i++){
        fuseArgv[i] = argv[i + 1];
    }
    
	return fuse_main(argc-1, fuseArgv, &zipfs_ops, NULL);
}