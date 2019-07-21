#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "checks.h"
#include "calendar.h"

typedef enum day {MON = 1, TUE = 2, WED = 3, THU = 4, FRI = 5, SAT = 6, SUN = 7} day;

void addEvent(config_t cfg, char name[30]);
void deleteCalendar(config_t cfg, char *name);
void viewCalendar(config_t cfg, char *name);
char* enumToString(enum day convert);

void addEvent(config_t cfg, char *name) {
    config_setting_t *root, *setting, *day, *event, *array;
    enum day addDay = 1;
    int startHour, startMinute, endHour, endMinute;
    char strAns[30], strStartHour[15], strStartMinute[15], strEndHour[15], strEndMinute[15];
    int  ans = 0;
    char title[30];
    char description[150];

    root = config_root_setting(&cfg);
    setting = config_setting_get_member(root, name); 

    do {
        printf("\nSelect a day to add an event to\n1. MON\t\t2. TUE\n3. WED\t\t4. THU\n5. FRI\t\t6. SAT\n7. SUN\n\n");
        scanf(" %s", strAns);
        ans = atoi(strAns);
    }   while (ans < 1 || ans > 7); 
    addDay = ans;
   
    day = config_setting_get_member(setting, enumToString(addDay));
    event = config_setting_add(day, "event", CONFIG_TYPE_GROUP);    

    printf("Title your event:\n\t");
    scanf(" %[^\n]%*c", title);

    printf("Write a description for your event:\n\t");
    scanf(" %[^\n]%*c", description);

    do {
    printf("Enter start and end time [HOUR : MINUTE - HOUR : MINUTE]\n\t");
    scanf("%s : %s - %s : %s", strStartHour, strStartMinute, strEndHour, strEndMinute);
    startHour = atoi(strStartHour);
    startMinute = atoi(strStartMinute);
    endHour = atoi(strEndHour);
    endMinute = atoi(strEndMinute);
    } while(!validTime(day, startHour, startMinute, endHour, endMinute));

    setting = config_setting_add(event, "title", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, title); 

    setting = config_setting_add(event, "description", CONFIG_TYPE_STRING);
    config_setting_set_string(setting, description);   

    array = config_setting_add(event, "startTime", CONFIG_TYPE_ARRAY);
    setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
    config_setting_set_int(setting, startHour);
    setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
    config_setting_set_int(setting, startMinute);

    array = config_setting_add(event, "endTime", CONFIG_TYPE_ARRAY);
    setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
    config_setting_set_int(setting, endHour);
    setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
    config_setting_set_int(setting, endMinute);


    if(! config_write_file(&cfg, "config.cfg")) {
        fprintf(stderr, "Error while writing file.\n");
    }

    fprintf(stderr, "\nUpdated configuration successfully written to: %s\n\n", "config.cfg");
    sortDay(cfg, day, config_setting_length(day));
}
void deleteEvent(config_t cfg, char *name) {
    config_setting_t *setting, *day, *event;
    char strAns[30], strDeleteEvent[30];
    int ans = 0, deleteEvent = 0;
    enum day deleteDay = 0;
    const char *title;
    int count = 0, prev = 0;
    setting = config_lookup(&cfg, name);
    do {

        if(count > prev) {
            printf("That day has no events to delete.\n");
            prev = count;
        }
        printf("\nSelect a day to delete an event from\n1. MON\t\t2. TUE\n3. WED\t\t4. THU\n5. FRI\t\t6. SAT\n7. SUN\t\t8. Back\n\n");
        scanf(" %s", strAns);
        ans = atoi(strAns);
        if(ans == 8) startMenu(cfg, name);
        if(ans > 1 && ans < 7 && config_setting_length(config_setting_get_elem(setting, ans - 1)) == 0) count++;
    }   while(ans < 1 || ans > 7 || config_setting_length(config_setting_get_elem(setting, ans - 1)) == 0); 
    deleteDay = ans;

    setting = config_lookup(&cfg, name);
    day = config_setting_get_elem(setting, deleteDay - 1);
    int eventCount = config_setting_length(day);
    printf("\nChoose an event to delete.\n");
    int i;
    for(i = 0; i < eventCount; i++) {
        event = config_setting_get_elem(day, i);
        config_setting_lookup_string(event, "title", &title);
        printf("%d. %s\n", i+1, title);
    }

    do {
        scanf(" %s", strDeleteEvent);
        deleteEvent = atoi(strDeleteEvent);
    } while(deleteEvent < 1  || deleteEvent > i);
    config_setting_remove_elem(day, deleteEvent - 1);

   if(! config_write_file(&cfg, "config.cfg")) {
        fprintf(stderr, "Error while writing file.\n");
    }

    fprintf(stderr, "\nUpdated configuration successfully written to: %s\n\n", "config.cfg");
}

void viewCalendar(config_t cfg, char *name) {
    config_setting_t *setting, *day, *event, *array;
    enum day printDay = 0;
    int maxCol = 0;
    const char *title;
    int startHour, startMinute, endHour, endMinute;
    const char *description;
    setting = config_lookup(&cfg, name);
    for(int i = 0; i < 7; i++) {
        day = config_setting_get_elem(setting, i);
        if(config_setting_length(day) > maxCol) maxCol = config_setting_length(day);
    }

    printf("%113s\n","Your Calendar");
    printf("%-35s%-35s%-35s%-35s%-35s%-35s%s\n\n", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN");
    int eventCount = 0;        

    for(int i = 0; i < maxCol; i++) {
        for(int n = 0; n < 7; n++) {
            day = config_setting_get_elem(setting, printDay);
            if(config_setting_length(day) == 0) { 
                printDay++; 
                printf("%-35s", "");
                continue;
            }
            if(config_setting_length(day) < maxCol && eventCount >= config_setting_length(day)) {
                printDay++;
                printf("%-35s", "");
                continue;
            }
            event = config_setting_get_elem(day, i);
            config_setting_lookup_string(event, "title", &title);

            printf("%-35.25s", title);
            printDay++;
        }

        printDay = 0;
        putchar('\n');
        for(int n = 0; n < 7; n++) {
            day = config_setting_get_elem(setting, printDay);
            if(config_setting_length(day) == 0) { 
                printDay++; 
                printf("%-35s", "");
                continue;
            }
            if(config_setting_length(day) < maxCol && eventCount >= config_setting_length(day)) {
                printDay++;
                printf("%-35s", "");
                continue;
            }
            event = config_setting_get_elem(day, i);
            config_setting_lookup_string(event, "description", &description);
            printf("%-35.25s", description);
            printDay++;
        } 
        printDay = 0;   
        putchar('\n');
        for(int n = 0; n < 7; n++) {
            day = config_setting_get_elem(setting, printDay);
            if(config_setting_length(day) == 0) { 
                printDay++; 
                printf("%-35s", "");
                continue;
            }
            if(config_setting_length(day) < maxCol && eventCount >= config_setting_length(day)) {
                printDay++;
                printf("%-35s", "");
                continue;
            }
            event = config_setting_get_elem(day, i);
            array = config_setting_get_member(event, "startTime");
            startHour = config_setting_get_int_elem(array, 0);
            startMinute = config_setting_get_int_elem(array, 1);
            array = config_setting_get_member(event, "endTime");
            endHour = config_setting_get_int_elem(array, 0);
            endMinute = config_setting_get_int_elem(array, 1);
            printf("%-.2d:%-.2d-%-.2d:%-26.2d", startHour, startMinute, endHour, endMinute);
            printDay++;
        }                 
        printf("\n\n");
        eventCount++;
        printDay = 0;    
    }
}

void deleteCalendar(config_t cfg, char *name) {
    config_setting_t *root;
    root = config_root_setting(&cfg);
    config_setting_remove(root, name);
   if(! config_write_file(&cfg, "config.cfg")) {
        fprintf(stderr, "Error while writing file.\n");
    }

    fprintf(stderr, "\nUpdated configuration successfully written to: %s\n\n", "config.cfg");
}

int EXIT() {
    printf("Program Closed.\n");
    exit(0);
}
 
char* enumToString(enum day convert) {
    switch (convert) {
        case 1:
            return "MON";
            break;
        case 2:
            return "TUE";
            break;
        case 3:
            return "WED";
            break;
        case 4:
            return "THU";
            break;  
        case 5:
            return "FRI";
            break;
        case 6:
            return "SAT";
            break;
        case 7:
            return "SUN";
            break;   
        default :
            return "MON";                                                               
    }
}

