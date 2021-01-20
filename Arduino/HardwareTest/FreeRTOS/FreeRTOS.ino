// the setup function runs once when you press reset or power the board
TaskHandle_t loop1Handle,loop2Handle;
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
void setup() {
  
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    loop1
    ,  "loop1"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  20  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &loop1Handle 
    ,  0);

  xTaskCreatePinnedToCore(
    loop2
    ,  "loop2"
    ,  1024  // Stack size
    ,  NULL
    ,  20  // Priority
    ,  &loop2Handle 
    ,  0);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

void loop1(void *pvParameters)  // This is a task.
{

  for (;;)
  {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed=1;
  TIMERG0.wdt_wprotect=0;
    Serial.println("loop1");
    Serial.println("loop1Done");
  }
}
void loop2(void *pvParameters)  // This is a task.
{

  for (;;)
  {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed=1;
  TIMERG0.wdt_wprotect=0;
    Serial.println("loop2");
    Serial.println("loop2Done");
  }
}
