#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define RESET   "\033[0m"
#define AMARILLO "\033[33m"
#define AZUL    "\033[34m"
#define ROJO    "\033[31m"


struct ConfigLiga {
    string nombre;
    int puntosGanar = 0;
    int puntosEmpatar = 0;
    int puntosPerder = 0;
    vector<string> equipos;
};

struct Partido {
    string fecha;
    string local;
    string visitante;
    int golesLocal;
    int golesVisitante;
};

struct Equipo {
    string nombre;
    int PJ = 0, PG = 0, PE = 0, PP = 0;
    int GF = 0, GC = 0, DG = 0, PTS = 0;
};


bool leerConfig(ConfigLiga &c) {
    ifstream file("../data/config.txt");

    if (!file.is_open()) {
        cout << ROJO << "Error: no se abre config.txt\n" << RESET;
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        string key, value;
        stringstream ss(line);

        getline(ss, key, '=');
        getline(ss, value);

        if (key == "liga") c.nombre = value;
        else if (key == "puntos_ganar") c.puntosGanar = stoi(value);
        else if (key == "puntos_empatar") c.puntosEmpatar = stoi(value);
        else if (key == "puntos_perder") c.puntosPerder = stoi(value);
        else if (key == "equipo") c.equipos.push_back(value);
    }

    return true;
}


void guardarPartido(Partido p) {
    ofstream file("../data/partidos.txt", ios::app);

    if (!file.is_open()) {
        cout << ROJO << "Error: no se abre partidos.txt\n" << RESET;
        return;
    }

    file << p.fecha << ";" << p.local << ";" << p.visitante << ";"
         << p.golesLocal << ";" << p.golesVisitante << endl;
}


void guardarJornada(Partido p) {
    ofstream file("../data/fechas.txt", ios::app);

    if (!file.is_open()) {
        cout << ROJO << "Error: no se abre fechas.txt\n" << RESET;
        return;
    }

    file << "JORNADA=1\n";
    file << p.fecha << ";" << p.local << ";" << p.visitante << ";"
         << p.golesLocal << ";" << p.golesVisitante << "\n";
    file << "FIN_JORNADA\n";
}


vector<Partido> leerPartidos() {
    vector<Partido> partidos;
    ifstream file("../data/partidos.txt");

    if (!file.is_open()) return partidos;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Partido p;
        string temp;

        getline(ss, p.fecha, ';');
        getline(ss, p.local, ';');
        getline(ss, p.visitante, ';');

        getline(ss, temp, ';');
        p.golesLocal = stoi(temp);

        getline(ss, temp);
        p.golesVisitante = stoi(temp);

        partidos.push_back(p);
    }

    return partidos;
}


bool partidoDuplicado(Partido nuevo) {
    vector<Partido> partidos = leerPartidos();

    for (auto &p : partidos) {
        if (p.fecha == nuevo.fecha &&
            p.local == nuevo.local &&
            p.visitante == nuevo.visitante) {
            return true;
        }
    }
    return false;
}


void mostrarPartidos() {
    vector<Partido> partidos = leerPartidos();

    cout << AMARILLO << "\n=== PARTIDOS ===\n" << RESET;

    for (auto &p : partidos) {
        cout << AZUL << p.fecha << RESET << " | "
             << p.local << " vs " << p.visitante
             << " (" << p.golesLocal << "-"
             << p.golesVisitante << ")\n";
    }
}


void mostrarJornadas() {
    ifstream file("../data/fechas.txt");

    if (!file.is_open()) {
        cout << ROJO << "No existe fechas.txt\n" << RESET;
        return;
    }

    cout << AMARILLO << "\n=== JORNADAS ===\n" << RESET;

    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
}


void historialEnfrentamientos(ConfigLiga &config) {
    int a, b;

    cout << "\nEquipos:\n";
    for (int i = 0; i < config.equipos.size(); i++) {
        cout << i << ". " << config.equipos[i] << endl;
    }

    cout << "Equipo 1: ";
    cin >> a;
    cout << "Equipo 2: ";
    cin >> b;

    if (a == b || a < 0 || b < 0 || a >= config.equipos.size() || b >= config.equipos.size()) {
        cout << ROJO << "Seleccion invalida\n" << RESET;
        return;
    }

    string eq1 = config.equipos[a];
    string eq2 = config.equipos[b];

    vector<Partido> partidos = leerPartidos();

    cout << AMARILLO << "\nHistorial entre " << eq1 << " y " << eq2 << ":\n" << RESET;

    for (auto &p : partidos) {
        if ((p.local == eq1 && p.visitante == eq2) ||
            (p.local == eq2 && p.visitante == eq1)) {

            cout << p.fecha << " | "
                 << p.local << " vs " << p.visitante
                 << " (" << p.golesLocal << "-"
                 << p.golesVisitante << ")\n";
        }
    }
}


void editarPartido() {
    vector<Partido> partidos = leerPartidos();

    if (partidos.empty()) {
        cout << ROJO << "No hay partidos\n" << RESET;
        return;
    }

    cout << "\nLista de partidos:\n";

    for (int i = 0; i < partidos.size(); i++) {
        cout << i << ". "
             << partidos[i].fecha << " | "
             << partidos[i].local << " vs "
             << partidos[i].visitante
             << " (" << partidos[i].golesLocal
             << "-" << partidos[i].golesVisitante << ")\n";
    }

    int idx;
    cout << "Seleccione partido a editar: ";
    cin >> idx;

    if (idx < 0 || idx >= partidos.size()) {
        cout << ROJO << "Indice invalido\n" << RESET;
        return;
    }

    cout << "Nuevo goles local: ";
    cin >> partidos[idx].golesLocal;

    cout << "Nuevo goles visitante: ";
    cin >> partidos[idx].golesVisitante;

    ofstream file("../data/partidos.txt");

    if (!file.is_open()) {
        cout << ROJO << "Error al escribir archivo\n" << RESET;
        return;
    }

    for (auto &p : partidos) {
        file << p.fecha << ";" << p.local << ";" << p.visitante << ";"
             << p.golesLocal << ";" << p.golesVisitante << endl;
    }

    cout << AMARILLO << "Partido actualizado\n" << RESET;
}


void registrarPartido(ConfigLiga &config) {
    Partido p;

    cout << "\nFecha (YYYY-MM-DD): ";
    cin >> p.fecha;

    cout << "\nEquipos:\n";
    for (int i = 0; i < config.equipos.size(); i++) {
        cout << i << ". " << config.equipos[i] << endl;
    }

    int a, b;

    cout << "Local: ";
    cin >> a;
    cout << "Visitante: ";
    cin >> b;

    if (a == b || a < 0 || b < 0 || a >= config.equipos.size() || b >= config.equipos.size()) {
        cout << ROJO << "Error seleccion de equipos\n" << RESET;
        return;
    }

    p.local = config.equipos[a];
    p.visitante = config.equipos[b];

    cout << "Goles local: ";
    cin >> p.golesLocal;
    cout << "Goles visitante: ";
    cin >> p.golesVisitante;

    if (partidoDuplicado(p)) {
        cout << ROJO << "Error: partido duplicado\n" << RESET;
        return;
    }

    guardarPartido(p);
    guardarJornada(p);

    cout << AMARILLO << "El partido ha sido guardado\n" << RESET;
}


void actualizarEquipo(Equipo* e, int gf, int gc, ConfigLiga &config) {
    e->PJ++;
    e->GF += gf;
    e->GC += gc;
    e->DG = e->GF - e->GC;

    if (gf > gc) {
        e->PG++;
        e->PTS += config.puntosGanar;
    }
    else if (gf == gc) {
        e->PE++;
        e->PTS += config.puntosEmpatar;
    }
    else {
        e->PP++;
        e->PTS += config.puntosPerder;
    }
}


vector<Equipo> construirTabla(vector<Partido> &partidos, ConfigLiga &config) {
    vector<Equipo> tabla;

    for (auto &nombre : config.equipos) {
        Equipo e;
        e.nombre = nombre;
        tabla.push_back(e);
    }

    for (auto &p : partidos) {
        for (int i = 0; i < tabla.size(); i++) {
            if (tabla[i].nombre == p.local) {
                actualizarEquipo(&tabla[i], p.golesLocal, p.golesVisitante, config);
            }
            else if (tabla[i].nombre == p.visitante) {
                actualizarEquipo(&tabla[i], p.golesVisitante, p.golesLocal, config);
            }
        }
    }

    return tabla;
}


bool comparar(Equipo a, Equipo b) {
    if (a.PTS != b.PTS) return a.PTS > b.PTS;
    if (a.DG != b.DG) return a.DG > b.DG;
    return a.GF > b.GF;
}

void ordenarTabla(vector<Equipo> &tabla) {
    sort(tabla.begin(), tabla.end(), comparar);
}


void mostrarTabla(vector<Equipo> &tabla) {
    cout << AMARILLO << "\n# Equipo PJ PG PE PP GF GC DG PTS\n" << RESET;

    int pos = 1;
    for (auto &e : tabla) {
        cout << pos++ << " " << e.nombre << " "
             << e.PJ << " " << e.PG << " " << e.PE << " "
             << e.PP << " " << e.GF << " " << e.GC << " "
             << e.DG << " " << e.PTS << endl;
    }
}


void guardarTabla(vector<Equipo> &tabla) {
    ofstream file("../data/tabla.txt");

    if (!file.is_open()) {
        cout << ROJO << "Error tabla.txt\n" << RESET;
        return;
    }

    file << "# Equipo PJ PG PE PP GF GC DG PTS\n";

    for (auto &e : tabla) {
        file << e.nombre << " "
             << e.PJ << " " << e.PG << " "
             << e.PE << " " << e.PP << " "
             << e.GF << " " << e.GC << " "
             << e.DG << " " << e.PTS << "\n";
    }
}


int menu(ConfigLiga &config) {
    cout << AMARILLO;
    cout << "\n========================================\n";
    cout << "        PROYECTO: " << config.nombre << "\n";
    cout << "========================================\n";
    cout << RESET;

    cout << AZUL << "1. " << RESET << "Ver tabla\n";
    cout << AZUL << "2. " << RESET << "Registrar un partido\n";
    cout << AZUL << "3. " << RESET << "Historial de jornadas\n";
    cout << AZUL << "4. " << RESET << "listado de todos los partidos\n";
    cout << AZUL << "5. " << RESET << "Enfrentamientos\n";
    cout << AZUL << "6. " << RESET << "Editar un partido\n";

    cout << ROJO << "7. Salir\n" << RESET;

    cout << "\n" << AMARILLO << "Seleccione una opcion (SOLO NUMERO): " << RESET;

    int op;
    cin >> op;
    return op;
}


int main() {
    ConfigLiga config;

    if (!leerConfig(config)) return 1;

    int opcion;

    do {
        opcion = menu(config);

        if (opcion == 1) {
            vector<Partido> partidos = leerPartidos();
            vector<Equipo> tabla = construirTabla(partidos, config);
            ordenarTabla(tabla);
            mostrarTabla(tabla);
            guardarTabla(tabla);
        }
        else if (opcion == 2) {
            registrarPartido(config);
        }
        else if (opcion == 3) {
            mostrarJornadas();
        }
        else if (opcion == 4) {
            mostrarPartidos();
        }
        else if (opcion == 5) {
            historialEnfrentamientos(config);
        }
        else if (opcion == 6) {
            editarPartido();
        }
        else if (opcion != 7) {
            cout << ROJO << "Opcion no es valida\n" << RESET;
        }

    } while (opcion != 7);

    return 0;
}