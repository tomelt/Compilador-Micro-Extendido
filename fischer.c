#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define NUMESTADOS 21
#define NUMCOLS 15
#define TAMLEX 33
#define TAMNOM 21

FILE *in;
typedef enum {
    INICIO,
    FIN,
    LEER,
    ESCRIBIR,
    ID,
    CONSTANTE,
    CARACTER,
    REAL,
    MIENTRAS,
    SI,
    REPETIR,
    HASTA,
    PARENIZQUIERDO,
    PARENDERECHO,
    PUNTOYCOMA,
    COMA,
    ASIGNACION,
    SUMA,
    RESTA,
    FDT,
    ERRORLEXICO
} TOKEN;


typedef struct {
char identifi[TAMLEX];
TOKEN t;
} RegTS;


RegTS TS[1000] = {
    {"inicio", INICIO},
    {"fin", FIN},
    {"leer", LEER},
    {"escribir", ESCRIBIR},
    {"caracter", CARACTER},
    {"real", REAL},
    {"mientras", MIENTRAS},
    {"si", SI},
    {"repetir", REPETIR},
    {"hasta", HASTA},
    {"$", 99}
};

typedef struct {
    TOKEN clase;
    char nombre[TAMLEX];
    double valor;
} REG_EXPRESION;

char buffer[TAMLEX];
TOKEN tokenActual;
int flagToken = 0;
TOKEN scanner(void);

int columna(int c);
int estadoFinal(int e);
void Objetivo(void);
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void ListaIdentificadores(void);
void Identificador(REG_EXPRESION *presul);

void ListaExpresiones(void);
void Condicion(void);
void Expresion(REG_EXPRESION *presul);
void Primaria(REG_EXPRESION *presul);
void OperadorAditivo(char *presul);
REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
char *ProcesarOp(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2);
void Match(TOKEN t);
TOKEN ProximoToken(void);
void ErrorLexico(void);
void ErrorSintactico(void);
void Generar(char *co, char *a, char *b, char *c);
char *Extraer(REG_EXPRESION *preg);
int Buscar(char *id, RegTS *TS, TOKEN *t);
void Colocar(char *id, RegTS *TS);
void Chequear(char *s);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);

/******** Programa principal ********/
int main(int argc, char *argv[]) {
    TOKEN tok;
    char nomArchi[TAMNOM];
    int l;
    /***************************Se abre el Archivo Fuente******************/
// verifica errores posibles
    if (argc == 1)
    {
        printf("Debe ingresar el nombre del archivo fuente (en lenguaje Micro) en la linea de comandos\n");
        return -1;
        } // no puso nombre de archivo fuente
        if (argc != 2)
        {
        printf("Numero incorrecto de argumentos\n");
        return -1;

    } // los argumentos deben ser 2
strcpy(nomArchi, argv[1]);
l = strlen(nomArchi);
if (l > TAMNOM)
{
printf("Nombre incorrecto del Archivo Fuente\n");
return -1;
}
if (nomArchi[l - 1] != 'm' || nomArchi[l - 2] != '.')
{
printf("Nombre incorrecto del Archivo Fuente\n");
return -1;
}
if ((in = fopen(nomArchi, "r")) == NULL)
{
printf("No se pudo abrir archivo fuente\n");
return -1; // no pudo abrir archivo
}
Objetivo();
fclose(in);
return 0;
}
void Objetivo(void) {
Programa();
Match(FDT);
Terminar();
}
void Programa(void) {
Comenzar();
Match(INICIO);
ListaSentencias();
Match(FIN);
}
void ListaSentencias(void) {
    Sentencia();
    while (1) {
        switch (ProximoToken()) {
    case ID:
    case LEER:
    case ESCRIBIR:
    case MIENTRAS:
    case SI:
    case REPETIR:
    Sentencia();
    break;
    default: return;
}
}
}
void Sentencia(void) {
TOKEN tok = ProximoToken();
REG_EXPRESION izq, der;
switch (tok) {
case ID:
Identificador(&izq);
Match(ASIGNACION);
Expresion(&der);
Asignar(izq, der);
Match(PUNTOYCOMA);
break;
case LEER:
Match(LEER);
Match(PARENIZQUIERDO);
ListaIdentificadores();
Match(PARENDERECHO);
Match(PUNTOYCOMA);
break;
case ESCRIBIR:
Match(ESCRIBIR);
Match(PARENIZQUIERDO);
ListaExpresiones();
Match(PARENDERECHO);
Match(PUNTOYCOMA);
break;
case REPETIR:
Match(REPETIR);
printf("Reconocio una sentencia DO WHILE\n");
Sentencia();
Match(HASTA);
Condicion();
Match(PUNTOYCOMA);

break;
case SI:
Match(SI);
Match(PARENIZQUIERDO);
Condicion();
Match(PARENDERECHO);
printf("Reconocio una sentencia IF\n");
Sentencia();
break;
case MIENTRAS: /* WHILE */
Match(MIENTRAS);
Match(PARENIZQUIERDO);
Condicion();
Match(PARENDERECHO);
printf("Reconocio una sentencia WHILE\n");
Sentencia();
break;
default: return;
}
}
void ListaIdentificadores(void) {
TOKEN t;
REG_EXPRESION reg;
Identificador(&reg);
Leer(reg);
for (t = ProximoToken(); t == COMA; t = ProximoToken()) {
Match(COMA);
Identificador(&reg);
Leer(reg);
}
}
void Identificador(REG_EXPRESION *presul) {
Match(ID);
presul->clase = ID;
strcpy(presul->nombre, buffer);
presul->valor = 0;
}
void ListaExpresiones(void) {
TOKEN t;
REG_EXPRESION reg;
Expresion(&reg);
Escribir(reg);
for (t = ProximoToken(); t == COMA; t = ProximoToken()) {
Match(COMA);
Expresion(&reg);
Escribir(reg);
}
}
void Condicion(void) {
TOKEN t;
REG_EXPRESION reg;
Expresion(&reg);
for (t = ProximoToken(); t == COMA; t = ProximoToken()) {
Match(COMA);
Expresion(&reg);
}
}
void Expresion(REG_EXPRESION *presul) {
REG_EXPRESION operandoDer, operandoIzq;
char op[TAMLEX];
TOKEN t;
Primaria(&operandoIzq);
for (t = ProximoToken(); t == SUMA || t == RESTA; t =
ProximoToken()) {
OperadorAditivo(op);
Primaria(&operandoDer);
operandoIzq = GenInfijo(operandoIzq, op, operandoDer);
}
*presul = operandoIzq;
}
void Primaria(REG_EXPRESION *presul) {
TOKEN tok = ProximoToken();
switch (tok) {
case ID:
Identificador(presul);
break;
case CONSTANTE:
Match(CONSTANTE);
*presul = ProcesarCte();
break;
case PARENIZQUIERDO:
Match(PARENIZQUIERDO);

Expresion(presul);
Match(PARENDERECHO);
break;
default: return;
}
}
void OperadorAditivo(char *presul) {
TOKEN t = ProximoToken();
if (t == SUMA || t == RESTA)
{
Match(t);
strcpy(presul, ProcesarOp());
}
else
ErrorSintactico();
}
/******** Semántica ********/
REG_EXPRESION ProcesarCte(void) {
REG_EXPRESION reg;
if (buffer[0]=='\'' && buffer[2]=='\'' && buffer[3]=='\0') {
reg.clase = CARACTER;
reg.nombre[0] = buffer[1];
reg.nombre[1] = '\0';
reg.valor = (double)buffer[1];
return reg;
}
strcpy(reg.nombre, buffer);
sscanf(buffer, "%lf", &reg.valor);
reg.clase = (strchr(buffer, '.') ? REAL : CONSTANTE);
return reg;
}
REG_EXPRESION ProcesarId(void){
REG_EXPRESION reg;
reg.clase=ID;
strcpy(reg.nombre,buffer);
reg.valor=0;
return reg;
}
char *ProcesarOp(void){
return buffer;

}
void Leer(REG_EXPRESION in){
switch(in.clase){
case CARACTER:
Generar("Read", in.nombre, "Char", "");
break;
case REAL:
Generar("Read", in.nombre, "Float", "");
break;
default:
Generar("Read", in.nombre, "Entera", "");
break;
}
}
void Escribir(REG_EXPRESION out){
switch(out.clase){
case CARACTER:
Generar("Write", Extraer(&out), "Char", "");
break;
case REAL:
Generar("Write", Extraer(&out), "Float", "");
break;
default:
Generar("Write", Extraer(&out), "Entera", "");
break;
}
}
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2){
REG_EXPRESION reg;
static unsigned int numTmp = 1;
char cadTemp[TAMLEX]="Temp&";
char cadNum[TAMLEX];
char cadOp[TAMLEX];
if (op[0]=='-') strcpy(cadOp,"Restar");
if (op[0]=='+') strcpy(cadOp,"Sumar");
sprintf(cadNum,"%u",numTmp++);
strcat(cadTemp,cadNum);
if (e1.clase==ID) Chequear(Extraer(&e1));
if (e2.clase==ID) Chequear(Extraer(&e2));
Chequear(cadTemp);
Generar(cadOp, Extraer(&e1), Extraer(&e2), cadTemp);
strcpy(reg.nombre,cadTemp);
reg.clase=ID;

reg.valor=0;
return reg;
}
/******** Utilitarias ********/
void Match(TOKEN t){
if (!(t==ProximoToken()))
ErrorSintactico();
flagToken=0;
}
TOKEN ProximoToken(void){
if (!flagToken){
tokenActual = scanner();
if (tokenActual == ERRORLEXICO) ErrorLexico();
flagToken = 1;
if (tokenActual == ID) {
Buscar(buffer, TS, &tokenActual);
}
}
return tokenActual;
}
void ErrorLexico(){
printf("Error Lexico\n");
}
void ErrorSintactico(){
printf("Error Sintactico\n");
}
void Generar(char *co, char *a, char *b, char *c){
printf("%s %s,%s,%s\n", co, a, b, c);
}
char *Extraer(REG_EXPRESION *preg){
return preg->nombre;
}

int Buscar(char *id, RegTS *TS, TOKEN *t){
int i=0;
while (strcmp("$", TS[i].identifi)){
if (!strcmp(id, TS[i].identifi)){
*t = TS[i].t;
return 1;
}
i++;
}
return 0;
}
void Colocar(char *id, RegTS *TS){
int i=0;
while (strcmp("$", TS[i].identifi))
i++;
if (i<999)
{
strcpy(TS[i].identifi, id);
TS[i].t = ID;
strcpy(TS[i+1].identifi, "$");
}
}
void Chequear(char *s){
TOKEN t;
if (!Buscar(s, TS, &t))
Colocar(s, TS);
}
void Comenzar(void)
{
}
void Terminar(void)
{
Generar("Detiene", "", "", "");
}
void Asignar(REG_EXPRESION izq, REG_EXPRESION der){
TOKEN t;
if (!Buscar(izq.nombre, TS, &t)){
Colocar(izq.nombre, TS);
switch(der.clase){

case REAL:
Generar("Declara", izq.nombre, "Float", "");
break;
case CARACTER:
Generar("Declara", izq.nombre, "Char", "");
break;
default:
Generar("Declara", izq.nombre, "Entera","");
break;
}
}
Generar("Almacena", Extraer(&der), izq.nombre, "");
}
/*********** Scanner con TABLA ***********/
TOKEN scanner(void){
int tabla[NUMESTADOS][NUMCOLS] = {
//L D + - ( ) . , ; : ' = EOF´ ´OTRO
{ 1, 3, 5, 6, 7, 8,18,11,12,13,16,18,14, 0, 18},
{ 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
{ 4, 3, 4, 4, 4, 4, 9, 4, 4, 4, 4, 4, 4, 4, 4},
{ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
{ 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{18,10,18,18,18,18,18,18,18,18,18,18,18,18, 18},
{ 4,10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
{11,11,11,11,11,11,11,11,11,11,11,11,11,11, 11},
{12,12,12,12,12,12,12,12,12,12,12,12,12,12, 12},
{18,18,18,18,18,18,18,18,18,18,18,19,18,18, 18},
{14,14,14,14,14,14,14,14,14,14,14,14,14,14, 14},
{18,18,18,18,18,18,18,18,18,18,18,18,18,18, 18},
{17,17,17,17,17,17,17,17,17,17,18,17,18,18, 17},
{18,18,18,18,18,18,18,18,18,18,20,18,18,18, 18},
{18,18,18,18,18,18,18,18,18,18,18,18,18,18, 18},
{19,19,19,19,19,19,19,19,19,19,19,19,19,19, 19},
{20,20,20,20,20,20,20,20,20,20,20,20,20,20, 20}
};
int car;
int col;
int estado = 0;
int i = 0;

do {
car = fgetc(in);
col = columna(car);
estado = tabla[estado][col];
if (col != 13) {
buffer[i] = (char)car;
i++;
}
} while (!estadoFinal(estado) && !(estado == 18));
buffer[i] = '\0';
switch (estado) {
case 2: if (col != 13){
ungetc(car,in);
buffer[i-1]='\0';
} return ID;
case 4: if (col != 13){
ungetc(car,in);
buffer[i-1]='\0';
} return CONSTANTE;
case 5: return SUMA;
case 6: return RESTA;
case 7: return PARENIZQUIERDO;
case 8: return PARENDERECHO;
case 11: return COMA;
case 12: return PUNTOYCOMA;
case 14: return FDT;
case 19: return ASIGNACION;
case 20: return CONSTANTE;
case 18: return ERRORLEXICO;
}
return 0;
}
int estadoFinal(int e){
if (e==0 || e==1 || e==3 || e==9 || e==10 || e==13 || e==16 || e==17
|| e==18) return 0;
return 1;
}
int columna(int c){
if (isalpha(c))
return 0;
if (isdigit(c))
return 1;

if (c == '+')
return 2;
if (c == '-')
return 3;
if (c == '(')
return 4;
if (c == ')')
return 5;
if (c == '.')
return 6;
if (c == ',')
return 7;
if (c == ';')
return 8;
if (c == ':')
return 9;
if (c == '\'')
return 10;
if (c == '=')
return 11;
if (c == EOF)
return 12;
if (isspace(c))
return 13;
return 14;
}