/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/buffer.h>

inline static idx_t get_idx(idx_t offset, idx_t depth)
{
        return (offset>>((BUFFER_TREE_DEPTH-depth+1)*12))&BUFFER_LIST_SIZE;
}

static int
buffer_init_branch(struct buffer_list* this)
{
        if (this == NULL)
                return -E_NULL_PTR;

        memset(this, 0, sizeof(struct buffer_list));
        return -E_SUCCESS;
}

/**
 * \fn buffer_rm_block
 * \brief Removes a block from the buffer and the related branches if empty
 *
 * \param this The buffer to work with
 * \param offset The index at which the block resides
 * \param depth For external calls, keep 0
 */

static int
buffer_rm_block(struct buffer_list* this, idx_t offset, idx_t depth)
{
        warning("buffer_rm_block not yet tested!\n");
        if (depth > 5)
                return -E_INVALID_ARG;

        if (this == NULL)
                goto err;

        mutex_lock(this->lock);

        idx_t idx = get_idx(offset, depth);

        int ret = -E_SUCCESS;
        if (depth != BUFFER_TREE_DEPTH+1)
        {
                struct buffer_list* list = this->lists[idx];
                if (list == NULL)
                        goto err_locked;
                ret = buffer_rm_block(list, offset, depth+1);
                switch(ret)
                {
                case -E_CLEAN_PARENT:
                        free(this->lists[idx]);
                        if (atomic_dec(&this->used) == 0)
                                return -E_CLEAN_PARENT;
                        break;
                default:
                        return ret;
                }
        }

        if (this->blocks[idx] == NULL)
                goto err_locked;

        free(this->blocks[idx]);
        if (atomic_dec(&this->used) == 0)
                return -E_CLEAN_PARENT;

        return -E_SUCCESS;

err_locked:
        mutex_unlock(this->lock);
err:
        return -E_NULL_PTR;
}

/**
 * \fn buffer_clean_up
 * \brief Remove all data from 0 up to base_idx
 *
 * \param this The buffer to clean up
 */

static int
buffer_clean_up(struct buffer* this)
{
        if (this == NULL)
                return -E_NULL_PTR;

        warning("buffer_clean_up not yet tested!\n");

        /** Clean up from the location last cleaned untill base_idx */

        idx_t idx = this->cleaned;
        for (; idx < this->base_idx; idx+=BUFFER_BLOCK_SIZE)
                buffer_rm_block(this->blocks, idx/BUFFER_BLOCK_SIZE, 0);

        this->cleaned = idx;

        return -E_SUCCESS;
}

/**
 * \fn buffer_add_block
 * \brief Add a block to a certain offset into the buffer
 *
 * \param this The buffer to place the new block into
 * \param list The list to add the block to
 * \param offset Where we should place the new block
 * \param depth for external calls, keep 0
 */

static int
buffer_add_block(this, list, offset, depth)
struct buffer_block* this;
struct buffer_list* list;
idx_t offset;
idx_t depth;
{
        warning("buffer_add_block not yet tested!\n");

        if (list == NULL)
                return -E_NULL_PTR;

        int ret = 0;
        idx_t list_idx = get_idx(offset, depth);

        mutex_lock(list->lock);

        if (depth != BUFFER_TREE_DEPTH+1)
        {
                if (list->lists[list_idx] == NULL)
                {
                        list->lists[list_idx] = kalloc
                                                   (sizeof(struct buffer_list));
                        if (list->lists[list_idx] == NULL)
                        {
                                mutex_unlock(list->lock);
                                return -E_NULL_PTR;
                        }
                        buffer_init_branch(list->lists[list_idx]);
                }
                ret =  buffer_add_block(this, list->lists[list_idx],
                                                                         offset,
                                                                       depth+1);
                mutex_unlock(list->lock);
                return ret;
        }

        if (list->blocks[list_idx] != NULL)
        {
                mutex_unlock(list->lock);
                return -E_ALREADY_INITIALISED;
        }

        list->blocks[list_idx] = this;
        mutex_unlock(list->lock);
        return -E_SUCCESS;
}

/**
 * \fn buffer_find_block
 * \brief Helper function to find the requested block_id
 *
 * \param this The buffer to seek in
 * \param offset The block offset in the buffer
 * \param depth For external calls, keep 0
 */

static struct buffer_block*
buffer_find_block(struct buffer_list* this, idx_t offset, idx_t depth)
{
        warning("buffer_find_block not yet tested!\n");

        if (this == NULL)
                goto err;

        idx_t list_idx = get_idx(offset, depth);
        struct buffer_block* ret;

        if (depth != BUFFER_TREE_DEPTH+1)
                ret = buffer_find_block(this->lists[list_idx], offset, depth);

        else
                ret = this->blocks[list_idx];

        return ret;

err:
        return NULL;
}

/**
 * \fn buffer_write
 * \brief Write data to stream
 *
 * \param this The stream to write to
 * \param buf The char array to write to the stream
 * \param num The size of the char array
 */

static int
buffer_write(struct vfile* this, char* buf, size_t num)
{
        warning("buffer_write not yet implemented!\n");
        return -E_NOFUNCTION;
}

/**
 * \fn buffer_read
 * \brief get data from stream
 *
 * \param this The stream to read from
 * \param buf The char array to write to from the stream
 * \param num The size of the char array
 */

static int
buffer_read(struct vfile* this, char* buf, size_t num)
{
        warning("buffer_read not yet implemented!\n");
        idx_t offset = this->cursor / BUFFER_BLOCK_SIZE;
        idx_t block_cur = this->cursor % BUFFER_BLOCK_SIZE;

        struct buffer* buffer = this->fs_data;
        if (buffer == NULL)
                return -E_NULL_PTR;

        struct buffer_block *b = buffer_find_block(buffer->blocks, offset, 0);
        if (b == NULL)
                return -E_NULL_PTR;

        size_t idx = 0;
        for (; idx < num; idx++, block_cur++)
        {
                if (block_cur >= BUFFER_BLOCK_SIZE)
                {
                        offset++;
                        block_cur -= BUFFER_BLOCK_SIZE;
                        b = buffer_find_block(buffer->blocks, offset, 0);
                        if (b == NULL)
                        {
                                this->cursor += idx;
                                return -E_NULL_PTR;
                        }
                }
                buf[idx] = b->data[block_cur];
        }

        this->cursor += idx;
        return -E_SUCCESS;
}

/**
 * \fn buffer_seek
 *
 * \brief seek in the file
 *
 * \param this a pointer to the file we're working with.
 * \param offset what's the distance from the "from" indicator.
 * \param from what's the point we have to seek from.
 */

static int
buffer_seek(struct vfile* this, idx_t offset, seek_t from)
{
        warning("buffer_seek hasn't been tested yet!\n");
        if (this == NULL || this->fs_data == NULL)
                return -E_NULL_PTR;

        struct buffer* buf = this->fs_data;
        switch(from)
        {
        case SEEK_SET:
                if (offset < buf->base_idx)
                        this->cursor = buf->base_idx;
                else if (offset > buf->size)
                        this->cursor = buf->size;
                else
                        this->cursor = offset;
                break;

        case SEEK_CUR:
                if (offset < 0 && (-offset < (this->cursor - buf->base_idx)))
                        this->cursor += offset;
                else if (offset < 0)
                        this->cursor = buf->base_idx;
                else if (offset > (buf->size-this->cursor))
                        this->cursor = buf->size;
                else
                        this->cursor += offset;
                break;

        case SEEK_END:
                if (offset > 0)
                        this->cursor = buf->size;
                else if (offset < 0 && -offset < (buf->size - buf->base_idx))
                        this->cursor += offset;
                else
                        this->cursor = buf->base_idx;
                break;

        default:
                debug("Buffer seek doesn't support mode: %X\n", from);
                return -E_INVALID_ARG;
                break;
        }
        return -E_SUCCESS;
}

/**
 * \fn buffer_close
 * \brief closes the buffer, and cleans up the data if it's the last buffer
 * \brief standing.
 *
 * \param this the buffer to close
 */

static int
buffer_close(struct vfile* this)
{
        if (this == NULL || this->fs_data == NULL)
                return -E_NULL_PTR;

        struct buffer* buf = (struct buffer*)this->fs_data;

        if (atomic_dec(&(buf->opened)) == 0)
        {
                /** clean up the entire buffer */
                buf->base_idx = buf->size;
                return buffer_clean_up(this->fs_data);
        };
        /** we have removed this instance by running atomic_dec  */
        return -E_SUCCESS;
}

/**
 * \fn buffer_duplicate
 * \brief takes only one argument, which is the buffer to duplicate. It returns
 * \brief the duplicated buffer.
 *
 * \param this the buffer to duplicate
 */

static struct buffer*
buffer_duplicate(struct buffer *this)
{
        if (!(this->rights & (BUFFER_ALLOW_DUPLICATE)))
                return NULL;

        atomic_inc(&(this->opened));
        return this;
}

/**
 * \fn buffer_init
 * \brief Initialise a new buffer
 * takes 2 arguments:
 *
 * \param size, sets the size of the buffer.
 *      If size == BUFFER_DYNAMIC_SIZE, the size will be set to 0 and the buffer
 *      now is allowed to grow dynamically.
 *
 * \param base_idx, tells us up to which point we're allowed to clean up.
 *      From 0 untill base_idx, nothing will be written.
 *      It will also set the standard cursor.
 *
 * This function returns the newly created buffer.
 */

int
buffer_init(struct vfile* this, idx_t size, idx_t base_idx)
{
        if (this == NULL)
                return -E_NULL_PTR;

        struct buffer* b = kalloc(sizeof(struct buffer));
        if (b == NULL)
                return -E_NOMEM;
        memset(b, 0, sizeof(struct buffer));

        this->read = buffer_read;
        this->seek = buffer_seek;
        this->write = buffer_write;
        this->close = buffer_close;

        b->duplicate = buffer_duplicate;

        b->size = (size == BUFFER_DYNAMIC_SIZE) ? 0 : size;
        b->base_idx = base_idx;
        b->rights |= (size == BUFFER_DYNAMIC_SIZE) ? BUFFER_ALLOW_GROWTH : 0;

        atomic_inc(&b->opened);

        this->fs_data = b;
        this->fs_data_size = sizeof(struct buffer);
        return -E_SUCCESS;
}