*Includes all sensors, for measuring MTBF and power consumption


You can modify the ff depending on length of testing:
- esp_sleep_enable_timer_wakeup
- numberOfTestingHours
- measurementsPerHour
- totalRuntime

number of failures (numFailures) depends kung nag-fail sensor reading and pag-transmit ng data to ThingsBoard

Sa void loop(),
- checks testExpired and kung true, mag-iinfinite loop na siya after measuring MTBF
