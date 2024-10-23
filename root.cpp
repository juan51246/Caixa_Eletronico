#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>

using namespace std;

const string ADMIN_LOGIN = "L578198";
const string ADMIN_SENHA = "Lt040903";

// Função para limpar a tela
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Classe para representar usuários
class Usuario {
public:
    string login;
    string senha;
    Usuario(string login, string senha) : login(login), senha(senha) {}

    // Salvar o usuário em arquivo
    static void salvarUsuario(const Usuario& usuario) {
        ofstream arquivo("usuarios.txt", ios::app);
        if (arquivo.is_open()) {
            arquivo << usuario.login << "," << usuario.senha << endl;
            arquivo.close();
        }
        else {
            cerr << "Erro ao abrir arquivo de usuários!" << endl;
        }
    }

    // Carregar todos os usuários do arquivo
    static void carregarUsuarios(vector<Usuario>& usuarios) {
        ifstream arquivo("usuarios.txt");
        string linha;
        if (arquivo.is_open()) {
            while (getline(arquivo, linha)) {
                size_t pos = linha.find(',');
                if (pos != string::npos) {
                    string login = linha.substr(0, pos);
                    string senha = linha.substr(pos + 1);
                    usuarios.push_back(Usuario(login, senha));
                }
            }
            arquivo.close();
        }
    }

    // Autenticar login
    static bool autenticar(const string& login, const string& senha, const vector<Usuario>& usuarios) {
        for (const auto& u : usuarios) {
            if (u.login == login && u.senha == senha) {
                return true;
            }
        }
        return false;
    }
};

// Classe para representar uma conta bancária
class Conta {
public:
    string titular;
    string banco;
    int numero;
    double saldo;

    Conta(string titular, string banco, int numero, double saldo = 0.0)
        : titular(titular), banco(banco), numero(numero), saldo(saldo) {}

    // Registrar transação no log
    static void registrarTransacao(const string& tipo, double valor, const Conta& conta) {
        ofstream log("log_transacoes.txt", ios::app);
        time_t agora = time(0);
        char dataHora[26];
        ctime_s(dataHora, sizeof(dataHora), &agora);

        if (log.is_open()) {
            log << "[" << dataHora << "] " << tipo << ": R$" << valor << " - Conta " << conta.numero << " (" << conta.banco << ")\n";
            log.close();
        }
    }

    // Salvar contas no arquivo
    static void salvarConta(const Conta& conta) {
        ofstream arquivo("contas.txt", ios::app);
        if (arquivo.is_open()) {
            arquivo << conta.titular << "," << conta.banco << "," << conta.numero << "," << conta.saldo << endl;
            arquivo.close();
        }
        else {
            cerr << "Erro ao abrir arquivo de contas!" << endl;
        }
    }

    // Carregar contas do arquivo
    static void carregarContas(vector<Conta>& contas) {
        ifstream arquivo("contas.txt");
        string linha;
        if (arquivo.is_open()) {
            while (getline(arquivo, linha)) {
                size_t pos = 0;
                string token;
                vector<string> dados;

                while ((pos = linha.find(',')) != string::npos) {
                    token = linha.substr(0, pos);
                    dados.push_back(token);
                    linha.erase(0, pos + 1);
                }
                dados.push_back(linha); // Adiciona o último token

                if (dados.size() == 4) {
                    string titular = dados[0];
                    string banco = dados[1];
                    int numero = stoi(dados[2]);
                    double saldo = stod(dados[3]);

                    contas.push_back(Conta(titular, banco, numero, saldo));
                }
            }
            arquivo.close();
        }
    }
};

// Função para criar uma nova conta
void criarConta(vector<Conta>& contas) {
    string titular, banco;
    int numero;

    cout << "Digite o nome do titular: ";
    cin.ignore();
    getline(cin, titular);

    cout << "Escolha o banco (Itau, Bradesco, Caixa Economica): ";
    getline(cin, banco);

    cout << "Digite o numero da conta: ";
    cin >> numero;

    Conta novaConta(titular, banco, numero);
    Conta::salvarConta(novaConta);
    contas.push_back(novaConta);

    cout << "Conta criada com sucesso!" << endl;
}

// Função para realizar uma transação
void realizarTransacao(Conta& conta, const string& tipo) {
    double valor;
    cout << "Digite o valor: R$";
    cin >> valor;

    if (tipo == "Deposito") {
        conta.saldo += valor;
    }
    else if (tipo == "Saque") {
        if (valor <= conta.saldo) {
            conta.saldo -= valor;
        }
        else {
            cout << "Saldo insuficiente!" << endl;
            return;
        }
    }

    Conta::registrarTransacao(tipo, valor, conta);
    cout << tipo << " realizado com sucesso! Saldo atual: R$" << conta.saldo << endl;
}

// Função para realizar transferência entre contas
void realizarTransferencia(Conta& origem, Conta& destino) {
    double valor;
    cout << "Digite o valor para transferência: R$";
    cin >> valor;

    if (valor <= origem.saldo) {
        origem.saldo -= valor;
        destino.saldo += valor;

        Conta::registrarTransacao("Transferencia (origem)", valor, origem);
        Conta::registrarTransacao("Transferencia (destino)", valor, destino);

        cout << "Transferência realizada com sucesso!" << endl;
    }
    else {
        cout << "Saldo insuficiente para transferência!" << endl;
    }
}

// Função principal de gerenciamento de contas
void gerenciarContas(vector<Conta>& contas) {
    int opcao;
    do {
        limparTela();
        cout << "\n--- Gerenciamento de Contas ---" << endl;
        cout << "1. Consultar contas" << endl;
        cout << "2. Realizar transação" << endl;
        cout << "3. Realizar transferência" << endl;
        cout << "4. Sair" << endl;
        cout << "Opção: ";
        cin >> opcao;

        switch (opcao) {
        case 1:
            for (const auto& conta : contas) {
                cout << "Titular: " << conta.titular << " - Banco: " << conta.banco
                    << " - Número: " << conta.numero << " - Saldo: R$" << conta.saldo << endl;
            }
            break;
        case 2: {
            int numeroConta;
            cout << "Digite o número da conta: ";
            cin >> numeroConta;

            for (auto& conta : contas) {
                if (conta.numero == numeroConta) {
                    string tipo;
                    cout << "Escolha o tipo de transação (Deposito/Saque): ";
                    cin >> tipo;
                    realizarTransacao(conta, tipo);
                    break;
                }
            }
            break;
        }
        case 3: {
            int numeroOrigem, numeroDestino;
            cout << "Digite o número da conta de origem: ";
            cin >> numeroOrigem;
            cout << "Digite o número da conta de destino: ";
            cin >> numeroDestino;

            Conta* origem = nullptr;
            Conta* destino = nullptr;

            for (auto& conta : contas) {
                if (conta.numero == numeroOrigem) {
                    origem = &conta;
                }
                else if (conta.numero == numeroDestino) {
                    destino = &conta;
                }
            }

            if (origem && destino) {
                realizarTransferencia(*origem, *destino);
            }
            else {
                cout << "Contas não encontradas." << endl;
            }
            break;
        }
        case 4:
            cout << "Saindo..." << endl;
            break;
        default:
            cout << "Opção inválida!" << endl;
        }

    } while (opcao != 4);
}

// Função para autenticar o administrador
bool autenticarAdmin() {
    string login, senha;
    cout << "Login: ";
    cin >> login;
    cout << "Senha: ";
    cin >> senha;

    if (login == ADMIN_LOGIN && senha == ADMIN_SENHA) {
        return true;
    }
    cerr << "Login ou senha incorretos!" << endl;
    return false;
}

int main() {
    vector<Usuario> usuarios;
    vector<Conta> contas;

    // Carregar dados de usuários e contas dos arquivos
    Usuario::carregarUsuarios(usuarios);
    Conta::carregarContas(contas);

    // Autenticar o administrador
    if (!autenticarAdmin()) {
        return 0;
    }

    int opcao;
    do {
        limparTela();
        cout << "\n--- Menu Principal ---" << endl;
        cout << "1. Criar nova conta" << endl;
        cout << "2. Login" << endl;
        cout << "3. Cadastrar novo usuário" << endl;
        cout << "4. Sair" << endl;
        cout << "Opção: ";
        cin >> opcao;

        switch (opcao) {
        case 1:
            criarConta(contas);
            break;
        case 2: {
            string login, senha;
            cout << "Login: ";
            cin >> login;
            cout << "Senha: ";
            cin >> senha;

            if (Usuario::autenticar(login, senha, usuarios)) {
                gerenciarContas(contas);
            }
            else {
                cout << "Login ou senha incorretos." << endl;
            }
            break;
        }
        case 3: {
            string login, senha;
            cout << "Digite o login: ";
            cin >> login;
            cout << "Digite a senha: ";
            cin >> senha;

            Usuario novoUsuario(login, senha);
            Usuario::salvarUsuario(novoUsuario);
            usuarios.push_back(novoUsuario);

            cout << "Usuário cadastrado com sucesso!" << endl;
            break;
        }
        case 4:
            cout << "Saindo..." << endl;
            break;
        default:
            cout << "Opção inválida!" << endl;
        }

    } while (opcao != 4);

    return 0;
}
