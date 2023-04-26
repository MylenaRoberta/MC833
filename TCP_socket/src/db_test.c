#include "database.h"

// void iterate_profile(result **res) {
//     // printf("EMAIL: %s\n", p.email);
//     // printf("NOME: %s\n", p.first_name);
//     // printf("SOBRENOME: %s\n", p.last_name);
//     // printf("RESIDÊNCIA: %s\n", p.location);
//     // printf("FORMAÇÃO: %s\n", p.major);
//     // printf("FORMATURA: %d\n", p.graduation_year);
//     // printf("HABILIDADES: %s\n", p.abilities);

//     result *r;

//     while(*res) {
//         r = *res;
//         printf("%s\n", r->row);
//         *res = r->next;
//         free(r->row);
//         free(r);
//     }
// }

// void iterate_profiles(profile ps[]) {
//     printf("\n");

//     for (int i = 0; i < PROFILES; i++) {
//         if (ps[i].email != NULL) {
//             printf("-------------------------------------------------\n");
//             iterate_profile(ps[i]);
//             printf("-------------------------------------------------\n");
//         } else {
//             printf("\n");
//             break;
//         }
//     }
// }

int main(void) {
    char *path = "../data/profiles.db";
    profile dummy;
    profile p = {NULL}, ps[PROFILES];
    
    // Initialize the profiles array
    for (int i = 0; i < PROFILES; i++) {
        ps[i] = p; // Dummy profile
    }

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
    get_all_profiles(db, &res);
    print_result(&res);
    // iterate_profiles(ps);

    // register_profile(db, dummy);
    // get_all_profiles(db, ps);
    // iterate_profiles(ps);

    // printf("--- major: Ciência da Computação\n");
    // get_profiles_from_major(db, ps, "Ciência da Computação");
    // iterate_profiles(ps);

    // printf("--- ability: Computação em Nuvem\n");
    // get_profiles_from_ability(db, ps, "Computação em Nuvem");
    // iterate_profiles(ps);

    // printf("--- ability: Ciência de Dados\n");
    // get_profiles_from_ability(db, ps, "Ciência de Dados");
    // iterate_profiles(ps);

    // printf("--- grad_year: 2018\n");
    // get_profiles_from_graduation_year(db, ps, 2018);
    // iterate_profiles(ps);

    // printf("--- email: maria_souza@gmail.com\n");
    // get_profile(db, ps, "maria_souza@gmail.com");
    // iterate_profiles(ps);

    // remove_profile(db, "maria_souza@gmail.com");
    // get_all_profiles(db, ps);
    // iterate_profiles(ps);

    // remove_profile(db, "maria_souza@gmail.com");
    // register_profile(db, dummy);

    // get_all_profiles(db, ps);
    // iterate_profiles(ps);

    close_db(db);

    return 0;
}
