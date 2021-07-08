//MDG-a_1_n500_m50
//MDG-b_21_n2000_m200
//MDG-b_30_n2000_m200
//MDG-c_1_n3000_m300
//MDG-c_2_n3000_m300
//MDG-c_8_n3000_m400
//MDG-c_9_n3000_m400
//MDG-c_10_n3000_m400
//MDG-c_13_n3000_m500
//MDG-c_14_n3000_m500
//MDG-c_15_n3000_m500
//MDG-c_19_n3000_m600
//MDG-c_20_n3000_m600
#include <iostream>
#include "random.h"
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <math.h>

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
void genera_solucion_aleatoria(int m, int n, vector<int> &Solucion, vector<int> &S){

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
        int x = Randint (0,n-1);
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




//ALGORITMO DE BUSQUEDA LOCAL
//m -> Cantidad de elementos a escoger para la solucion
//matriz_dist -> Matriz que contiene las distancias entre todos los puntos
//n -> numero de elementos totales del problema (la matriz sera nxn)
//Solucion -> vector por referencia en el que se guardaran los puntos seleccionados como solucion del problema
//S -> Vector que contiene los puntos no seleccionados y del que se iran borrando conforme estos se cojan (tambien por referencia para los borrados)
//contador -> lleva el numero de iteraciones que hace la funcion para condicion de parada. Como maximo puede ser recursiva 100000 veces
//distancia_total_solucion -> variable por referencia que contiene la distancia total entre los puntos seleccionados una vez termina la funcion
//auxiliar_rellenar_vecinos -> vector que tendra aquellos elementos que se eliminen de la solucion para luego añadirlos al vecindario
void solucion_BL(int m, double **matriz_dist, int n, vector<int> &Solucion, vector<int> &S, int contador, double &distancia_total_solucion, vector<int> &auxiliar_rellenar_vecinos){


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

        //Cuenta como evaluacion cada vecino que miramos si entraria o no
        contador++;

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
                    //Los elementos extraidos de solucion pasan a ser del vecindario pero al final de la funcion para no meter elementos que acabamos de sacar
                    auxiliar_rellenar_vecinos.push_back(Solucion[indice_aporte_mas_bajo]);
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
    if(!seguir && contador <9999)    solucion_BL(m, matriz_dist, n, Solucion, S, contador+1, distancia_total_solucion, auxiliar_rellenar_vecinos);
    else{ //Si es la ultima iteracion que vamos a hacer, calculamos ya la distancia total con todas las sustituiciones posibles hechas. Esto evita calcularlo todo de nuevo cada vez que haya algun cambio en la solucion


        S.insert( S.end(), auxiliar_rellenar_vecinos.begin(), auxiliar_rellenar_vecinos.end() );
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

//FUNCION PARA CALCULAR EL APORTE DE UN PUNTO DENTRO DE LA SOLUCION, YA SEA PARTE DE ESA O DEL VECINDARIO EN SUSTITUCION DE ALGUN ELEMENTO SELECCIONADO
//punto -> es el punto en [0,n-1] del que queremos calcular el aporte dentro de Solucion
//Solucion -> vector que contiene todos los m puntos seleccionados como solucion
//punto_ignorar -> punto hacia el que no queremos saber la distancia. Si calculo el aporte de un punto de dentro de la
    //solucion, este punto sera el mismo que 'punto' para ignorar la distancia de un punto a el mismo (porque es 0, iteracion vacia)
    //pero en el caso de calcular el aporte de un vecino, este punto es aquel al que sustituiria, por lo que no deberiamos
    //contar con la distancia a este punto de la solucion (dejara de ser un seleccionado si el vecino es mejor)
//matriz_dist -> matriz con las distancias entre todos los puntos
/////return -> aporte de 'punto' dentro del conjunto 'Solucion'
double calcula_aporte(int punto , vector<int> Solucion, int punto_ignorar , double ** matriz_dist){

    //Variable que contendra el aporte del punto 'punto' dentro de 'Solucion'
    double aporte = 0;
    //Contador para recorrer todos los puntos de 'Solucion' y calcular la distancia a todos los puntos
    int contador = 0;

    //Bucle para recorrer todos los puntos seleccionados que pertenecen a Solucion
    while(contador < Solucion.size()){

        //Si no hay que ignorar ese punto, sumamos la distancia de 'punto' a este elemento
        if(Solucion[contador] != punto_ignorar && punto<Solucion[contador])    aporte+=matriz_dist[punto][Solucion[contador]];
        else if(Solucion[contador] != punto_ignorar)          aporte+=matriz_dist[Solucion[contador]][punto];

        //Avanzamos las iteraciones
        contador++;

    }

    //Devolvemos el aporte de este punto
    return aporte;

}

//FUNCION QUE CALCULA EL COSTE DE UNA SOLUCION (DISTANCIA DE TODOS LOS PUNTOS CON TODOS LOS PUNTOS SIN REPETIR)
//Solucion -> Vector que contiene los m elementos seleccionados como solucion
//matriz_dist -> matriz con la distancia entre todos los puntos
///// return -> devuelve el coste de 'Solucion'
double calcula_coste(vector<int> Solucion, double **matriz_dist){

    //Variable que contendra el coste total de 'Solucion'
    double distancia = 0;
    //Contador para recorrer todos los puntos del conjunto (menos el ultimo)
    int contador1 = 0;
    //Contador para recorrer todos los puntos del conjunto (menos el primero)
    int contador2;

    //Bucle que recorre todos los puntos de la solucion para calcular sus distancias a los puntos siguientes (por eso no se usa el ultimo)
    while(contador1 < Solucion.size()-1){

        //En cada iteracion del bucle grande , empezamos el interno por el siguiente punto porque no nos interesa calcular
        //la distancia con puntos anteriores, nos salen distancias repetidas
        contador2 = contador1+1;

        while(contador2 < Solucion.size()){

            //Sumamos la distancia desde dos puntos
            if(Solucion[contador1]<Solucion[contador2])     distancia+=matriz_dist[Solucion[contador1]][Solucion[contador2]];
            else      distancia+=matriz_dist[Solucion[contador2]][Solucion[contador1]];

            //Avanzamos las iteraciones del bucle interno (puntos por delante del actual hasta el final)
            contador2++;

        }

        //Avanzamos las iteraciones del bucle externo (puntos de los que coger la distancia hasta los siguiente)
        contador1++;

    }

    //Devolvemos el coste de 'Solucion'
    return distancia;

}

//FUNCION QUE REALIZA EL INTERCAMBIO DE UN ELEMENTO ENTRE EL VECINDARIO Y EL VECTOR SOLUCION
//Solucion -> vector del que se van a sacar elementos seleccionados y en el que se va a introducir algun vecino
//no_seleccionados -> vector de elementos no seleccionados que representa el vecinadrio
//escogido -> indice del elemento que vamos a sacar de seleccionados 'Solucion'
//no_escogido -> indice del elemento que vamos a sacar del vecindario y vamos a meter en 'Solucion'
void intercambio_vecino(vector<int> &Solucion, vector<int> &no_seleccionados, int escogido, int no_escogido ){

    //Metemos en el vecindario el elemento sacado de 'Solucion'
    no_seleccionados.push_back(Solucion[escogido]);
    //Borramos el elemento de la solucion
    Solucion.erase(Solucion.begin()+escogido);
    //Metemos en la solucion el elemento del vecinadrio
    Solucion.push_back(no_seleccionados[no_escogido]);
    //Eliminamos ese vecino pues ya esta dentro de la solucion
    no_seleccionados.erase(no_seleccionados.begin()+ no_escogido);

}


//FUNCION QUE REALIZA LA MUTACION DE t ELEMENTOS EN BUSQUEDA LOCAL REITERADA (ILS)
//solucion -> vector de seleccionados con m elementos de los que se van a intercambiar t con el vecindario
//t -> cantidad de elementos aleatorios que se van a extraer de Solucion y se van a meter en el vecindario
//no_seleccionados -> vector del que se van a sacar los t vecinos aleatorios y se van a meter los t seleccionados sacados de 'Solucion'
void mutacion(vector<int> &solucion, double t, vector<int> &no_seleccionados){

    //Variables que contendran los numeros aleatorios que se correspondan con los indices de los numeros que sacaremos de cada conjunto
    int x, y;
    //Contador para hacer t intercambios aleatorios
    int contador = 0;

    //Guardamos el numero de vecinos original para que los numeros se generen entre 0 y este y asi, cuando añadamos
    //al vecindario los que sacamos de 'Solucion', estos queden en posiciones que no vamos a generar. Con esto evitamos
    //meter elementos que acabamos de sacar
    int tam_vecindario = no_seleccionados.size();

    //Bucle para hacer los t intercambios
    while(contador<t){

        //Genero un aleatorio que se corresponda con la posicion de Solucion que voy a vaciar
        x =  Randint(0, solucion.size()-1);
        //Genero un aleatorio que se corresponda con la posicion del vecindario de la que sacare el elemento
        //(este no puede ser mayor que el tamaño original para no acceder a los que voy añadiendo conforme los saco de Solucion)
        y =  Randint(0, tam_vecindario-contador-1);
        //Realizamos el intercambio entre Solucion y vecindario
        intercambio_vecino(solucion, no_seleccionados, x, y );
        //Avanzamos el contador para hacer los t intercambios
        contador++;

    }

}

//ALGORITMO DEL ENFRIAMIENTO SIMULADO
//Solucion -> vector con m elementos seleccionados en el que se devolvera la mejor solucion obtenida con este algoritmo
//matriz_dist -> matriz con la distancia entre todos los puntos
//m -> cantidad de elementos que se tienen que seleccionar de entre los n que tiene cada conjunto de datos
//n -> elementos total del fichero de datos que usemos necesario para la generacion de soluciones aleatorias
//no_seleccionados -> vector que contendra el vecindario de la mejor solucion
//evaluaciones -> entero que indica el numero de evaluaciones a realizar. El algoritmo realiza 100000 pero cuando se usa en ILS se reduce a 10000
/////return -> devuelve el coste de la mejor solucion obtenida con el algoritmo
double Enfriamiento_Simulado(vector<int> &Solucion, double **matriz_dist, int m, int n, vector<int> &no_seleccionados, int evaluaciones){

    //Numero maximo de vecinos que vamos a probar en nuestro conjunto por cada temperatura del algoritmo
    int max_no_seleccionados = 10 * m;
    //Para sesgar el numero de conjuntos que aceptamos cuando la temperatura aun es alta. Las soluciones peores pueden
    //ser seleccionadas probabilisticamente en funcion de la temperatura en ese momento, por eso al principio puede aceptar
    //muchas soluciones peores, lo cual se limita para que si acepta muchas baje la temperatura
    double max_exitos = 0.1 * max_no_seleccionados;
    //veces que se reducira la temperatura, son iteraciones del bucle externo
    double M = evaluaciones/max_no_seleccionados;

    //Generamos una solucion aleatoria original
    if(evaluaciones == 100000) genera_solucion_aleatoria( m, n , Solucion, no_seleccionados);

    //Hasta ahora el mejor coste es la de la unica solucion generada
    double mejor_coste = calcula_coste(Solucion, matriz_dist);
    //Hasta ahora la mejor solucion generada es la unica que se ha generado
    vector<int> mejor_solucion = Solucion;
    //El vecindario de la mejor solucion tiene que ser guardado para cuando se hagan las evaluaciones y este se manipule,
    //poder reestablecerlo en caso de quedarnos con la mejor solucion guardada sin actualizarla
    vector<int> no_seleccionados_mejor_solucion = no_seleccionados;

    //Cantidad de soluciones que se aceptan en cada reduccion de la temperatura. Se cuentan sean la mejor solucion hasta
    //el momento y las que se acepten por pura aleatoriedad
    int aceptados = 1;
    //Temperatura inicial calculada a raiz del coste
    double temperatura_inicial =  0.3 * mejor_coste / -log(0.3);
    //Temperatura final a priori. El algoritmo para cuando la temperatura inicial sea menor que esta
    double temperatura_final = 10e-3;

    //Si de entrada la temperatura inicial da demasiado pequeña y ya es menor que la final, la final se reduce
    temperatura_final = (temperatura_final >= temperatura_inicial) ? (10e-5) : (10e-3);

    //Beta es una constante que se utiliza en la formula de enfriamiento en cada iteracion
    double const beta = (temperatura_inicial - temperatura_final) / (M * temperatura_inicial * temperatura_final);

    //Coste auxiliar para cada solucion que se genere, para comprobar si es mejor que la mejor hasta el momento
    double coste = mejor_coste;

    //Las condiciones de parada son enfriar lo suficiente o no haber aceptado ni una solucion vecina aleatoria, por lo que
    //no renta seguir haciendo iteraciones, dificilmente mejoremos la solucion que tenemos y la temperatura es lo
    //suficientemente baja como para no aceptar apenas soluciones peores
    while(temperatura_inicial>temperatura_final && aceptados!=0){

        //De entrada, por cada temperatura, se reestablece no haber aceptado aun ninguna solucion vecina
        aceptados = 0;
        //Contador para saber cuantas soluciones vecinas hemos probado
        int contador = 0;

        //Probamos soluciones vecinas hasta haber probado todas las permitidas o haber aceptado demasiadas
        while(contador < max_no_seleccionados && aceptados < max_exitos){

            //Variables que contendran los numeros aleatorios que se correspondan con los elementos que intercambiamos entre Solucion y vecindario
            int escogido = Randint(0, Solucion.size() - 1);
            int no_escogido = Randint(0, no_seleccionados.size());

            //Calculamos el aporte actual del punto que esta dentro de Solucion
            double aporte_seleccionado = calcula_aporte(Solucion[escogido], Solucion, Solucion[escogido], matriz_dist);
            //Calculamos el aporte que tendria el vecino generado dentro de la Solucion si lo cambiasemos por el 'escogido'
            double aporte_no_seleccionado = calcula_aporte(no_seleccionados[no_escogido], Solucion, Solucion[escogido], matriz_dist);

            //Si el vecino aportaria mas estando dentro ó
            //Si el vecino empeora la solucion pero aleatoriamente se acepta como nueva solucion (depende de T)
            if(aporte_no_seleccionado > aporte_seleccionado || (aporte_no_seleccionado <= aporte_seleccionado && (Randfloat(0, 1) <= exp((aporte_seleccionado -  aporte_no_seleccionado) / temperatura_inicial)))){

                //Aceptamos la solucion como nueva solucion y lo tenemos en cuenta en el contador de los aceptados
                aceptados++;

                //Realizamos el intercambio entre 'Solucion' y vecindario
                intercambio_vecino(Solucion, no_seleccionados, escogido, no_escogido );

                //Actualizamos el coste de la solucion actual, que es el de antes menos lo que aportaba el punto extraido
                //mas lo que aporte el nuevo introducido (FACTORIZACION, NO SE CALCULA DE NUEVO)
                coste = coste - aporte_seleccionado + aporte_no_seleccionado;

                //Si el coste de la solucion actual es mejor que el de la mejor encontrada
                if(coste > mejor_coste){

                    //guardo la solucion como la mejor hasta el momento
                    mejor_solucion = Solucion;
                    //actualizo cual es el mejor coste que hemos obtenido
                    mejor_coste = coste;
                    //Actualizo el vecindario de la mejor solucion
                    no_seleccionados_mejor_solucion = no_seleccionados;

                }

            }

            //Avanzamos el contador hasta generar todas las soluciones vecinas que podamos
            contador++;

        }

        //Reducimos la temperatura
        temperatura_inicial = temperatura_inicial / (1 + beta*temperatura_inicial);

    }

    //Al terminar devuelvo por referencia la mejor solucion obtenida y su vecindario correspondiente
    Solucion = mejor_solucion;
    no_seleccionados = no_seleccionados_mejor_solucion;

    //Devolvemos el coste de la mejor solucion obtenida
    return mejor_coste;

}

//ALGORITMO DE BUSQUEDA MULTIARRANQUE BASICA (BMB)
//Solucion -> vector con m elementos seleccionados en el que se devolvera la mejor solucion obtenida con este algoritmo
//matriz_dist -> matriz con la distancia entre todos los puntos
//m -> cantidad de elementos que se tienen que seleccionar de entre los n que tiene cada conjunto de datos
//n -> elementos total del fichero de datos que usemos necesario para la generacion de soluciones aleatorias
//no_seleccionados -> vector que contendra el vecindario de la mejor solucion
/////return -> devuelve el coste de la mejor solucion obtenida con el algoritmo
double Busqueda_Multiarranque_Basica(vector<int> &Solucion, double **matriz_dist, int m, int n, vector<int> no_seleccionados){

    //Vector que contendra la mejor solucion encontrada en cada momento
    vector<int> mejor_solucion;
    //Coste de la mejor solucion encontrada. De entrada un numero negativo para que desde la primera iteracion se actualice
    double mejor_coste = -1;
    //Coste de la solucion que evaluemos en cada momento para saber si es mejor que la mejor encontrada
    double coste;
    //Contador para realizar las 10 iteraciones que se nos piden
    int contador = 0;
    //Inicializamos todas las candidatas de una vez aunque las reestablezcamos en cada iteracion
    vector<int> vecindario_original;

    inicializa_candidatas(vecindario_original, n);
    no_seleccionados = vecindario_original;

    //Para 10 iteraciones realizamos el experimento
    while(contador < 10){

        //Vaciamos la solucion y el vecindario por si guarda valores basura de iteraciones antiguas
        limpia_vectores(Solucion, no_seleccionados);

        //Reestablecemos el vecindario al original
        no_seleccionados = vecindario_original;

        //Generamos una solucion aleatoria
        genera_solucion_aleatoria( m, n , Solucion, no_seleccionados);

        //Vector auxiliar vacio en el que se meteran los vecinos que se saquen de 'Solucion' para añadirlos al vecindario
        vector<int> auxiliar_rellenar_vecinos;

        //Se restaura el coste ya que este se obtiene al aplicar BL (calcularlo antes es perder tiempo)
        coste = 0;

        //Aplicamos Busqueda Local a la solucion aleatoria
        solucion_BL(m,matriz_dist,n,Solucion, no_seleccionados, 0, coste, auxiliar_rellenar_vecinos);

        //Si ahora la solucion es mejor que la mejor que teniamos
        if(coste>mejor_coste){

            //Guardamos la mejor solucion
            mejor_solucion = Solucion;
            //Actualizamos el coste mas alto encontrado hasta ahora
            mejor_coste = coste;

        }

        //Avanzamos las iteraciones hasta llegar 10
        contador++;

    }

    //Devolvemos por referencia dentro de 'Solucion' la mejor solucion obtenida
    Solucion = mejor_solucion;

    //La funcion devuelve el coste de la mejor solucion obtenida
    return mejor_coste;

}


//ALGORITMO DE BUSQUEDA LOCAL REITERADA (ILS)
//Solucion -> vector con m elementos seleccionados en el que se devolvera la mejor solucion obtenida con este algoritmo
//matriz_dist -> matriz con la distancia entre todos los puntos
//m -> cantidad de elementos que se tienen que seleccionar de entre los n que tiene cada conjunto de datos
//n -> elementos total del fichero de datos que usemos necesario para la generacion de soluciones aleatorias
//no_seleccionados -> vector que contendra el vecindario de la mejor solucion
/////return -> devuelve el coste de la mejor solucion obtenida con el algoritmo
double Busqueda_Local_Reiterada(vector<int> &Solucion, double **matriz_dist, int m, int n, vector<int> no_seleccionados){

    //Numero de elementos que vamos a intercambiar entre 'Solucion' y el vecindario en la mutacion
    int t = 0.1*m;

    //Vaciamos la solucion y el vecindario
    limpia_vectores(Solucion, no_seleccionados);
    //Inicializamos las candidatas
    inicializa_candidatas(no_seleccionados, n);
    //Generamos una solucion aleatoria inicial
    genera_solucion_aleatoria( m, n , Solucion, no_seleccionados);

    //La mejor solucion obtenida hasta ahora es la unica generada
    vector<int> mejor_solucion = Solucion;
    //El mejor coste es el de la mejor solucion (al principio el unico coste que tenemos)
    double mejor_coste = calcula_coste(Solucion, matriz_dist);
    //Vecindario de la mejor solucion hasta ahora para cuando lo modifiquemos probando nuevas soluciones,
    //poder restablecerlo si no conseguimos mejorar la solucion
    vector<int> no_seleccionados_mejor_solucion = no_seleccionados;

    //Coste de cada solucion para compararla con la mejor y ver si la sustituimos
    double coste = 0;
    //Vector auxiliar en el que meter los elementos que saquemos de Solucion para añadirlos al vecindario al final
    //se añaden despues de los intercambios para no meter elementos que acabamos de sacar
    vector<int> auxiliar_rellenar_vecinos;

    //Se realiza Busqueda Local al vector de solucion generado aleatoriamente al principio
    solucion_BL(m,matriz_dist,n,Solucion, no_seleccionados, 0, coste, auxiliar_rellenar_vecinos);

    //Si BL mejora la solucion actualizamos cual es la mejor
    if(coste>mejor_coste){

        //Guardamos el mejor coste hasta ahora
        mejor_coste = coste;
        //Guardamos la mejor solucion
        mejor_solucion = Solucion;
        //Guardamos tambien el vecindario de la mejor solucion
        no_seleccionados_mejor_solucion = no_seleccionados;

    }

    //Contador para el resto de iteraciones
    int contador = 0;

    //Se realizan las 9 comprobaciones restantes
    while(contador < 9){

        //Partimos de la mejor solucion
        Solucion = mejor_solucion;
        //Y del vecindario de la mejor solucion
        no_seleccionados = no_seleccionados_mejor_solucion;

        //Se muta la mejor solucion hasta ahora
        mutacion(Solucion, t, no_seleccionados);

        //Vaciamos el vector auxiliar que vamos a usar para BL
        auxiliar_rellenar_vecinos.clear();

        //Reestablecemos a 0 el coste ya que este lo sabremos despues de BL
        coste = 0;

        //Aplicamos BL a la mutacion de la mejor solucion hasta ahora
        solucion_BL(m,matriz_dist,n,Solucion, no_seleccionados, 0, coste,auxiliar_rellenar_vecinos);

        //Si despues de mutar y hacer BL hemos mejorado la mejor solucion, se actualiza
        if(coste>mejor_coste){

            //Guardamos el mejor coste hasta ahora
            mejor_coste = coste;
            //La mejor solucion se actualiza con la actual
            mejor_solucion = Solucion;
            //El vecindario de la mejor tiene que ser actualizado porque en cada iteracion
            //este se altera y si no mejoramos la solucion debemos volver a este vecindario para las futuras comprobaciones
            no_seleccionados_mejor_solucion = no_seleccionados;

        }

        //Para las 9 comprobaciones totales avanzamos el contador
        contador++;

    }

    //Devolvemos por referencia dentro de 'Solucion' la mejor solucion obtenida
    Solucion = mejor_solucion;

    //Devolvemos el coste de la mejor solucion
    return mejor_coste;

}

//ALGORITMO DE BUSQUEDA LOCAL REITERADA APLICANDO ENFRIAMIENTO SIMULADO(ILS_ES)
//Solucion -> vector con m elementos seleccionados en el que se devolvera la mejor solucion obtenida con este algoritmo
//matriz_dist -> matriz con la distancia entre todos los puntos
//m -> cantidad de elementos que se tienen que seleccionar de entre los n que tiene cada conjunto de datos
//n -> elementos total del fichero de datos que usemos necesario para la generacion de soluciones aleatorias
//no_seleccionados -> vector que contendra el vecindario de la mejor solucion
/////return -> devuelve el coste de la mejor solucion obtenida con el algoritmo
double Busqueda_Local_Reiterada_Enfriamiento_Simulado(vector<int> &Solucion, double **matriz_dist, int m, int n, vector<int> no_seleccionados){

    //Numero de elementos que vamos a intercambiar entre 'Solucion' y el vecindario en la mutacion
    int t = 0.1*m;

    //Vaciamos la solucion y el vecindario
    limpia_vectores(Solucion, no_seleccionados);
    //Inicializamos las candidatas
    inicializa_candidatas(no_seleccionados, n);
    //Generamos una solucion aleatoria inicial
    genera_solucion_aleatoria( m, n , Solucion, no_seleccionados);

    //La mejor solucion obtenida hasta ahora es la unica generada
    vector<int> mejor_solucion = Solucion;
    //El mejor coste es el de la mejor solucion (al principio el unico coste que tenemos)
    double mejor_coste = calcula_coste(Solucion, matriz_dist);
    //Vecindario de la mejor solucion hasta ahora para cuando lo modifiquemos probando nuevas soluciones,
    //poder restablecerlo si no conseguimos mejorar la solucion
    vector<int> no_seleccionados_mejor_solucion = no_seleccionados;

    //Coste de cada solucion para compararla con la mejor y ver si la sustituimos
    double coste = Enfriamiento_Simulado(Solucion, matriz_dist, m, n, no_seleccionados, 10000);

    //Si BL mejora la solucion actualizamos cual es la mejor
    if(coste>mejor_coste){

        //Guardamos el mejor coste hasta ahora
        mejor_coste = coste;
        //Guardamos la mejor solucion
        mejor_solucion = Solucion;
        //Guardamos tambien el vecindario de la mejor solucion
        no_seleccionados_mejor_solucion = no_seleccionados;

    }

    //Contador para el resto de iteraciones
    int contador = 0;

    //Se realizan las 9 comprobaciones restantes
    while(contador < 9){

        //Partimos de la mejor solucion
        Solucion = mejor_solucion;
        //Y del vecindario de la mejor solucion
        no_seleccionados = no_seleccionados_mejor_solucion;

        //Se muta la mejor solucion hasta ahora
        mutacion(Solucion, t, no_seleccionados);

        //Reestablecemos a 0 el coste ya que este lo sabremos despues de BL
        coste = 0;

        //Aplicamos Enfriamiento Simulado a la mutacion de la mejor solucion hasta ahora
        coste = Enfriamiento_Simulado(Solucion, matriz_dist, m, n, no_seleccionados, 10000);


        //Si despues de mutar y hacer BL hemos mejorado la mejor solucion, se actualiza
        if(coste>mejor_coste){

            //Guardamos el mejor coste hasta ahora
            mejor_coste = coste;
            //La mejor solucion se actualiza con la actual
            mejor_solucion = Solucion;
            //El vecindario de la mejor tiene que ser actualizado porque en cada iteracion
            //este se altera y si no mejoramos la solucion debemos volver a este vecindario para las futuras comprobaciones
            no_seleccionados_mejor_solucion = no_seleccionados;

        }

        //Para las 9 comprobaciones totales avanzamos el contador
        contador++;

    }

    //Devolvemos por referencia dentro de 'Solucion' la mejor solucion obtenida
    Solucion = mejor_solucion;

    //Devolvemos el coste de la mejor solucion
    return mejor_coste;

}


int main(int argc, char *argv[]){

    //Inicializamos la semilla por si no la pasa como argumento
    int semilla =5;

    //Si mete mas argumentos de los que debe
    if(argc > 3){
        cout << "NUMERO DE ARGUMENTOS ERRONEO: EJECUTABLE NOMBRE_FICHERO SEMILLA" << endl;
        exit(1);
    }
    else  if(argc == 3){
        semilla = atoi(argv[2]);

    }
    else{
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

    //Inicializamos la semilla para numeros aleatorios
    Set_random(semilla);

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


    double mejor_coste = Enfriamiento_Simulado(Solucion, matriz_dist, m, n,S, 100000);
    cout<<"EL MEJOR COSTE OBTENIDO CON ENFRIAMIENTO SIMULADO ES: "<<mejor_coste<<endl;

    mejor_coste = Busqueda_Multiarranque_Basica(Solucion, matriz_dist, m, n, S);
    cout<<"EL MEJOR COSTE OBTENIDO CON BUSQUEDA MULTIARRANQUE BASICA ES: "<<mejor_coste<<endl;

    mejor_coste = Busqueda_Local_Reiterada(Solucion, matriz_dist,  m, n, S);
    cout<<"EL MEJOR COSTE OBTENIDO CON BUSQUEDA LOCAL REITERADA ES: "<<mejor_coste<<endl;

    mejor_coste = Busqueda_Local_Reiterada_Enfriamiento_Simulado(Solucion, matriz_dist,  m, n, S);
    cout<<"EL MEJOR COSTE OBTENIDO CON BUSQUEDA LOCAL REITERADA ENFRIAMIENTO SIMULADO ES: "<<mejor_coste<<endl;

    vaciar_matriz(matriz_dist, n);


    return 0;
}


