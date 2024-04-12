#include <stdio.h>
#include <stdlib.h>

int main() {
    // ELIMINAR el consolidado2.csv
    int resultado1 = system("rm -r *.csv");
    // Verificar si el comando se ejecutó correctamente
    if (resultado1 == 0) {
        printf("Archivo: 'consolidado.csv' ELIMINADO CORRECTAMENTE.\n");
    } else {
        printf("Archivo: 'consolidado.csv' NO HA SIDO ELIMINADO.\n");
    }

    // ELIMINAR TODOS LOS ELEMNTOS DENTRO DEL DIRECTORIO "procesados"
    int resultado2 = system("rm -r procesados/*");
    // Verificar si el comando se ejecutó correctamente
    if (resultado2 == 0) {
        printf("Archivos eliminados correctamente.\n");
    } else {
        printf("Error al eliminar archivos.\n");
    }

    system("tree");

    return 0;
}
