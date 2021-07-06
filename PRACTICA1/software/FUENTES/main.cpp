#include <iostream>
#include "random.h"
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <chrono>

using namespace std::chrono;
using namespace std;

//FUNCION QUE OBTIENE DE UN FICHERO DE DATOS LA CANTIDAD DE PUNTOS TOTALES Y LOS QUE NECESITAMOS ESCOGER
//m -> Variable por referencia donde se devuelve la cantidad de puntos a seleccionar en la solucion
//MDG -> nombre del  fichero del que sacar los datos (se pasa como argumento de la ejecucion del programa)
//return n -> devuelve un entero con el numero de elementos en total que tiene el fichero
int obtiene_n_m(int &m, const char MDG[]){

    //Es la cantidad de puntos que devolveremos como el total de puntos dentro del fichero
    int n = 0;
    //Leemos el fichero pasado como argumento en la variable f
    ifstream f(MDG);

    //Si el fichero se ha leido correctamente
    if(f){

        f >> n; //El primero elemento que contiene el fichero es la cantidad de puntos totales que hay
        f >> m; //El segundo elemento que contiene el fichero es la cantidad de puntos que hay que coger

    //Si el fichero no se ha leido correctamente
    }else{
        //Mensaje de error
        cout << "Error leyendo el archivo de entrada con los datos" << endl ;
        exit(-1);
    }

    //Cerramos el fichero cuando hemos leido lo que queriamos
    f.close();

    //Devuelve el numero de puntos total que habia dentro del fichero
    return n;

}

//FUNCION QUE LIBERA LA MEMORIA OCUPADA POR UNA MATRIZ DE PUNTEROS
//matriz_dist -> Matriz a eliminar
//n -> numero de filas o columnas que tiene la matriz para conocer sus dimensiones al borrarla
void vaciar_matriz(double **matriz_dist, int n){

    //Bucle que recorre todo el vector de vectores vaciando cada uno independiente
    for(int i = 0; i < n; i++)
        //Elimina cada vector del vector original
        delete[] matriz_dist[i];

    //Cuando cada vector ha sido liberado, liberamos el vector que los contenia
    delete[] matriz_dist;

}

//FUNCION QUE INTRODUCE EN UN VECTOR DE N ELEMENTOS EL INDICE DE CADA PUNTO DEL CONJUNTO DE POSIBLES SOLUCIONES(0,N-1)
//S -> Vector en el que se introducen los puntos posibles pero no seleccionados (inicialmente todos)
//n -> Numero de elementos que tiene que meter
void inicializa_candidatas( vector<int> &S, int n){

    //Introduce el indice de todos los puntos en el vector de puntos candidatos no seleccionados
    for(int i = 0 ; i < n ; i++)  S.push_back(i);

}

//FUNCION QUE OBTIENE UN VECTOR CON LA SOLUCION SELECCIONADA, EL NUMERO DE ELEMENTOS TOTALES, EL NUMERO DE ELEMENTOS A SELECCIONAR Y EL COSTE TOTAL DE LA SOLUCION
//m -> Variable con la cantidad de puntos a seleccionar en la solucion
//n -> Variable con la cantidad de puntos totales en el conjunto de datos
//Solucion -> Vector con los puntos seleccionados como solucion
//distancia_total_solucion -> Coste total obtenido de los puntos seleccionados
void imprimir_solucion(int n, int m, vector<int> Solucion, double distancia_total_solucion){

    //Mostramos la cantidad de puntos totales y puntos seleccionados
    cout <<"n (Cantidad de puntos): " <<n<< " y m (puntos a elegir): "<< m<<" "<<endl;
    //Contador para el numero de iteraciones del while
    int contador = 0;

    //Bucle que recorre el vector de solucion para mostrar los datos por pantalla
    while(contador < Solucion.size()){

        //Mostramos cada punto seleccionado
        cout<< "Solucion ["<<contador<<"]: "<<Solucion[contador];
        //Aumentamos el numero de iteraciones con las que recorremos el vector
        contador++;

        //Salto de linea cada 3 elementos para mostrar los datos en 3 columnas y aprovechar el ancho de la pantalla
        if(contador%3 ==0){
            cout<<endl;
        //Si seguimos mostrando los datos en la misma fila, tabulamos para mostar los datos ordenados
        }else{
            cout<<"\t";
        }

    }

    //Mostramos el coste total  con los puntos seleccionados como solucion
    cout<<endl;
    cout<<"\n\tDistancia total de la solucion: "<<distancia_total_solucion<<endl;

}

//FUNCION QUE LIMPIA EL CONTENIDO DE LOS VECTORES PARA QUE ESTOS SEAN USADOS DE NUEVO PARA EL SIGUIENTE ALGORITMO
//Solucion -> Vector que contenia las soluciones del algoritmo anterior
//S -> Vector que contenia originalmente todos los puntos y finalmente solo aquellos que no fueron seleccionados como parte de la solucion
void limpia_vectores(vector<int> &Solucion,vector<int> &S){
    //Se limpia el vector de las soluciones
    Solucion.clear();
    //Se limpia el vector de los puntos candidatos
    S.clear();
}

//FUNCION QUE RELLENA LA MATRIZ DE DISTANCIAS CON 0 PARA QUE LA DIAGONAL INFERIOR (NO USADA) NO CONTENGA BASURA
//matriz_dist -> Matriz que hay que rellenar con 0s
//n -> numero de filas o columnas que tiene la matriz para conocer sus dimensiones al borrarla
void rellena_matriz_ceros(double **matriz_dist, int n){

    //Bucle que mete ceros en cada hueco de la matriz
    for(int i = 0; i < n ; i++)
        for(int j = 0; j< n; j++)
            matriz_dist[i][j] = 0;


}

//FUNCION QUE RELLENA LAS POSICIONES i,j CON LA DISTANCIA ENTRE LOS PUNTOS i y j OBTENIDAS DEL FICHERO DE DATOS
//matriz_dist -> Matriz en la que almacenar los datos
//MDG -> nombre del  fichero del que sacar los datos (se pasa como argumento de la ejecucion del programa)
void inicializa_matriz_dist(double **matriz_dist, const char MDG[]){

    //Leemos el fichero pasado como parametros en la variable f
    ifstream f(MDG);

    //Variables que corresponden al indice del primero punto, indice del segundo y distancia entre ellos, respectivamente
    string i, j, distancia;
    //Variables auxiliares para hacer castings de los datos leidos para pasarlos al tipo de datos que queremos
    int primer_punto = 0;
    int segundo_punto = 0;
    double distancia_double = 0;

    //Si el fichero ha sido leido correctamente
    if(f){

        //Desechamos la primera linea que contiene informacion ya obtenida en otra funcion correspondiente a la cantidad de puntos totales y en la solucion
        getline(f,distancia);

        //Empezamos leyendo los 3 primeros elementos para quitar una iteracion del bucle y que no se nos quede fuera de la matriz
        f >> i; f >> j; f >> distancia;

        //Bucle mientras no lleguemos al final del archivo
        while (!f.eof()){

            //casting del punto i leido (primer punto)
            primer_punto = stoi(i);
            //casting del punto j leido (segundo punto)
            segundo_punto = stoi(j);
            //casting de la distancia leida entre un punto y el otro
            distancia_double = strtod(distancia.c_str(),NULL);

            //Introducimos en la posicion correcta la distancia correspondiente a esos puntos
            matriz_dist[primer_punto][segundo_punto] = distancia_double;

            //leemos la siguiente linea (si es la ultima se sale del bucle ya)
            f >> i;f >> j;f >> distancia;

        }

    }//Si no se abre correctamente el fichero
    else{
        //Mensaje de error
        cout << "Error leyendo el archivo de entrada con los datos"<< endl ;
        exit(-1);
    }

    //Antes de terminar la funcion cerramos el archivo del que sacamos los datos
    f.close();

}

//FUNCION QUE GENERA UN CONJUNTO DE M ELEMENTOS ALEATORIOS COMO SOLUCION DE PARTIDA PARA EL ALGORITMO DE BL
//semilla -> Variable que contiene el entero con el que inicializar la semilla de numeros aleatorios
//m -> Variable que contiene el numero de elementos a seleccionar
//n -> Variable que contiene el numero de elementos total que necesitamos
//Solucion -> Vector en el que introduciremos los m elementos aleatorios como solucion (sin repetir)
//S -> Vector del que se van a ir borrando los elementos que se seleccionen para la solucion aleatoria inicial. Solo contendra los no seleccionados
void genera_solucion_aleatoria(int semilla, int m, int n, vector<int> &Solucion, vector<int> &S){

    //Inicializamos la semilla de numeros aleatorios
    Set_random(semilla);
    //Booleano nos dira si el numero generado aleatoriamente está o no dentro del conjunto de Soluciones (para no repetirlos)
    bool noesta;
    //Contador de las iteraciones del bucle que rellena Soluciones
    int contador= 0 ;
    //Entero que contiene la posicion del vector S en el que se encuentra el elemento que añadimos a Solucion para eliminarlo
    int donde = 0;

    //Bucle que genera los m elementos necesarios. Se hacen 4 iteraciones mas porque inentendiblemente genera 4 numeros similares a la semilla consecutivos
    while(contador < m+4){
        //En cada iteracion ponemos en el booleano que el elemento añadido a Solucion no esta ya dentro para buscarlo luego
        noesta = true;
        //Se genera un numero aleatorio entre 0 y el numero de elementos total
        int x = Randint (0,n);
        //Si ya han pasado los 4 primeros numeros que sabemos que son iguales
        if(contador>=4){

            //Recorremos Soluciones para ver si esta ya dentro el numero generado aleatoriamente
            for(int j = 0; j < Solucion.size(); j++){

                //Si el numero esta, pone el booleano a false
                if(x == Solucion[j]) noesta=false;

            }
            //Si no estaba el elemento, lo añado
            if(noesta){

                //Busca dentro de S el elemento añadido en Soluciones para borrarlo de ahi
                for(int j = 0; j < S.size() && noesta; j++){

                    //Cuando encuentra el elemento guarda la posicion en la que esta para eliminarlo
                    if(x == S[j]){
                       donde = j;
                       noesta = false; //Para dejar de buscar si ya lo hemos encontrado. Detiene el bucle
                    }

                }

                //Añadimos el elemento a las soluciones
                Solucion.push_back(x);
                //Eliminamos el elemento añadido de las candidatas
                S.erase(S.begin()+donde);
                //Aumentamos las iteraciones
                contador++;
            }

        //Si aun estamos en las 4 primeras iteraciones dejamos correr la iteracion y generamos otro
        }else contador++;

    }


}


//FUNCION QUE OBTIENE LA DISTANCIA TOTAL DE UN PUNTO AL RESTO
//matriz_dist -> Matriz que contiene las distancias entre los puntos i y j en la posicion [i][j]
//n -> numero total de puntos para conocer la dimension de la matriz (nxn)
//punto -> indice del punto del que queremos conocer la distancia total acumulada al resto de puntos
double total_por_punto(double **matriz_dist, int n, int punto){

    //Variable que contendra la sumatoria de todas las distancias desde el punto 'punto'
    double sumatoria = 0;
    //Contador para las iteraciones del bucle que recorra todos los puntos del conjunto
    int contador = 0;
    //Booleano que nos indica si estamos sumando la columna o la fila. Solo esta rellena la mitad superior de la matriz, por lo que debemos coger para el punto i la posicion matriz[i][contador] si i < contador y matriz[contador][i] si contador < i
    bool columna = true;
    //Bucle que recorre la columna punto y fila punto de la matriz sumando las distancias
    while(contador<n-1){
        //Si tengo que leer la columna punto (si punto < contador)
        if(columna)
            sumatoria += matriz_dist[contador][punto];
        //Si tengo que leer la fila punto (si contador < punto)
        else
            sumatoria += matriz_dist[punto][contador];

        //Aumentamos el contador de las iteraciones
        contador++;

        //Si vamos por la posicion de punto, salto la iteracion (la distancia de un punto a el mismo es 0) y paso a leer la fila (a partir de aqui punto < contador)
        if(contador==punto){
            contador++;
            columna = false;
        }
    }

    return sumatoria;

}


//ALGORITMO DE GREEDY
//m -> Cantidad de elementos a escoger para la solucion
//matriz_dist -> Matriz que contiene las distancias entre todos los puntos
//n -> numero de elementos totales del problema (la matriz sera nxn)
//Solucion -> vector por referencia en el que se guardaran los puntos seleccionados como solucion del problema
//S -> Vector que contiene los puntos no seleccionados y del que se iran borrando conforme estos se cojan (tambien por referencia para los borrados)
//distancia_total_solucion -> variable por referencia que contiene la distancia total entre los puntos seleccionados una vez termina la funcion
void solucion_greedy(int m, double **matriz_dist, int n, vector<int> &Solucion, vector<int> &S, double &distancia_total_solucion){ //luego cambiar el tipo de lo que devuelve y que el vector sea por referencia

    //Variable que contendra la sumatoria de la distancia de un punto al resto del conjunto original de puntos para escoger de primeras el mas lejano a todos
    double sumatoria_distancias;
    //Variable que contendra la distancia mas alta obtenida en cada momento para la comparacion y busqueda del punto mas lejano
    double dist_mas_larga;
    //Originalmente la distancia mas larga es 0 y la ofrece el punto 0.
    sumatoria_distancias =  dist_mas_larga = 0;
    //Variable que guarda el elemento que esta mas lejos de todos en cada momento en el que se encuentre uno mas lejano al anterior mas lejano
    int punto_mas_distanciado = 0;

    //Si es la primera vez que se llama a la funcion y aun no tenemos ningun elemento seleccionado
    if(Solucion.size() == 0){

        //Bucle que recorre todos los puntos
        for(int i  = 0 ; i < n ; i++){

            //Llamamos a la funcion que calcula la distancia de un punto al resto y guardamos su valor
            sumatoria_distancias = total_por_punto(matriz_dist, n, i);

            //Si este valor es mayor que el que teniamos, este punto es el mas lejano hasta ahora y nos interesa guardarlo como posible primer punto
            if(sumatoria_distancias>dist_mas_larga){
                //guardamos el indice del elemento para introducirlo posteriormente en el vector de soluciones
                punto_mas_distanciado = i;
                //Actualizamos cual es la distancia mas larga para seguir buscando si hay un posible mejor primer punto
                dist_mas_larga = sumatoria_distancias;
            }

        }

        //Cuando termine el bucle y hayamos obtenido el punto mas lejano a todos, lo introducimos como el primer punto de la solucion.
        Solucion.push_back(S[punto_mas_distanciado]);
        //Una vez guardado, lo eliminamos de los puntos no seleccionados
        S.erase(S.begin()+punto_mas_distanciado);

    }else{  //Si no es la primera iteracion (si ya tenemos al menos un punto seleccionado en el vector de solucion)

        //Variable que contendra la menor distancia de cada momento. Se inicializa a un valor alto para que el primer elemento ya sea escogido y se utilice de referencia
        double menor_distancia = 1000000;
        //Se reutiliza la variable anterior para buscar la distancia mas larga de entre las mas cortas. Como buscamos un maximo, lo inicializamos al minimo
        dist_mas_larga = 0;
        //Se reutiliza la variable anterior para guardar el indice del elemento que se encuentre mas lejos de entre los mas cercanos.
        punto_mas_distanciado = 0;

        //Bucle que recorre todos los elementos que aun no tenemos en la solucion
        for(int i = 0 ; i < S.size() ; i++){
            //Para cada elemento ponemos a 0 la distancia acumulada de este
            sumatoria_distancias = 0;
            //Y reestablecemos la menor distancia como un numero elevado para que cada elemento opte a suplirlo
            menor_distancia = 1000000;

            //Bucle que recorre cada elemento de los ya seleccionados en la solucion
            for(int j = 0; j < Solucion.size(); j++){

                //Si la distancia entre el punto no seleccionado a uno que si es menor que la distancia del mismo no seleccionado a otro del conjunto de soluciones, esta es la nueva distancia minima
                if(S[i]<Solucion[j] && matriz_dist[S[i]][Solucion[j]]<menor_distancia) menor_distancia = matriz_dist[S[i]][Solucion[j]];
                else if(S[i]>Solucion[j] && matriz_dist[Solucion[j]][S[i]]<menor_distancia) menor_distancia = matriz_dist[Solucion[j]][S[i]];

            }

            //Al obtener la distancia mas corta desde un punto no seleccionado a alguno de los elementos seleccionados vemos si esta es mayor que la que ofrece otro punto no seleccionado. Si lo es, actualizamos el valor
            if(menor_distancia>dist_mas_larga){
                //Guardamos la nueva meyor distancia de entre las menores de cada punto no seleccionado con alguno seleccionado
                dist_mas_larga = menor_distancia;
                //Guardamos el indice del punto que por ahora es solucion
                punto_mas_distanciado = i;
            }
        }

        //Al comprobar todos los no seleccionados, escogemos aquel que es el mejor candidato y lo introducimos en la solucion
        Solucion.push_back(S[punto_mas_distanciado]);
        //Eliminamos el punto seleccionado de los candidatos
        S.erase(S.begin()+punto_mas_distanciado);

    }

    //Calculamos la distancia del punto recien añadido al resto y se suma al coste total. Este ira incrementando en cada llamada recursiva y finalmente devolvera el total tras la ultima llamada
    for(int i = 0; i < Solucion.size()-1; i++){
        if(Solucion[Solucion.size()-1] > Solucion[i]) distancia_total_solucion += matriz_dist[Solucion[i]][Solucion[Solucion.size()-1]];
        else distancia_total_solucion += matriz_dist[Solucion[Solucion.size()-1]][Solucion[i]];
    }

    //Si aun no hemos seleccionado todos los elementos que queremos, volvemos a llamar a la funcion
    if(Solucion.size()<m) solucion_greedy(m, matriz_dist, n, Solucion, S, distancia_total_solucion);


}

//ALGORITMO DE BUSQUEDA LOCAL
//m -> Cantidad de elementos a escoger para la solucion
//matriz_dist -> Matriz que contiene las distancias entre todos los puntos
//n -> numero de elementos totales del problema (la matriz sera nxn)
//Solucion -> vector por referencia en el que se guardaran los puntos seleccionados como solucion del problema
//S -> Vector que contiene los puntos no seleccionados y del que se iran borrando conforme estos se cojan (tambien por referencia para los borrados)
//contador -> lleva el numero de iteraciones que hace la funcion para condicion de parada. Como maximo puede ser recursiva 100000 veces
//distancia_total_solucion -> variable por referencia que contiene la distancia total entre los puntos seleccionados una vez termina la funcion
void solucion_BL(int m, double **matriz_dist, int n, vector<int> &Solucion, vector<int> &S, int contador, double &distancia_total_solucion){ //luego cambiar el tipo de lo que devuelve y que el vector sea por referencia

    //Variable que contiene el aporte de cada punto del conjunto de soluciones
    double total_distancia_punto_actual;
    //Variable que contendra el aporte mas bajo en cada momento para buscar el elemento que menos aporte
    double aporte_mas_bajo = 1000000;
    //Indice del elemento que menos aporta al conjunto de soluciones
    int indice_aporte_mas_bajo = 0;

    //Variables para las iteraciones del while
    int simula_i, simula_j;
    simula_i = simula_j = 0;

    //Bucle que calcula la distancia de cada punto de la solucion con el resto de estos. Esto es obtener el aporte de cada elemento
    while(simula_i<(Solucion.size())){

        //La distancia entre el punto i y el j, si i = j , es cero (distancia con uno mismo, nos saltamos la iteracion)
        if(simula_i == simula_j ){
          simula_j++;
        }
        else{ //Si no es el mismo elemento, calculamos la distancia entre estos

            if(simula_j == Solucion.size()){ //Si el elemento que estamos comprobando es el ultimo de los que hay en solucion

                //Reiniciamos el valor para que el siguiente punto empiece a comparar con todos desde el principio tambien
                simula_j = 0;

                //Si el aporte del punto i actual es menor que el menor aporte hasta ahora, este sera el nuevo menos influyente
                if(total_distancia_punto_actual<aporte_mas_bajo){
                    //Actualizamos el menor aporte hasta ahora
                    aporte_mas_bajo=total_distancia_punto_actual;
                    //Guardamos el indice del elemento que aporta menos por ahora
                    indice_aporte_mas_bajo=simula_i;

                }
                //Avanzamos al siguiente elemento de la solucion al que calcular el aporte
                simula_i++;
                //Reestablecemos la variable que contiene la sumatoria de las distancias de este punto con todos los demas
                total_distancia_punto_actual = 0;

            //Si estamos aun comparando el elemento i con el resto de elementos de la solucion
            }else{

                //Vamos sumando su distancia con la de todos en la sumatoria para luego comparar su aporte con el menor de todos
                if(Solucion[simula_i]<Solucion[simula_j]) total_distancia_punto_actual+=matriz_dist[Solucion[simula_i]][Solucion[simula_j]];
                else total_distancia_punto_actual+=matriz_dist[Solucion[simula_j]][Solucion[simula_i]];
                simula_j++;

            }
        }

    }

    //Variable que nos indica si seguir buscando elementos para añadir por otros en el conjunto de soluciones. Cuando encontramos una mejor opcion la metemos sin saber si es la mejor y dejamos de buscar (se pone a false)
    bool seguir = true;

    //Reutilizamos las variables anteriores para el nuevo bucle. Se reestablecen
    simula_i = simula_j = 0;
    //Se reestablece tambien la variable de los aportes para buscar cual de los no seleccionados tendria mejor aporte e introducirlo por el obtenido como el que menos aporta
    total_distancia_punto_actual=0;

    //Bucle que comprueba todos los elementos no seleccionados mientras no se haya encontrado uno que mejore la solucion actual
    while(simula_i<(S.size()) && seguir){

        //Si estamos cogiendo la distancia del punto no seleccionado, no lo comparamos con el del menor aporte porque ese punto no estara dentro si este entrase
        if(indice_aporte_mas_bajo == simula_j ){
          simula_j++;
        }
        else{ //Si estamos comparando el punto no seleccionado con el resto de seleccionados, calculamos la distancia

            if(simula_j == Solucion.size()){ //Si el elemento que estamos comprobando es el ultimo de los que hay en solucion

                //Reiniciamos el valor para que el siguiente punto empiece a comparar con todos desde el principio tambien
                simula_j = 0;

                //Si el aporte del punto no seleccionado actual seria mejor que el elemento de la solucion que queremos sustituir
                if(total_distancia_punto_actual>aporte_mas_bajo){

                    //Indicamos que no queremos seguir buscando puntos porque este nos vale
                    seguir=false;
                    //Borramos de la solucion el elemento que menos aportaba
                    Solucion.erase(Solucion.begin()+indice_aporte_mas_bajo);
                    //Añadimos a la solucion el nuevo elemento que aporta mas que el anterior
                    Solucion.push_back(S[simula_i]);
                    //Borramos el elemento de los posibles candidatos, pues ya esta seleccionado
                    S.erase(S.begin()+simula_i);

                }
                //Avanzamos al siguiente elemento de la solucion al que calcular el aporte
                simula_i++;
                //Reestablecemos la variable que contiene la sumatoria de las distancias de este punto con todos los demas
                total_distancia_punto_actual = 0;
             //Si estamos aun comparando el elemento i con el resto de elementos de la solucion
             }else{

                //Vamos sumando su distancia con la de todos en la sumatoria para luego comparar su aporte con el menor de todos
                if(S[simula_i]<Solucion[simula_j]) total_distancia_punto_actual+=matriz_dist[S[simula_i]][Solucion[simula_j]];
                else total_distancia_punto_actual+=matriz_dist[Solucion[simula_j]][S[simula_i]];
                simula_j++;

            }

        }

    }

    //Si despues de realizar todas las comprobaciones seguir esta en falso, es que no se ha encontrado ningun elemento que mejore a la solucion y no renta seguir buscando mas puntos
    //Si el numero de iteraciones es menor que el maximo y estamos encontrando soluciones que mejoren la actual, llamamos recursivamente a la funcion
    if(!seguir && contador <99999)    solucion_BL(m, matriz_dist, n, Solucion, S, contador+1, distancia_total_solucion);
    else{ //Si es la ultima iteracion que vamos a hacer, calculamos ya la distancia total con todas las sustituiciones posibles hechas. Esto evita calcularlo todo de nuevo cada vez que haya algun cambio en la solucion

        //Reestablecemos las variables para el nuevo bucle
        distancia_total_solucion =  simula_i = simula_j = 0;
        //Recorremos todos los elementos de la solucion (menos el ultimo porque siempre se compara con el siguiente y el ultimo no lo tiene)
        while(simula_i<(Solucion.size()-1)){

            //Si estamos mirando la distancia de un elemento consigo mismo pasamos la iteracion (es 0)
            if(simula_i == simula_j ){
              simula_j++;
            }
            else{

                //Si estamos obteniendo la distancia de un punto con el ultimo del conjunto de soluciones
                if(simula_j == Solucion.size()){

                    //Avanzamos al siguiente elemento de la solucion
                    simula_i++;
                    //Empezamos a comparar por el siguiente
                    simula_j = simula_i+1;

                }else{

                    //Añadimos la distancia a la sumatoria
                    if(Solucion[simula_i]<Solucion[simula_j]) distancia_total_solucion+=matriz_dist[Solucion[simula_i]][Solucion[simula_j]];
                    else distancia_total_solucion+=matriz_dist[Solucion[simula_j]][Solucion[simula_i]];
                    simula_j++;

                }

            }

        }

    }

}


int main(int argc, char *argv[]){

    int semilla =5;
    if(argc > 3){
        cout << "NUMERO DE ARGUMENTOS ERRONEO: EJECUTABLE NOMBRE_FICHERO SEMILLA" << endl;
        exit(1);
    }
    else  if(argc == 3){
        semilla = atoi(argv[2]);

    }
    else{
        cout << "NUMERO DE ARGUMENTOS ERRONEO: EJECUTABLE NOMBRE_FICHERO SEMILLA" << endl;
        cout << "SE HA INICIALIZADO LA SEMILLA POR DEFECTO A 5" << endl;
    }

    //Variable en la que se almacenara por referencia la distancia total entre todos los puntos de la solucion obtenida
    double distancia_total_solucion =0;
    //Declaracion del vector de soluciones candidatas (todas las n existentes) y del vector de soluciones (las m seleccionadas)
    vector<int> S, Solucion;
    //Variable en la que se almacenara la cantidad de elementos a seleccionar del conjunto. Este dato se obtiene de los ficheros de datos
    int m = 0 ;
    //Variable que contiene la cantidad total de elementos que existe en el fichero de datos pasado como argumento al programa
    int n = obtiene_n_m(m, argv[1]);

    //Declaracion e inicializacion de la matriz que contiene las distancias entre todos los puntos
    double **matriz_dist = new double*[n];
    for(int i = 0 ; i < n; i++)
        matriz_dist[i] =  new double[n];

    //Funcion que rellena de 0 la matriz de matriz_dist
    rellena_matriz_ceros(matriz_dist, n);
    //Sobreescribe la diagonal superior de la matriz de matriz_dist por las matriz_dist entre ij
    inicializa_matriz_dist(matriz_dist, argv[1]);
    //Introduce cada punto de 0 a n en el vector S (todos los puntos no Solucioneccionados)
    inicializa_candidatas(S, n);

    //Rellena Solucion con m numeros aleatorios entre 0 y n sin repetir para la BL (conjunto de soluciones aleatorias)
    genera_solucion_aleatoria(semilla, m, n , Solucion, S);

    //Medir el tiempo antes de ejecutar la funcion
    auto start_bl = high_resolution_clock::now();
    //Algoritmo de busqueda local
    solucion_BL(m, matriz_dist, n, Solucion, S, 0, distancia_total_solucion);
    //Medir el tiempo despues de ejecutar la funcion
    auto stop_bl = high_resolution_clock::now();

    //Funcion para mostrar el resultado por pantalla
    cout<<"----------SOLUCION DE BL----------"<<endl;
    imprimir_solucion(n, m, Solucion, distancia_total_solucion);
    //Calcular la diferencia de tiempo (tiempo de ejecucion)
    auto duration_bl = duration_cast<microseconds>(stop_bl - start_bl);
    cout << "\tDURACION BL: "<< duration_bl.count() << endl;

    distancia_total_solucion = 0;

    //Funcion para limpiar los vectores de soluciones y de no Solucioneccionados para tenerlos vacios en Greedy
    limpia_vectores(Solucion,S);
    //Introduce cada punto de 0 a n en el vector S (todos los puntos no Solucioneccionados)
    inicializa_candidatas(S, n);

    //Medir el tiempo antes de ejecutar la funcion
    auto start_greedy = high_resolution_clock::now();
    //Algoritmo greedy
    solucion_greedy(m, matriz_dist, n, Solucion, S, distancia_total_solucion);
    //Medir el tiempo despues de ejecutar la funcion
    auto stop_greedy = high_resolution_clock::now();
    //Calcular la diferencia de tiempo (tiempo de ejecucion)
    auto duration_greedy = duration_cast<microseconds>(stop_greedy - start_greedy);
    //cout << distancia_total_solucion<<" "<< duration_greedy.count() << endl;

    cout<<"\n\n----------SOLUCION DE GREEDY----------"<<endl;
    imprimir_solucion(n, m, Solucion, distancia_total_solucion);
    cout << "\tDURACION GREEDY: "<< duration_greedy.count() << endl;

    vaciar_matriz(matriz_dist, n);


    return 0;
}

