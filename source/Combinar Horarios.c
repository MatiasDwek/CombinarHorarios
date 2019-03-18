#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMERO_DE_MATERIAS 10
#define HORAS_DIA_ESTUDIO (22 - 8)
#define INTERVALO 2
#define DIAS_ESTUDIO 6
#define LARGO_NOMBRE_MATERIAS (60 + 8 + TERMINADOR)
#define NUMERO_COMISIONES 10

#define TERMINADOR 1

enum estados {AFUERA, ADENTRO_NOMBRE_MATERIA, TITULOS, COMISION, HORARIOS, PROFESORES_CUPO};
enum dias_facu {LUNES = 0, MARTES, MIERCOLES, JUEVES, VIERNES, SABADO};

int verificar_dia(char c, FILE *p_horarios_r);
int combinar(int materia, int comision);
int validar_horarios(int materia, int comision);
void volcar(int materia, int comision);
void limpiar(int materia, int comision);
void imprimir(int materia, int comision);
int verificar_ymedia(int dia, int hora);
void buscar_comisiones(int *comisiones_usadas);


char tabla_horarios[NUMERO_DE_MATERIAS][NUMERO_COMISIONES][DIAS_ESTUDIO][INTERVALO * HORAS_DIA_ESTUDIO + 1];
char nombres_materias[NUMERO_DE_MATERIAS][LARGO_NOMBRE_MATERIAS];
char comisiones_materias[NUMERO_DE_MATERIAS][NUMERO_COMISIONES][5 + TERMINADOR];

int horario[DIAS_ESTUDIO][INTERVALO * HORAS_DIA_ESTUDIO + 1];

const char *dias[6] = {"Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
char dia_apuntado[10];

char comisiones_en_materias[NUMERO_DE_MATERIAS];

int materias_totales;
int cursa_sabado = 0;

FILE *p_imprimir;

int main()
{
    FILE *p_horarios_r;
    char c;
    char d;
    int exit = 0;
    int materia = 0, estado = AFUERA;
    int contador_nombres_materias = 0;
    int contador_comisiones_materias = 0;
    int contador_comisiones = -1;
    int contador_dias = 0;
    int combinaciones = 0;

    int numero_dia;
    int hora_inicial, hora_final;
    int hora1, hora2;
    int i;

    fpos_t position;


    if ((p_horarios_r = fopen("Materias.txt","r")) == NULL)
    {
        printf("Error en la apertura del archivo \"Materias\".\n");
        printf("Pulse enter para cerrar.\n");
        getchar();
        return 0;
    }


    if ((p_imprimir = fopen("Horarios posibles.txt","w")) == NULL)
    {
        printf("Error en la apertura del archivo \"Horarios posibles\".\n");
        printf("Pulse enter para cerrar.\n");
        getchar();
        return 0;
    }


    fprintf(p_imprimir, "Se encontraron %d combinaciones posibles:\n\n   ", combinaciones);

    while (((c = fgetc(p_horarios_r)) != EOF) && (materia <= 9) && (exit == 0))
    {
        switch (estado)
        {
        case AFUERA:
            //fgetpos (p_horarios_r, &position);
            if ((c != '\n') && (c != ' '))
            {
                estado = ADENTRO_NOMBRE_MATERIA;
                contador_nombres_materias = 0;
                contador_comisiones_materias = 0;
                contador_comisiones = -1;
                contador_dias = 0;
                nombres_materias[materia][contador_nombres_materias++] = c;
            }
            //fsetpos(p_horarios_r, &position);
            break;

        case ADENTRO_NOMBRE_MATERIA:
            fgetpos (p_horarios_r, &position);
            d = fgetc(p_horarios_r);
            if ((c == '\n') && ((d = fgetc(p_horarios_r)) != '\n'))
            {
                fsetpos(p_horarios_r, &position);
                if ((d == 'C') || (d == 'o'))
                {
                    fgetc(p_horarios_r); //Saltear \n que separa nombre de la materia de titulo
                    estado = TITULOS;
                }
                else
                {
                    fgetc(p_horarios_r);
                    estado = COMISION;
                }


            }
            else
            {
                fsetpos(p_horarios_r, &position);
                if (contador_nombres_materias < LARGO_NOMBRE_MATERIAS - TERMINADOR)
                    nombres_materias[materia][contador_nombres_materias++] = c;
                else
                {
                    if (contador_nombres_materias == LARGO_NOMBRE_MATERIAS - TERMINADOR)
                        nombres_materias[materia][contador_nombres_materias] = '\0';
                    else
                        printf("nombre de materia muy largo, sera truncado");
                }
            }
            break;

        case TITULOS:
            if (c == '\n')
                estado = COMISION;
            break;

        case COMISION:
            if (c == '\n')
            {
                estado = HORARIOS;
                contador_comisiones_materias = 0;
/**
                printf("%s\n", nombres_materias[materia]);
                printf("%s\n", comisiones_materias[materia][contador_comisiones]);
                printf("%d\n", materia);
**/
                contador_comisiones++;
            }
            else
            {
                if (contador_comisiones_materias < 5 + TERMINADOR - TERMINADOR)
                {
                    comisiones_materias[materia][contador_comisiones][contador_comisiones_materias++] = c;
                }

                else
                {
                    if (contador_comisiones_materias == 5 + TERMINADOR - TERMINADOR)
                        comisiones_materias[materia][contador_comisiones][contador_comisiones_materias] = '\0';
                    else
                        printf("nombre de la comision muy largo, sera truncado");
                }

            }
            break;

        case HORARIOS:
            while ((numero_dia = verificar_dia(c, p_horarios_r)) != -1)
            {
                hora1 = fgetc(p_horarios_r) - 48;
                hora2 = fgetc(p_horarios_r) - 48;
                hora_inicial = 2 * (hora1 * 10 + hora2 - 8);

                fgetc(p_horarios_r); //Saltear ':'
                if (fgetc(p_horarios_r) == '3')
                    hora_inicial++;

                for (i = 0; i < 4; i++) //Saltear "0 - "
                    fgetc(p_horarios_r);

                hora1 = fgetc(p_horarios_r) - 48;
                hora2 = fgetc(p_horarios_r) - 48;
                hora_final = 2 * (hora1 * 10 + hora2 - 8);

                fgetc(p_horarios_r); //Saltear ':'
                if (fgetc(p_horarios_r) == '3')
                    hora_final++;

                while ((c = fgetc(p_horarios_r)) != '\n')   //Saltear aula
                    ;

                i = 0;
                while (i < (hora_final - hora_inicial))
                {
                    tabla_horarios[materia][contador_comisiones][numero_dia][hora_inicial + i] = 1;
                    if (tabla_horarios[materia][contador_comisiones][SABADO][hora_inicial + i])
                        cursa_sabado = 1;
                    i++;
                }

                c = fgetc(p_horarios_r);
            }
/**
            for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
                printf("%d", tabla_horarios[materia][contador_comisiones][LUNES][i]);
            printf("\n");

            for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
                printf("%d", tabla_horarios[materia][contador_comisiones][MARTES][i]);
            printf("\n");

            for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
                printf("%d", tabla_horarios[materia][contador_comisiones][MIERCOLES][i]);
            printf("\n");

            for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
                printf("%d", tabla_horarios[materia][contador_comisiones][JUEVES][i]);
            printf("\n");

            for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
                printf("%d", tabla_horarios[materia][contador_comisiones][VIERNES][i]);
            printf("\n");
            printf("\n");
**/
            comisiones_en_materias[materia] = contador_comisiones + 1;

            while (((c = fgetc(p_horarios_r)) != '/') || (c != '/'))    //Saltear profesores
                ;

            while ((c = fgetc(p_horarios_r)) != '\n')   //Saltear cupo maximo
            {
                if (c == EOF)
                {
                    exit = 1;
                    break;
                }
            }

            fgetpos (p_horarios_r, &position);
            if ((c == EOF) || ((c = fgetc(p_horarios_r)) == EOF) || ((c = fgetc(p_horarios_r)) == EOF) || (c = fgetc(p_horarios_r)) == EOF || (c = fgetc(p_horarios_r)) == EOF)
                exit = 1;
            fsetpos(p_horarios_r, &position);

            fgetpos (p_horarios_r, &position);

            if ((((c = fgetc(p_horarios_r)) == '\n') || ((c >= 48) && (c <= 57))) || (c == ' '))
            {
                estado = AFUERA;
                materia++;
            }
            else
                estado = COMISION;

            fsetpos(p_horarios_r, &position);

            break;


        }
    }

    fclose(p_horarios_r);


    materias_totales = materia;
    combinaciones = combinar(0, 0);

    fprintf(p_imprimir, "\t\t\t\t\t\t\t\t\t(c) Matias Dwek 2013");

    rewind(p_imprimir);
    fprintf(p_imprimir, "Se encontraron %d combinaciones posibles:\n\n", combinaciones);
    fclose(p_imprimir);

    printf("Se encontraron %d combinaciones posibles.\nLos horarios fueron generados en el archivo \"Horarios posibles\".\n", combinaciones);
    printf("Pulse enter para cerrar.\n");
    getchar();

    return 0;
}



int verificar_dia(char c, FILE *p_horarios_r)
{
    int numero_dia = -1;
    int i = 0;

    dia_apuntado[i++] = c;
    while ((c != '/') && ((c = fgetc(p_horarios_r)) != ' ') && (i < 9))
    {
        dia_apuntado[i++] = c;
    }
    if (i == 9)
        dia_apuntado[i] = '\0';
    else
        dia_apuntado[i++] = '\0';/// REVISAR

    for (i = 0; i < 6; i++)
    {
        if (strcmp(dia_apuntado, *(dias + i)) == 0)
        {
            numero_dia = i;
        }
    }

    return numero_dia;
}

int combinar(int materia, int comision)
{

    static int comision_apuntada[NUMERO_DE_MATERIAS];   //Guarda la comision de las materias
    static int cantidad_de_combinaciones = 0;

    if (materia <= materias_totales)    //Si no es la ultima materia
    {
        if (comision < comisiones_en_materias[materia])
        {
            if (validar_horarios(materia, comision))    //Si no coinciden los horarios
            {
                volcar(materia, comision);  //Completa el arreglo con los horarios
                comision_apuntada[materia] = comision;  //Guardar comision
                combinar(materia + 1, 0);    //Pasar a siguiente materia
            }
            else
                combinar(materia, comision + 1);    //Avanzar a siguien comision
        }
        else
        {
            if ((materia - 1) >= 0)
            {
                limpiar(materia - 1, comision_apuntada[materia - 1]);
                combinar(materia - 1, comision_apuntada[materia - 1] + 1);  //Vovler a la materia anterior y pasar a la siguiente comsion de la materia anterior
            }
            else
            {
                return cantidad_de_combinaciones;
            }

        }


    }
    else    //Si es la ultima materia
    {
        cantidad_de_combinaciones++;
        imprimir(materia, comision);




        /**
                for (j = 0; j < DIAS_ESTUDIO - 1; j++)  //IMPRIMIR en pantalla
                {
                    for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
                    {
                        printf("%2d", horario[j][i]);
                    }
                    printf("\n");
                }
                printf("\n");
        **/



        limpiar(materia - 1, comision_apuntada[materia - 1]);
        combinar(materia - 1, comision_apuntada[materia - 1] + 1);  //Volver a la materia anterior y la comision siguiente
    }
    return cantidad_de_combinaciones;
}

int validar_horarios(int materia, int comision) //Devuelve 1 si se puede usar ese horario, 0 si ya esta ocupado
{
    int dia, hora, flag = 0;
    for (dia = 0; dia < DIAS_ESTUDIO; dia++)
    {
        for (hora = 0; hora < (INTERVALO * HORAS_DIA_ESTUDIO + 1); hora++)
        {
            if ((tabla_horarios[materia][comision][dia][hora] == 1) && (horario[dia][hora] != 0))
            {
                flag = 1;
                break;
            }
        }
        if (flag == 1)
            break;
    }
    return !flag;
}

void volcar(int materia, int comision)
{
    int dia, hora;
    for (dia = 0; dia < DIAS_ESTUDIO; dia++)
    {
        for (hora = 0; hora < (INTERVALO * HORAS_DIA_ESTUDIO + 1); hora++)
        {
            if (tabla_horarios[materia][comision][dia][hora] == 1)
                horario[dia][hora] = (materia + 1) * 100 + (comision + 1);
        }
    }
}

void limpiar(int materia, int comision)
{
    int dia, hora;
    for (dia = 0; dia < DIAS_ESTUDIO; dia++)
    {
        for (hora = 0; hora < (INTERVALO * HORAS_DIA_ESTUDIO + 1); hora++)
        {
            if (horario[dia][hora] == (materia + 1) * 100 + (comision + 1))
                horario[dia][hora] = 0;
        }
    }


    /*

        for (j = 0; j < DIAS_ESTUDIO; j++)  //IMPRIMIR en pantalla
        {
            for (i = 0; i < (INTERVALO * HORAS_DIA_ESTUDIO + 1); i++)
            {
                printf("%2d", horario[j][i]);
            }
            printf("\n");
        }
        printf("\n");
    */
}

void imprimir(int materia, int comision)
{
    int i, j, k, n, s;
    int dia;
    int hora_comienzo, hora_final;
    int flag;
    int comisiones_usadas[NUMERO_DE_MATERIAS];

    for (i = 0; i < NUMERO_DE_MATERIAS; i++)
        comisiones_usadas[i] = 0;

    if (cursa_sabado != 1)
    {
        fprintf(p_imprimir, "\t  ");
        for (i = 0; i < 5; i++)
        {
            fputc('|', p_imprimir);
            for (j = 0; j < 15; j++)
                fputc('¯', p_imprimir);
        }
        fputc('|', p_imprimir);
        fputc('\n', p_imprimir);

        fprintf(p_imprimir, "\t  |");
        fprintf(p_imprimir, "\tLunes\t  |");
        fprintf(p_imprimir, "\tMartes\t  |");
        fprintf(p_imprimir, "   Miércoles   |");
        fprintf(p_imprimir, "    Jueves\t  |");
        fprintf(p_imprimir, "    Viernes\t  |");
        fputc('\n', p_imprimir);

        fprintf(p_imprimir, "|¯¯¯¯¯¯¯¯¯");
        for (i = 0; i < 5; i++)
        {
            fputc('|', p_imprimir);
            for (j = 0; j < 15; j++)
                fputc('¯', p_imprimir);
        }
        fputc('|', p_imprimir);
        fputc('\n', p_imprimir);

        for(hora_comienzo = 8, hora_final = 9; hora_comienzo <= 21; hora_comienzo++, hora_final++)
        {
            if (hora_comienzo == 8)
                fprintf(p_imprimir, "|  %d - %d  |", hora_comienzo, hora_final);
            if (hora_comienzo == 9)
                fprintf(p_imprimir, "|  %d - %d |", hora_comienzo, hora_final);
            if (hora_comienzo == 10)
                fprintf(p_imprimir, "| %d - %d |", hora_comienzo, hora_final);
            if (hora_comienzo >= 11)
                fprintf(p_imprimir, "| %d - %d |", hora_comienzo, hora_final);

            for (dia = 0; dia < 5; dia++)
            {
                if (((horario[dia][2 * (hora_comienzo - 8)]) != 0) && (flag = verificar_ymedia(dia, 2 * (hora_comienzo - 8))) && (flag != 2))
                {
                    if ((nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][6] == '-') && (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][7] == ' '))
                        k = 8;
                    else
                        k = 0;
                    for (n = k; n < k + 14; n++)
                        fprintf(p_imprimir, "%c", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][n]);
                    if (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)] + 1) / 100) - 1][k + 15] == '\0')
                        fprintf(p_imprimir, "%c|", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][k + 14]);
                    else
                        fprintf(p_imprimir, ".|");
                }
                else
                {
                    if (!(verificar_ymedia(dia, 2 * (hora_comienzo - 8))) && (flag != 2))
                    {
                        fprintf(p_imprimir, "%d:30 ", hora_comienzo);
                        if ((nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8  + 1)]) / 100) - 1][6] == '-') && (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8 + 1)]) / 100) - 1][7] == ' '))
                            k = 8;
                        else
                            k = 0;
                        s = 8;
                        if (hora_comienzo <= 9)
                            s++;
                        for (n = k; n < k + s; n++)
                            fprintf(p_imprimir, "%c", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8) + 1]) / 100) - 1][n]);
                        if (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8) + 1]) / 100) - 1][k + 9] == '\0')
                            fprintf(p_imprimir, "%c|", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8) + 1]) / 100) - 1][k + s]);
                        else
                            fprintf(p_imprimir, ".|");
                    }
                    else
                        fprintf(p_imprimir, " \t\t  |");
                }
            }

            if (hora_comienzo < 21)
            {
                fputc('\n', p_imprimir);
                fprintf(p_imprimir, "|¯¯¯¯¯¯¯¯¯");
                for (dia = 0; dia < 5; dia++)
                {
                    fputc('|', p_imprimir);
                    for (j = 0; j < 15; j++)
                        fputc('¯', p_imprimir);
                }
                fputc('|', p_imprimir);
            }
            else
            {
                fputc('\n', p_imprimir);
                fprintf(p_imprimir, " ¯¯¯¯¯¯¯¯¯");
                for (dia = 0; dia < 5; dia++)
                {
                    fputc('¯', p_imprimir);
                    for (j = 0; j < 15; j++)
                        fputc('¯', p_imprimir);
                }
            }


            fputc('\n', p_imprimir);
        }


        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
        fprintf(p_imprimir, "Comisión\tMateria\n\n");
        buscar_comisiones(comisiones_usadas);
        for (i = 0; i < materia; i++)
            fprintf(p_imprimir, "%s\t\t%s\n", comisiones_materias[i][comisiones_usadas[i] - 1], nombres_materias[i]);

        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
        fprintf(p_imprimir, "			_________________________________________");
        fputc('\n', p_imprimir);
        fprintf(p_imprimir, "			¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯");
        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);

    }
    else
    {
        fprintf(p_imprimir, "\t  ");
        for (i = 0; i < 6; i++)
        {
            fputc('|', p_imprimir);
            for (j = 0; j < 15; j++)
                fputc('¯', p_imprimir);
        }
        fputc('|', p_imprimir);
        fputc('\n', p_imprimir);

        fprintf(p_imprimir, "\t  |");
        fprintf(p_imprimir, "\tLunes\t  |");
        fprintf(p_imprimir, "\tMartes\t  |");
        fprintf(p_imprimir, "   Miércoles   |");
        fprintf(p_imprimir, "    Jueves\t  |");
        fprintf(p_imprimir, "    Viernes\t  |");
        fprintf(p_imprimir, "\tSábado\t  |");
        fputc('\n', p_imprimir);

        fprintf(p_imprimir, "|¯¯¯¯¯¯¯¯¯");
        for (i = 0; i < 6; i++)
        {
            fputc('|', p_imprimir);
            for (j = 0; j < 15; j++)
                fputc('¯', p_imprimir);
        }
        fputc('|', p_imprimir);
        fputc('\n', p_imprimir);

        for(hora_comienzo = 8, hora_final = 9; hora_comienzo <= 21; hora_comienzo++, hora_final++)
        {
            if (hora_comienzo == 8)
                fprintf(p_imprimir, "|  %d - %d  |", hora_comienzo, hora_final);
            if (hora_comienzo == 9)
                fprintf(p_imprimir, "|  %d - %d |", hora_comienzo, hora_final);
            if (hora_comienzo == 10)
                fprintf(p_imprimir, "| %d - %d |", hora_comienzo, hora_final);
            if (hora_comienzo >= 11)
                fprintf(p_imprimir, "| %d - %d |", hora_comienzo, hora_final);

            for (dia = 0; dia < 6; dia++)
            {
                if (((horario[dia][2 * (hora_comienzo - 8)]) != 0) && (flag = verificar_ymedia(dia, 2 * (hora_comienzo - 8))) && (flag != 2))
                {
                    if ((nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][6] == '-') && (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][7] == ' '))
                        k = 8;
                    else
                        k = 0;
                    for (n = k; n < k + 14; n++)
                        fprintf(p_imprimir, "%c", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][n]);
                    if (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)] + 1) / 100) - 1][k + 15] == '\0')
                        fprintf(p_imprimir, "%c|", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8)]) / 100) - 1][k + 14]);
                    else
                        fprintf(p_imprimir, ".|");
                }
                else
                {
                    if (!(verificar_ymedia(dia, 2 * (hora_comienzo - 8))) && (flag != 2))
                    {
                        fprintf(p_imprimir, "%d:30 ", hora_comienzo);
                        if ((nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8  + 1)]) / 100) - 1][6] == '-') && (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8 + 1)]) / 100) - 1][7] == ' '))
                            k = 8;
                        else
                            k = 0;
                        s = 8;
                        if (hora_comienzo <= 9)
                            s++;
                        for (n = k; n < k + s; n++)
                            fprintf(p_imprimir, "%c", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8) + 1]) / 100) - 1][n]);
                        if (nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8) + 1]) / 100) - 1][k + 9] == '\0')
                            fprintf(p_imprimir, "%c|", nombres_materias[(int)((horario[dia][2 * (hora_comienzo - 8) + 1]) / 100) - 1][k + s]);
                        else
                            fprintf(p_imprimir, ".|");
                    }
                    else
                        fprintf(p_imprimir, " \t\t  |");
                }
            }

            if (hora_comienzo < 21)
            {
                fputc('\n', p_imprimir);
                fprintf(p_imprimir, "|¯¯¯¯¯¯¯¯¯");
                for (dia = 0; dia < 6; dia++)
                {
                    fputc('|', p_imprimir);
                    for (j = 0; j < 15; j++)
                        fputc('¯', p_imprimir);
                }
                fputc('|', p_imprimir);
            }
            else
            {
                fputc('\n', p_imprimir);
                fprintf(p_imprimir, " ¯¯¯¯¯¯¯¯¯");
                for (dia = 0; dia < 6; dia++)
                {
                    fputc('¯', p_imprimir);
                    for (j = 0; j < 15; j++)
                        fputc('¯', p_imprimir);
                }
            }


            fputc('\n', p_imprimir);
        }


        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
        fprintf(p_imprimir, "Comisión\tMateria\n\n");
        buscar_comisiones(comisiones_usadas);
        for (i = 0; i < materia; i++)
            fprintf(p_imprimir, "%s\t\t%s\n", comisiones_materias[i][comisiones_usadas[i] - 1], nombres_materias[i]);

        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
        fprintf(p_imprimir, "			_________________________________________");
        fputc('\n', p_imprimir);
        fprintf(p_imprimir, "			¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯");
        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
        fputc('\n', p_imprimir);
    }

}

int verificar_ymedia(int dia, int hora) //Devuelve 1 o 2 si no hay media hora y 0 si la hay
{
    int materia_y_comision = horario[dia][hora];
    int materia_y_comision_s = horario[dia][hora + 1];
    int contador = 0;

    if((materia_y_comision != 0) && (materia_y_comision_s == 0))
        return 2;

    if (((horario[dia][hora + 1]) != horario[dia][hora]))
        return 0;
    else
    {
        while (horario[dia][hora++] == materia_y_comision)
        {
            contador++;
        }
    }

    if (((contador % 2) != 0) && (materia_y_comision != 0))
        return 0;
    else
        return 1;
}

void buscar_comisiones(int *comisiones_usadas)
{
    int dia, hora;

    for (dia = 0; dia < DIAS_ESTUDIO; dia++)
    {
        for (hora = 0; hora < (INTERVALO * HORAS_DIA_ESTUDIO + 1); hora++)
        {
            if (horario[dia][hora] != 0)
            {
                if (comisiones_usadas[(int)(horario[dia][hora] / 100) - 1] == 0)
                    comisiones_usadas[(int)(horario[dia][hora] / 100) - 1] = horario[dia][hora] % 100 - 1;
            }
        }
    }
}
