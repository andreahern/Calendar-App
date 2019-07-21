#include <libconfig.h>

extern int validTime(config_setting_t *day, int startHour, int startMinute, int endHour, int endMinute);
extern int overlap(config_setting_t *day, int startHour, int startMinute, int endHour, int endMinute);
extern void sortDay(config_t cfg, config_setting_t *day, int n);
extern int convertToMinutes(int hour, int minute);