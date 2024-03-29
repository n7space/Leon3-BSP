/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022 N7 Space Sp. z o.o.
 * 
 * Leon3 BSP for the Test Environment was developed under the project AURORA.
 * This project has received funding from the European Union’s Horizon 2020
 * research and innovation programme under grant agreement No 101004291”
 *
 * Leon3 BSP for the Test Environment is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * Leon3 BSP for the Test Environment is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Leon3 BSP for the Test Environment. If not,
 * see <http://www.gnu.org/licenses/>.
 */

/// \brief Module representing fixed-size byte queue based on circular buffer.

/**
 * @defgroup ByteFifo ByteFifo
 * @ingroup Utils
 * @{
 */

#ifndef UTILS_BYTEFIFO_H
#define UTILS_BYTEFIFO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// \brief Structure representing single queue instance.
typedef struct
{
    uint8_t* volatile begin; ///< Pointer to beginning of buffer area.
    uint8_t* volatile end;   ///< Pointer to end of buffer area.
    uint8_t* volatile first; ///< Pointer to oldest item in queue.
    uint8_t* volatile last;  ///< Pointer used as next insert location.
} ByteFifo;
/// \brief ByteFifo constructor macro, creates empty queue with given name and
/// capacity. It creates memory block on stack, so it is mostly useful in tests.
/// \param [in] NAME name of ByteFifo to create.
/// \param [in] CAPACITY capacity of created ByteFifo.
// clang-format off
#define BYTE_FIFO_CREATE(NAME, CAPACITY)                                \
  uint8_t NAME ## MemoryBlock[(CAPACITY)] = { 0 };                      \
  ByteFifo NAME = { .begin = NAME ## MemoryBlock,                       \
                    .end   = NAME ## MemoryBlock + (CAPACITY),          \
                    .first = NULL,                                      \
                    .last  = NAME ## MemoryBlock }
// clang-format on

/// \brief ByteFifo constructor macro, creates queue with provided contents.
///        It creates memory block on stack, so it is mostly useful in tests.
/// \param [in] NAME name of ByteFifo to create.
/// \param [in] ... contents of created ByteFifo.
// clang-format off
#define BYTE_FIFO_CREATE_FILLED(NAME, ...)                                      \
  uint8_t NAME ## MemoryBlock[] = __VA_ARGS__;                                  \
  ByteFifo NAME = { .begin = NAME ## MemoryBlock,                               \
                    .end   = NAME ## MemoryBlock + sizeof(NAME ## MemoryBlock), \
                    .first = NAME ## MemoryBlock,                               \
                    .last  = NAME ## MemoryBlock }
// clang-format on

/// \brief ByteFifo initialisation procedure, assigns all fields properly.
///        Should be called before any use of ByteFifo.
/// \param [in,out] fifo pointer to ByteFifo to initialise.
/// \param [in] memoryBlock memory block to be assigned to ByteFifo as its
///             storage area.
/// \param [in] memoryBlockSize size of memory block.
void ByteFifo_init(ByteFifo* const fifo,
                   uint8_t* const memoryBlock,
                   const size_t memoryBlockSize);

/// \brief ByteFifo initialization procedure, used to "attach" a FIFO queue on
///        top of a buffer with data already inside. Should be called before any
///        use of the instance.
/// \param [in,out] fifo Pointer to a ByteFifo instance to be initialized.
/// \param [in] memoryBlock Array of bytes to have a FIFO attached to.
/// \param [in] memoryBlockSize Number of bytes in the queue. Has to equal the
///             number of bytes to be pulled from it.
void ByteFifo_initFromBytes(ByteFifo* const fifo,
                            uint8_t* const memoryBlock,
                            const size_t memoryBlockSize);

/// \brief Clears queue.
/// \param [in,out] fifo queue to clear.
static inline void
ByteFifo_clear(ByteFifo* const fifo)
{
    fifo->first = NULL;
    fifo->last = fifo->begin;
}

/// \brief Checks if queue is full.
/// \param [in] fifo queue to check.
/// \retval true when queue is full (next push will not be accepted).
/// \retval false otherwise
static inline bool
ByteFifo_isFull(const ByteFifo* const fifo)
{
    return fifo->first == fifo->last;
}

/// \brief Checks if queue is empty.
/// \param [in] fifo queue to check.
/// \retval true when queue is empty (next pull will not be accepted).
/// \retval false otherwise
static inline bool
ByteFifo_isEmpty(const ByteFifo* const fifo)
{
    return fifo->first == NULL;
}

/// \brief Returns the number of elements in queue.
/// \param [in] fifo Queue to check.
/// \returns The number of elements.
static inline size_t
ByteFifo_getCount(const ByteFifo* const fifo)
{
    if(ByteFifo_isEmpty(fifo))
        return 0;

    intptr_t ptrDifference = fifo->last - fifo->first;
    if(ptrDifference <= 0)
        ptrDifference += fifo->end - fifo->begin;

    return (size_t)ptrDifference;
}

/// \brief Pushes given item as last in queue.
/// \param [in,out] fifo target queue.
/// \param [in] data data to push.
/// \retval true on successful push
/// \retval false otherwise (queue is full)
static inline bool
ByteFifo_push(ByteFifo* const fifo, const uint8_t data)
{
    if(ByteFifo_isFull(fifo))
        return false;

    if(fifo->first == NULL)
        fifo->first = fifo->last;
    *fifo->last = data;
    fifo->last++;
    if(fifo->last == fifo->end)
        fifo->last = fifo->begin;

    return true;
}

/// \brief Pull first item from queue. Removes pulled item from queue.
/// \param [in,out] fifo target queue.
/// \param [out] data address to store pulled data.
/// \retval true on successful pull
/// \retval false otherwise (queue is empty)
static inline bool
ByteFifo_pull(ByteFifo* const fifo, uint8_t* const data)
{
    if(ByteFifo_isEmpty(fifo))
        return false;

    *data = *fifo->first;
    fifo->first++;
    if(fifo->first == fifo->end)
        fifo->first = fifo->begin;
    if(fifo->first == fifo->last)
        fifo->first = NULL;
    return true;
}

#endif // UTILS_BYTEFIFO_H

/** @} */
