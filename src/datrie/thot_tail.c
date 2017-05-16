/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * libdatrie - Double-Array Trie Library
 * Copyright (C) 2006  Theppitak Karoonboonyanan <theppitak@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * tail.c - trie tail for keeping suffixes
 * Created: 2006-08-15
 * Author:  Theppitak Karoonboonyanan <theppitak@gmail.com>
 */

#include <string.h>
#include <stdlib.h>
#ifndef _MSC_VER /* for SIZE_MAX */
# include <stdint.h>
#endif
#include <stdio.h>

#include "thot_tail.h"
#include "trie-private.h"
#include "fileutils.h"

/*----------------------------------*
 *    INTERNAL TYPES DECLARATIONS   *
 *----------------------------------*/

/*-----------------------------------*
 *    PRIVATE METHODS DECLARATIONS   *
 *-----------------------------------*/

static TrieIndex    tail_alloc_block (Tail *t);
static void         tail_free_block (Tail *t, TrieIndex block);

/* ==================== BEGIN IMPLEMENTATION PART ====================  */

/*------------------------------------*
 *   INTERNAL TYPES IMPLEMENTATIONS   *
 *------------------------------------*/

/*------------------------------*
 *    PRIVATE DATA DEFINITONS   *
 *------------------------------*/

typedef struct {
    TrieIndex   next_free;
    TrieData    data;
} TailBlock;

struct _Tail {
    TrieIndex   num_tails;
    TailBlock  *tails;
    TrieIndex   first_free;
    Bool        is_mapped;
};

/*-----------------------------*
 *    METHODS IMPLEMENTAIONS   *
 *-----------------------------*/

#define TAIL_SIGNATURE      0xDFFCDFFC
#define TAIL_START_BLOCKNO  1

/* Tail Header:
 * INT32: signature
 * INT32: pointer to first free slot
 * INT32: number of tail blocks
 *
 * Tail Blocks:
 * INT32: pointer to next free block (-1 for allocated blocks)
 * INT32: data for the key
 * INT16: length
 * BYTES[length]: suffix string (no terminating '\0')
 */

/**
 * @brief Create a new tail object
 *
 * Create a new empty tail object.
 */
Tail *
tail_new ()
{
    Tail       *t;

    t = (Tail *) malloc (sizeof (Tail));
    if (UNLIKELY (!t))
        return NULL;

    t->first_free = 0;
    t->num_tails  = 0;
    t->tails      = NULL;
    t->is_mapped  = FALSE;

    return t;
}

/**
 * @brief Read tail data from file
 *
 * @param file : the file to read
 *
 * @return a pointer to the openned tail data, NULL on failure
 *
 * Read tail data from the opened file, starting from the current
 * file pointer until the end of tail data block. On return, the
 * file pointer is left at the position after the read block.
 */
Tail *
tail_fread (FILE *file, void *mapped_file_content)
{
    long        save_pos;
    Tail       *t;
    TrieIndex   i;
    uint32      sig;

    /* check signature */
    save_pos = ftell (file);
    if (!file_read_int32 (file, (int32 *) &sig) || TAIL_SIGNATURE != sig)
        goto exit_file_read;

    t = (Tail *) malloc (sizeof (Tail));
    if (UNLIKELY (!t))
        goto exit_file_read;

    if (!file_read_int32 (file, &t->first_free) ||
        !file_read_int32 (file, &t->num_tails))
    {
        goto exit_tail_created;
    }
    if (t->num_tails > SIZE_MAX / sizeof (TailBlock))
        goto exit_tail_created;
    t->is_mapped = TRUE;
    t->tails = (TailBlock *) (mapped_file_content + ftell (file));

    return t;


exit_tail_created:
    free (t);
exit_file_read:
    fseek (file, save_pos, SEEK_SET);
    return NULL;
}

/**
 * @brief Free tail data
 *
 * @param t : the tail data
 *
 * @return 0 on success, non-zero on failure
 *
 * Free the given tail data.
 */
void
tail_free (Tail *t)
{
    TrieIndex   i;

    if (t->tails && !t->is_mapped) {
        free (t->tails);
    }
    free (t);
}

/**
 * @brief Write tail data
 *
 * @param t     : the tail data
 * @param file  : the file to write to
 *
 * @return 0 on success, non-zero on failure
 *
 * Write tail data to the given @a file, starting from the current file
 * pointer. On return, the file pointer is left after the tail data block.
 */
int
tail_fwrite (const Tail *t, FILE *file)
{
    TrieIndex   i;

    if (!file_write_int32 (file, TAIL_SIGNATURE) ||
        !file_write_int32 (file, t->first_free)  ||
        !file_write_int32 (file, t->num_tails))
    {
        return -1;
    }
    for (i = 0; i < t->num_tails; i++) {
        int16   length;

        if (!file_write_int32 (file, t->tails[i].next_free) ||
            !file_write_int32 (file, t->tails[i].data))
        {
            return -1;
        }
    }

    return 0;
}

/**
 * @brief Add a new suffix
 *
 * @param t      : the tail data
 * @param suffix : the new suffix
 *
 * @return the index of the newly added suffix,
 *         or TRIE_INDEX_ERROR on failure.
 *
 * Add a new suffix entry to tail.
 */
TrieIndex
tail_add_suffix (Tail *t, const TrieChar *suffix)
{
    TrieIndex   new_block;

    new_block = tail_alloc_block (t);
    if (UNLIKELY (TRIE_INDEX_ERROR == new_block))
        return TRIE_INDEX_ERROR;

    return new_block;
}

static TrieIndex
tail_alloc_block (Tail *t)
{
    TrieIndex   block;

    if (0 != t->first_free) {
        block = t->first_free;
        t->first_free = t->tails[block].next_free;
    } else {
        void *new_block;

        block = t->num_tails;

        new_block = realloc (t->tails, (t->num_tails + 1) * sizeof (TailBlock));
        if (UNLIKELY (!new_block))
            return TRIE_INDEX_ERROR;

        t->tails = (TailBlock *) new_block;
        ++t->num_tails;
    }
    t->tails[block].next_free = -1;
    t->tails[block].data = TRIE_DATA_ERROR;

    return block + TAIL_START_BLOCKNO;
}

static void
tail_free_block (Tail *t, TrieIndex block)
{
    TrieIndex   i, j;

    block -= TAIL_START_BLOCKNO;

    if (block >= t->num_tails)
        return;

    t->tails[block].data = TRIE_DATA_ERROR;

    /* find insertion point */
    j = 0;
    for (i = t->first_free; i != 0 && i < block; i = t->tails[i].next_free)
        j = i;

    /* insert free block between j and i */
    t->tails[block].next_free = i;
    if (0 != j)
        t->tails[j].next_free = block;
    else
        t->first_free = block;
}

/**
 * @brief Get data associated to suffix entry
 *
 * @param t      : the tail data
 * @param index  : the index of the suffix
 *
 * @return the data associated to the suffix entry
 *
 * Get data associated to suffix entry @a index in tail data.
 */
TrieData
tail_get_data (const Tail *t, TrieIndex index)
{
    index -= TAIL_START_BLOCKNO;
    return LIKELY (index < t->num_tails)
             ? t->tails[index].data : TRIE_DATA_ERROR;
}

/**
 * @brief Set data associated to suffix entry
 *
 * @param t      : the tail data
 * @param index  : the index of the suffix
 * @param data   : the data to set
 *
 * @return boolean indicating success
 *
 * Set data associated to suffix entry @a index in tail data.
 */
Bool
tail_set_data (Tail *t, TrieIndex index, TrieData data)
{
    index -= TAIL_START_BLOCKNO;
    if (LIKELY (index < t->num_tails)) {
        t->tails[index].data = data;
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Delete suffix entry
 *
 * @param t      : the tail data
 * @param index  : the index of the suffix to delete
 *
 * Delete suffix entry from the tail data.
 */
void
tail_delete (Tail *t, TrieIndex index)
{
    tail_free_block (t, index);
}

/*
vi:ts=4:ai:expandtab
*/
