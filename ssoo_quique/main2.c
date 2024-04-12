#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 256
#define MAX_FILENAME_LENGTH 512 // Aumentamos el tamaño del búfer

// Variables globales del MAIN
char line[MAX_LINE_LENGTH];
char PATH_FILES[MAX_LINE_LENGTH];
char INVENTORY_FILE[MAX_LINE_LENGTH];
char LOG_FILE[MAX_LINE_LENGTH];
int NUM_PROCESOS = 0;
int NUM_SUCURSALES = 0;
int NUM_FILES_TOT = 0;
int SIMULATE_SLEEP = 0;

// Definimos el semáforo
pthread_mutex_t mutex;

void LeerConfig(const char *nombreArchivo) {
    char *key, *value;
    FILE *file;

    // Abre el fichero de properties
    file = fopen(nombreArchivo, "r");
    if (!file) {
        perror("No se pudo abrir el archivo 'fp.conf'.");
        exit(EXIT_FAILURE);
    }

    // Lee el fichero línea por línea
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        // Elimina el salto de línea al final, si existe
        line[strcspn(line, "\n")] = 0;

        // Divide la línea en clave y valor
        key = strtok(line, "=");
        value = strtok(NULL, "=");

        // Comprueba si la clave y el valor son válidos y guarda los datos
        if (key && value) {
            if (strcmp(key, "PATH_FILES") == 0) {
                strncpy(PATH_FILES, value, MAX_LINE_LENGTH);
            } else if (strcmp(key, "INVENTORY_FILE") == 0) {
                strncpy(INVENTORY_FILE, value, MAX_LINE_LENGTH);
            } else if (strcmp(key, "LOG_FILE") == 0) {
                strncpy(LOG_FILE, value, MAX_LINE_LENGTH);
            } else if (strcmp(key, "NUM_PROCESOS") == 0) {
                NUM_PROCESOS = atoi(value);
            } else if (strcmp(key, "NUM_SUCURSALES") == 0) {
                NUM_SUCURSALES = atoi(value);
            } else if (strcmp(key, "NUM_FILES_TOT") == 0) {
                NUM_FILES_TOT = atoi(value);
            } else if (strcmp(key, "SIMULATE_SLEEP") == 0) {
                SIMULATE_SLEEP = atoi(value);
            }
        }
    }
    // Cierra el fichero
    fclose(file);

    printf("PATH FILE: %s\n", PATH_FILES);
    printf("INVENTORY FILES: %s\n", INVENTORY_FILE);
    printf("LOG FILE: %s\n", LOG_FILE);
    printf("NUM PROCESOS: %d\n", NUM_PROCESOS);
    printf("NUM SUCURSALES: %d\n", NUM_SUCURSALES);
    printf("NUM FILES TOTAL: %d\n", NUM_FILES_TOT);
    printf("SIMULATE SLEEP: %d\n", SIMULATE_SLEEP);

    FILE* log;
    // Abrir el archivo fil_log para escritura
    log = fopen("file_log.log", "a");
    if (log == NULL) {
        perror("Error al abrir el archivo LOG");
        exit(EXIT_FAILURE);
    }

    // Imprime las variables para verificar
    fprintf(log,"|-----------------------------------|\n");
    fprintf(log,"|-          LOG DEL MAIN           -|\n");
    fprintf(log,"|-----------------------------------|\n\n");

    fprintf(log,"------------------------------------\n");
    fprintf(log,"------ LECTURA DEL 'fp.conf '-------\n");
    fprintf(log,"  PATH FILE: %s\n", PATH_FILES);
    fprintf(log,"  INVENTORY FILES: %s\n", INVENTORY_FILE);
    fprintf(log,"  LOG FILE: %s\n", LOG_FILE);
    fprintf(log,"  NUM PROCESOS: %d\n", NUM_PROCESOS);
    fprintf(log,"  NUM SUCURSALES: %d\n", NUM_SUCURSALES);
    fprintf(log,"  NUM FILES TOTAL: %d\n", NUM_FILES_TOT);
    fprintf(log,"  SIMULATE SLEEP: %d\n", SIMULATE_SLEEP);
    fprintf(log,"------------------------------------\n\n");

    printf("\n Despues escribir en el file_log.log (LeerConfig).");
    fclose(log);
}

int ContarFicheros(){
    int pillado = 1;
    int iteraciones = 20;
    while(pillado == 1){
        DIR *dir;
        struct dirent *entry;

        // Abrir el directorio actual
        dir = opendir(".");
        if (dir == NULL) {
            perror("Error al abrir el directorio");
            exit(EXIT_FAILURE);
        }

        while ((entry = readdir(dir))) {
            if ((strstr(entry->d_name, "SU")) != NULL) { // Comprobamos si el fichero tiene en el nombre la declaracion del tipo .csv
                NUM_FILES_TOT++;
            }
        }
        
        closedir(dir);
        if(NUM_FILES_TOT != 0){
            iteraciones--;
            printf("iteracion %d", iteraciones);
            if(iteraciones == 0){
                return 1;
            }
            break;
        }
    }
}
// ----------------------- PROCESAR FICHEROS -------------


void copy_file_content(const char *input_filename, FILE *output_file) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Error al abrir archivo de entrada <- copy_file_content");
        exit(EXIT_FAILURE);
    }
    
    int c;
    while ((c = fgetc(input_file)) != EOF) {
        fputc(c, output_file);
    }

    fclose(input_file);
}


void *Procesador(void *arg){
    int exitazo = 1;

    FILE* log;
    // Abrir el archivo fil_log para escritura
    log = fopen("file_log.log", "a");
    if (log == NULL) {
        perror("Error al abrir el archivo LOG");
        exit(EXIT_FAILURE);
    }

    while(exitazo == 1){
        // Bloqueamos el mutex para los hilos
        pthread_mutex_lock(&mutex);
        fprintf(log,"-------------------------------------------------------\n");
        fprintf(log,"             BLOQUEAMOS EL MUTEX \n");

        int numeroHilo = *((int *) arg); // Convertir el argumento a entero
        printf("\n  --- Hilo %d ejecutandose --- \n", numeroHilo);

        // Escribimos en el log
        fprintf(log,"-------------------------------------------------------\n");
        fprintf(log,"-> Hilo %d ejecutandose -\n", numeroHilo);

        // Para manejar el directiorio y los elementos en el usamos lo siguiente
        DIR *dir;
        struct dirent *entry;
        FILE *consolidated_file;
        char filename[MAX_FILENAME_LENGTH];
        char filename_copy[MAX_FILENAME_LENGTH];

        // Abrir el directorio actual
        dir = opendir(".");
        if (dir == NULL) {
            perror("Error al abrir el directorio");
            exit(EXIT_FAILURE);
        }

        // Abrir el archivo consolidado para escritura
        consolidated_file = fopen("consolidado.csv", "a");
        if (consolidated_file == NULL) {
            perror("Error al abrir el archivo consolidado");
            exit(EXIT_FAILURE);
        }

        // Leer archivos del directorio actual
        while ((entry = readdir(dir))) {
            if ((strstr(entry->d_name, "SU")) != NULL) { // Comprobamos si el fichero tiene en el nombre la declaracion del tipo .csv
                // Copiar contenido del archivo de entrada al archivo consolidado
                strcpy(filename, entry->d_name);
                printf("\n --> Procesando el fichero: %s\n", filename);

                fprintf(log,"-> Procesando el fichero: %s\n", filename);
                
                copy_file_content(entry->d_name, consolidated_file);

                strcpy(filename_copy, filename); // copiamos solo el nombre del fichero, para tener luego el nombre y la ruta final en dos variables separadas para el log

                // Mover archivo procesado al directorio "procesados"
                sprintf(filename, "procesados/%s", entry->d_name);
                fprintf(log,"-> Mover el fichero: '%s' a la \n  -> Ruta Final: './%s' \n", filename_copy, filename); // la primera variable es el nombre copiado, la segunda es la direccion/ruta final
                NUM_FILES_TOT--;
                printf( " ---------------------->   VALOR DE NUMFILES %d", NUM_FILES_TOT);

                if (rename(entry->d_name, filename) != 0) {
                    NUM_FILES_TOT++;
                    perror("Error al mover archivo procesado");
                    exit(EXIT_FAILURE);
                }else{
                    break;
                }
            }
            
        }

        // Cerrar directorio
        closedir(dir);

        // Cerrar archivo consolidado
        fclose(consolidated_file);

        printf("\n\n -- El HILO %d ha completado el Procesamiento -- \n\n", numeroHilo);
        fprintf(log,"-> El HILO %d ha completado el Procesamiento \n", numeroHilo);
        fprintf(log,"-------------------------------------------------------\n");

        fprintf(log,"    RECUENTO DE FICHEROS: \n");


        if(NUM_FILES_TOT == 0){
            printf(" => NUMERO TOTAL DE FICHEROS %i = 0\n\n", NUM_FILES_TOT);
            fprintf(log," - Ya no quedan ficheros sin procesar: \n => NUMERO TOTAL DE FICHEROS %i = 0 \n", NUM_FILES_TOT);
            fprintf(log,"-------------------------------------------------------\n");
            
            int result = ContarFicheros();

            if (result == 1){
                exitazo++;
            }

        }else{
            fprintf(log," => NUMERO TOTAL DE FICHEROS RESTANTES: %i \n", NUM_FILES_TOT);
            fprintf(log,"-------------------------------------------------------\n");
        }

        // Desbloqueamos el mutex para el siguiente hilo
        fprintf(log,"             DESBLOQUEAMOS EL MUTEX \n");
        fprintf(log,"-------------------------------------------------------\n\n");
        pthread_mutex_unlock(&mutex); 
    }
    fclose(log);
    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    return NULL;
}



// ---------------------- MAIN ------------------------



int main() {
    LeerConfig("fp.conf");

    // crea tantos hilos como procesos haya
    pthread_t hilos[NUM_PROCESOS];

    pthread_mutex_init(&mutex, NULL);

    int numfiles = NUM_FILES_TOT;

    for (int i = 0; i<= NUM_PROCESOS; i++){
        int *index = (int *)malloc(sizeof(int)); // reserva memoria
        *index = i;
        pthread_create(&hilos[i], NULL, Procesador, (void*)index);
    }

    for (int g = 0; g<= NUM_PROCESOS; g++){
        pthread_join(hilos[g], NULL);
        printf("\n  --- FIN de hilo %d ejecutandose --- \n", g);
    }

    // Destruimos el semáforo
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}
