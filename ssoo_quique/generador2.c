#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256

int COUNT_SU001 = 0;
int COUNT_SU002 = 0;
int COUNT_SU003 = 0;
int COUNT_SU004 = 0;
int DIA = 0;
int MES = 0;
int HORA1 = 0;
int HORA2 = 0;

char line[MAX_LINE_LENGTH];
int NUM_FILES_TOT = 0;

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
            if (strcmp(key, "NUM_FILES_TOT") == 0) {
                NUM_FILES_TOT = atoi(value);
            }
        }
    }
    
    printf("NUM FILES TOTAL: %d\n", NUM_FILES_TOT);

    // Cierra el fichero
    fclose(file);
}




void generate_line(char *OPE, char *DATE_START, char *DATE_END, char *USER, char *COMPRA, char *ITEM, int AMOUNT, char *STATUS) {
    printf("Operacion Creada: %s;%s;%s;%s;%s;%s;%d;%s\n", OPE, DATE_START, DATE_END, USER, COMPRA, ITEM, AMOUNT, STATUS);
}

void generate_file(char *PREFIX, int FILENUM) {
    char FILENAME[50];
    char *STATUS_OPTIONS[] = {"ERROR", "FINALIZADO", "CORRECTO", "CORRECTO", "FINALIZADO"};
    char *USER_OPTIONS[] = {"USER001", "USER002", "USER003"};
    MES =  (rand() % 12)+1;
    DIA = (rand() % 31)+1;
    HORA1 = (rand() % 60);
    HORA2 = (rand() % 60);
    char FECHA1[20];
    char FECHA2[20];
    sprintf(FECHA1, "%02d/%02d/2024 13:%02d", DIA, MES, HORA1);
    sprintf(FECHA2, "%02d/%02d/2024 15:%02d", DIA, MES, HORA2);

    switch (*PREFIX) {
        case '1':
            COUNT_SU001++;
            sprintf(FILENAME, "SU00%s_OPE%s_%02d%02d2024_%d.csv", PREFIX, PREFIX, DIA, MES, COUNT_SU001);
            break;
        case '2':
            COUNT_SU002++;
            sprintf(FILENAME, "SU00%s_OPE%s_%02d%02d2024_%d.csv", PREFIX, PREFIX, DIA, MES, COUNT_SU002);
            break;
        case '3':
            COUNT_SU003++;
            sprintf(FILENAME, "SU00%s_OPE%s_%02d%02d2024_%d.csv", PREFIX, PREFIX, DIA, MES, COUNT_SU003);
            break;
        case '4':
            COUNT_SU004++;
            sprintf(FILENAME, "SU00%s_OPE%s_%02d%02d2024_%d.csv", PREFIX, PREFIX, DIA, MES, COUNT_SU004);
            break;
        default:
            printf("Prefijo no reconocido: %s\n", PREFIX);
            return;
    }

    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error al abrir el archivo: %s\n", FILENAME);
        return;
    }

    FILE *log = fopen("file_log.log", "a");
    if (log == NULL) {
        printf("Error al abrir el archivo: 'file_log.log'\n");
        return;
    }

    fprintf(log,"----------------------------------------------------------------------------\n");

    for (int i = 1; i <= 10; i++) {
	HORA1 = (rand() % 60);
	HORA2 = (rand() % 60);
	sprintf(FECHA1, "%02d/%02d/2024 13:%02d", DIA, MES, HORA1);
	sprintf(FECHA2, "%02d/%02d/2024 15:%02d", DIA, MES, HORA2);

        int STATUS_INDEX = rand() % 5;
        int USER_INDEX = rand() % 3;
        char OPE[10], DATE_START[20], DATE_END[20], USER[8], COMPRA[10], ITEM[5], STATUS[20];
        sprintf(OPE, "OPE00%s", PREFIX);
        strcpy(DATE_START, FECHA1);
        strcpy(DATE_END, FECHA2);
        strcpy(USER, USER_OPTIONS[USER_INDEX]);
        sprintf(COMPRA, "COMPRA%02d", (i % 2)+1);
        sprintf(ITEM, "%d", i);
        int AMOUNT = rand() % 500 - 250;
        strcpy(STATUS, STATUS_OPTIONS[STATUS_INDEX]);
        generate_line(OPE, DATE_START, DATE_END, USER, COMPRA, ITEM, AMOUNT, STATUS);
        fprintf(file, "%s;%s;%s;%s;%s;%s;%d;%s\n", OPE, DATE_START, DATE_END, USER, COMPRA, ITEM, AMOUNT, STATUS);
        fprintf(log, "%s;%s;%s;%s;%s;%s;%d;%s\n", OPE, DATE_START, DATE_END, USER, COMPRA, ITEM, AMOUNT, STATUS); // print en el log
    }

    printf("Archivo generado: %s\n", FILENAME);
    fprintf(log,"----------------------------------------------------------------------------\n");
    fprintf(log,"Archivo generado: %s\n", FILENAME); // print en el log
    fprintf(log,"----------------------------------------------------------------------------\n\n");
    fclose(log);
    fclose(file);
}

void generate_files() {
    char *PREFIXES[] = {"1", "2", "3", "4"};
    int NUM_FILES = NUM_FILES_TOT;

    srand(time(NULL));

    for (int j = 1; j <= NUM_FILES; j++) {
        int PREFIX_INDEX = rand() % 4;
        char *PREFIX = PREFIXES[PREFIX_INDEX];
        generate_file(PREFIX, j);
    }
}

int main(){
    FILE *log = fopen("file_log.log", "w");
    if (log == NULL) {
        printf("Error al abrir el archivo: 'file_log.log'\n");
        return 0;
    }

    fprintf(log,"|-----------------------------------|\n");
    fprintf(log,"|-         ARCHIVO DE LOG          -|\n");
    fprintf(log,"|-----------------------------------|\n\n");

    fprintf(log,"|-----------------------------------|\n");
    fprintf(log,"|-        LOG DEL GENERADOR        -|\n");
    fprintf(log,"|-----------------------------------|\n\n");

    fclose(log);

    LeerConfig("fp.conf");

    generate_files();

    log = fopen("file_log.log", "a");
    if (log == NULL) {
        printf("Error al abrir el archivo: 'file_log.log'\n");
        return 0;
    }
    printf("Archivos con prefijo SU001_: %d\n", COUNT_SU001);
    printf("Archivos con prefijo SU002_: %d\n", COUNT_SU002);
    printf("Archivos con prefijo SU003_: %d\n", COUNT_SU003);
    printf("Archivos con prefijo SU004_: %d\n", COUNT_SU004);

    fprintf(log,"----------------------------------------------------------------------------\n\n");
    // print en el log
    fprintf(log,"Archivos con prefijo SU001_: %d\n", COUNT_SU001);
    fprintf(log,"Archivos con prefijo SU002_: %d\n", COUNT_SU002);
    fprintf(log,"Archivos con prefijo SU003_: %d\n", COUNT_SU003);
    fprintf(log,"Archivos con prefijo SU004_: %d\n\n\n", COUNT_SU004);
    fclose(log);


    return 1;
}
