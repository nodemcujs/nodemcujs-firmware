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

#include "ecma-dataview-object.h"
#include "ecma-gc.h"

#if ENABLED (JERRY_ES2015_BUILTIN_DATAVIEW)

#ifdef CONFIG_DISABLE_ES2015_TYPEDARRAY_BUILTIN
#error "DataView builtin requires ES2015 TypedArray builtin"
#endif /* !CONFIG_DISABLE_ES2015_TYPEDARRAY_BUILTIN */

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

/**
 * This object has a custom dispatch function.
 */
#define BUILTIN_CUSTOM_DISPATCH

/**
 * List of built-in routine identifiers.
 */
enum
{
  ECMA_DATAVIEW_PROTOTYPE_ROUTINE_START = ECMA_BUILTIN_ID__COUNT - 1,
  ECMA_DATAVIEW_PROTOTYPE_BUFFER_GETTER,
  ECMA_DATAVIEW_PROTOTYPE_BYTE_LENGTH_GETTER,
  ECMA_DATAVIEW_PROTOTYPE_BYTE_OFFSET_GETTER,
  ECMA_DATAVIEW_PROTOTYPE_GET_INT8,
  ECMA_DATAVIEW_PROTOTYPE_GET_UINT8,
  ECMA_DATAVIEW_PROTOTYPE_GET_INT16,
  ECMA_DATAVIEW_PROTOTYPE_GET_UINT16,
  ECMA_DATAVIEW_PROTOTYPE_GET_INT32,
  ECMA_DATAVIEW_PROTOTYPE_GET_UINT32,
  ECMA_DATAVIEW_PROTOTYPE_GET_FLOAT32,
#if ENABLED (JERRY_NUMBER_TYPE_FLOAT64)
  ECMA_DATAVIEW_PROTOTYPE_GET_FLOAT64,
#endif /* ENABLED (JERRY_NUMBER_TYPE_FLOAT64) */
  ECMA_DATAVIEW_PROTOTYPE_SET_INT8,
  ECMA_DATAVIEW_PROTOTYPE_SET_UINT8,
  ECMA_DATAVIEW_PROTOTYPE_SET_INT16,
  ECMA_DATAVIEW_PROTOTYPE_SET_UINT16,
  ECMA_DATAVIEW_PROTOTYPE_SET_INT32,
  ECMA_DATAVIEW_PROTOTYPE_SET_UINT32,
  ECMA_DATAVIEW_PROTOTYPE_SET_FLOAT32,
#if ENABLED (JERRY_NUMBER_TYPE_FLOAT64)
  ECMA_DATAVIEW_PROTOTYPE_SET_FLOAT64,
#endif /* ENABLED (JERRY_NUMBER_TYPE_FLOAT64) */
};

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-dataview-prototype.inc.h"
#define BUILTIN_UNDERSCORED_ID dataview_prototype
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup dataviewprototype ECMA DataView.prototype object built-in
 * @{
 */

/**
 * Corresponding typedarray mappings for the {get,set}{[U]int, Float}{8, 16, 32, 64} routines
 */
static const uint8_t ecma_dataview_type_mapping[] =
{
  ECMA_BUILTIN_ID_INT8ARRAY,
  ECMA_BUILTIN_ID_UINT8ARRAY,
  ECMA_BUILTIN_ID_INT16ARRAY,
  ECMA_BUILTIN_ID_UINT16ARRAY,
  ECMA_BUILTIN_ID_INT32ARRAY,
  ECMA_BUILTIN_ID_UINT32ARRAY,
  ECMA_BUILTIN_ID_FLOAT32ARRAY,
#if ENABLED (JERRY_NUMBER_TYPE_FLOAT64)
  ECMA_BUILTIN_ID_FLOAT64ARRAY,
#endif /* ENABLED (JERRY_NUMBER_TYPE_FLOAT64) */
};

/**
 * The DataView.prototype object's {buffer, byteOffset, byteLength} getters
 *
 * See also:
 *          ECMA-262 v6, 24.2.4.1
 *          ECMA-262 v6, 24.2.4.2
 *          ECMA-262 v6, 24.2.4.3
 *
 * @return ecma value
 *         Returned value must be freed with ecma_free_value.
 */
static ecma_value_t
ecma_builtin_dataview_prototype_object_getters (ecma_value_t this_arg, /**< this argument */
                                                uint16_t builtin_routine_id) /**< built-in wide routine identifier */
{
  ecma_dataview_object_t *obj_p = ecma_op_dataview_get_object (this_arg);

  if (JERRY_UNLIKELY (obj_p == NULL))
  {
    return ECMA_VALUE_ERROR;
  }

  switch (builtin_routine_id)
  {
    case ECMA_DATAVIEW_PROTOTYPE_BUFFER_GETTER:
    {
      ecma_object_t *buffer_p = obj_p->buffer_p;
      ecma_ref_object (buffer_p);

      return ecma_make_object_value (buffer_p);
    }
    case ECMA_DATAVIEW_PROTOTYPE_BYTE_LENGTH_GETTER:
    {
      return ecma_make_uint32_value (obj_p->header.u.class_prop.u.length);
    }
    default:
    {
      JERRY_ASSERT (builtin_routine_id == ECMA_DATAVIEW_PROTOTYPE_BYTE_OFFSET_GETTER);
      return ecma_make_uint32_value (obj_p->byte_offset);
    }
  }
} /* ecma_builtin_dataview_prototype_object_getters */

/**
 * Dispatcher of the built-in's routines
 *
 * @return ecma value
 *         Returned value must be freed with ecma_free_value.
 */
ecma_value_t
ecma_builtin_dataview_prototype_dispatch_routine (uint16_t builtin_routine_id, /**< built-in wide routine identifier */
                                                  ecma_value_t this_arg, /**< 'this' argument value */
                                                  const ecma_value_t arguments_list_p[], /**< list of arguments
                                                                                          *   passed to routine */
                                                  ecma_length_t arguments_number) /**< length of arguments' list */
{
  ecma_value_t byte_offset = arguments_number > 0 ? arguments_list_p[0] : ECMA_VALUE_UNDEFINED;

  switch (builtin_routine_id)
  {
    case ECMA_DATAVIEW_PROTOTYPE_BUFFER_GETTER:
    case ECMA_DATAVIEW_PROTOTYPE_BYTE_LENGTH_GETTER:
    case ECMA_DATAVIEW_PROTOTYPE_BYTE_OFFSET_GETTER:
    {
      return ecma_builtin_dataview_prototype_object_getters (this_arg, builtin_routine_id);
    }
    case ECMA_DATAVIEW_PROTOTYPE_GET_FLOAT32:
#if ENABLED (JERRY_NUMBER_TYPE_FLOAT64)
    case ECMA_DATAVIEW_PROTOTYPE_GET_FLOAT64:
#endif /* ENABLED (JERRY_NUMBER_TYPE_FLOAT64) */
    case ECMA_DATAVIEW_PROTOTYPE_GET_INT16:
    case ECMA_DATAVIEW_PROTOTYPE_GET_INT32:
    case ECMA_DATAVIEW_PROTOTYPE_GET_UINT16:
    case ECMA_DATAVIEW_PROTOTYPE_GET_UINT32:
    {
      ecma_value_t little_endian = arguments_number > 1 ? arguments_list_p[1] : ECMA_VALUE_FALSE;
      uint8_t type = ecma_dataview_type_mapping[builtin_routine_id - ECMA_DATAVIEW_PROTOTYPE_GET_INT8];

      return ecma_op_dataview_get_set_view_value (this_arg, byte_offset, little_endian, ECMA_VALUE_EMPTY, type);
    }
    case ECMA_DATAVIEW_PROTOTYPE_SET_FLOAT32:
#if ENABLED (JERRY_NUMBER_TYPE_FLOAT64)
    case ECMA_DATAVIEW_PROTOTYPE_SET_FLOAT64:
#endif /* ENABLED (JERRY_NUMBER_TYPE_FLOAT64) */
    case ECMA_DATAVIEW_PROTOTYPE_SET_INT16:
    case ECMA_DATAVIEW_PROTOTYPE_SET_INT32:
    case ECMA_DATAVIEW_PROTOTYPE_SET_UINT16:
    case ECMA_DATAVIEW_PROTOTYPE_SET_UINT32:
    {
      ecma_value_t value_to_set = arguments_number > 1 ? arguments_list_p[1] : ECMA_VALUE_UNDEFINED;
      ecma_value_t little_endian = arguments_number > 2 ? arguments_list_p[2] : ECMA_VALUE_FALSE;
      uint8_t type = ecma_dataview_type_mapping[builtin_routine_id - ECMA_DATAVIEW_PROTOTYPE_SET_INT8];

      return ecma_op_dataview_get_set_view_value (this_arg, byte_offset, little_endian, value_to_set, type);
    }
    case ECMA_DATAVIEW_PROTOTYPE_GET_INT8:
    case ECMA_DATAVIEW_PROTOTYPE_GET_UINT8:
    {
      uint8_t type = ecma_dataview_type_mapping[builtin_routine_id - ECMA_DATAVIEW_PROTOTYPE_GET_INT8];

      return ecma_op_dataview_get_set_view_value (this_arg, byte_offset, ECMA_VALUE_FALSE, ECMA_VALUE_EMPTY, type);
    }
    default:
    {
      JERRY_ASSERT (builtin_routine_id == ECMA_DATAVIEW_PROTOTYPE_SET_INT8
                    || builtin_routine_id == ECMA_DATAVIEW_PROTOTYPE_SET_UINT8);
      ecma_value_t value_to_set = arguments_number > 1 ? arguments_list_p[1] : ECMA_VALUE_UNDEFINED;
      uint8_t type = ecma_dataview_type_mapping[builtin_routine_id - ECMA_DATAVIEW_PROTOTYPE_SET_INT8];

      return ecma_op_dataview_get_set_view_value (this_arg, byte_offset, ECMA_VALUE_FALSE, value_to_set, type);
    }
  }
} /* ecma_builtin_dataview_prototype_dispatch_routine */

/**
 * @}
 * @}
 * @}
 */

#endif /* ENABLED (JERRY_ES2015_BUILTIN_DATAVIEW */
