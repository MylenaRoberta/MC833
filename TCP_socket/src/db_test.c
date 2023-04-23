#include "database.h"

int main(void) {
    char *path = "../data/profiles.db";
    profile dummy;

    dummy.email = "'jane_doe.com'";
    dummy.first_name = "'Jane'";
    dummy.last_name = "'Doe'";
    dummy.location = "'None'";
    dummy.major = "'None'";
    dummy.graduation_year = 2099;
    dummy.ability_a = "'None'";
    dummy.ability_b = "'None'";
    dummy.ability_c = "'None'";

    sqlite3 *db = open_db(path);
    initialize_db(db);

    // Queries
    get_all_profiles(db);

    register_profile(db, dummy);
    get_all_profiles(db);
    // remove_profile(db, "jane_doe.com");

    get_profiles_from_major(db, "Ciência da Computação");
    get_profiles_from_ability(db, "Computação em Nuvem");
    get_profiles_from_ability(db, "Ciência de Dados");
    get_profiles_from_graduation_year(db, 2018);
    get_profile(db, "maria_souza@gmail.com");

    remove_profile(db, "maria_souza@gmail.com");
    get_all_profiles(db);

    register_profile(db, dummy);
    get_all_profiles(db);

    close_db(db);

    return 0;
}
