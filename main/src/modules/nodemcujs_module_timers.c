#include "nodemcujs.h"
#include "nodemcujs_binding.h"
#include "nodemcujs_magic_strings.h"

#include "clist.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

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

JS_FUNCTION(SetIntervalHandler) {
  jerry_value_t jscallback = JS_GET_ARG(0, function);
  TickType_t interval = (TickType_t)JS_GET_ARG(1, number);

  if (timerId == 0)
  {
    timerId = 1;
  }
  jerry_timer_t *timer = (jerry_timer_t *)malloc(sizeof(struct jerry_timer_t));
  timer->repeat = true;
  // Acquire types with reference counter (increase the references).
  timer->funcObj = jerry_acquire_value(jscallback);
  timer->timerId = timerId;

  TimerHandle_t timerHandler = xTimerCreate("jtmr", interval / portTICK_PERIOD_MS, pdTRUE, (void *)timer->timerId, timeout_callback);
  timer->timerHandler = timerHandler;

  list_node_t *node = list_node_new(timer);

  list_rpush(timerIdList, node);

  xTimerStart(timerHandler, NULL);

  return jerry_create_number(timerId++);
}

JS_FUNCTION(SetTimeoutHandler) {
  jerry_value_t jscallback = JS_GET_ARG(0, function);
  TickType_t timeout = (TickType_t)JS_GET_ARG(1, number);

  if (timerId == 0)
  {
    timerId = 1;
  }
  jerry_timer_t *timer = (jerry_timer_t *)malloc(sizeof(struct jerry_timer_t));
  timer->repeat = false;
  // Acquire types with reference counter (increase the references).
  timer->funcObj = jerry_acquire_value(jscallback);
  timer->timerId = timerId;

  TimerHandle_t timerHandler = xTimerCreate("jtmr", timeout / portTICK_PERIOD_MS, pdTRUE, (void *)timer->timerId, timeout_callback);
  timer->timerHandler = timerHandler;

  list_node_t *node = list_node_new(timer);

  list_rpush(timerIdList, node);

  xTimerStart(timerHandler, NULL);

  return jerry_create_number(timerId++);
}

JS_FUNCTION(ClearTimerHandler) {
  uint8_t id = (uint8_t)JS_GET_ARG(0, number);

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

void nodemcujs_module_init_timers()
{
  timerIdList = list_new();

  jerry_value_t timer = jerry_create_object();

  nodemcujs_jval_set_method(timer, "setInterval", SetIntervalHandler);
  nodemcujs_jval_set_method(timer, NODEMCUJS_MAGIC_STRING_SETTIMEOUT, SetTimeoutHandler);
  nodemcujs_jval_set_method(timer, "clearInterval", ClearTimerHandler);
  nodemcujs_jval_set_method(timer, "clearTimeout", ClearTimerHandler);

  jerry_release_value(timer);
}