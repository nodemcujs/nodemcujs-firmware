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

#include "ecma-builtin-helpers.h"
#include "ecma-builtins.h"
#include "ecma-iterator-object.h"
#include "ecma-typedarray-object.h"

#if ENABLED (JERRY_ES2015_BUILTIN_ITERATOR)

#if !ENABLED (JERRY_ES2015_BUILTIN_SYMBOL)
#error "Iterator builtin requires ES2015 symbol builtin"
#endif /* !ENABLED (JERRY_ES2015_BUILTIN_SYMBOL) */

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-array-iterator-prototype.inc.h"
#define BUILTIN_UNDERSCORED_ID array_iterator_prototype
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup %arrayiteratorprototype% ECMA %ArrayIteratorPrototype% object built-in
 * @{
 */

/**
 * The %ArrayIteratorPrototype% object's 'next' routine
 *
 * See also:
 *          ECMA-262 v6, 22.1.5.2.1
 *
 * Note:
 *     Returned value must be freed with ecma_free_value.
 *
 * @return iterator result object, if success
 *         error - otherwise
 */
static ecma_value_t
ecma_builtin_array_iterator_prototype_object_next (ecma_value_t this_val) /**< this argument */
{
  /* 1 - 2. */
  if (!ecma_is_value_object (this_val))
  {
    return ecma_raise_type_error (ECMA_ERR_MSG ("Argument 'this' is not an object."));
  }

  ecma_object_t *obj_p = ecma_get_object_from_value (this_val);
  ecma_extended_object_t *ext_obj_p = (ecma_extended_object_t *) obj_p;

  /* 3. */
  if (ext_obj_p->u.pseudo_array.type != ECMA_PSEUDO_ARRAY_ITERATOR)
  {
    return ecma_raise_type_error (ECMA_ERR_MSG ("Argument 'this' is not an iterator."));
  }

  ecma_object_t *array_object_p = ECMA_GET_POINTER (ecma_object_t,
                                                    ext_obj_p->u.pseudo_array.u2.iterated_value_cp);


  /* 4 - 5 */
  if (array_object_p == NULL)
  {
    return ecma_create_iter_result_object (ECMA_VALUE_UNDEFINED, ECMA_VALUE_TRUE);
  }

  uint32_t length;

  /* 8 - 9. */
#if ENABLED (JERRY_ES2015_BUILTIN_TYPEDARRAY)
  if (ecma_is_typedarray (ecma_make_object_value (array_object_p)))
  {
    length = ecma_typedarray_get_length (array_object_p);
  }
  else
  {
#endif /* ENABLED (JERRY_ES2015_BUILTIN_TYPEDARRAY) */
    ecma_value_t len_value = ecma_op_object_get (array_object_p,
                                                 ecma_get_magic_string (LIT_MAGIC_STRING_LENGTH));

    if (ECMA_IS_VALUE_ERROR (len_value))
    {
      return len_value;
    }

    ecma_number_t length_number;
    ecma_value_t length_value = ecma_get_number (len_value, &length_number);

    if (ECMA_IS_VALUE_ERROR (length_value))
    {
      ecma_free_value (len_value);
      return length_value;
    }

    length = ecma_number_to_uint32 (length_number);

    ecma_free_value (len_value);
#if ENABLED (JERRY_ES2015_BUILTIN_TYPEDARRAY)
  }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_TYPEDARRAY) */

  uint32_t index = ext_obj_p->u.pseudo_array.u1.iterator_index;

  if (JERRY_UNLIKELY (index == ECMA_ITERATOR_INDEX_LIMIT))
  {
    /* After the ECMA_ITERATOR_INDEX_LIMIT limit is reached the [[%Iterator%NextIndex]]
       property is stored as an internal property */
    ecma_string_t *prop_name_p = ecma_get_magic_string (LIT_INTERNAL_MAGIC_STRING_ITERATOR_NEXT_INDEX);
    ecma_value_t index_value = ecma_op_object_get (obj_p, prop_name_p);

    if (!ecma_is_value_undefined (index_value))
    {
      index = (uint32_t) (ecma_get_number_from_value (index_value) + 1);
    }

    ecma_value_t put_result = ecma_op_object_put (obj_p,
                                                  prop_name_p,
                                                  ecma_make_uint32_value (index),
                                                  true);

    JERRY_ASSERT (ecma_is_value_true (put_result));

    ecma_free_value (index_value);
  }
  else
  {
    /* 11. */
    ext_obj_p->u.pseudo_array.u1.iterator_index++;
  }

  if (index >= length)
  {
    ECMA_SET_POINTER (ext_obj_p->u.pseudo_array.u2.iterated_value_cp, NULL);
    return ecma_create_iter_result_object (ECMA_VALUE_UNDEFINED, ECMA_VALUE_TRUE);
  }

  /* 7. */
  uint8_t iterator_type = ext_obj_p->u.pseudo_array.extra_info;

  if (iterator_type == ECMA_ARRAY_ITERATOR_KEYS)
  {
    /* 12. */
    return ecma_create_iter_result_object (ecma_make_uint32_value (index), ECMA_VALUE_FALSE);
  }

  /* 13. */
  ecma_string_t *index_string_p = ecma_new_ecma_string_from_uint32 (index);

  /* 14. */
  ecma_value_t get_value = ecma_op_object_get (array_object_p, index_string_p);
  ecma_deref_ecma_string (index_string_p);

  /* 15. */
  if (ECMA_IS_VALUE_ERROR (get_value))
  {
    return get_value;
  }

  ecma_value_t result;

  /* 16. */
  if (iterator_type == ECMA_ARRAY_ITERATOR_VALUES)
  {
    result = ecma_create_iter_result_object (get_value, ECMA_VALUE_FALSE);
  }
  else
  {
    /* 17.a */
    JERRY_ASSERT (iterator_type == ECMA_ARRAY_ITERATOR_KEYS_VALUES);

    /* 17.b */
    ecma_value_t entry_array_value;
    entry_array_value = ecma_create_array_from_iter_element (get_value,
                                                             ecma_make_uint32_value (index));

    result = ecma_create_iter_result_object (entry_array_value, ECMA_VALUE_FALSE);
    ecma_free_value (entry_array_value);
  }

  ecma_free_value (get_value);

  return result;
} /* ecma_builtin_array_iterator_prototype_object_next */

/**
 * @}
 * @}
 * @}
 */

#endif /* ENABLED (JERRY_ES2015_BUILTIN_ITERATOR) */
