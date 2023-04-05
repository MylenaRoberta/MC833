#include "operations.h"
#include <stdio.h>

int register_profile(profile new_profile) {
    FILE * file = fopen("../data/profiles.txt", "a");
    fputs("ASDAS", file);
    return 1;
}

char* get_all_profiles() {
    return "";
}

char* get_all_profiles_from_major(char* major) {
    return "";
}

char* get_all_profiles_from_ability(char* ability) {
    return "";
}

char* get_all_profiles_from_graduation_year(int year) {
    return "";
}

char* get_profile(char* email) {
    return "";
}

char* remove_profile(char* email) {
    return "";
}
