#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <chrono>

using namespace std;

#define CSV_FILE "ventas_sudamerica.csv"

struct Venta
{
    string pais;

    string producto;

    int unidades;

    double precioUnitario;
};

static size_t contadorCondicionales = 0;
inline void tickIf() { ++contadorCondicionales; }

vector<string> splitCSV(const string &linea, char delimitador = ',')
{
    vector<string> tokens;
    bool enComillas = false;
    string token;

    for (size_t i = 0; i < linea.size(); ++i)
    {
        char c = linea[i];
        if (c == '\"')
        {
            enComillas = !enComillas;
            continue;
        }
        if (c == delimitador && !enComillas)
        {
            tokens.push_back(token);
            token.clear();
        }
        else
        {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}

bool cargarDatos(
    unordered_map<string, int> &ventasPorPais,

    unordered_map<string, int> &ventasPorProducto,

    map<string, double> &precioMaximoPorProducto)
{
    ifstream archivo(CSV_FILE);
    if (!archivo.is_open())
    {
        cerr << "Error: con el archivo '" << CSV_FILE << "'." << endl;
        return false;
    }

    string linea;
    getline(archivo, linea);

    while (getline(archivo, linea))
    {
        vector<string> campos = splitCSV(linea);

        if (campos.size() < 10)
        {
            tickIf();
            continue;
        }

        Venta venta;

        venta.pais = campos[2];

        venta.producto = campos[5];

        venta.unidades = stoi(campos[7]);

        venta.precioUnitario = stod(campos[8]);

        ventasPorPais[venta.pais] += venta.unidades;

        ventasPorProducto[venta.producto] += venta.unidades;

        auto it = precioMaximoPorProducto.find(venta.producto);

        if (it == precioMaximoPorProducto.end())
        {
            precioMaximoPorProducto[venta.producto] = venta.precioUnitario;
        }
        else
        {
            if (venta.precioUnitario > it->second)
            {
                it->second = venta.precioUnitario;
            }
        }
    }

    return true;
}

void mostrarVentasPorPais(const unordered_map<string, int> &ventasPorPais)
{
    cout << "\n=== Cantidad de unidades vendidas por país ==" << endl;
    for (const auto &par : ventasPorPais)
    {
        cout << "- " << par.first << ": " << par.second << " unidades" << endl;
    }
}

pair<string, int> obtenerPaisTop(const unordered_map<string, int> &ventasPorPais)
{
    string topPais;

    int maxVentas = 0;
    for (const auto &par : ventasPorPais)
    {
        if (par.second > maxVentas)
        {
            maxVentas = par.second;
            topPais = par.first;
        }
    }
    return {topPais, maxVentas};
}

pair<string, int> obtenerProductoTop(const unordered_map<string, int> &ventasPorProducto)
{
    string topProd;

    int maxUnidades = 0;
    for (const auto &par : ventasPorProducto)
    {
        if (par.second > maxUnidades)
        {
            maxUnidades = par.second;
            topProd = par.first;
        }
    }
    return {topProd, maxUnidades};
}

pair<string, double> obtenerProductoMasCaro(const map<string, double> &precioMaximoPorProducto)
{
    string prodCarisimo;

    double precio = 0.0;

    for (const auto &par : precioMaximoPorProducto)
    {
        if (par.second > precio)
        {
            precio = par.second;
            prodCarisimo = par.first;
        }
    }
    return {prodCarisimo, precio};
}

void pausa()
{
    cout << "\nPulse ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main()
{
    unordered_map<string, int> ventasPorPais;

    unordered_map<string, int> ventasPorProducto;

    map<string, double> precioMaximoPorProducto;

    auto comienzoCarga = chrono::steady_clock::now();

    bool ok = cargarDatos(ventasPorPais, ventasPorProducto, precioMaximoPorProducto);

    auto finCarga = chrono::steady_clock::now();

    if (!ok)
    {
        return 1;
    }

    chrono::duration<double, milli> tiempoCarga = finCarga - comienzoCarga;
    cout << "Datos cargados en " << tiempoCarga.count() << " ms.\n";

    int opcion = 0;
    while (true)
    {
        cout << "\n++++++++++++++++++++ MENU ++++++++++++++++++++" << endl;

        cout << "1. Mostrar cantidad vendida por cada país" << endl;

        cout << "2. Mostrar país con más ventas" << endl;

        cout << "3. Mostrar producto más vendido" << endl;

        cout << "4. Mostrar producto con precio máximo" << endl;

        cout << "5. Mostrar métricas de eficiencia" << endl;

        cout <<  "6. Mostrar promedio de precio por producto" << endl;

        cout << "7. Mostrar ventas totales" << endl;

        cout << "8. Mostrar cantidad de productos diferentes" << endl;

        cout << "9. Mostrar cantidad de países diferentes" << endl;

        cout << "10. Mostrar listado de productos y sus ventas" << endl;

        cout << "11. Mostrar listado de países y sus ventas" << endl;

        cout << "12. Mostrar producto con menor precio unitario" << endl;

        cout << "13. Mostrar país con menor ventas" << endl;

        cout << "14. Mostrar producto con menor ventas" << endl;

        cout << "15. Salir" << endl;

        cout << "Seleccione una opción: ";
        cin >> opcion;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Entrada invalida. Intente nuevamente." << endl;
            continue;
        }

        auto inicioProceso = chrono::steady_clock::now();

        switch (opcion)
        {
        case 1:
        {
            mostrarVentasPorPais(ventasPorPais);
            pausa();
            break;
        }
        case 2:
        {
            auto [pais, unidades] = obtenerPaisTop(ventasPorPais);

            cout << "\nEl país con mayor cantidad de unidades vendidas es "

                 << pais << " con " << unidades << " unidades." << endl;

            pausa();
            break;
        }
        case 3:
        {
            auto [producto, cant] = obtenerProductoTop(ventasPorProducto);

            cout << "\nEl producto más vendido es '" << producto

                 << "' con " << cant << " unidades." << endl;

            pausa();
            break;
        }
        case 4:
        {
            auto [producto, precio] = obtenerProductoMasCaro(precioMaximoPorProducto);

            cout << "\nEl producto más caro registrado es '" << producto

                 << "' con un precio unitario de $" << precio << endl;

            pausa();
            break;
        }
        case 5:
        {
            auto finProceso = chrono::steady_clock::now();

            chrono::duration<double, milli> dur = finProceso - inicioProceso;

            cout << "\n=== Métricas ===" << endl;

            cout << "Tiempo de la última operación: " << dur.count() << " ms" << endl;

            cout << "Condicionales evaluadas (aproximado): "

                 << contadorCondicionales << endl;

            pausa();
            break;
        }
        case 15:
        {
            cout << "\nSaliendo... Gracias por utilizar el programa" << endl;
            return 0;
        }
        case 7:
        {
            int total = 0;
            for (const auto &par : ventasPorPais)
            {
                total += par.second;
            }
            cout << "\nEl total de unidades vendidas en Sudamérica es: " << total << " unidades." << endl;
            pausa();
            break;
        }
        case 8:
        {
            cout << "\nCantidad de productos diferentes vendidos: " << ventasPorProducto.size() << endl;
            pausa();
            break;
        }
        case 9:
        {
            cout << "\nCantidad de países diferentes en el registro: " << ventasPorPais.size() << endl;
            pausa();
            break;
        }
        case 10:
        {
            cout << "\n=== Listado de productos y sus unidades vendidas ===" << endl;
            for (const auto &par : ventasPorProducto)
            {
                cout << "- " << par.first << ": " << par.second << " unidades" << endl;
            }
            pausa();
            break;
        }
        case 11:
        {
            cout << "\n=== Listado de países y sus unidades vendidas ===" << endl;
            for (const auto &par : ventasPorPais)
            {
                cout << "- " << par.first << ": " << par.second << " unidades" << endl;
            }
            pausa();
            break;
        }
        case 12:
        {
            string prodMasBarato;
            double precioMin = numeric_limits<double>::max();
            for (const auto &par : precioMaximoPorProducto)
            {
                if (par.second < precioMin)
                {
                    precioMin = par.second;
                    prodMasBarato = par.first;
                }
            }
            cout << "\nEl producto más barato registrado es '" << prodMasBarato

                 << "' con un precio unitario de $" << precioMin << endl;
            pausa();
            break;
        }
        case 13:
        {
            string paisMin;
            int minVentas = numeric_limits<int>::max();
            for (const auto &par : ventasPorPais)
            {
                if (par.second < minVentas)
                {
                    minVentas = par.second;
                    paisMin = par.first;
                }
            }
            cout << "\nEl país con menor cantidad de unidades vendidas es "
                 << paisMin << " con " << minVentas << " unidades." << endl;
            pausa();
            break;
        }
        case 14:
        {
            string prodMin;
            int minUnidades = numeric_limits<int>::max();
            for (const auto &par : ventasPorProducto)
            {
                if (par.second < minUnidades)
                {
                    minUnidades = par.second;
                    prodMin = par.first;
                }
            }
            cout << "\nEl producto con menor cantidad de unidades vendidas es '"

                 << prodMin << "' con " << minUnidades << " unidades." << endl;
            pausa();
            break;
        }
        case 6:
        {
            map<string, pair<double, int>> sumaPreciosPorProducto;

            ifstream archivo(CSV_FILE);

            if (!archivo.is_open())
            {
                cerr << "Error al abrir archivo para calcular promedios." << endl;
                break;
            }

            string linea;
            getline(archivo, linea);

            while (getline(archivo, linea))
            {
                vector<string> campos = splitCSV(linea);
                if (campos.size() < 10)
                {
                    tickIf();
                    continue;
                }

                string producto = campos[5];

                int unidades = stoi(campos[7]);

                double precio = stod(campos[8]);

                sumaPreciosPorProducto[producto].first += precio * unidades;

                sumaPreciosPorProducto[producto].second += unidades;
            }

            cout << "\n=== Promedio de precio unitario por producto ===" << endl;

            for (const auto &par : sumaPreciosPorProducto)
            {
                const string &producto = par.first;

                double sumaPrecioTotal = par.second.first;

                int totalUnidades = par.second.second;

                double promedio = (totalUnidades == 0) ? 0.0 : sumaPrecioTotal / totalUnidades;

                cout << "- " << producto << ": $" << promedio << " promedio unitario" << endl;
            }
            pausa();
            break;
        }
        default:
        {
            cerr << "Opción no encontrada. Elija entre 1 y 15." << endl;
        }
        }
    }

    return 0;
}
