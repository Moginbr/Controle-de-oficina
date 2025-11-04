#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

//definindo limite máximo
#define MAX_CLIENTES 100
#define MAX_VEICULOS 100
#define MAX_ORDENS 100

//Estrutura para Status da Ordem de Serviço
typedef enum {
    AGUARDANDO_AVALIACAO,
    EM_REPARO,
    FINALIZADO,
    ENTREGUE
} Status;

// Estrutura para Cliente
typedef struct {
    char nome[100];
    char cpf[15];  // 000.000.000-00 (14 caracteres + null terminator)
    char telefone[16]; // (00) 90000-0000 (15 caracteres + null terminator)
} Cliente;

// Estrutura para Veículo
typedef struct {
    char placa[8]; // AAA0A00 ou AAA0000
    char modelo[50];
    int ano;
    char cpfDono[15]; 
} Veiculo;

// Estrutura para Ordem de Serviço
typedef struct {
    int id;
    char placaVeiculo[8]; 
    char dataEntrada[11];
    char descricaoProblema[200];
    Status status;
} OrdemServico;

// ----------------------------------------------------------------------------------------------

// Declarando arrays para armazenar o necessário.
Cliente clientes[MAX_CLIENTES];
Veiculo veiculos[MAX_VEICULOS];
OrdemServico ordens[MAX_ORDENS];

// ----------------------------------------------------------------------------------------------

//Declarando Variáveis globais para controle.
int numClientes = 0;
int numVeiculos = 0;
int numOrdens = 0;
int nextOrdemId = 1;

// ----------------------------------------------------------------------------------------------

// Declarando funções para utilização.
int validarCPF(char *cpf);
int validarTelefone(char *telefone);
int validarPlaca(char *placa);
int validarData(char *data);
int validarAno(int ano);
int ehBissexto(int ano);
void formatarCPF(char *cpf);
void formatarTelefone(char *telefone);
void formatarPlaca(char *placa);
void limparBuffer();
void exibirMenuPrincipal();
void menuClientes();
void menuVeiculos();
void menuOrdensServico();
void menuRelatorios();
void cadastrarCliente();
void listarClientes();
int buscarClientePorCPF(char *cpf);
void removerCliente();
void atualizarCliente();
void adicionarVeiculo();
void listarVeiculos();
int buscarVeiculoPorPlaca(char *placa);
void removerVeiculo();
void atualizarVeiculo();
void criarOrdemServico();
void listarOrdensServico();
void atualizarStatusOrdem();
void listarOrdensPorStatus();
void buscarOrdemPorData();
void criarRelatorioHistoricoVeiculo();
void criarRelatorioOrdensData();
void criarRelatorioVeiculosCPF();
void criarRelatorioOrdensStatus();
void criarRelatorioClientesRecorrentes();
void salvarDados();
void carregarDados();

// ----------------------------------------------------------------------------------------------

//Válida se é ano bissexto dividindo o ano por 4, 100 e 400.
int ehBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

// ----------------------------------------------------------------------------------------------

//Valida o formato do CPF verificando quantidade de dígitos e caracteres válidos
int validarCPF(char *cpf) {
    int countDigitos = 0;
    
    // Conta apenas dígitos
    for (int i = 0; cpf[i] != '\0'; i++) {
        if (isdigit(cpf[i])) {
            countDigitos++;
        }
    }
    
    // Verifica se tem exatamente 11 dígitos
    if (countDigitos != 11) {
        return 0;
    }
    
    // Verifica se todos os caracteres são dígitos ou formatação esperada
    for (int i = 0; cpf[i] != '\0'; i++) {
        if (!isdigit(cpf[i]) && cpf[i] != '.' && cpf[i] != '-') {
            return 0;
        }
    }
    
    return 1;
}

// ----------------------------------------------------------------------------------------------

//Valida telefone contando quantidade de dígitos (10 ou 11)
int validarTelefone(char *telefone) {
    int countDigitos = 0;
    
    // Conta apenas dígitos
    for (int i = 0; telefone[i] != '\0'; i++) {
        if (isdigit(telefone[i])) {
            countDigitos++;
        }
    }
    
    // Verifica se tem 10 ou 11 dígitos
    if (countDigitos != 10 && countDigitos != 11) {
        return 0;
    }
    
    return 1;
}

// ----------------------------------------------------------------------------------------------

//Valida placa nos formatos Mercosul (AAA0A00) e antigo (AAA0000)
int validarPlaca(char *placa) {
    // Remove formatação temporariamente para validação
    char placaTemp[8];
    int j = 0;
    for (int i = 0; placa[i] != '\0' && j < 7; i++) {
        if (isalnum(placa[i])) {
            placaTemp[j++] = toupper(placa[i]);
        }
    }
    placaTemp[j] = '\0';
    
    if (strlen(placaTemp) != 7) return 0;
    
    // Verifica formato Mercosul (AAA0A00) ou antigo (AAA0000)
    int formatoMercosul = 1;
    int formatoAntigo = 1;
    
    // Verifica formato Mercosul: AAA0A00
    for (int i = 0; i < 3; i++) {
        if (!isalpha(placaTemp[i])) formatoMercosul = 0;
    }
    if (!isdigit(placaTemp[3])) formatoMercosul = 0;
    if (!isalpha(placaTemp[4])) formatoMercosul = 0;
    for (int i = 5; i < 7; i++) {
        if (!isdigit(placaTemp[i])) formatoMercosul = 0;
    }
    
    // Verifica formato antigo: AAA0000
    for (int i = 0; i < 3; i++) {
        if (!isalpha(placaTemp[i])) formatoAntigo = 0;
    }
    for (int i = 3; i < 7; i++) {
        if (!isdigit(placaTemp[i])) formatoAntigo = 0;
    }
    
    return formatoMercosul || formatoAntigo;
}

// ----------------------------------------------------------------------------------------------

//Valida ano entre 1900 e ano atual + 1
int validarAno(int ano) {
    // Obtém o ano atual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int anoAtual = tm.tm_year + 1900;
    
    // Verifica se tem 4 dígitos e está em um intervalo razoável
    if (ano < 1900 || ano > anoAtual + 1) {
        return 0;
    }
    return 1;
}

// ----------------------------------------------------------------------------------------------

//Valida data no formato DD/MM/AAAA com verificação de calendário
int validarData(char *data) {
    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;
    
    // Verifica se todos os caracteres são dígitos ou barras
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit(data[i])) return 0;
    }
    
    // Extrai dia, mês e ano
    int dia, mes, ano;
    sscanf(data, "%d/%d/%d", &dia, &mes, &ano);
    
    // Valida o ano
    if (!validarAno(ano)) return 0;
    
    // Valida o mês
    if (mes < 1 || mes > 12) return 0;
    
    // Valida o dia conforme o mês
    int diasNoMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Ajusta fevereiro para anos bissextos
    if (mes == 2 && ehBissexto(ano)) {
        if (dia < 1 || dia > 29) return 0;
    } else {
        if (dia < 1 || dia > diasNoMes[mes-1]) return 0;
    }
    
    return 1;
}

// ----------------------------------------------------------------------------------------------

void formatarCPF(char *cpf) {
    //Converte uma string de CPF para o formato 000.000.000-00
    // Remove formatação temporariamente
    /* O processo começa na extração apenas dos digitos númericos da string;
    Depois o armazena temporariamente em uma string vazia */
    char cpfTemp[12];
    int j = 0;
    
    // Extrai apenas os dígitos
    //Processo de validação para verificar se o caractere é um dígito.
    for (int i = 0; cpf[i] != '\0' && j < 11; i++) {
        if (isdigit(cpf[i])) {
            cpfTemp[j++] = cpf[i];
        }
    }
    cpfTemp[j] = '\0';
    
    // Verifica se tem exatamente 11 dígitos
    // Aplica a formatação
    sprintf(cpf, "%.3s.%.3s.%.3s-%.2s", 
            cpfTemp, cpfTemp + 3, cpfTemp + 6, cpfTemp + 9);
}

// ----------------------------------------------------------------------------------------------

void formatarTelefone(char *telefone) {
    //Mesmo esquema do CPF.
    char telTemp[12]; //Buffer temporário para os dígitos
    int j = 0;
    
    //Extrai apenas os dígitos
    for (int i = 0; telefone[i] != '\0' && j < 11; i++) {
        if (isdigit(telefone[i])) {
            telTemp[j++] = telefone[i];
        }
    }
    telTemp[j] = '\0';
    
    // Aplica a formatação baseada no tamanho
    if (strlen(telTemp) == 11) {
        //joga para o formato de celular (00) 00000-0000
        sprintf(telefone, "(%.2s) %.5s-%.4s", 
                telTemp, telTemp + 2, telTemp + 7);
    } else if (strlen(telTemp) == 10) {
        //Formato fixo: (00) 0000-0000
        sprintf(telefone, "(%.2s) %.4s-%.4s", 
                telTemp, telTemp + 2, telTemp + 6);
    } else {
        //Caso não tenha o tamanho certo de dígitos ele retorna
        return;
    }
}

// ----------------------------------------------------------------------------------------------

void formatarPlaca(char *placa) {
    char placaTemp[8]; 
    int j = 0;
    
    // Extrai apenas letras e números
    for (int i = 0; placa[i] != '\0' && j < 7; i++) {
        if (isalnum(placa[i])) { //verifica se é número ou letra
            placaTemp[j++] = toupper(placa[i]); //Converte para letra maiúscula
        }
    }
    placaTemp[j] = '\0';
    
    // Copia de volta para a placa original
    strcpy(placa, placaTemp);
}

// ----------------------------------------------------------------------------------------------

//O limparBuffer faz a limpeza do buffer de entrada para evitar problemas com leituras subsequentes
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ----------------------------------------------------------------------------------------------

//A função servirá para apresentar o menu principal do program
void exibirMenuPrincipal() {
    int opcao;//Var para receber a decisão
    do { //Criando um Do para caso a var opcao atenda o critério, se não, roda
        //Tela
        printf("\n--- SISTEMA DE REGISTRO - OFICINA DO JORGIN  ---\n");
        printf("1. Gerenciar Clientes\n");
        printf("2. Gerenciar Veiculos\n");
        printf("3. Gerenciar Ordens de Servico\n");
        printf("4. Relatorios\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBuffer();

        //Switch para dar as opções e ser direcionado para outras funções/menu
        switch (opcao) {
            case 1:
                menuClientes();
                break;
            case 2:
                menuVeiculos();
                break;
            case 3:
                menuOrdensServico();
                break;
            case 4:
                menuRelatorios();
                break;
            case 5:
                salvarDados();
                printf("Dados salvos. Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 5);
}

// ----------------------------------------------------------------------------------------------

void menuClientes() {
    //Será mesmo esquema do Menu principal (isso seguirá para os outros menus)
    int opcao;
    do {
        printf("\n--- CADASTRO DE CLIENTES ---\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Listar Clientes\n");
        printf("3. Remover Cliente\n");
        printf("4. Atualizar Cliente\n");
        printf("5. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();
        
        //opções de funções para o gerenciamento de clientes
        switch(opcao) {
            case 1:
                cadastrarCliente();
                break;
            case 2:
                listarClientes();
                break;
            case 3:
                removerCliente();
                break;
            case 4:
                atualizarCliente();
                break;
            case 5:
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 5);
}

// ----------------------------------------------------------------------------------------------

void cadastrarCliente() {
    //A função vai verificar se há espaço no array
    //Faz a coleta e validação do CPF, coletando também o nome do ser humano
    //Verifica a capacidade de clientes já definida para o sistema
    if (numClientes >= MAX_CLIENTES) {
        printf("Limite de clientes atingido!\n");
        return;
    }
    
    //Criando ponteiro para novos clientes no array
    Cliente *c = &clientes[numClientes];
    char cpfInput[20];
    char telefoneInput[20];
    
    //Coletando e validando o CPF com a função validarCPF
    printf("CPF (formato: 000.000.000-00): ");
    fgets(cpfInput, sizeof(cpfInput), stdin);
    cpfInput[strcspn(cpfInput, "\n")] = 0;
    
    if (!validarCPF(cpfInput)) {
        printf("CPF inválido! Use o formato 000.000.000-00 com exatamente 11 dígitos.\n");
        return;
    }
    
    // Formata e verifica se já existe
    strcpy(c->cpf, cpfInput);
    formatarCPF(c->cpf);
    
    if (buscarClientePorCPF(c->cpf) != -1) {
        printf("Cliente com este CPF já existe!\n");
        return;
    }
    
    //Nome do cliente
    printf("Nome: ");
    fgets(c->nome, sizeof(c->nome), stdin);
    c->nome[strcspn(c->nome, "\n")] = 0;
    
    //Telefone do cliente
    printf("Telefone (formato: (00) 00000-0000): ");
    fgets(telefoneInput, sizeof(telefoneInput), stdin);
    telefoneInput[strcspn(telefoneInput, "\n")] = 0;
    
    //Validação do telefone com a função validarTelefone
    if (!validarTelefone(telefoneInput)) {
        printf("Telefone inválido! Use 10 ou 11 dígitos.\n");
        return;
    }
    
    // Formata o telefone
    strcpy(c->telefone, telefoneInput);
    formatarTelefone(c->telefone);

    numClientes++;
    printf("Cliente cadastrado com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void listarClientes() {
    //Verifica se há clientes para serem listados
    if (numClientes == 0) {
        printf("Nenhum cliente cadastrado!\n");
        return;
    }
    
    //Percorre o array de clientes e imprime os dados formatados
    printf("\n--- LISTA DE CLIENTES ---\n");
    for (int i = 0; i < numClientes; i++) {
        printf("CPF: %s, Nome: %s, Telefone: %s\n", 
               clientes[i].cpf, clientes[i].nome, clientes[i].telefone);
    }
}

// ----------------------------------------------------------------------------------------------

int buscarClientePorCPF(char *cpf) {
    char cpfFormatado[15];
    //Formato padronizado do CPF para busca
    strcpy(cpfFormatado, cpf);
    formatarCPF(cpfFormatado);
    
    //Percorre o array de clientes buscando o CPF formatado
    for (int i = 0; i < numClientes; i++) {
        if (strcmp(clientes[i].cpf, cpfFormatado) == 0) {
            return i; // Retorna o índice se for encontrado
        }
    }
    return -1;
}

// ----------------------------------------------------------------------------------------------

void removerCliente() {
    char cpf[20];
    printf("Digite o CPF do cliente a ser removido: ");
    scanf("%19s", cpf);
    limparBuffer();
    
    // Busca o cliente pelo CPF com a função buscarClientePorCPF
    int idx = buscarClientePorCPF(cpf);
    if (idx == -1) {
        printf("Cliente não encontrado!\n");
        return;
    }
    
    // Verificar se o cliente possui veículos cadastrados
    for (int i = 0; i < numVeiculos; i++) {
        if (strcmp(veiculos[i].cpfDono, clientes[idx].cpf) == 0) {
            printf("Cliente possui veículos cadastrados. Remova os veículos primeiro.\n");
            return;
        }
    }
    
    // Remover cliente do array
    for (int i = idx; i < numClientes - 1; i++) {
        clientes[i] = clientes[i + 1];
    }
    numClientes--;
    printf("Cliente removido com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void atualizarCliente() {
    char cpf[20];
    printf("CPF do cliente a atualizar: ");
    scanf("%19s", cpf);
    limparBuffer();
    
    //Localizando cliente pelo CPF
    int index = buscarClientePorCPF(cpf);
    if (index == -1) {
        printf("Cliente não encontrado!\n");
        return;
    }
    
    Cliente *c = &clientes[index];
    printf("Cliente atual: Nome: %s, Telefone: %s\n", c->nome, c->telefone);
    
    //Atualizar o nome se preferir
    printf("Novo nome (deixe em branco para manter): ");
    char novoNome[100];
    fgets(novoNome, sizeof(novoNome), stdin); //Usa o fgets para permitir espaços
    novoNome[strcspn(novoNome, "\n")] = 0;//o strcspn remove o \n do final
    
    if (strlen(novoNome) > 0) { //strlen verifica se a string não está vazia
        strcpy(c->nome, novoNome); //Caso não vazio, atualiza
    }
    
    printf("Novo telefone (deixe em branco para manter): ");
    char novoTelefone[20];
    fgets(novoTelefone, sizeof(novoTelefone), stdin);
    novoTelefone[strcspn(novoTelefone, "\n")] = 0;
    
    if (strlen(novoTelefone) > 0) {
        if (!validarTelefone(novoTelefone)) {
            printf("Telefone inválido! Use 10 ou 11 dígitos.\n");
            return;
        }
        strcpy(c->telefone, novoTelefone);
        formatarTelefone(c->telefone);
    }
    
    printf("Cliente atualizado com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void menuVeiculos() {
    int opcao;
    //Menu de veículos Igual ao que é utilizado no menu de clientes
    do {
        printf("\n--- CADASTRO DE VEÍCULOS ---\n");
        printf("1. Cadastrar Veículo\n");
        printf("2. Listar Veículos\n");
        printf("3. Remover Veículo\n");
        printf("4. Atualizar Veículo\n");
        printf("5. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();
        
        switch(opcao) {
            case 1:
                adicionarVeiculo();
                break;
            case 2:
                listarVeiculos();
                break;
            case 3:
                removerVeiculo();
                break;
            case 4:
                atualizarVeiculo();
                break;
            case 5:
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 5);
}

// ----------------------------------------------------------------------------------------------

void adicionarVeiculo() {
    //Verifica a capacidade de veículos já definida para o sistema
    if (numVeiculos >= MAX_VEICULOS) {
        printf("Limite máximo de veículos atingido!\n");
        return;
    }
    
    //ponteiro para novos veículos no array
    Veiculo *v = &veiculos[numVeiculos];
    char placaInput[20];
    char cpfDonoInput[20];
    char anoInput[10];
    
    //Faz a validação da placa com a função validarPlaca
    printf("Placa (formato: ABC1234 ou ABC1D23): ");
    fgets(placaInput, sizeof(placaInput), stdin);
    placaInput[strcspn(placaInput, "\n")] = 0;
    
    // Validação da placa
    if (!validarPlaca(placaInput)) {
        printf("Placa inválida! Use o formato AAA1234 (antigo) ou AAA1A23 (Mercosul).\n");
        return;
    }
    
    // Verifica se não existe placa igual já cadastrada
    strcpy(v->placa, placaInput);
    formatarPlaca(v->placa);
    
    if (buscarVeiculoPorPlaca(v->placa) != -1) {
        printf("Placa já cadastrada!\n");
        return;
    }
    
    // Coleta modelo do veículo que foi informado
    printf("Modelo: ");
    fgets(v->modelo, sizeof(v->modelo), stdin);
    v->modelo[strcspn(v->modelo, "\n")] = 0;
    
    printf("Ano (formato: AAAA): ");
    fgets(anoInput, sizeof(anoInput), stdin);
    anoInput[strcspn(anoInput, "\n")] = 0;
    
    // Validação do ano
    if (strlen(anoInput) != 4) {
        printf("Ano inválido! Deve ter exatamente 4 dígitos.\n");
        return;
    }
    
    //o For verifica se todos os caracteres são dígitos
    for (int i = 0; i < 4; i++) {
        if (!isdigit(anoInput[i])) {
            printf("Ano inválido! Deve conter apenas números.\n");
            return;
        }
    }
    
    //o ponteiro v recebe o ano convertido para inteiro e valida
    v->ano = atoi(anoInput);
    if (!validarAno(v->ano)) {
        printf("Ano inválido! Deve estar entre 1900 e o ano atual.\n");
        return;
    }
    
    // Coleta e validação do CPF do dono
    printf("CPF do proprietário: ");
    scanf("%19s", cpfDonoInput);
    limparBuffer();
    
    if (!validarCPF(cpfDonoInput)) {
        printf("CPF inválido! Use o formato 000.000.000-00.\n");
        return;
    }
    
    // Formata o CPF do dono
    strcpy(v->cpfDono, cpfDonoInput);
    formatarCPF(v->cpfDono);
    
    if (buscarClientePorCPF(v->cpfDono) == -1) {
        printf("CPF não cadastrado! Cadastre o cliente primeiro.\n");
        return;
    }
    
    numVeiculos++;
    printf("Veículo cadastrado com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void listarVeiculos() {
    //Verifica se existem veículos cadastrados
    if (numVeiculos == 0) {
        printf("Nenhum veículo cadastrado.\n");
        return;
    }
    
    printf("\n--- LISTA DE VEÍCULOS ---\n");
    //Percorre todos os veículos e exibe com informações do proprietário
    for (int i = 0; i < numVeiculos; i++) {
        //Busca o cliente pelo CPF para obter o nome do proprietário
        int clienteIndex = buscarClientePorCPF(veiculos[i].cpfDono);
        char nomeDono[100] = "Não encontrado";
        if (clienteIndex != -1) {
            strcpy(nomeDono, clientes[clienteIndex].nome);
        }
        
        printf("Placa: %s | Modelo: %s | Ano: %d | Dono: %s\n", 
               veiculos[i].placa, veiculos[i].modelo, veiculos[i].ano, nomeDono);
    }
}

// ----------------------------------------------------------------------------------------------

int buscarVeiculoPorPlaca(char *placa) {
    //Formata a placa para padronizar a busca
    char placaFormatada[8];
    strcpy(placaFormatada, placa);
    formatarPlaca(placaFormatada);
    
    //Percorre o array de veículos comparando as placas
    for (int i = 0; i < numVeiculos; i++) {
        if (strcmp(veiculos[i].placa, placaFormatada) == 0) {
            return i; //Retorna o índice se encontrar
        }
    }
    return -1; //Retorna -1 se não encontrar
}

// ----------------------------------------------------------------------------------------------

void removerVeiculo() {
    char placa[20];
    printf("Digite a placa do veículo a ser removido: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = 0;

    //Busca o veículo pela placa usando a função buscarVeiculoPorPlaca
    int idx = buscarVeiculoPorPlaca(placa);
    if (idx == -1) {
        printf("Veículo não encontrado!\n");
        return;
    }
    
    // Verifica se o veículo possui ordens de serviço associadas
    // Percorre todas as ordens procurando pela placa do veículo
    for (int i = 0; i < numOrdens; i++) {
        if (strcmp(ordens[i].placaVeiculo, veiculos[idx].placa) == 0) {
            printf("Veículo possui ordens de serviço associadas. Remova as ordens primeiro.\n");
            return;
        }
    }
    
    // Remove o veículo do array movendo os elementos seguintes
    for (int i = idx; i < numVeiculos - 1; i++) {
        veiculos[i] = veiculos[i + 1]; //Desloca os elementos para preencher o espaço
    }
    numVeiculos--; //Decrementa o contador de veículos
    printf("Veículo removido com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void atualizarVeiculo() {
    char placa[20];
    printf("Placa do veículo a atualizar: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = 0;

    //Busca o veículo pela placa
    int index = buscarVeiculoPorPlaca(placa);
    if (index == -1) {
        printf("Veículo não encontrado!\n");
        return;
    }
    
    //Ponteiro para o veículo a ser atualizado
    Veiculo *v = &veiculos[index];
    printf("Veículo atual: Modelo: %s, Ano: %d, CPF do dono: %s\n", 
           v->modelo, v->ano, v->cpfDono);
    
    //Atualização do modelo (campo opcional)
    printf("Novo modelo (deixe em branco para manter): ");
    char novoModelo[50];
    fgets(novoModelo, sizeof(novoModelo), stdin);
    novoModelo[strcspn(novoModelo, "\n")] = 0;
    
    //Se o usuário digitou algo, atualiza o modelo
    if (strlen(novoModelo) > 0) {
        strcpy(v->modelo, novoModelo);
    }
    
    //Atualização do ano (campo opcional com validação)
    printf("Novo ano (deixe em branco para manter): ");
    char anoInput[10];
    fgets(anoInput, sizeof(anoInput), stdin);
    anoInput[strcspn(anoInput, "\n")] = 0;
    
    if (strlen(anoInput) > 0) {
        //Valida se tem exatamente 4 dígitos
        if (strlen(anoInput) != 4) {
            printf("Ano inválido! Deve ter exatamente 4 dígitos.\n");
            return;
        }
        
        //Verifica se todos os caracteres são dígitos
        for (int i = 0; i < 4; i++) {
            if (!isdigit(anoInput[i])) {
                printf("Ano inválido! Deve conter apenas números.\n");
                return;
            }
        }
        
        //Converte para inteiro e valida o ano
        int novoAno = atoi(anoInput);
        if (!validarAno(novoAno)) {
            printf("Ano inválido! Deve estar entre 1900 e o ano atual.\n");
            return;
        }
        v->ano = novoAno; //Atualiza o ano
    }

    //Atualização do CPF do dono (campo opcional com validação)
    printf("Novo CPF do dono (deixe em branco para manter): ");
    char novoCPF[20];
    fgets(novoCPF, sizeof(novoCPF), stdin);
    novoCPF[strcspn(novoCPF, "\n")] = 0;
    
    if (strlen(novoCPF) > 0) {
        //Valida o formato do CPF
        if (!validarCPF(novoCPF)) {
            printf("CPF inválido! Use o formato 000.000.000-00.\n");
            return;
        }
        //Verifica se o novo cliente existe
        if (buscarClientePorCPF(novoCPF) == -1) {
            printf("Cliente não encontrado!\n");
            return;
        }
        //Atualiza o CPF do dono
        strcpy(v->cpfDono, novoCPF);
        formatarCPF(v->cpfDono);
    }
    
    printf("Veículo atualizado com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void menuOrdensServico() {
    int opcao;
    //Menu para gerenciamento de ordens de serviço
    do {
        printf("\n--- GERENCIAR ORDENS DE SERVIÇO ---\n");
        printf("1. Criar Ordem de Serviço\n");
        printf("2. Listar Ordens de Serviço\n");
        printf("3. Atualizar Status da Ordem\n");
        printf("4. Listar Ordens por Status\n");
        printf("5. Buscar Ordem por Data\n");
        printf("6. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();
        
        switch(opcao) {
            case 1:
                criarOrdemServico();
                break;
            case 2:
                listarOrdensServico();
                break;
            case 3:
                atualizarStatusOrdem();
                break;
            case 4:
                listarOrdensPorStatus();
                break;
            case 5:
                buscarOrdemPorData();
                break;
            case 6:
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 6);
}

// ----------------------------------------------------------------------------------------------

void criarOrdemServico() {
    //Verifica se há espaço para novas ordens
    if (numOrdens >= MAX_ORDENS) {
        printf("Limite de ordens de serviço atingido!\n");
        return;
    }
    
    //Cria nova ordem e atribui ID automático
    OrdemServico *o = &ordens[numOrdens];
    o->id = nextOrdemId++;
    
    char placaInput[20];
    printf("Placa do veículo: ");
    fgets(placaInput, sizeof(placaInput), stdin);
    placaInput[strcspn(placaInput, "\n")] = 0;
    
    // Formata a placa para busca
    char placaFormatada[8];
    strcpy(placaFormatada, placaInput);
    formatarPlaca(placaFormatada);
    
    //Verifica se o veículo existe
    if (buscarVeiculoPorPlaca(placaFormatada) == -1) {
        printf("Veículo não cadastrado! Cadastre o veículo primeiro.\n");
        return;
    }
    
    strcpy(o->placaVeiculo, placaFormatada);
    
    //Solicita e valida data de entrada
    printf("Data de entrada (DD/MM/AAAA): ");
    fgets(o->dataEntrada, sizeof(o->dataEntrada), stdin);
    o->dataEntrada[strcspn(o->dataEntrada, "\n")] = 0;
    
    if (!validarData(o->dataEntrada)) {
        printf("Data inválida! Use o formato DD/MM/AAAA e verifique se a data existe.\n");
        return;
    }
    
    //Solicita descrição do problema
    printf("Descrição do problema: ");
    fgets(o->descricaoProblema, sizeof(o->descricaoProblema), stdin);
    o->descricaoProblema[strcspn(o->descricaoProblema, "\n")] = 0;
    
    //Define status inicial como "Aguardando Avaliação"
    o->status = AGUARDANDO_AVALIACAO;
    
    numOrdens++;
    printf("Ordem de serviço criada com sucesso! ID: %d\n", o->id);
}

// ----------------------------------------------------------------------------------------------

void listarOrdensServico() {
    //Verifica se existem ordens cadastradas
    if (numOrdens == 0) {
        printf("Nenhuma ordem de serviço cadastrada.\n");
        return;
    }
    
    printf("\n--- LISTA DE ORDENS DE SERVIÇO ---\n");
    //Percorre e exibe todas as ordens com status convertido para texto
    for (int i = 0; i < numOrdens; i++) {
        char *statusStr;
        //Converte o enum Status para string legível
        switch (ordens[i].status) {
            case AGUARDANDO_AVALIACAO: statusStr = "Aguardando Avaliacao"; break;
            case EM_REPARO: statusStr = "Em Reparo"; break;
            case FINALIZADO: statusStr = "Finalizado"; break;
            case ENTREGUE: statusStr = "Entregue"; break;
            default: statusStr = "Desconhecido"; break;
        }
        
        printf("ID: %d | Placa: %s | Data Entrada: %s | Problema: %s | Status: %s\n", 
               ordens[i].id, ordens[i].placaVeiculo, ordens[i].dataEntrada, 
               ordens[i].descricaoProblema, statusStr);
    }
}

// ----------------------------------------------------------------------------------------------

void atualizarStatusOrdem() {
    int id;
    printf("ID da ordem a atualizar: ");
    scanf("%d", &id);
    limparBuffer();
    
    //Busca a ordem pelo ID
    int index = -1;
    for (int i = 0; i < numOrdens; i++) {
        if (ordens[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf("Ordem de serviço não encontrada!\n");
        return;
    }
    
    //Exibe status atual
    printf("Status atual: ");
    switch (ordens[index].status) {
        case AGUARDANDO_AVALIACAO: printf("Aguardando Avaliacao\n"); break;
        case EM_REPARO: printf("Em Reparo\n"); break;
        case FINALIZADO: printf("Finalizado\n"); break;
        case ENTREGUE: printf("Entregue\n"); break;
        default: printf("Desconhecido\n"); break;
    }
    
    //Menu de opções de status
    printf("Novo status:\n");
    printf("1. Aguardando Avaliacao\n");
    printf("2. Em Reparo\n");
    printf("3. Finalizado\n");
    printf("4. Entregue\n");
    printf("Escolha uma opção: ");
    
    int opcao;
    scanf("%d", &opcao);
    limparBuffer();
    
    //Atualiza status baseado na opção escolhida
    switch(opcao) {
        case 1: ordens[index].status = AGUARDANDO_AVALIACAO; break;
        case 2: ordens[index].status = EM_REPARO; break;
        case 3: ordens[index].status = FINALIZADO; break;
        case 4: ordens[index].status = ENTREGUE; break;
        default:
            printf("Opção inválida!\n");
            return;
    }
    
    printf("Status atualizado com sucesso!\n");
}

// ----------------------------------------------------------------------------------------------

void listarOrdensPorStatus() {
    //Menu para escolha do status para filtro
    printf("Escolha o status para filtrar:\n");
    printf("1. Aguardando Avaliacao\n");
    printf("2. Em Reparo\n");
    printf("3. Finalizado\n");
    printf("4. Entregue\n");
    printf("Escolha uma opção: ");

    int opcao;
    scanf("%d", &opcao);
    limparBuffer();

    //Valida opção escolhida
    if (opcao < 1 || opcao > 4) {
        printf("Opção inválida!\n");
        return;
    }

    //Converte opção para enum Status
    Status statusFiltro = (Status)(opcao - 1);
    int found = 0;

    printf("\n--- ORDENS DE SERVIÇO COM STATUS SELECIONADO ---\n");
    //Percorre ordens filtrando pelo status escolhido
    for (int i = 0; i < numOrdens; i++) {
        if (ordens[i].status == statusFiltro) {
            char *statusStr;
            switch (ordens[i].status) {
                case AGUARDANDO_AVALIACAO: statusStr = "Aguardando Avaliacao"; break;
                case EM_REPARO: statusStr = "Em Reparo"; break;
                case FINALIZADO: statusStr = "Finalizado"; break;
                case ENTREGUE: statusStr = "Entregue"; break;
                default: statusStr = "Desconhecido"; break;
            }

            printf("ID: %d | Placa: %s | Data Entrada: %s | Status: %s\n", 
                   ordens[i].id, ordens[i].placaVeiculo, ordens[i].dataEntrada, statusStr);
            printf("Problema: %s\n", ordens[i].descricaoProblema);
            printf("----------------------------------------\n");
            found = 1;
        }
    }
    if (!found) {
        printf("Nenhuma ordem encontrada com o status selecionado.\n");
    }
}

// ----------------------------------------------------------------------------------------------

void buscarOrdemPorData() {
    char data[11];
    printf("Digite a data (DD/MM/AAAA) para buscar ordens: ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = 0;

    //Valida data informada
    if (!validarData(data)) {
        printf("Data inválida! Use o formato DD/MM/AAAA e verifique se a data existe.\n");
        return;
    }

    int found = 0;
    printf("\n--- ORDENS DE SERVIÇO NA DATA %s ---\n", data);
    //Percorre ordens filtrando pela data
    for (int i = 0; i < numOrdens; i++) {
        if (strcmp(ordens[i].dataEntrada, data) == 0) {
            char *statusStr;
            switch (ordens[i].status) {
                case AGUARDANDO_AVALIACAO: statusStr = "Aguardando Avaliacao"; break;
                case EM_REPARO: statusStr = "Em Reparo"; break;
                case FINALIZADO: statusStr = "Finalizado"; break;
                case ENTREGUE: statusStr = "Entregue"; break;
                default: statusStr = "Desconhecido"; break;
            }

            printf("ID: %d | Placa: %s | Status: %s\n", 
                   ordens[i].id, ordens[i].placaVeiculo, statusStr);
            printf("Problema: %s\n", ordens[i].descricaoProblema);
            printf("----------------------------------------\n");
            found = 1;
        }
    }
    if (!found) {
        printf("Nenhuma ordem encontrada na data especificada.\n");
    }
}

// ----------------------------------------------------------------------------------------------

void menuRelatorios() {
    int opcao;
    //Menu para geração de relatórios
    do {
        printf("\n--- RELATÓRIOS ---\n");
        printf("1. Histórico de Veículos\n");
        printf("2. Ordens por Data\n");
        printf("3. Veículos por CPF do Cliente\n");
        printf("4. Ordens por Status\n");
        printf("5. Clientes Recorrentes\n");
        printf("6. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();
        
        switch(opcao) {
            case 1:
                criarRelatorioHistoricoVeiculo();
                break;
            case 2:
                criarRelatorioOrdensData();
                break;
            case 3:
                criarRelatorioVeiculosCPF();
                break;
            case 4:
                criarRelatorioOrdensStatus();
                break;
            case 5:
                criarRelatorioClientesRecorrentes();
                break;
            case 6:
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 6);
}

// ----------------------------------------------------------------------------------------------

void criarRelatorioHistoricoVeiculo() {
    char placa[20];
    printf("Digite a placa do veículo para o relatório: ");
    fgets(placa, sizeof(placa), stdin);
    placa[strcspn(placa, "\n")] = 0;

    // Formata a placa para busca
    char placaFormatada[8];
    strcpy(placaFormatada, placa);
    formatarPlaca(placaFormatada);

    //Verifica se veículo existe
    if (buscarVeiculoPorPlaca(placaFormatada) == -1) {
        printf("Veículo não cadastrado!\n");
        return;
    }

    //Cria arquivo de relatório
    char filename[50];
    snprintf(filename, sizeof(filename), "historico_%s.txt", placaFormatada);
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Erro ao criar o arquivo de relatório!\n");
        return;
    }

    fprintf(file, "--- HISTÓRICO DO VEÍCULO %s ---\n", placaFormatada);

    int encontrado = 0;
    //Percorre ordens filtrando pela placa do veículo
    for (int i = 0; i < numOrdens; i++) {
        if (strcmp(ordens[i].placaVeiculo, placaFormatada) == 0) {
            char *statusStr;
            switch (ordens[i].status) {
                case AGUARDANDO_AVALIACAO: statusStr = "Aguardando Avaliacao"; break;
                case EM_REPARO: statusStr = "Em Reparo"; break;
                case FINALIZADO: statusStr = "Finalizado"; break;
                case ENTREGUE: statusStr = "Entregue"; break;
                default: statusStr = "Desconhecido"; break;
            }

            //Escreve dados da ordem no arquivo
            fprintf(file, "ID: %d\n", ordens[i].id);
            fprintf(file, "Data Entrada: %s\n", ordens[i].dataEntrada);
            fprintf(file, "Problema: %s\n", ordens[i].descricaoProblema);
            fprintf(file, "Status: %s\n", statusStr);
            fprintf(file, "-------------------------\n");
            encontrado = 1;
        }
    }
    fclose(file);

    if (!encontrado) {
        //Remove arquivo se não encontrou ordens
        remove(filename);
        printf("Nenhuma ordem de serviço encontrada para este veículo.\n");
    } else {
        printf("Relatório salvo em %s\n", filename);
    }
}

// ----------------------------------------------------------------------------------------------

void criarRelatorioOrdensData() {
    char data[11];
    printf("Digite a data (DD/MM/AAAA) para o relatório: ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = 0;

    //Valida data informada
    if (!validarData(data)) {
        printf("Data inválida! Use o formato DD/MM/AAAA e verifique se a data existe.\n");
        return;
    }

    //Cria arquivo de relatório
    char filename[50];
    snprintf(filename, sizeof(filename), "ordens_%s.txt", data);
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Erro ao criar o arquivo de relatório!\n");
        return;
    }

    fprintf(file, "--- ORDENS DE SERVIÇO NA DATA %s ---\n", data);

    int encontrado = 0;
    //Percorre ordens filtrando pela data
    for (int i = 0; i < numOrdens; i++) {
        if (strcmp(ordens[i].dataEntrada, data) == 0) {
            char *statusStr;
            switch (ordens[i].status) {
                case AGUARDANDO_AVALIACAO: statusStr = "Aguardando Avaliacao"; break;
                case EM_REPARO: statusStr = "Em Reparo"; break;
                case FINALIZADO: statusStr = "Finalizado"; break;
                case ENTREGUE: statusStr = "Entregue"; break;
                default: statusStr = "Desconhecido"; break;
            }

            //Escreve dados da ordem no arquivo
            fprintf(file, "ID: %d\n", ordens[i].id);
            fprintf(file, "Placa: %s\n", ordens[i].placaVeiculo);
            fprintf(file, "Data Entrada: %s\n", ordens[i].dataEntrada);
            fprintf(file, "Problema: %s\n", ordens[i].descricaoProblema);
            fprintf(file, "Status: %s\n", statusStr);
            fprintf(file, "-------------------------\n");
            encontrado = 1;
        }
    }

    fclose(file);

    if (encontrado) {
        printf("Relatório salvo em %s\n", filename);
    } else {
        //Remove arquivo se não encontrou ordens
        remove(filename);
        printf("Nenhuma ordem de serviço encontrada na data especificada.\n");
    }
}

// ----------------------------------------------------------------------------------------------

void criarRelatorioVeiculosCPF() {
    char cpf[20];
    printf("Digite o CPF do cliente para o relatório: ");
    scanf("%19s", cpf);
    limparBuffer();

    // Formata o CPF para busca
    char cpfFormatado[15];
    strcpy(cpfFormatado, cpf);
    formatarCPF(cpfFormatado);

    //Busca cliente pelo CPF
    int clienteIndex = buscarClientePorCPF(cpfFormatado);
    if (clienteIndex == -1) {
        printf("Cliente não cadastrado!\n");
        return;
    }

    //Cria arquivo de relatório
    char filename[50];
    snprintf(filename, sizeof(filename), "veiculos_%s.txt", cpfFormatado);
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Erro ao criar o arquivo de relatório!\n");
        return;
    }

    //Escreve cabeçalho com dados do cliente
    fprintf(file, "Veículos do cliente %s (CPF: %s):\n", clientes[clienteIndex].nome, cpfFormatado);
    fprintf(file, "Telefone: %s\n", clientes[clienteIndex].telefone);
    fprintf(file, "================================\n\n");

    int encontrado = 0;
    //Percorre veículos filtrando pelo CPF do dono
    for (int i = 0; i < numVeiculos; i++) {
        if (strcmp(veiculos[i].cpfDono, cpfFormatado) == 0) {
            //Escreve dados do veículo no arquivo
            fprintf(file, "Placa: %s\n", veiculos[i].placa);
            fprintf(file, "Modelo: %s\n", veiculos[i].modelo);
            fprintf(file, "Ano: %d\n", veiculos[i].ano);
            fprintf(file, "-------------------------\n");
            encontrado = 1;
        }
    }
    fclose(file);

    if (encontrado) {
        printf("Relatório salvo em %s\n", filename);
    } else {
        //Remove arquivo se não encontrou veículos
        remove(filename);
        printf("Nenhum veículo encontrado para este cliente.\n");
    }
}

// ----------------------------------------------------------------------------------------------

void criarRelatorioOrdensStatus() {
    //Menu para escolha do status
    printf("Escolha o status para o relatório: \n");
    printf("1. Aguardando Avaliacao\n");
    printf("2. Em Reparo\n");
    printf("3. Finalizado\n");
    printf("4. Entregue\n");

    int opcao;
    scanf("%d", &opcao);
    limparBuffer();

    //Valida opção escolhida
    if (opcao < 1 || opcao > 4) {
        printf("Opção inválida!\n");
        return;
    }

    //Converte opção para enum Status
    Status statusFiltro = (Status)(opcao - 1);
    char *statusStr;
    char *filenameStatus;

    //Define strings baseadas no status escolhido
    switch(statusFiltro) {
        case AGUARDANDO_AVALIACAO:
            statusStr = "Aguardando Avaliacao";
            filenameStatus = "aguardando_avaliacao";
            break;
        case EM_REPARO:
            statusStr = "Em Reparo";
            filenameStatus = "em_reparo";
            break;
        case FINALIZADO:
            statusStr = "Finalizado";
            filenameStatus = "finalizado";
            break;
        case ENTREGUE:
            statusStr = "Entregue";
            filenameStatus = "entregue";
            break;
        default:
            statusStr = "Desconhecido";
            filenameStatus = "desconhecido";
            break;
    }

    //Cria arquivo de relatório
    char filename[50];
    snprintf(filename, sizeof(filename), "ordens_%s.txt", filenameStatus);
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Erro ao criar o arquivo de relatório!\n");
        return;
    }

    fprintf(file, "Ordens de serviço com status: %s\n", statusStr);
    fprintf(file, "================================\n\n");

    int encontrado = 0;
    //Percorre ordens filtrando pelo status
    for (int i = 0; i < numOrdens; i++) {
        if (ordens[i].status == statusFiltro) {
            //Escreve dados da ordem no arquivo
            fprintf(file, "ID: %d\n", ordens[i].id);
            fprintf(file, "Placa: %s\n", ordens[i].placaVeiculo);
            fprintf(file, "Data Entrada: %s\n", ordens[i].dataEntrada);
            fprintf(file, "Problema: %s\n", ordens[i].descricaoProblema);
            fprintf(file, "-------------------------\n");
            encontrado = 1;
        }
    }

    fclose(file);

    if (encontrado) {
        printf("Relatório salvo em %s\n", filename);
    } else {
        //Remove arquivo se não encontrou ordens
        remove(filename);
        printf("Nenhuma ordem encontrada com o status selecionado.\n");
    }
}

// ----------------------------------------------------------------------------------------------

void criarRelatorioClientesRecorrentes() {
    //Cria arquivo de relatório
    FILE *file = fopen("clientes_recorrentes.txt", "w");

    if (file == NULL) {
        printf("Erro ao criar o arquivo de relatório!\n");
        return;
    }

    fprintf(file, "Clientes Recorrentes (mais de uma ordem de serviço):\n");
    fprintf(file, "================================\n\n");

    //Array para contar ordens por cliente
    int countRecorrentes[MAX_CLIENTES] = {0};

    //Conta quantas ordens cada cliente possui
    for (int i = 0; i < numOrdens; i++) {
        //Busca veículo da ordem
        int veiculoIndex = buscarVeiculoPorPlaca(ordens[i].placaVeiculo);
        if (veiculoIndex != -1) {
            //Busca cliente pelo CPF do dono do veículo
            int clienteIndex = buscarClientePorCPF(veiculos[veiculoIndex].cpfDono);
            if (clienteIndex != -1) {
                countRecorrentes[clienteIndex]++; //Incrementa contador
            }
        }
    }

    //Ordena clientes por quantidade de ordens (decrescente)
    for (int i = 0; i < numClientes; i++) {
        for (int j = i + 1; j < numClientes; j++) {
            if (countRecorrentes[j] > countRecorrentes[i]) {
                //Troca as contagens
                int temp = countRecorrentes[i];
                countRecorrentes[i] = countRecorrentes[j];
                countRecorrentes[j] = temp;

                //Troca os clientes de posição mantendo correspondência
                Cliente tempC = clientes[i];
                clientes[i] = clientes[j];
                clientes[j] = tempC;
            }
        }
    }

    //Escreve clientes com mais de uma ordem no arquivo
    int encontrado = 0;
    for (int i = 0; i < numClientes; i++) {
        if (countRecorrentes[i] > 1) {
            fprintf(file, "Nome: %s\n", clientes[i].nome);
            fprintf(file, "CPF: %s\n", clientes[i].cpf);
            fprintf(file, "Telefone: %s\n", clientes[i].telefone);
            fprintf(file, "Número de Ordens: %d\n", countRecorrentes[i]);
            fprintf(file, "-------------------------\n");
            encontrado = 1;
        }
    }

    fclose(file);

    if (encontrado) {
        printf("Relatório salvo em clientes_recorrentes.txt\n");
    } else {
        //Remove arquivo se não encontrou clientes recorrentes
        printf("Nenhum cliente recorrente encontrado.\n");
        remove("clientes_recorrentes.txt");
    }
}

// ----------------------------------------------------------------------------------------------

void salvarDados() {
    //Salva os dados dos clientes em arquivo binário
    FILE *file = fopen("clientes.dat", "wb");
    if (file != NULL) {
        //Salva primeiro o número de clientes, depois o array completo
        fwrite(&numClientes, sizeof(int), 1, file);
        fwrite(clientes, sizeof(Cliente), numClientes, file);
        fclose(file);
    }

    //Salva os dados dos veículos em arquivo binário
    file = fopen("veiculos.dat", "wb");
    if (file != NULL) {
        fwrite(&numVeiculos, sizeof(int), 1, file);
        fwrite(veiculos, sizeof(Veiculo), numVeiculos, file);
        fclose(file);
    }

    //Salva os dados das ordens de serviço em arquivo binário
    file = fopen("ordens.dat", "wb");
    if (file != NULL) {
        fwrite(&numOrdens, sizeof(int), 1, file);
        fwrite(ordens, sizeof(OrdemServico), numOrdens, file);
        fwrite(&nextOrdemId, sizeof(int), 1, file); //Salva o próximo ID também
        fclose(file);
    }
}

// ----------------------------------------------------------------------------------------------

void carregarDados() {
    //Carrega os dados dos clientes do arquivo binário
    FILE *file = fopen("clientes.dat", "rb");
    if (file != NULL) {
        //Lê primeiro o número de clientes, depois o array completo
        fread(&numClientes, sizeof(int), 1, file);
        fread(clientes, sizeof(Cliente), numClientes, file);
        fclose(file);
    }

    //Carrega os dados dos veículos do arquivo binário
    file = fopen("veiculos.dat", "rb");
    if (file != NULL) {
        fread(&numVeiculos, sizeof(int), 1, file);
        fread(veiculos, sizeof(Veiculo), numVeiculos, file);
        fclose(file);
    }

    //Carrega os dados das ordens de serviço do arquivo binário
    file = fopen("ordens.dat", "rb");
    if (file != NULL) {
        fread(&numOrdens, sizeof(int), 1, file);
        fread(ordens, sizeof(OrdemServico), numOrdens, file);
        fread(&nextOrdemId, sizeof(int), 1, file); //Carrega o próximo ID também
        fclose(file);
    }
}

// ----------------------------------------------------------------------------------------------

int main() {
    //Carrega os dados salvos anteriormente ao iniciar o programa
    carregarDados();
    
    //Chama o menu principal que controla todo o fluxo do sistema
    exibirMenuPrincipal();
    
    return 0; //Retorna 0 indicando que o programa terminou com sucesso
}