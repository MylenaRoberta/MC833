typedef struct
{
    char* email;
    char* first_name;
    char* last_name;
    char* location;
    char* major;
    int graduation_year;
    char* abilities;
} profile;

// Função que registra um novo perfil
int register_profile(profile new_profile);

// Função que lista todas as informações de todos os perfis
char* get_all_profiles();

// Função que lista todos os perfis (email e nome) de um determinado curso
char* get_all_profiles_from_major(char* major);

// Função que lista todos os perfis (email e nome) que possuem uma determinada habilidade
char* get_all_profiles_from_ability(char* ability);

// Função que lista todos os perfis (email, nome e curso) de um determinado ano de formação
char* get_all_profiles_from_graduation_year(int year);

// Função que retorna todas as informações de um determinado perfil, dado seu email
char* get_profile(char* email);

// Função que remove um perfil a partir de seu email
char* remove_profile(char* email);