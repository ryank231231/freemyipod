/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: dir.c 13741 2007-06-30 02:08:27Z jethead71 $
 *
 * Copyright (C) 2002 by Björn Stenberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "global.h"
#include "libc/include/errno.h"
#include "libc/include/string.h"
#include "fat.h"
#include "dir.h"
#include "debug.h"
#include "thread.h"

static struct mutex dir_mutex;
static DIR* opendirs;

#ifdef HAVE_HOTSWAP
// release all dir handles on a given volume "by force", to avoid leaks
int release_dirs(int volume)
{
    DIR* prev;
    int closed = 0;
    mutex_lock(&dir_mutex, TIMEOUT_BLOCK);
#ifdef HAVE_MULTIVOLUME
    DIR* d;
    while (opendirs && opendirs->fatdir.file.volume == volume)
    {
        prev = opendirs;
        opendirs = opendirs->next;
        free(prev);
        closed++;
    }
    for (d = opendirs; d; d = d->next)
        while (d->next && d->next->fatdir.file.volume == volume)
        {
            prev = d->next;
            d->next = d->next->next;
            free(prev);
            closed++;
        }
#else
    (void)volume;
    while (opendirs)
    {
        prev = opendirs;
        opendirs = opendirs->next;
        free(prev);
        closed++;
    }
#endif
    mutex_unlock(&dir_mutex);
    return closed; /* return how many we did */
}
#endif /* #ifdef HAVE_HOTSWAP */

DIR* opendir(const char* name)
{
    char namecopy[MAX_PATH];
    char* part;
    char* end;
    struct fat_direntry entry;
    DIR* pdir;
#ifdef HAVE_MULTIVOLUME
    int volume;
#endif

    if ( name[0] != '/' ) {
        DEBUGF("Only absolute paths supported right now");
        return NULL;
    }

    pdir = (DIR*)memalign(0x10, sizeof(DIR));
    if (!pdir)
    {
        DEBUGF("Failed to allocate directory handle");
        return NULL;
    }
    reownalloc(pdir, KERNEL_OWNER(KERNEL_OWNER_DIR_HANDLE));
    pdir->process = current_thread;

#ifdef HAVE_MULTIVOLUME
    /* try to extract a heading volume name, if present */
    volume = strip_volume(name, namecopy);
    pdir->volumecounter = 0;
#else
    strlcpy(namecopy, name, sizeof(namecopy)); /* just copy */
#endif

    if ( fat_opendir(IF_MV2(volume,) &pdir->fatdir, 0, NULL) < 0 ) {
        DEBUGF("Failed opening root dir");
        free(pdir);
        return NULL;
    }

    for ( part = strtok_r(namecopy, "/", &end); part;
          part = strtok_r(NULL, "/", &end)) {
        /* scan dir for name */
        while (1) {
            if ((fat_getnext(&pdir->fatdir,&entry) < 0) ||
                (!entry.name[0])) {
                DEBUGF("Directory not found");
                free(pdir);
                return NULL;
            }
            if ( (entry.attr & FAT_ATTR_DIRECTORY) &&
                 (!strcasecmp(part, entry.name)) ) {
                /* In reality, the parent_dir parameter of fat_opendir seems
                 * useless because it's sole purpose it to have a way to
                 * update the file metadata, but here we are only reading
                 * a directory so there's no need for that kind of stuff.
                 * However, the rmdir function uses a ugly hack to
                 * avoid opening a directory twice when deleting it and thus
                 * needs those information. That's why we pass pdir->fatdir both
                 * as the parent directory and the resulting one (this is safe,
                 * in doubt, check fat_open(dir) code) which will allow this kind of
                 * (ugly) things */
                if ( fat_opendir(IF_MV2(volume,)
                                 &pdir->fatdir,
                                 entry.firstcluster,
                                 &pdir->fatdir) < 0 ) {
                    DEBUGF("Failed opening dir '%s' (%ld)",
                           part, entry.firstcluster);
                    free(pdir);
                    return NULL;
                }
#ifdef HAVE_MULTIVOLUME
                pdir->volumecounter = -1; /* n.a. to subdirs */
#endif
                break;
            }
        }
    }

    mutex_lock(&dir_mutex, TIMEOUT_BLOCK);
    pdir->next = opendirs;
    opendirs = pdir;
    mutex_unlock(&dir_mutex);

    return pdir;
}

void reown_dir(DIR* dir, struct scheduler_thread* owner)
{
    dir->process = owner;
}

int closedir(DIR* dir)
{
    if (!dir) return 0;
    DIR* d;
    mutex_lock(&dir_mutex, TIMEOUT_BLOCK);
    if (opendirs == dir) opendirs = dir->next;
    else
        for (d = opendirs; d; d = d->next)
            if (d->next == dir)
                d->next = dir->next;
    mutex_unlock(&dir_mutex);
    free(dir);
    return 0;
}

int closedir_all_of_process(struct scheduler_thread* process)
{
    DIR* d;
    DIR* prev;
    int closed = 0;
    mutex_lock(&dir_mutex, TIMEOUT_BLOCK);
    while (opendirs && opendirs->process == process)
    {
        prev = opendirs;
        opendirs = opendirs->next;
        free(prev);
        closed++;
    }
    for (d = opendirs; d; d = d->next)
        while (d->next && d->next->process == process)
        {
            prev = d->next;
            d->next = d->next->next;
            free(prev);
            closed++;
        }
    mutex_unlock(&dir_mutex);
    return closed; /* return how many we did */
}

struct dirent* readdir(DIR* dir)
{
    struct fat_direntry entry;
    struct dirent* theent = &(dir->theent);

    if (!dir) return NULL;

#ifdef HAVE_MULTIVOLUME
    /* Volumes (secondary file systems) get inserted into the root directory
        of the first volume, since we have no separate top level. */
    if (dir->volumecounter >= 0 /* on a root dir */
     && dir->volumecounter < NUM_VOLUMES /* in range */
     && dir->fatdir.file.volume == 0) /* at volume 0 */
    {   /* fake special directories, which don't really exist, but
           will get redirected upon opendir() */
        while (++dir->volumecounter < NUM_VOLUMES)
        {
            if (fat_ismounted(dir->volumecounter))
            {
                memset(theent, 0, sizeof(*theent));
                theent->attribute = FAT_ATTR_DIRECTORY | FAT_ATTR_VOLUME;
                snprintf(theent->d_name, sizeof(theent->d_name), 
                         VOL_NAMES, dir->volumecounter);
                return theent;
            }
        }
    }
#endif
    /* normal directory entry fetching follows here */
    if (fat_getnext(&(dir->fatdir),&entry) < 0)
        return NULL;

    if ( !entry.name[0] )
        return NULL;

    strlcpy(theent->d_name, entry.name, sizeof(theent->d_name));
    theent->attribute = entry.attr;
    theent->size = entry.filesize;
    theent->startcluster = entry.firstcluster;
    theent->wrtdate = entry.wrtdate;
    theent->wrttime = entry.wrttime;

    return theent;
}

int mkdir(const char *name)
{
    DIR *dir;
    char namecopy[MAX_PATH];
    char* end;
    char *basename;
    char *parent;
    struct dirent *entry;
    int rc;

    if ( name[0] != '/' ) {
        DEBUGF("mkdir: Only absolute paths supported right now");
        return -1;
    }

    strlcpy(namecopy, name, sizeof(namecopy));

    /* Split the base name and the path */
    end = strrchr(namecopy, '/');
    *end = 0;
    basename = end+1;

    if(namecopy == end) /* Root dir? */
        parent = "/";
    else
        parent = namecopy;
        
    DEBUGF("mkdir: parent: %s, name: %s", parent, basename);

    dir = opendir(parent);
    
    if(!dir) {
        DEBUGF("mkdir: can't open parent dir");
        return -2;
    }    

    if(basename[0] == 0) {
        DEBUGF("mkdir: Empty dir name");
        errno = EINVAL;
        return -3;
    }
    
    /* Now check if the name already exists */
    while ((entry = readdir(dir))) {
        if ( !strcasecmp(basename, entry->d_name) ) {
            DEBUGF("mkdir error: file exists");
            errno = EEXIST;
            closedir(dir);
            return - 4;
        }
    }

    rc = fat_create_dir(basename, &(dir->fatdir));
    closedir(dir);
    
    return rc;
}

int rmdir(const char* name)
{
    int rc;
    DIR* dir;
    struct dirent* entry;
    
    dir = opendir(name);
    if (!dir)
    {
        errno = ENOENT; /* open error */
        return -1;
    }

    /* check if the directory is empty */
    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") &&
            strcmp(entry->d_name, ".."))
        {
            DEBUGF("rmdir error: not empty");
            errno = ENOTEMPTY;
            closedir(dir);
            return -2;
        }
    }

    rc = fat_remove(&(dir->fatdir.file));
    if ( rc < 0 ) {
        DEBUGF("Failed removing dir: %d", rc);
        errno = EIO;
        rc = rc * 10 - 3;
    }

    closedir(dir);
    return rc;
}

#ifdef HAVE_MULTIVOLUME
/* returns on which volume this is, and copies the reduced name
   (sortof a preprocessor for volume-decorated pathnames) */
int strip_volume(const char* name, char* namecopy)
{
    int volume = 0;
    const char *temp = name;

    while (*temp == '/')          /* skip all leading slashes */
        ++temp;

    if (*temp && !strncmp(temp, VOL_NAMES, VOL_ENUM_POS))
    {
        temp += VOL_ENUM_POS;     /* behind special name */
        volume = atoi(temp);      /* number is following */
        temp = strchr(temp, '/'); /* search for slash behind */
        if (temp != NULL)
            name = temp;          /* use the part behind the volume */
        else
            name = "/";           /* else this must be the root dir */
    }

    strlcpy(namecopy, name, MAX_PATH);

    return volume;
}
#endif /* #ifdef HAVE_MULTIVOLUME */
