#include <libconfig.h>

extern void addEvent(config_t cfg, char name[30]);
extern void deleteEvent(config_t cfg, char name[30]);
extern void viewCalendar(config_t cfg, char name[30]);
extern void deleteCalendar(config_t cfg, char *name);
extern void EXIT();
