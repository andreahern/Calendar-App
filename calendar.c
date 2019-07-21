#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "options.h"

typedef enum { false, true } bool;
void startMenu(config_t cfg, char name[30]);
int loadConfig(config_t cfg, char* name);

int setUp() {
    config_t cfg;
    config_setting_t *setting;
    char name[30] = "";
    char ans = ' ';
    bool newCalendar = false;
    config_init(&cfg);

    if(! config_read_file(&cfg, "config.cfg"))
      {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }
    printf("Hello, what's your name? ");
    scanf("%s", name);

    setting = config_lookup(&cfg, name);
    if(setting != NULL) { 
        printf("Welcome back, %s\n", name); 
        startMenu(cfg, name);
    }
    else { 
        printf("You don't seem to have a calendar yet.\n");
        printf("Would you like to create a new calendar? [Y/N] ");
            scanf("%s", &ans);
            newCalendar = (ans == 'Y') ? true : false;
    }
    if(newCalendar) {
        loadConfig(cfg, name);
        startMenu(cfg, name);
    }
}

int loadConfig(config_t cfg, char name[30]) {
    config_setting_t *root, *setting, *day, *event;
    root = config_root_setting(&cfg);
    setting = config_setting_add(root, name, CONFIG_TYPE_GROUP);
    day = config_setting_add(setting, "MON", CONFIG_TYPE_LIST);
    day = config_setting_add(setting, "TUE", CONFIG_TYPE_LIST);
    day = config_setting_add(setting, "WED", CONFIG_TYPE_LIST);
    day = config_setting_add(setting, "THU", CONFIG_TYPE_LIST);
    day = config_setting_add(setting, "FRI", CONFIG_TYPE_LIST);
    day = config_setting_add(setting, "SAT", CONFIG_TYPE_LIST);
    day = config_setting_add(setting, "SUN", CONFIG_TYPE_LIST);
    config_write_file(&cfg, "config.cfg");
}

void startMenu(config_t cfg, char name[30]) {
    char strOption[30];
    int option = 0;
    viewCalendar(cfg, name);
   
    while(option != 5) {
        printf("\n\nMain Menu\n1. Add event\n2. Delete event\n3. View Calendar\n4. Delete Calendar\n5. Quit\n\n");
        scanf(" %s", strOption);
        option = atoi(strOption);

        switch (option)
        {
        case 1:
            addEvent(cfg, name);
            break;
        case 2:
            deleteEvent(cfg, name);
            break;
        case 3:
            viewCalendar(cfg, name);
            break;
        case 4:
            deleteCalendar(cfg, name);
                printf("\nWould you like to create a new calendar? [Y/N] ");
                char ans = ' ';
                scanf("%s", &ans);
                if(ans == 'Y') loadConfig(cfg, name);
                else EXIT(&cfg);

            break;
        case 5:
            config_destroy(&cfg);
            EXIT();
            break;     
        default:
            printf("Please enter a correct value.\n");
            break;
        }
    }
} 

int main() {
    setUp();
    return 0;
}