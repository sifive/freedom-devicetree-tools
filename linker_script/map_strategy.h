/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __MAP_STRATEGY__H
#define __MAP_STRATEGY__H

#include <list>
#include <string>

#include <fdt.h++>

#include <memory.h>
#include <linker_script.h>

using std::list;
using std::string;

typedef enum {
  LINK_STRATEGY_DEFAULT,
  LINK_STRATEGY_SCRATCHPAD,
  LINK_STRATEGY_RAMRODATA,
} LinkStrategy;

typedef enum {
  CODE_MODEL_MEDLOW,
  CODE_MODEL_MEDANY,
} CodeModel;

class MapStrategy {
  public:
    /*!
     * @brief Check if a list of memories will be compatible with a given code model
     */
    bool check_cmodel_support(const fdt &dtb, CodeModel cmodel, Memory ram, Memory rom, Memory itim);

    /*!
     * @brief Check if the memory map is supported by any code model
     *
     * Prints warnings to stderr if a code model is not supported.
     * Exits with an error code if no code model is supported.
     */
    void check_all_cmodels(const fdt &dtb, Memory ram, Memory rom, Memory itim);

    /*!
     * @brief Print the details of the chosen strategy to stdout
     */
    void print_chosen_strategy(const fdt &dtb, string name,
                               LinkStrategy layout,
                               Memory ram, Memory rom, Memory itim);

    /*!
     * @brief Detects if a memory corresponding to an RTL reset vector exists
     */
    bool has_testram(list<Memory> memories);

    /*!
     * @brief Returns the memory which is selected by RTL reset vector heuristics
     */
    Memory find_testram(list<Memory> memories);

    /*!
     * @brief Detects if the MapStrategy is valid for the list of available memories
     */
    virtual bool valid(const fdt &dtb, list<Memory> available_memories) = 0;

    /*!
     * @brief builds a LinkerScript using the available memories for the strategy
     */
    virtual LinkerScript create_layout(const fdt &dtb, list<Memory> available_memories,
                                       LinkStrategy link_strategy) = 0;
};

#endif /* __MAP_STRATEGY_H */

