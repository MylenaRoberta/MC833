#include "database.h"

void iterate_profile(profile p) {
    printf("EMAIL: %s\n", p.email);
    printf("NOME: %s\n", p.first_name);
    printf("SOBRENOME: %s\n", p.last_name);
    printf("RESIDÊNCIA: %s\n", p.location);
    printf("FORMAÇÃO: %s\n", p.major);
    printf("FORMATURA: %d\n", p.graduation_year);
    printf("HABILIDADE_A: %s\n", p.ability_a);
    printf("HABILIDADE_B: %s\n", p.ability_b);
    printf("HABILIDADE_C: %s\n", p.ability_c);
}

void iterate_profiles(profile ps[]) {
    for (int i = 0; i < 10; i++) {
        if (ps[i].email != NULL) {
            printf("-------------------------------------------------\n");
            iterate_profile(ps[i]);
            printf("-------------------------------------------------\n");
        } else {
            break;
        }
    }
}

int main(void) {
    char *path = "../data/profiles.db";
    profile p, dummy;
    profile ps[10];

    dummy.email = "jane_doe.com";
    dummy.first_name = "Jane";
    dummy.last_name = "Doe";
    dummy.location = "None";
    dummy.major = "Ciência da Computação";
    dummy.graduation_year = 2018;
    dummy.ability_a = "None";
    dummy.ability_b = "None";
    dummy.ability_c = "Internet das Coisas";

    sqlite3 *db = open_db(path);
    initialize_db(db);

    /* // Teste das operações
    get_all_profiles(db);

    register_profile(db, dummy);
    get_all_profiles(db);
    remove_profile(db, "jane_doe.com");

    get_profiles_from_major(db, "Ciência da Computação");
    get_profiles_from_ability(db, "Computação em Nuvem");
    get_profiles_from_ability(db, "Ciência de Dados");
    get_profiles_from_graduation_year(db, 2018);
    get_profile(db, "maria_souza@gmail.com");

    remove_profile(db, "maria_souza@gmail.com");
    get_all_profiles(db);

    register_profile(db, dummy);
    get_all_profiles(db);
    */

    get_all_profiles(db, ps);
    iterate_profiles(ps);
    iterate_profile(get_profile(db, "maria_souza@gmail.com"));

    close_db(db);

    return 0;
}
