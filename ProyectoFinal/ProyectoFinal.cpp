#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX_LEN_STR 50

typedef struct{
    int id;
    char name[MAX_LEN_STR];
    char nickname[MAX_LEN_STR];
    int age;
    int points;
}Player;

typedef struct PlayerNode{
    Player player;
    struct PlayerNode *next;
    struct PlayerNode *prev;
}PlayerNode;

//Para personajes
typedef struct{
    int id;
    char name[MAX_LEN_STR];
    int victories;
} Character;

typedef struct CharacterNode{
    Character character;
    struct CharacterNode *next;
    struct CharacterNode *prev;
} CharacterNode;

// Prototipos
void cleanBuffer();
Player createPlayer(int);
PlayerNode* createPlayerNode(Player);
void addPlayer(PlayerNode**, int*);
void savePlayersToFile(PlayerNode*);
void printAllPlayers(PlayerNode*);
void loadPlayersFromFile(PlayerNode** headPlayer, int* idCounter);
void loadCharactersFromFile(CharacterNode**, const char*);
void assignRandomCharacters(PlayerNode*, CharacterNode*);
int countCharacters(CharacterNode*);
CharacterNode* getCharacterAtIndex(CharacterNode*, int);
void updatePlayerPoints(PlayerNode* headPlayer, int winnerId);
void updateCharacterVictory(CharacterNode* headCharacter, const char* characterName);
void printTopPlayers(PlayerNode* headPlayer);
void printTopCharacters(CharacterNode* headCharacter);
void loadCharacterVictories(CharacterNode* headCharacter, const char* filename);
void saveCharacterVictories(CharacterNode* headCharacter, const char* filename);

// Menú
void menu();

// MAIN --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main() {
    PlayerNode* headPlayer = NULL;
    CharacterNode* headCharacter = NULL;
    int running = 1;
    int playerIdCounter = 1;

    // Cargar personajes y jugadores al inicio
    loadPlayersFromFile(&headPlayer, &playerIdCounter);
    loadCharactersFromFile(&headCharacter, "characters.txt");
    loadCharacterVictories(headCharacter, "partidas.txt");


    while (running) {
        printf("\n=== MENU ===\n");
        printf("1. Agregar jugadores\n");
        printf("2. Iniciar partida (asignar personajes)\n");
        printf("3. Consultar estadisticas\n");
        printf("4. Registrar ganador y actualizar estadisticas\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        
        int opcion;
        scanf("%d", &opcion);
        cleanBuffer();

        switch(opcion){
            case 1:
                addPlayer(&headPlayer, &playerIdCounter);
                break;
            case 2:
                assignRandomCharacters(headPlayer, headCharacter);
                break;
            case 3:
                printAllPlayers(headPlayer);
                printTopPlayers(headPlayer);
                printTopCharacters(headCharacter);
                break;
            case 4: {
                int ganadorId;
                char nombrePersonaje[MAX_LEN_STR];
                printf("Ingrese ID del jugador ganador (El ID se asigna automaticamente cuando se agrega un jugador): ");
                scanf("%d", &ganadorId);
                cleanBuffer();
                
                // Validar personaje
				int valido = 0;
				while (!valido){
				    printf("Ingrese el nombre EXACTO del personaje usado por el ganador: ");
				    fgets(nombrePersonaje, MAX_LEN_STR, stdin);
				    nombrePersonaje[strcspn(nombrePersonaje, "\n")] = 0;
				
				    // Buscar si existe en la lista
				    CharacterNode* currChar = headCharacter;
				    while (currChar != NULL) {
				        if (strcmp(currChar->character.name, nombrePersonaje) == 0) {
				            valido = 1;
				            break;
				        }
				        currChar = currChar->next;
				    }
				
				    if (!valido) {
				        printf("Personaje no encontrado. Verifica la ortografia exacta (usa mayusculas si es necesario)\n");
				    }
				}

                updatePlayerPoints(headPlayer, ganadorId);
                updateCharacterVictory(headCharacter, nombrePersonaje);
                break;
            }
            case 5:
                savePlayersToFile(headPlayer);
				saveCharacterVictories(headCharacter, "partidas.txt");

                printf("Gracias por usar el sistema :)\n");
                running = 0;
                break;

            default:
                printf("Opcion invalida\n");
        }
    }

    return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/* ---------- Funciones ---------- */

void cleanBuffer(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
}

Player createPlayer(int id){
    Player newPlayer;
    newPlayer.id = id;
    
    printf("\nDame el nombre del jugador: ");
    fgets(newPlayer.name, MAX_LEN_STR, stdin);
    newPlayer.name[strcspn(newPlayer.name, "\n")] = 0;

    printf("Dame el nickname sin espacios: ");
    scanf("%s", newPlayer.nickname);

    printf("Dame edad: ");
    scanf("%d", &newPlayer.age);
    newPlayer.points = 0;

    cleanBuffer();
    return newPlayer;
}

PlayerNode* createPlayerNode(Player newPlayer){
    PlayerNode* newNode = (PlayerNode*)malloc(sizeof(PlayerNode));
    newNode->player = newPlayer;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void addPlayer(PlayerNode **headPlayer, int* idCounter){
    Player newPlayer = createPlayer(*idCounter);
    (*idCounter)++;
    PlayerNode *newNode = createPlayerNode(newPlayer);

    if (*headPlayer == NULL) {
        *headPlayer = newNode;
        return;
    }

    PlayerNode* last = *headPlayer;
    while (last->next != NULL)
        last = last->next;

    last->next = newNode;
    newNode->prev = last;
}

void savePlayersToFile(PlayerNode *headPlayer) {
    FILE* file = fopen("players.txt", "w");
    if (!file) {
        perror("Error al guardar jugadores");
        return;
    }

    PlayerNode* current = headPlayer;
    while (current != NULL) {
        fprintf(file, "%d|%s|%s|%d|%d\n", 
            current->player.id,
            current->player.name,
            current->player.nickname,
            current->player.age,
            current->player.points);
        current = current->next;
    }

    fclose(file);
    printf("Jugadores guardados\n");
}

void printAllPlayers(PlayerNode *headPlayer){
    PlayerNode* current = headPlayer;
    if (!current) {
        printf("No hay jugadores registrados\n");
        return;
    }

    while (current != NULL) {
        printf("\nJUGADOR:\n");
        printf("Id: %d\n", current->player.id);
        printf("Nombre: %s\n", current->player.name);
        printf("Nickname: %s\n", current->player.nickname);
        printf("Edad: %d\n", current->player.age);
        printf("Puntos: %d\n", current->player.points);
        current = current->next;
    }
}

CharacterNode* createCharacterNode(Character newCharacter){
    CharacterNode* newNode = (CharacterNode*)malloc(sizeof(CharacterNode));
    newNode->character = newCharacter;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void addCharacter(CharacterNode** head, Character newCharacter){
    CharacterNode* newNode = createCharacterNode(newCharacter);
    if (*head == NULL){
        *head = newNode;
        return;
    }
    CharacterNode* last = *head;
    while (last->next != NULL){
        last = last->next;
    }
    last->next = newNode;
    newNode->prev = last;
}

void loadPlayersFromFile(PlayerNode** headPlayer, int* idCounter) {
    FILE* file = fopen("players.txt", "r");
    if (!file) {
        printf("No se encontro el archivo 'players.txt', iniciando sin jugadores.\n");
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        Player newPlayer;
        sscanf(line, "%d|%[^|]|%[^|]|%d|%d", 
               &newPlayer.id, 
               newPlayer.name, 
               newPlayer.nickname, 
               &newPlayer.age, 
               &newPlayer.points);

        PlayerNode* newNode = createPlayerNode(newPlayer);

        if (*headPlayer == NULL) {
            *headPlayer = newNode;
        } else {
            PlayerNode* last = *headPlayer;
            while (last->next != NULL)
                last = last->next;

            last->next = newNode;
            newNode->prev = last;
        }

        // Asegura que el contador de IDs continúe correctamente
        if (newPlayer.id >= *idCounter)
            *idCounter = newPlayer.id + 1;
    }

    fclose(file);
}

void loadCharactersFromFile(CharacterNode** head, const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir archivo de personajes");
        return;
    }

    char line[MAX_LEN_STR];
    int id = 1;
    while (fgets(line, sizeof(line), file)){
        line[strcspn(line, "\n")] = '\0';
        Character c = {id++, "", 0};
        strncpy(c.name, line, MAX_LEN_STR);
        c.name[MAX_LEN_STR - 1] = '\0';
        addCharacter(head, c);
    }

    fclose(file);
}

int countCharacters(CharacterNode* head){
    int count = 0;
    while (head != NULL){
        count++;
        head = head->next;
    }
    return count;
}

CharacterNode* getCharacterAtIndex(CharacterNode* head, int index){
    int i = 0;
    while (head != NULL && i < index){
        head = head->next;
        i++;
    }
    return head;
}

void assignRandomCharacters(PlayerNode* headPlayer, CharacterNode* headCharacter){
    int totalCharacters = countCharacters(headCharacter);
    if (totalCharacters == 0){
        printf("No hay personajes disponibles\n");
        return;
    }

    int* used = (int*)calloc(totalCharacters, sizeof(int));
    if (!used){
        perror("Error de memoria");
        return;
    }

    srand(time(NULL));
    PlayerNode* current = headPlayer;
    while (current != NULL){
        int index;
        do {
            index = rand() % totalCharacters;
        } while (used[index]);

        used[index] = 1;
        CharacterNode* character = getCharacterAtIndex(headCharacter, index);

        printf("Jugador %s (nickname: %s) ha sido asignado al personaje: %s\n",
               current->player.name,
               current->player.nickname,
               character->character.name);

        current = current->next;
    }

    free(used);
}

void updatePlayerPoints(PlayerNode* headPlayer, int winnerId) {
    PlayerNode* current = headPlayer;
    while (current != NULL) {
        if (current->player.id == winnerId) {
            current->player.points += 1;
            printf("Puntos actualizados para %s. Total: %d\n", current->player.nickname, current->player.points);
            return;
        }
        current = current->next;
    }
    printf("Jugador con ID %d no encontrado.\n", winnerId);
}

void updateCharacterVictory(CharacterNode* headCharacter, const char* characterName) {
    CharacterNode* current = headCharacter;
    while (current != NULL) {
        if (strcmp(current->character.name, characterName) == 0) {
            current->character.victories += 1;
            return;
        }
        current = current->next;
    }
}

void printTopPlayers(PlayerNode* headPlayer) {
    PlayerNode* array[100];
    int size = 0;

    PlayerNode* current = headPlayer;
    while (current != NULL && size < 100) {
        array[size++] = current;
        current = current->next;
    }

    for (int i = 0; i < size-1; ++i) {
        for (int j = i+1; j < size; ++j) {
            if (array[j]->player.points > array[i]->player.points) {
                PlayerNode* temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }

    printf("\nTop 10 jugadores con mas victorias:\n");
    for (int i = 0; i < size && i < 10; ++i) {
        printf("%d. %s (%s) - %d puntos\n", i+1, array[i]->player.name, array[i]->player.nickname, array[i]->player.points);
    }
}

void printTopCharacters(CharacterNode* headCharacter) {
    CharacterNode* array[100];
    int size = 0;

    CharacterNode* current = headCharacter;
    while (current != NULL && size < 100) {
        array[size++] = current;
        current = current->next;
    }

    for (int i = 0; i < size-1; ++i) {
        for (int j = i+1; j < size; ++j) {
            if (array[j]->character.victories > array[i]->character.victories) {
                CharacterNode* temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }

    printf("\nTop 5 personajes con mas victorias:\n");
    for (int i = 0; i < size && i < 5; ++i) {
        printf("%d. %s - %d victorias\n", i+1, array[i]->character.name, array[i]->character.victories);
    }
}

void loadCharacterVictories(CharacterNode* headCharacter, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Archivo de partidas no encontrado\n");
        return;
    }

    char line[MAX_LEN_STR];
    while (fgets(line, sizeof(line), file)) {
        char name[MAX_LEN_STR];
        int victories;
        sscanf(line, "%[^|]|%d", name, &victories);

        CharacterNode* current = headCharacter;
        while (current != NULL) {
            if (strcmp(current->character.name, name) == 0) {
                current->character.victories = victories;
                break;
            }
            current = current->next;
        }
    }

    fclose(file);
}

void saveCharacterVictories(CharacterNode* headCharacter, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("No se pudo guardar estadisticas de personajes");
        return;
    }

    CharacterNode* current = headCharacter;
    while (current != NULL) {
        fprintf(file, "%s|%d\n", current->character.name, current->character.victories);
        current = current->next;
    }

    fclose(file);
    printf("Estadisticas de personajes guardadas en '%s'\n", filename);
}
