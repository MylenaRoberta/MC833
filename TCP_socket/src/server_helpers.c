#include "database.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

profile* create_profile(char* email, char* first_name, char* last_name, char* location, char* major, int graduation_year, char* abilities) {
    profile* new_profile = malloc(sizeof(profile));
;
    new_profile->abilities = malloc(sizeof(char*));
    new_profile->email = malloc(sizeof(char*));
    new_profile->first_name = malloc(sizeof(char*));
    new_profile->last_name = malloc(sizeof(char*));
    new_profile->location = malloc(sizeof(char*));
    new_profile->major = malloc(sizeof(char*));

    strcpy(new_profile->abilities, abilities);
    strcpy(new_profile->email, email);
    strcpy(new_profile->first_name, first_name);
    new_profile->graduation_year = graduation_year;
    strcpy(new_profile->last_name, last_name);
    strcpy(new_profile->location, location);
    strcpy(new_profile->major, major);

    return new_profile;
}

void free_profile(profile* deleted_profile) {
    free(deleted_profile->abilities);
    free(deleted_profile->email);
    free(deleted_profile->first_name);
    free(deleted_profile->last_name);
    free(deleted_profile->location);
    free(deleted_profile->major);

    free(deleted_profile);
}

int main(int argc, char const *argv[])
{
    return 0;
}
