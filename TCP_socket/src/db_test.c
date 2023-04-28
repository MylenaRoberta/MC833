#include "database.h"

int main(void) {
    char *path = "../data/profiles.db";
    profile dummy;

    dummy.email = "jane_doe.com";
    dummy.first_name = "Jane";
    dummy.last_name = "Doe";
    dummy.location = "None";
    dummy.major = "Ciência da Computação";
    dummy.graduation_year = 2018;
    dummy.abilities = "None, Computação em Nuvem, Internet das Coisas";

    sqlite3 *db = open_db(path);
    result *res;
    initialize_db(db);

    // Teste das operações
    printf("\n---------------------------------------------\n");
    get_all_profiles(db, &res);
    print_result(&res);

    printf("\n---------------------------------------------\n");
    register_profile(db, dummy);

    printf("\n---------------------------------------------\n");
    get_all_profiles(db, &res);
    print_result(&res);

    printf("\n---------------------------------------------\n");
    printf("--- major: Ciência da Computação\n");
    get_profiles_from_major(db, &res, "Ciência da Computação");
    print_result(&res);

    printf("\n---------------------------------------------\n");
    printf("--- ability: Computação em Nuvem\n");
    get_profiles_from_ability(db, &res, "Computação em Nuvem");
    print_result(&res);

    printf("\n---------------------------------------------\n");
    printf("--- ability: Ciência de Dados\n");
    get_profiles_from_ability(db, &res, "Ciência de Dados");
    print_result(&res);

    printf("\n---------------------------------------------\n");
    printf("--- grad_year: 2016\n");
    get_profiles_from_graduation_year(db, &res, 2016);
    print_result(&res);

    printf("\n---------------------------------------------\n");
    printf("--- email: juliana.fernandes@outlook.com\n");
    get_profile(db, &res, "juliana.fernandes@outlook.com");
    print_result(&res);

    printf("\n---------------------------------------------\n");
    remove_profile(db, "juliana.fernandes@outlook.com");
    get_all_profiles(db, &res);
    print_result(&res);

    printf("\n---------------------------------------------\n");
    remove_profile(db, "juliana.fernandes@outlook.com");
    register_profile(db, dummy);

    printf("\n---------------------------------------------\n");
    get_all_profiles(db, &res);
    print_result(&res);

    close_db(db);

    return 0;
}
