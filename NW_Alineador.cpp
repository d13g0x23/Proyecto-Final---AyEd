#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

using namespace std;

int GAP; 
vector<vector<int>> Matriz_U;

// Lee y valida los argumentos de entrada
void PasarArgumentos(int argc, char *argv[], string &secArchivo1, string &secArchivo2, string &matrizArchivo, int &penalidadGap) {
    if (argc != 9) {
        cerr << "Uso: " << argv[0] << " -C1 sec1.txt -C2 sec2.txt -U matrizU.txt -V penalidad" << endl;
        exit(1);
    }

    for (int i = 1; i < argc; i += 2) {
        string arg = argv[i];
        if (arg == "-C1") {
            secArchivo1 = argv[i + 1];
        } else if (arg == "-C2") {
            secArchivo2 = argv[i + 1];
        } else if (arg == "-U") {
            matrizArchivo = argv[i + 1];
        } else if (arg == "-V") {
            penalidadGap = atoi(argv[i + 1]);
        } else {
            cerr << "Argumento no reconocido: " << arg << endl;
            exit(1);
        }
    }
}

// Lee la secuencia desde el txt
string LeerSecuencia(const string &filename) {
    ifstream archivo(filename);
    if (!archivo) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        exit(1);
    }
    string secuencia;
    archivo >> secuencia;
    archivo.close();

    if (secuencia.empty()) {
        cerr << "Error: El archivo " << filename << " está vacío o no contiene una secuencia válida." << endl;
        exit(1);
    }

    return secuencia;
}

// Lee la matriz de puntuación desde el txt
void LeerMatrizPuntajes(const string &filename, vector<vector<int>> &matriz) {
    ifstream archivo(filename);
    if (!archivo) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        exit(1);
    }
    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        vector<int> fila;
        int valor;
        while (ss >> valor) {
            fila.push_back(valor);
        }
        matriz.push_back(fila);
    }
    archivo.close();

    if (matriz.size() != 4 || matriz[0].size() != 4) {
        cerr << "Error: La matriz de emparejamiento debe ser de tamaño 4x4." << endl;
        exit(1);
    }
}

// Obtiene el puntaje de emparejamiento
int ObtenerPuntaje(char a, char b) {
    int i, j;

    if (a == 'A') {
        i = 0;
    } else if (a == 'C') {
        i = 1;
    } else if (a == 'G') {
        i = 2;
    } else if (a == 'T') {
        i = 3;
    } else {
        i = -1;
    }

    if (b == 'A') {
        j = 0;
    } else if (b == 'C') {
        j = 1;
    } else if (b == 'G') {
        j = 2;
    } else if (b == 'T') {
        j = 3;
    } else {
        j = -1;
    }

    if (i >= 0 && j >= 0) {
        return Matriz_U[i][j];
    } else {
        cerr << "Error: Caracter no válido en las secuencias." << endl;
        exit(1);
    }
}

// Inicializa la matriz de puntajes
vector<vector<int>> InicializarMatriz(int filas, int columnas) {
    vector<vector<int>> matriz(filas, vector<int>(columnas, 0));

    // Inicializa las primeras filas y columnas para los gaps
    int i = 1;
    while (i < filas) {
        matriz[i][0] = i * GAP;
        ++i;
    }

    int j = 1;
    while (j < columnas) {
        matriz[0][j] = j * GAP;
        ++j;
    }

    return matriz;
}

// Llena la matriz de puntajes
void LlenarMatriz(vector<vector<int>> &matriz, const string &secS, const string &secT) {
    int n = secS.size(), m = secT.size();

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            // Puntaje por coincidencia (match)
            int match = matriz[i - 1][j - 1] + ObtenerPuntaje(secS[i - 1], secT[j - 1]);
            
            // Puntaje por gap en la secuencia 1
            int borrarGap = matriz[i - 1][j] + GAP; // Gap en secuencia 1
            
            // Puntaje por gap en la secuencia 2
            int insertarGap = matriz[i][j - 1] + GAP; // Gap en secuencia 2
            
            // Asignamos el máximo puntaje de las tres opciones
            matriz[i][j] = max({match, borrarGap, insertarGap});
        }
    }
}

// Muestra la matriz de puntajes
void MostrarMatriz(const vector<vector<int>> &matriz, const string &secS, const string &secT) {
    cout << "\nMatriz de puntajes:\n    ";
    cout << "    ";
    for (size_t i = 0; i < secT.length(); ++i) {
        cout << secT[i] << "   ";
    }
    cout << endl;

    for (size_t i = 0; i < matriz.size(); ++i) {
        if (i > 0) {
            cout << secS[i - 1] << " ";
        } else {
            cout << "  ";
        }
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            cout << setw(3) << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

// Reconstruye el alineamiento óptimo
pair<string, string> Traceback(const vector<vector<int>> &matriz, const string &secS, const string &secT) {
    string alineamiento1, alineamiento2;
    int i = secS.size(), j = secT.size();

    // Seguimos el camino hacia atrás en la matriz para reconstruir el alineamiento
    while (i > 0 || j > 0) {
        // Caso 1: El puntaje proviene de una coincidencia
        if (i > 0 && j > 0 && matriz[i][j] == matriz[i - 1][j - 1] + ObtenerPuntaje(secS[i - 1], secT[j - 1])) {
            alineamiento1 = secS[i - 1] + alineamiento1;
            alineamiento2 = secT[j - 1] + alineamiento2;
            --i; --j;
        }
        // Caso 2: El puntaje proviene de un gap en la secuencia 1
        else if (i > 0 && matriz[i][j] == matriz[i - 1][j] + GAP) {
            alineamiento1 = secS[i - 1] + alineamiento1;
            alineamiento2 = '-' + alineamiento2;
            --i;
        }
        // Caso 3: El puntaje proviene de un gap en la secuencia 2
        else if (j > 0 && matriz[i][j] == matriz[i][j - 1] + GAP) {
            alineamiento1 = '-' + alineamiento1;
            alineamiento2 = secT[j - 1] + alineamiento2;
            --j;
        }
        // Caso 4: Si no proviene de coincidencia ni gap, entonces es una coincidencia entre los caracteres de las secuencias
        else {
            alineamiento1 = secS[i - 1] + alineamiento1;
            alineamiento2 = secT[j - 1] + alineamiento2;
            --i;
            --j;
        }
    }

    return {alineamiento1, alineamiento2};
}

// Genera un archivo DOT (Graphviz)
void GenerarArchivoDOT(const string &alineamiento1, const string &alineamiento2, const string &filename) {
    ofstream archivo(filename);
    if (!archivo) {
        cerr << "Error al crear el archivo DOT: " << filename << endl;
        exit(1);
    }

    archivo << "digraph Alineamiento {\n";
    archivo << "  rankdir=LR;\n";
    archivo << "  node [shape=record];\n";

    for (size_t i = 0; i < alineamiento1.size(); ++i) {
        archivo << "  node" << i << " [label=\"{ " << alineamiento1[i] << " | " << alineamiento2[i] << " }\"];\n";
        if (i > 0) {
            archivo << "  node" << i - 1 << " -> node" << i << ";\n";
        }
    }
    archivo << "}\n";
    archivo.close();
    cout << "Archivo DOT generado como: " << filename << endl;
}

// Muestra la imagen automáticamente
void MostrarImagen(const string &alineamiento1, const string &alineamiento2, const string &filename) {
    GenerarArchivoDOT(alineamiento1, alineamiento2, filename);
    system("dot -Tpng Alineamiento.dot -o Alineamiento.png");

    const char* comando;
    
    if (system("uname") == 0) {
        comando = "xdg-open Alineamiento.png";  // Linuss
    } else if (system("ver") == 0) {
        comando = "start Alineamiento.png";  // Windows
    } else {
        cerr << "No se pudo detectar el sistema operativo. No se puede abrir la imagen." << endl;
        return;
    }
    system(comando);
}

// Muestra los resultados
void MostrarResultados(const string &alineamiento1, const string &alineamiento2, const vector<vector<int>> &matriz, const string &secS, const string &secT) {
    cout << "\nAlineamiento óptimo:\n";
    cout << alineamiento1 << endl;
    cout << alineamiento2 << endl;
    cout << "\nPuntaje máximo: " << matriz[secS.size()][secT.size()] << endl;
}

/*  Formato matrizU.txt:
         A    C    G    T
    A   [ 3,  -1,  -2,  -1 ]
    C   [ -1,  3,  -1,  -2 ]
    G   [ -2,  -1,  3,  -1 ]
    T   [ -1,  -2,  -1,  3 ]

*/

/* Algunas librerias importantes (según yo):
    #include <fstream> ---> Lectura y escritura de archivos
    #include <sstream> ---> Covierte cadenas de texto a datos numéricos
    #include <iomanip> ---> Controla el formato de salida (dar formato a la matriz de puntajes con setw())
    #include <cstdlib> ---> Para la función atoi() (de cadena a entero) y exit(), en caso de errores
    #include <algorithm> ---> Para la función max(), para llenar la matriz de puntajes
*/

int main(int argc, char *argv[]) {
    string secArchivo1, secArchivo2, matrizArchivo;
    int penalidadGap;

    PasarArgumentos(argc, argv, secArchivo1, secArchivo2, matrizArchivo, penalidadGap);
    GAP = penalidadGap;

    string secS = LeerSecuencia(secArchivo1);
    string secT = LeerSecuencia(secArchivo2);
    LeerMatrizPuntajes(matrizArchivo, Matriz_U);

    cout << "Secuencia S: " << secS << endl;
    cout << "Secuencia T: " << secT << endl;
    cout << "\n";

    vector<vector<int>> matriz = InicializarMatriz(secS.size() + 1, secT.size() + 1);
    LlenarMatriz(matriz, secS, secT);

    MostrarMatriz(matriz, secS, secT);

    auto alineamiento = Traceback(matriz, secS, secT);
    MostrarResultados(alineamiento.first, alineamiento.second, matriz, secS, secT);

    MostrarImagen(alineamiento.first, alineamiento.second, "Alineamiento.dot");

    return 0;
}