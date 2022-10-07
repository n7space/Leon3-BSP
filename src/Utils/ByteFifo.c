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

#include "ByteFifo.h"

void
ByteFifo_init(ByteFifo* const fifo,
              uint8_t* const memoryBlock,
              const size_t memoryBlockSize)
{
    if(memoryBlockSize == 0) {
        return;
    }

    fifo->begin = memoryBlock;
    fifo->end = memoryBlock + memoryBlockSize;
    fifo->first = NULL;
    fifo->last = memoryBlock;
}

void
ByteFifo_initFromBytes(ByteFifo* const fifo,
                       uint8_t* const memoryBlock,
                       const size_t memoryBlockSize)
{
    if(memoryBlockSize == 0) {
        return;
    }

    fifo->begin = memoryBlock;
    fifo->end = memoryBlock + memoryBlockSize;
    fifo->first = memoryBlock;
    fifo->last = memoryBlock;
}
