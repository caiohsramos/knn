/* Programa que le diversos arquivos de dados e realiza as operacoes
 * 	desejadas de acordo com a escolha do usuario.
 * 
 * T3 - Caio Ramos NUSP 9292991 
 */

//bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//atalhos para os diferentes tipos de dados
#define INT 1
#define DOUBLE 2
#define CHAR 3

typedef struct {
	char *nome; //nome do campo
	char *nomeTipo; //nome do tipo do campo
	int tipo; //tipo do campo (de acordo com os defines)
	int tamanho; //tamanho em bytes do dado do campo
} CAMPO;

typedef struct {
	double dist;
	int offset;
} DIST;

typedef struct {
	int n_registros; //numero de registros no ".data"
	CAMPO *campo; //vetor para os campos do ".schema"
	int n_campos; //numero de campos
	DIST *dist;
	char *nomeArquivo; //nome inicial do arquivo que sera usado
	char *nomeData; //nome do arquivo ".data"
	int tamanhoRegistro; //tamanho de cada registro do ".data"
} LISTA;

/*
 * Funcao que le o nome do arquivo ".schema" e retorna o endereco alocado da string
 * 
 * name: char *lerNomeSchema(void)
 * @param: nenhum
 * @return: ponteiro para uma string alocada com o nome do arquivo
 * 
 */

char *lerNomeSchema(void) {
	char *nome = NULL;
	nome = (char*) malloc (30*sizeof(char));
	scanf("%s", nome);
	getchar();
	return nome;
}

/*
 * Funcao que inicializa uma struct LISTA, que guarda os dados necessarios
 * 	para manipular todos os arquivos
 * 
 * name: LISTA *criarLista(void)
 * @param: nenhum
 * @return: ponteiro para uma LISTA, com as variaveis iniciadas
 * 
 */

LISTA *criarLista (void) {
	LISTA *lista = (LISTA*) malloc(sizeof(LISTA));
	lista->n_registros = 0;
	lista->n_campos = 0;
	lista->tamanhoRegistro = 0;
	lista->dist = NULL;
	lista->nomeData = NULL;
	lista->nomeArquivo = NULL;
	lista->campo = NULL;
	return lista;
}

/*
 * Funcao que cria um novo espaco para um campo que sera lido do ".schema"
 * 
 * name: void criaCampo(LISTA*)
 * @param: lista - ponteiro para struct lista que contem os dados a serem manipulados
 * @return: nenhum
 * 
 */

void criaCampo(LISTA *lista) {
	lista->campo = (CAMPO*) realloc (lista->campo, sizeof(CAMPO)*(lista->n_campos+1));
}

/*
 * Funcao que compara uma string com o nome de um tipo de dado e atribui
 * um tipo a variavel "campo.tipo" do ultimo campo lido do ".schema".
 * 
 * name: void atribuiTipo(LISTA*,char*)
 * @param: lista - ponteiro para a struct LISTA, tipo - string com o 
 * 			tipo de dado a ser atribuido.
 * @return: nenhum
 * 
 */

void atribuiTipo (LISTA *lista, char *tipo) {
	if (!strcmp(tipo, "int")) {
		lista->campo[lista->n_campos].tipo = INT;
	} else if (!strcmp(tipo, "double")) {
		lista->campo[lista->n_campos].tipo = DOUBLE;
	} else {
		//no caso de umc char[], o tamanho e calculado aqui...
		lista->campo[lista->n_campos].tipo = CHAR;
		sscanf(&tipo[5], "%d", &lista->campo[lista->n_campos].tamanho);
	}
	lista->campo[lista->n_campos].nomeTipo = tipo;
}

void criaId(LISTA *lista) {
	criaCampo(lista);
	lista->campo[0].nome = (char*)malloc(sizeof(char)*30);
	strcpy(lista->campo[0].nome, "id");
	lista->campo[0].nomeTipo = (char*)malloc(sizeof(char)*30);
	strcpy(lista->campo[0].nomeTipo, "int");
	lista->campo[0].tipo = INT;
	lista->n_campos++;
}

void criaDist(LISTA *lista) {
	int n;
	n = lista->n_campos;
	criaCampo(lista);
	lista->campo[n].nome = (char*)malloc(sizeof(char)*30);
	strcpy(lista->campo[n].nome, "dist");
	lista->campo[n].nomeTipo = (char*)malloc(sizeof(char)*30);
	strcpy(lista->campo[n].nomeTipo, "double");
	lista->campo[n].tipo = DOUBLE;
	lista->n_campos++;
}

/*
 * Funcao que abre o ".schema", le e organiza as informacoes neles contidas.
 * 		A leitura gravara os dados nas variaveis da struct LISTA.
 * 
 * name: void processarSchema(char*, LISTA*)
 * @param: nomeSchema - string com o nome do arquivo ".schema", 
 * 		   lista - ponteiro para struct LISTA que guardara as informacoes sobre os arquivos
 * @return: nenhum
 * 
 */

void processarSchema (char *nomeSchema, LISTA *lista) {
	FILE *fp = NULL;
	char *token = NULL;
	criaId(lista);
	token = (char*) malloc(30*sizeof(char)); //variavel que guardara palavra por palavra do arquivo
	fp = fopen(nomeSchema, "r");
	while (fgetc(fp) != ' '); //move o ponteiro do arquivo para o primerio token a ser lido
	fscanf(fp, "%s", token);
	lista->nomeArquivo = token; //guarda o nome inicial do arquivo
	token = (char*) malloc(30*sizeof(char));
	fscanf(fp, "%s", token);
	do {
		if (!feof(fp)) {
			criaCampo(lista);
			//nome do campo
			lista->campo[lista->n_campos].nome = token;
			
			//tipo do campo
			token = (char*) malloc(30*sizeof(char));
			fscanf(fp, "%s", token);
			atribuiTipo(lista, token);
			
			//le o proximo nome do campo
			token = (char*) malloc(30*sizeof(char));
			fscanf(fp, "%s", token);
			
			//incrementa o numero de campos lidos
			lista->n_campos++;
		}
	} while (!feof(fp)); //realiza a rotina ate o fim de arquivo
	//libera o espaco alocado para o token se ele nao for nulo
	if(token != NULL) free(token);
	fclose(fp); //fecha o arquivo
	
	criaDist(lista);
	
	//cria o nome para o arquivo ".data"
	lista->nomeData = (char*)malloc(sizeof(char)*30);
	strcpy(lista->nomeData, lista->nomeArquivo);
	strcat(lista->nomeData, ".data");
}

/*
 * Funcao que calcula os tamanhos dos campos lidos de acordo com os tipos guardados,
 * 		alem do tamanho total de cada registro.
 * 
 * name: void calculaTamanhos(LISTA*)
 * @param: lista - ponteiro para struct LISTA
 * @return: nenhum
 * 
 */

void calculaTamanhos(LISTA *lista) {
	int i, n;
	n = lista->n_campos;
	for (i = 0; i < n; i++) {
		if(lista->campo[i].tipo == INT) {
			lista->campo[i].tamanho = 4;
			lista->tamanhoRegistro += lista->campo[i].tamanho;
		}
		if(lista->campo[i].tipo == DOUBLE) {
			lista->campo[i].tamanho = 8;
			lista->tamanhoRegistro += lista->campo[i].tamanho;
		}
		if(lista->campo[i].tipo == CHAR) {
			lista->tamanhoRegistro += lista->campo[i].tamanho;
		}
	}
}

/*
 * Funcao que faz a impressao do ".schema", sem carrega-lo para a memoria.
 * 
 * name: void dump_schema(LISTA*)
 * @param: lista - ponteiro para struct LISTA
 * @return: nenhum
 * 
 */

void dump_schema (LISTA *lista) {
	int i, n;
	n = lista->n_campos;
	printf("table %s(%d bytes)\n", lista->nomeArquivo, lista->tamanhoRegistro);
	for (i = 0; i < n; i++) {
		printf("%s %s(%d bytes)\n", lista->campo[i].nome, lista->campo[i].nomeTipo, lista->campo[i].tamanho);
	}
}

/*
 * Funcao que faz a impressao do ".data" sem carrega-lo completamente para a memoria
 * 
 * name: void dump_data(LISTA*)
 * @param: ponteiro para struct LISTA
 * @return: nenhum
 * 
 */

void dump_data(LISTA *lista) {
	int i, n, j;
	FILE *fp = NULL;
	void *p = NULL;
	fp = fopen(lista->nomeData, "r");
	fseek(fp, 0, SEEK_END);
	
	//calcula o numero de registros do .data
	lista->n_registros = ((ftell(fp))/(double)(lista->tamanhoRegistro));
	
	fseek(fp, 0, SEEK_SET);
	n = lista->n_campos;
	for(i = 0; i < lista->n_registros; i++) { //percorre os 'n' registos
		for(j = 0; j < n; j++) { //percorre os 'n' campos
			switch (lista->campo[j].tipo) {
				//para cada caso e feito uma alocacao do tamanho do dado a ser impresso
				//	em um ponteiro void, e um casting e feito na hora da impressao.
				case INT:
					p = malloc(sizeof(int));
					fread(p, sizeof(int), 1, fp);
					printf("%s = %d\n", lista->campo[j].nome, *((int*)p));
					free(p);
					break;
				case DOUBLE:
					p = malloc(sizeof(double));
					fread(p, sizeof(double), 1, fp);
					printf("%s = %.2lf\n", lista->campo[j].nome, *((double*)p));
					free(p);
					break;
				case CHAR:
					p = malloc(sizeof(char)*(lista->campo[j].tamanho));
					fread(p, sizeof(char), lista->campo[j].tamanho, fp);
					printf("%s = %s\n", lista->campo[j].nome, (char*)p);
					free(p);
					break;
			}
		}
	}
	fclose(fp);
}

/*
 * Funcao que insere um novo registro no ".data"
 * 
 * name: void insert(LISTA*)
 * @param: lista - ponteiro
 * @return: nenhum
 * 
 *
 */
void insert(LISTA *lista) {
	void *dado = NULL;
	int i, id;
	FILE *fp = NULL;
	double dist = 0.0;
	fp = fopen(lista->nomeData, "w");
	do {
		scanf("%d", &id);
		if(id != -1) {
			fwrite(&id, sizeof(int), 1, fp);
			for(i = 1; i < lista->n_campos-1; i++) {
				dado = malloc(lista->campo[i].tamanho);
				//faz a leitura adequada para o campo...
				switch(lista->campo[i].tipo) {
					case INT:
						scanf("%d", (int*)dado);
						break;
					case DOUBLE:
						scanf("%lf", (double*)dado);
						break;
					case CHAR:
						scanf("%s", (char*)dado);
						break;
				}
				//escreve o dado no arquivo
				fwrite(dado, lista->campo[i].tamanho, 1, fp);
				free(dado);
			}
			fwrite(&dist, sizeof(double), 1, fp);
		}
	} while(id != -1);
	fclose(fp);
}

/*
 * Funcao que libera as alocacoes de cada campo de LISTA
 * name: void liberaCampos(LISTA*)
 * @param: lista - ponteiro para struct LISTA
 * @return: nenhum
 * 
 */

void liberaCampos(LISTA *lista) {
	int i, n;
	n = lista->n_campos;
	for (i = 0; i < n; i++) {
		free(lista->campo[i].nome);
		free(lista->campo[i].nomeTipo);
	}
	free(lista->campo);
}

void liberaDist(LISTA *lista) {
	free(lista->dist);
	lista->dist = NULL;
}

void ordenaDist(LISTA *lista) {
	int i, j;
	DIST eleito;
	for (i = 1; i < lista->n_registros; i++){
		eleito.dist = lista->dist[i].dist;
		eleito.offset = lista->dist[i].offset;
		j = i - 1;
		while ((j>=0) && (eleito.dist < lista->dist[j].dist)) {
			lista->dist[j+1].dist = lista->dist[j].dist;
			lista->dist[j+1].offset = lista->dist[j].offset;
			j--;
		}
		lista->dist[j+1].dist = eleito.dist;
		lista->dist[j+1].offset = eleito.offset;
	}
}

void novoDist(LISTA *lista, int offset, int offset_reg, int j) {
	FILE *fp = NULL;
	fp = fopen(lista->nomeData, "r");
	fseek(fp, offset, SEEK_SET);
	//adiciona um index ao vetor de index
	lista->dist = (DIST*) realloc(lista->dist, sizeof(DIST)*(j+1));
	//le a chave do ".data"
	fread(&lista->dist[j].dist, sizeof(double), 1, fp);
	lista->dist[j].offset = offset_reg;
	fclose(fp);
}

void criaVetorDistancia(LISTA * lista) {
	int n, j, soma = 0;
	n = (lista->n_campos-1);
	for(j = 0; j < n; j++) {
		soma += lista->campo[j].tamanho;
	}
	for(j = 0; j < lista->n_registros; j++) { //percorre os 'n' registros...
		//...criando um index para cada um
		novoDist(lista, (((lista->tamanhoRegistro)*j)+soma), (j*lista->tamanhoRegistro), j);
	}
}

void dump_nn(LISTA *lista) {
	int i, n_vizinhos, n, soma_bytes, j, id;
	double dist, soma;
	void *p1;
	void **p2;
	FILE *fp = NULL;
	scanf("%d", &n_vizinhos);
	scanf("%d", &id);
	n = lista->n_campos;
	fp = fopen(lista->nomeData, "r+");
	fseek(fp, 0, SEEK_END);
	p2 = malloc(sizeof(void*)*(n-2));
	for(i = 0; i < n-3; i++) {
		p2[i] = malloc(lista->campo[i+1].tamanho);
		//faz a leitura adequada para o campo...
		switch(lista->campo[i+1].tipo) {
			case INT:
				scanf("%d", (int*)p2[i]);
				break;
			case DOUBLE:
				scanf("%lf", (double*)p2[i]);
				break;
		}
	}
	
	lista->n_registros = ((ftell(fp))/(double)(lista->tamanhoRegistro));
	for (i = 0; i <= lista->n_registros; i++) {
		soma = 0;
		soma_bytes = sizeof(int);
		for(j = 1; j < n-2; j++) { //percorre os 'n' campos
			fseek(fp, ((i*lista->tamanhoRegistro)+soma_bytes), SEEK_SET);
			switch (lista->campo[j].tipo) {
				case INT:
					p1 = malloc(sizeof(int));
					fread(p1, sizeof(int), 1, fp);
					soma += (((*(int*)p1)-(*(int*)p2[j-1]))*((*(int*)p1)-(*(int*)p2[j-1])));
					free(p1);
					soma_bytes += sizeof(int);
					break;
				case DOUBLE:
					p1 = malloc(sizeof(double));
					fread(p1, sizeof(double), 1, fp);
					soma += (((*(double*)p1)-(*(double*)(p2[j-1])))*((*(double*)p1)-(*(double*)(p2[j-1]))));
					free(p1);
					soma_bytes += sizeof(double);
					break;
			}
		}
		dist = sqrt(soma);
		fseek(fp, lista->campo[j].tamanho, SEEK_CUR);
		fwrite(&dist, sizeof(double), 1, fp);
	}
	
	for(i = 0; i < n-3; i++) {
		free(p2[i]);
	}
	free(p2);
	criaVetorDistancia(lista);
	ordenaDist(lista);
	for(i = 0; i < n_vizinhos; i++) {
		fseek(fp, lista->dist[i].offset, SEEK_SET);
		for(j = 0; j < n; j++) { //percorre os 'n' campos
			switch (lista->campo[j].tipo) {
				//para cada caso e feito uma alocacao do tamanho do dado a ser impresso
				//	em um ponteiro void, e um casting e feito na hora da impressao.
				case INT:
					p1 = malloc(sizeof(int));
					fread(p1, sizeof(int), 1, fp);
					printf("%s = %d\n", lista->campo[j].nome, *((int*)p1));
					free(p1);
					break;
				case DOUBLE:
					p1 = malloc(sizeof(double));
					fread(p1, sizeof(double), 1, fp);
					printf("%s = %.2lf\n", lista->campo[j].nome, *((double*)p1));
					free(p1);
					break;
				case CHAR:
					p1 = malloc(sizeof(char)*(lista->campo[j].tamanho));
					fread(p1, sizeof(char), lista->campo[j].tamanho, fp);
					printf("%s = %s\n", lista->campo[j].nome, (char*)p1);
					free(p1);
					break;
			}
		}
	}
	liberaDist(lista);
	fclose(fp);
	
}


//inicio do programa
int main (int argc, char *arg[]) {
	char *nomeSchema = NULL;
	char *opt = NULL;
	LISTA *lista = NULL;
	//processamento dos arquivos
	nomeSchema = lerNomeSchema();
	lista = criarLista();
	processarSchema(nomeSchema, lista);
	calculaTamanhos(lista);
	insert(lista);
	opt = (char*) malloc(sizeof(char)*20);
	
	//realiza as operacoes
	do {
		scanf("%s", opt);
		if(!strcmp(opt, "dump_schema")) dump_schema(lista);
		if(!strcmp(opt, "dump_data")) dump_data(lista);
		if(!strcmp(opt, "dump_nn")) dump_nn(lista);
	} while(strcmp(opt, "exit"));
	
	//liberacoes de memoria
	free(opt);
	liberaCampos(lista);
	free(lista->nomeData);
	free(lista->nomeArquivo);
	free(nomeSchema);
	free(lista);
	//fim do programa
	return 0;
}
