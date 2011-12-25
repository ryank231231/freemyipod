//
//
//    Copyright 2010 TheSeven
//
//
//    This file is part of emCORE.
//
//    emCORE is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emCORE is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emCORE.  If not, see <http://www.gnu.org/licenses/>.
//
//


#include "global.h"
#include "console.h"
#include "execimage.h"
#include "mmu.h"
#include "malloc.h"


struct scheduler_thread* execimage(void* image, bool copy, int argc, const char** argv)
{
    int i;
    struct emcoreapp_header* header = (struct emcoreapp_header*)image;
    if (memcmp(header, "emCOexec", 8))
    {
        cprintf(CONSOLE_BOOT, "execimage: Bad signature: "
                              "%02X %02X %02X %02X %02X %02X %02X %02X\n",
                header->signature[0], header->signature[1], header->signature[2],
                header->signature[3], header->signature[4], header->signature[5],
                header->signature[6], header->signature[7]);
        if (!copy) free(image);
        return NULL;
    }
    if (header->version != EMCOREAPP_HEADER_VERSION)
    {
        cprintf(CONSOLE_BOOT, "execimage: Unsupported version! (%08X)\n", header->version);
        if (!copy) free(image);
        return NULL;
    }
    off_t offset = header->textstart;
    size_t textsize = header->textsize;
    size_t bsssize = header->bsssize;
    size_t stacksize = header->stacksize;
	off_t entrypoint = header->entrypoint;
	off_t relocstart = header->relocstart - offset;
	uint32_t reloccount = header->reloccount;
	bool compressed = header->flags & EMCOREAPP_FLAG_COMPRESSED;
	bool lib = header->flags & EMCOREAPP_FLAG_LIBRARY;
    uint32_t argsize = 0;
    if (argv)
        for (i = 0; i < argc; i++)
            argsize += 5 + strlen(argv[i]);
    else argc = 0;
    size_t finalsize;
    if (lib) finalsize = textsize + bsssize;
    else finalsize = textsize + bsssize + argsize + stacksize;
    size_t datasize = relocstart + reloccount * 4;
    size_t tempsize = MAX(finalsize, datasize);
    if (compressed)
    {
        void* alloc = memalign(CACHEALIGN_SIZE, tempsize);
        if (!alloc)
        {
            cprintf(CONSOLE_BOOT, "execimage: Out of memory!\n");
            if (!copy) free(image);
            return NULL;
        }
        uint32_t decompsize;
        ucl_decompress(image + offset, datasize, alloc, &decompsize);
        if (!copy) free(image);
        if (datasize != decompsize)
        {
            cprintf(CONSOLE_BOOT, "execimage: Decompression failed!\n");
            free(alloc);
            return NULL;
        }
        image = alloc;
    }
    else if (copy || (((uint32_t)image) & (CACHEALIGN_SIZE - 1)))
    {
        void* alloc = memalign(CACHEALIGN_SIZE, tempsize);
        if (!alloc)
        {
            cprintf(CONSOLE_BOOT, "execimage: Out of memory!\n");
            if (!copy) free(image);
            return NULL;
        }
        memcpy(alloc, image + offset, datasize);
        if (!copy) free(image);
        image = alloc;
    }
    else
    {
        memcpy(image, image + offset, datasize);
        void* newimage = realign(image, CACHEALIGN_SIZE, tempsize);
        if (!newimage)
        {
            free(image);
            cprintf(CONSOLE_BOOT, "execimage: Out of memory!\n");
            return NULL;
        }
        else image = newimage;
    }
    for (; reloccount; reloccount--, relocstart += 4)
    {
        off_t reloc = *((off_t*)(image + relocstart));
        uint32_t data = *((uint32_t*)(image + reloc));
        *((void**)(image + reloc)) = image + data;
    }
    if (tempsize != finalsize) realloc(image, finalsize); /* Can only shrink => safe */
    void* ptr = image + textsize;
    memset(ptr, 0, bsssize);
    ptr += bsssize;
    if (argv)
    {
        memcpy(image + textsize + bsssize, argv, argc * 4);
        ptr += argc * 4;
        for (i = 0; i < argc; i++)
        {
            uint32_t len = strlen(argv[i]);
            memcpy(ptr, argv[i], len);
            ptr += len;
        }
    }
    clean_dcache();
    invalidate_icache();
    struct scheduler_thread* thread;
    if (lib)
        thread = (struct scheduler_thread*)library_register(image, image + entrypoint, argc, argv);
    else
    {
        thread = thread_create(NULL, NULL, image + entrypoint, ptr, stacksize,
                               USER_THREAD, 127, false, (void*)argc, argv, NULL, NULL);
        if (thread)
        {
            reownalloc(image, OWNER_TYPE(OWNER_THREAD, thread));
            thread_resume(thread);
        }
        else free(image);
    }
    return thread;
}
