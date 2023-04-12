#include "database.h"

int main(void)
{
    char *path = "../data/profiles.db";

    sqlite3 *db = open_db(path);
    initialize_db(db);

    // Queries
    get_all_profiles(db);
    //get_profiles_from_major(db, "'Ciência da Computação'");
    //get_profiles_from_ability(db, "'Ciência de Dados'");
    get_profiles_from_graduation_year(db, 2018);
    //get_profile(db, "'maria_souza@gmail.com'");

    close_db(db);

    return 0;
}
