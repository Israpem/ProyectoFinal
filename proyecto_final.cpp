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
	PlayerNode *next;
	PlayerNode *prev;
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

int countCharacters(CharacterNode* head){ // cuenta nodos
    int count = 0;
    while (head != NULL){
        count++;
        head = head->next;
    }
    return count;
}

CharacterNode* getCharacterAtIndex(CharacterNode* head, int index){ // obtener personaje aleatorio
    int i = 0;
    while (head != NULL && i < index){
        head = head->next;
        i++;
    }
    return head;
}

//Prototypes
//General
void cleanBuffer();
//Player
Player createPlayer();
PlayerNode* createPlayerNode(Player);
void addPlayer(PlayerNode**);
void savePlayersToFile(PlayerNode*);
void printAllPlayers(PlayerNode*);
void loadCharactersFromFile(CharacterNode** head, const char* filename);
void assignRandomCharacters(PlayerNode* headPlayer, CharacterNode* headCharacter);

/*
......... MAIN .........................................................................................................................................................
*/
int main(){
	PlayerNode *headPlayer = NULL;
	addPlayer(&headPlayer);
	addPlayer(&headPlayer);
	addPlayer(&headPlayer);
	savePlayersToFile(headPlayer);
	printAllPlayers(headPlayer);
	CharacterNode* headCharacter = NULL;
    loadCharactersFromFile(&headCharacter, "characters.txt");
	return 0;
}

/*
......... PLAYER FUNCTIONS .............................................................................................................................................
*/
Player createPlayer(){
	Player newPlayer;
	newPlayer.id = 0; // change this line
	
	printf("\nDame el nombre del jugador:");
	fgets(newPlayer.name, MAX_LEN_STR, stdin);
	newPlayer.name[strcspn(newPlayer.name, "\n")] = 0;
	printf("\nDame el nickname sin espacios: ");
	scanf("%s", newPlayer.nickname);
	printf("\nDame edad: ");
	scanf("%d", &newPlayer.age);
	newPlayer.points = 0;
	
	cleanBuffer();
	return newPlayer;
}

PlayerNode* createPlayerNode(Player newPlayer){
    PlayerNode *newPlayerNode;
	newPlayerNode = (PlayerNode*)malloc(
							sizeof(PlayerNode));
    newPlayerNode->player = newPlayer;
    newPlayerNode->next = NULL;
    newPlayerNode->prev = NULL;
    return newPlayerNode;
}

void addPlayer(PlayerNode **headPlayer){
	Player newPlayer = createPlayer();
    PlayerNode *newPlayerNode = createPlayerNode(
									newPlayer);
    if(*headPlayer==NULL){
        *headPlayer = newPlayerNode;
        return;
    }
    PlayerNode *lastNode = *headPlayer;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    lastNode->next = newPlayerNode;
    newPlayerNode->prev = lastNode;
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

void loadCharactersFromFile(CharacterNode** head, const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir archivo de personajes");
        return;
    }

    char line[MAX_LEN_STR];
    int id = 1;
    while (fgets(line, sizeof(line), file)){
        line[strcspn(line, "\n")] = '\0'; // eliminar salto de línea
        Character c = {id++, "", 0};
        strncpy(c.name, line, MAX_LEN_STR);
        c.name[MAX_LEN_STR - 1] = '\0'; // seguridad
        addCharacter(head, c);
    }

    fclose(file);
}

/*
Clean buffer
*/
void cleanBuffer(){
	int c;
	while ((c = getchar()) != '\n' 
							&& c != EOF); 
}

/*
Save the players to a file with | as the separator.
*/
void savePlayersToFile(PlayerNode *headPlayer) {
    FILE* file = fopen("players.txt", "w");
    if (file == NULL) {
        perror("Error en archivo\n");
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
    printf("Jugadores guardados en 'players.txt'\n");
}

void printAllPlayers(PlayerNode *headPlayer){
	PlayerNode* current = headPlayer;
    while (current != NULL) {
    	printf("\nJUGADOR:\n");
        printf("Id: %d\n",current->player.id);
        printf("Nombre: %s\n",
						current->player.name);
		printf("Nickname: %s\n",
						current->player.nickname);
		printf("Edad: %d\n",
						current->player.age);
		printf("Puntos: %d\n",
						current->player.points);
        current = current->next;
    }
}

void assignRandomCharacters(PlayerNode* headPlayer, CharacterNode* headCharacter){
    int totalCharacters = countCharacters(headCharacter);
    if (totalCharacters == 0){
        printf("No hay personajes disponibles.\n");
        return;
    }

    // marcar personajes usados
    int* used = (int*)calloc(totalCharacters, sizeof(int));
    if (!used){
        perror("Error al asignar memoria");
        return;
    }

    srand(time(NULL));

    PlayerNode* currentPlayer = headPlayer;
    while (currentPlayer != NULL){
        int index;
        do{
            index = rand() % totalCharacters;
        } while (used[index]); // hasta encontrar uno no usado

        used[index] = 1; // marcar como usado

        CharacterNode* assignedChar = getCharacterAtIndex(headCharacter, index);
        printf("Jugador %s (Nickname: %s) ha sido asignado al personaje: %s\n",
               currentPlayer->player.name,
               currentPlayer->player.nickname,
               assignedChar->character.name);

        currentPlayer = currentPlayer->next;
    }

    free(used);
}
