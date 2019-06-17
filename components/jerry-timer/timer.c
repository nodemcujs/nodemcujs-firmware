#include "include/timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "clist.h"

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

clist_t *timerIdList;
uint8_t timerId = 0;

typedef struct jerry_timer_t
{
  jerry_value_t funcObj;
  uint8_t timerId;
  TimerHandle_t timerHandler;
  bool repeat;
} jerry_timer_t;

static void timeout_callback(TimerHandle_t pxTimer)
{
  list_node_t *node;
  jerry_timer_t *timer = NULL;
  list_iterator_t *it = list_iterator_new(timerIdList, LIST_HEAD);
  while ((node = list_iterator_next(it)))
  {
    timer = (jerry_timer_t *)node->val;
    if (timer->timerId == (uint8_t)pvTimerGetTimerID(pxTimer))
    {
      jerry_call_function(timer->funcObj, NULL, NULL, 0);
      if (!timer->repeat)
      {
        jerry_release_value(timer->funcObj);
        xTimerStop(timer->timerHandler, NULL);
        xTimerDelete(timer->timerHandler, NULL);
        free(timer);
        list_remove(timerIdList, node);
      }
      break;
    }
  }
  list_iterator_destroy(it);
}

static jerry_value_t setInterval_handler(const jerry_value_t func_value, /**< function object */
                                         const jerry_value_t this_value, /**< this arg */
                                         const jerry_value_t *args_p,    /**< function arguments */
                                         const jerry_length_t args_cnt)  /**< number of function arguments */
{
  jerry_value_t func_obj = *args_p++;
  jerry_value_t interval_obj = *args_p;
  TickType_t interval = (TickType_t)jerry_get_number_value(interval_obj);

  if (timerId == 0)
  {
    timerId = 1;
  }
  jerry_timer_t *timer = (jerry_timer_t *)malloc(sizeof(struct jerry_timer_t));
  timer->repeat = true;
  // Acquire types with reference counter (increase the references).
  timer->funcObj = jerry_acquire_value(func_obj);
  timer->timerId = timerId;

  TimerHandle_t timerHandler = xTimerCreate("jtmr", interval / portTICK_PERIOD_MS, pdTRUE, (void *)timer->timerId, timeout_callback);
  timer->timerHandler = timerHandler;

  list_node_t *node = list_node_new(timer);

  list_rpush(timerIdList, node);

  xTimerStart(timerHandler, NULL);

  return jerry_create_number(timerId++);
}

static jerry_value_t setTimeout_handler(const jerry_value_t func_value, /**< function object */
                                        const jerry_value_t this_value, /**< this arg */
                                        const jerry_value_t *args_p,    /**< function arguments */
                                        const jerry_length_t args_cnt)  /**< number of function arguments */
{
  jerry_value_t func_obj = *args_p++;
  jerry_value_t timeout_obj = *args_p;
  TickType_t timeout = (TickType_t)jerry_get_number_value(timeout_obj);

  if (timerId == 0)
  {
    timerId = 1;
  }
  jerry_timer_t *timer = (jerry_timer_t *)malloc(sizeof(struct jerry_timer_t));
  timer->repeat = false;
  // Acquire types with reference counter (increase the references).
  timer->funcObj = jerry_acquire_value(func_obj);
  timer->timerId = timerId;

  TimerHandle_t timerHandler = xTimerCreate("jtmr", timeout / portTICK_PERIOD_MS, pdTRUE, (void *)timer->timerId, timeout_callback);
  timer->timerHandler = timerHandler;

  list_node_t *node = list_node_new(timer);

  list_rpush(timerIdList, node);

  xTimerStart(timerHandler, NULL);

  return jerry_create_number(timerId++);
}

static jerry_value_t clearTimer_handler(const jerry_value_t func_value, /**< function object */
                                        const jerry_value_t this_value, /**< this arg */
                                        const jerry_value_t *args_p,    /**< function arguments */
                                        const jerry_length_t args_cnt)  /**< number of function arguments */
{
  jerry_value_t timerId = *args_p;
  uint8_t id = (uint8_t)jerry_get_number_value(timerId);

  list_node_t *node;
  jerry_timer_t *timer;
  list_iterator_t *it = list_iterator_new(timerIdList, LIST_HEAD);
  while ((node = list_iterator_next(it)))
  {
    timer = (jerry_timer_t *)node->val;
    if (timer->timerId == id)
    {
      jerry_release_value(timer->funcObj);
      xTimerStop(timer->timerHandler, NULL);
      xTimerDelete(timer->timerHandler, NULL);
      free(timer);
      list_remove(timerIdList, node);
      break;
    }
  }
  list_iterator_destroy(it);
  return jerry_create_undefined();
}

void module_timer_init()
{
  timerIdList = list_new();

  jerry_value_t global = jerry_get_global_object();

  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"setInterval");
  jerry_value_t value = jerry_create_external_function(setInterval_handler);
  jerry_release_value(jerry_set_property(global, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  prop_name = jerry_create_string((const jerry_char_t *)"clearInterval");
  value = jerry_create_external_function(clearTimer_handler);
  jerry_release_value(jerry_set_property(global, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  prop_name = jerry_create_string((const jerry_char_t *)"setTimeout");
  value = jerry_create_external_function(setTimeout_handler);
  jerry_release_value(jerry_set_property(global, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  prop_name = jerry_create_string((const jerry_char_t *)"clearTimeout");
  value = jerry_create_external_function(clearTimer_handler);
  jerry_release_value(jerry_set_property(global, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  jerry_release_value(global);
}