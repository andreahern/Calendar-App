#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "options.h"

int validTime(config_setting_t *day, int startHour, int startMinute, int endHour, int endMinute);
int overlap(config_setting_t *day, int startHour, int startMinute, int endHour, int endMinute);
void sortDay(config_t cfg, config_setting_t *day, int n);
int convertToMinutes(int hour, int minute);

int validTime(config_setting_t *day, int startHour, int startMinute, int endHour, int endMinute) {
    if ((startHour >= 0 && startHour < 24) && (startMinute >= 0 && startMinute < 60)) {
        if ((endHour >= 0 && endHour < 24) && (endMinute >= 0 && endMinute < 60)) {
            if(startHour < endHour) return overlap(day, startHour, startMinute, endHour, endMinute);
            else if(startMinute < endMinute) return overlap(day, startHour, startMinute, endHour, endMinute);
        }
    }
    return (0);
}

int overlap(config_setting_t *day, int startHour, int startMinute, int endHour, int endMinute) {
    config_setting_t *event, *array;
    for(int i = 0; i < config_setting_length(day); i++) {
        event = config_setting_get_elem(day, i);
        if(config_setting_length(event) == 0) continue;

        float startTime = convertToMinutes(startHour, startMinute);
        float endTime = convertToMinutes(endHour, endMinute);

        array = config_setting_get_member(event, "startTime");
        float cmpStartTime = convertToMinutes(config_setting_get_int_elem(array, 0), config_setting_get_int_elem(array, 1));

        array = config_setting_get_member(event, "endTime");
        float cmpEndTime = convertToMinutes(config_setting_get_int_elem(array, 0), config_setting_get_int_elem(array, 1));

        
        if(startTime >= cmpStartTime && startTime <= cmpEndTime) { 
            printf("You have overlapping times\n\n");
            return (0);
        }
        else if(startTime <= cmpStartTime && endTime >= cmpStartTime) {
            printf("You have overlapping times\n\n");
            return (0);
        }
    }
    return (1);    
}

void sortDay(config_t cfg, config_setting_t *day, int n) {
    if(n == 1) return;

    int i, j, startHour, startMinute, endHour, endMinute, time;
    int cmpStartHour, cmpStartMinute, cmpEndHour, cmpEndMinute;
    const char *tmpTitle, *tmpDescription, *tmpCmpTitle, *tmpCmpDescription;
    char title[150], description[150], cmpTitle[150], cmpDescription[150];
    config_setting_t *setting, *event, *array;

    for(i = 1; i < n; i++) {
        event = config_setting_get_elem(day, i);
    
        config_setting_lookup_string(event,"title", &tmpTitle);
        config_setting_lookup_string(event,"description", &tmpDescription);
        strcpy(title, tmpTitle);
        strcpy(description, tmpDescription);
        array = config_setting_get_member(event, "startTime");
        startHour = config_setting_get_int_elem(array, 0);
        startMinute = config_setting_get_int_elem(array, 1);
        array = config_setting_get_member(event, "endTime");
        endHour = config_setting_get_int_elem(array, 0);
        endMinute = config_setting_get_int_elem(array, 1);
        time = convertToMinutes(startHour, startMinute);
        j = i - 1;

        event = config_setting_get_elem(day, j);
        array = config_setting_get_member(event, "startTime");
        int cmpTime = convertToMinutes(config_setting_get_int_elem(array, 0), config_setting_get_int_elem(array, 1));

        while(j >= 0 && cmpTime > time) {
            config_setting_lookup_string(event,"title", &tmpCmpTitle);
            config_setting_lookup_string(event,"description", &tmpCmpDescription);
            strcpy(cmpTitle, tmpCmpTitle);  
            strcpy(cmpDescription, tmpCmpDescription);
            array = config_setting_get_member(event, "startTime");
            cmpStartHour = config_setting_get_int_elem(array, 0);
            cmpStartMinute = config_setting_get_int_elem(array, 1);
            array = config_setting_get_member(event, "endTime");
            cmpEndHour = config_setting_get_int_elem(array, 0);
            cmpEndMinute = config_setting_get_int_elem(array, 1);

            event = config_setting_get_elem(day, j + 1);
            config_setting_remove(event, "title");
            setting = config_setting_add(event, "title", CONFIG_TYPE_STRING);

            config_setting_remove(event, "description");
            setting = config_setting_add(event, "description", CONFIG_TYPE_STRING);

            config_setting_remove(event, "startTime");
            array = config_setting_add(event, "startTime", CONFIG_TYPE_ARRAY);
            setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
            config_setting_set_int(setting, cmpStartHour);
            setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
            config_setting_set_int(setting, cmpStartMinute);

            config_setting_remove(event, "endTime");
            array = config_setting_add(event, "endTime", CONFIG_TYPE_ARRAY);
            setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
            config_setting_set_int(setting, cmpEndHour);
            setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
            config_setting_set_int(setting, cmpEndMinute); 

            setting = config_setting_get_elem(event, 0);
            config_setting_set_string(setting, cmpTitle);
            setting = config_setting_get_elem(event, 1);
            config_setting_set_string(setting, cmpDescription);

            j = j - 1;

            if(j >= 0) {
                event = config_setting_get_elem(day, j);
                array = config_setting_get_member(event, "startTime");
                cmpTime = convertToMinutes(config_setting_get_int_elem(array, 0), config_setting_get_int_elem(array, 1));       
            }
        }
        event = config_setting_get_elem(day, j + 1);
        config_setting_remove(event, "title");
        setting = config_setting_add(event, "title", CONFIG_TYPE_STRING);

        config_setting_remove(event, "description");
        setting = config_setting_add(event, "description", CONFIG_TYPE_STRING);

        config_setting_remove(event, "startTime");
        array = config_setting_add(event, "startTime", CONFIG_TYPE_ARRAY);
        setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
        config_setting_set_int(setting, startHour);
        setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
        config_setting_set_int(setting, startMinute);

        config_setting_remove(event, "endTime");
        array = config_setting_add(event, "endTime", CONFIG_TYPE_ARRAY);
        setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
        config_setting_set_int(setting, endHour);
        setting = config_setting_add(array, NULL, CONFIG_TYPE_INT);
        config_setting_set_int(setting, endMinute);         

        setting = config_setting_get_elem(event, 0);
        config_setting_set_string(setting, title);
        setting = config_setting_get_elem(event, 1);
        config_setting_set_string(setting, description);
    }

    if(! config_write_file(&cfg, "config.cfg")) {
        fprintf(stderr, "Error while writing file.\n");
    }
}

int convertToMinutes(int hour, int minute) {
    return (hour * 60) + minute;
}