/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ECMA_CONTAINER_OBJECT_H
#define ECMA_CONTAINER_OBJECT_H

#include "ecma-globals.h"

#if ENABLED (JERRY_ES2015_BUILTIN_MAP)

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmamaphelpers ECMA builtin map/set helper functions
 * @{
 */

ecma_value_t ecma_op_container_create (const ecma_value_t *arguments_list_p, ecma_length_t arguments_list_len,
                                       bool is_set);
ecma_value_t ecma_op_container_size (ecma_value_t this_arg, bool is_set);
ecma_value_t ecma_op_container_get (ecma_value_t this_arg, ecma_value_t key_arg);
ecma_value_t ecma_op_container_foreach (ecma_value_t this_arg, ecma_value_t predicate, ecma_value_t predicate_this_arg,
                                        bool is_set);
ecma_value_t ecma_op_container_has (ecma_value_t this_arg, ecma_value_t key_arg, bool is_set);
ecma_value_t ecma_op_container_set (ecma_value_t this_arg, ecma_value_t key_arg, ecma_value_t value_arg, bool is_set);
void ecma_op_container_clear_map (ecma_map_object_t *map_object_p);
ecma_value_t ecma_op_container_clear (ecma_value_t this_arg, bool is_set);
ecma_value_t ecma_op_container_delete (ecma_value_t this_arg, ecma_value_t key_arg, bool is_set);

/**
 * @}
 * @}
 */

#endif /* ENABLED (JERRY_ES2015_BUILTIN_MAP) */

#endif /* !ECMA_CONTAINER_OBJECT_H */
