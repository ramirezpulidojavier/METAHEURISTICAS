//ARGUMENTOS:
//"C:/Users/xaviv/Desktop/JAVIER/UNIVERSIDAD/CURSO 20.21/CURSO/2CUATRI/METAHEURISTICAS/PRACTICA2/datos/MDG-a_1_n500_m50.txt" 5
#include "random.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace std::chrono;
using namespace std;

//Funcion que recibe un vector en binario y devuelve uno en decimal equivalente que contiene los mismos elementos que el pasado
/////Decimal -> vector del que obtener los seleccionados en binario que se pasaran a decimal
/////////return -> vector decimal que contiene los numeros correspondientes a las posiciones de Decimal en las que habia un 1
vector<int> pasar_binario_a_decimal(vector<int> Decimal){

	//Vector que devolveremos con los elementos en decimal
    vector<int> binario;
    //Variable con las iteraciones a traves de 'Decimal'
    int j = 0;

    //Recorremos en busca de 1's que indiquen que el punto con indice equivalente a esa posicion esta cogido
    while(j<Decimal.size()){

    	//Si esta posicion es un 1, es porque ese numero es seleccionado y lo metemos en decimal en el vector que devolveremos
        if(Decimal[j]==1)  binario.push_back(j);

        //Avanzamos las iteraciones
        j++;

    }

    //Devolvemos el vector en decimal que contiene los mismos seleccionados que el introducido en binario
    return binario;
}


//CLASE CROMOSOMA: Representa a cada elemento de la poblacion
class Cromosoma{

private:
	//Distancia total obtenida con los puntos seleccionados en esta solucion (distancia total entre los '1' de 'genes')
    double coste_acumulado;
    //Vector de n elementos y m 1's que representa la solucion ofrecida por este cromosoma
    vector<int> genes;
    //Flag que indica si un cromosoma tiene que recalcular su coste o no
    bool reevaluar;

public:

	//Constructor desde 0
	/////m -> Cantidad de elementos a seleccionar por cada solucion (cantidad de 1's en el vector de genes de cada cromosoma)
	/////n -> Cantidad de elementos totales por cada solucion (cantidad de elementos total en el vecto de genes de cada cromosoma)
	/////matriz_dist -> matriz que contiene la distancia entre todos los puntos
    Cromosoma(int m, int n, double **matriz_dist){

    	//Esto introduce en el vector 'genes' del cromosoma una solucion aleatoria con n elementos y m 1's
        genera_solucion_aleatoria(m, n, genes);
        //Calculamos el coste de esa solucion aleatoria y la almacenamos en el atributo de la clase
        coste_acumulado = calcula_coste(pasar_binario_a_decimal(genes), matriz_dist);
        //Inicialmente no hace falta reevaluar un objeto porque de entrada no sufre modificaciones
        reevaluar = false;

    }

	//Constructor con un vector de genes como parametro
	/////genes_calculados -> vector de genes ya creado que se introduce directamente en el atributo de la clase sin generarlo aleatoriamente
	/////matriz_dist -> matriz que contiene la distancia entre todos los puntos
    Cromosoma(vector<int> genes_calculados, double **matriz_dist){

    	//Calculamos el coste que tiene el vecto de genes que asignamos al objeto
        coste_acumulado = calcula_coste(pasar_binario_a_decimal(genes_calculados), matriz_dist);
        //Guardamos el vector en el atributo de la clase
        genes = genes_calculados;
        //Igualmente reevaluar esta a false porque se acaba de calcular el coste y no hay que rehacerlo hasta que se modifique
        reevaluar = false;

    }

    //Funcion que devuelve el coste del vector de genes de un cromosoma
    //////////return ->	el coste_acumulado del objeto
    double get_coste(){       		return coste_acumulado;}

    //Funcion para modificar el coste de un cromosoma
    /////coste -> Nuevo coste con el que sobreescribir el anterior
    void set_coste(double coste){   coste_acumulado = coste;}

    //Funcion para obtener el gen colocado en una posicion concreta del vector 'genes'
    /////indice -> posicion del vector del que se quiere obtener el gen
    //////////return ->	el 0 o 1 que se encuentre en la posicion 'indice'
    int get_gen_concreto(int indice){

    	//Es el valor que devolvera si el indice introducido no es correcto
    	int error = -1;

    	//Si la posicion introducida es correcta, devuelve el valor que se encuentre en ella
    	if(indice >= 0 && indice < genes.size())	 return genes[indice];

    	//Si llega hasta aqui es porque el indice no cumplia con lo deseado y el valor que se obtiene es incoherente
    	return error;

    }

    //Funcion para modificar un gen concreto del vector 'genes' de un cromosoma sin tener que cambiar el vector completo
    /////indice -> posicion del vector 'genes' al que cambiar el valor
    /////valor -> nuevo valor que introducir en la posicion indicada con el otro parametro
    void set_gen_concreto(int indice, int valor){

    	//Si la posicion indicada es correcta y el valor que se intenta meter es diferente al que ya habia
        if(indice >= 0 && indice < genes.size() && genes[indice] != valor){

        	//Actualizamos el valor de esa posicion
        	genes[indice] = valor;
        	//Al sufrir un cambio, indicamos que este cromosoma necesita calcular de nuevo su coste
        	reevaluar = true;

        }


    }

    //Funcion que devuelve el vecto de genes de un cromosoma
    //////////return ->	vector de genes del objeto
    vector<int> get_genes(){		return genes;}

    //Funcion para modificar el valor del flag reevaluar desde fuera de clase
    /////flag -> valor del booleano que sobreescribira el que tenia 'reevaluar' previamente
    void set_reevaluar(bool flag){  reevaluar = flag;}

    //Funcion para introducir un nuevo vector de genes al objeto
    ///// genes_nuevos -> nuevo vector binario que sobreescribira el vector 'genes' del objeto
    /////matriz_dist -> matriz con la distancia entre todos los puntos
    void set_genes(vector<int> genes_nuevos, double **matriz_dist){

    	//Vaciamos el vector que tenia previamente el objeto
        genes.clear();
        //Introducimos el nuevo vecto como 'genes'
        genes = genes_nuevos;
        //Actualizamos el coste de esta solucion
        coste_acumulado = calcula_coste(pasar_binario_a_decimal(genes), matriz_dist);

    }

    //Funcion que devuelve el booleano 'reevaluar' del objeto
    //////////return ->	booleano 'reevaluar' del objeto
    bool get_reevaluar(){	return reevaluar;}


    //FUNCION QUE GENERA UN CONJUNTO DE M ELEMENTOS ALEATORIOS
    //m -> Variable que contiene el numero de elementos a seleccionar (cantidad de 1's en el vector de genes)
    //n -> Variable que contiene el numero de elementos total que necesitamos (cantidad de elementos en el vector de genes)
    //Solucion -> Vector en el que introduciremos los m 1's en posiciones aleatorias(sin repetir)
    void genera_solucion_aleatoria(int m, int n, vector<int> &Solucion){

        //Contador de las iteraciones del bucle que rellena Soluciones
        int contador= 0 ;

        //Introduzco en la solucion n 0's de partida para luego añadir los 1's en las posiciones aleatorias
        vector<int> Pseudosolucion(n, 0);
        Solucion = Pseudosolucion;

        //Bucle que genera los m elementos necesarios. Se hacen 5 iteraciones mas porque inentendiblemente genera 5 numeros similares a la semilla consecutivos
        while(contador < m+5){
            //Se genera un numero aleatorio entre 0 y el numero de elementos total que indicara la posision donde ira un 1
            int x = Randint (0,n-1);
            //Si ya han pasado los 5 primeros numeros inutiles que sabemos que son iguales
            if(contador>4){

            	//Comprobamos que ese elemento no este cogido aun
                if(Solucion[x] == 0){

                	//Lo introducimos a la solucion
                    Solucion[x]=1;
                    //Avanzamos el contador
                    contador++;

                }

            //Si aun estamos en las 5 primeras iteraciones dejamos correr la iteracion y generamos otro
            }else contador++;

        }

    }

    //Funcion que calcular el coste que tiene un vector binario (el coste de 'genes')
    /////Solucion -> vector binario del que sacar los puntos seleccionados para calcular la distancia total
    /////matriz_dist -> matriz que contiene la distancia entre todos los puntos
    //////////return ->	coste total que ofrece el vector 'Solucion'
    double calcula_coste(vector<int> Solucion, double **matriz_dist){

    	//Sera la variable que se devuelva con la suma total, inicialmente a 0
        double distancia_total_solucion = 0;
        //Iteraciones que realizaremos por cada punto seleccionado
        int punto_seleccionado = 0;
        //Iteraciones que realizaremos por cada punto restante del vector para calcular la distancia entre punto_seleccionado y los demas
        int punto_restante = 0;

        //Bucle que recorre todos los puntos del vector (menos el ultimo porque ya se habran calculado todas las distancias hasta este)
        while(punto_seleccionado < Solucion.size()-1){

        	//En cada punto seleccionado, empezamos recorriendo a partir del siguiente porque las distancias anteriores estaran sumadas
        	punto_restante = punto_seleccionado+1;
			//Bucle que recorre desde el punto seleccionado en adelante para calcular las distancias no calculadas de este
			while(punto_restante < Solucion.size()){

                if(Solucion[punto_restante] < Solucion[punto_seleccionado])
                    distancia_total_solucion += matriz_dist[Solucion[punto_restante]][Solucion[punto_seleccionado]];

                else
                	distancia_total_solucion += matriz_dist[Solucion[punto_seleccionado]][Solucion[punto_restante]];

                //Avanzamos las iteraciones entre el resto de puntos con los que calcular la distancia desde punto_seleccionado
                punto_restante++;

            }
            //Tras calcular la distancia de un punto con todos los siguiente pasamos al punto que continua y volvemos a calcular
            punto_seleccionado++;

        }

        //Devuelve el total calculado de esta solucion
        return distancia_total_solucion;

    }


};

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
        //casting del punto i leido (primer punto)
        primer_punto = stoi(i);
        //casting del punto j leido (segundo punto)
        segundo_punto = stoi(j);
        //casting de la distancia leida entre un punto y el otro
        distancia_double = strtod(distancia.c_str(),NULL);

        //Introducimos en la posicion correcta la distancia correspondiente a esos puntos
        matriz_dist[primer_punto][segundo_punto] = distancia_double;

    }//Si no se abre correctamente el fichero
    else{
        //Mensaje de error
        cout << "Error leyendo el archivo de entrada con los datos"<< endl ;
        exit(-1);
    }

    //Antes de terminar la funcion cerramos el archivo del que sacamos los datos
    f.close();

}


//ALGORITMO DE BUSQUEDA LOCAL
//matriz_dist -> Matriz que contiene las distancias entre todos los puntos
//Solucion -> vector por referencia en el que se guardaran los puntos seleccionados como solucion del problema
//S -> Vector que contiene los puntos no seleccionados y del que se iran borrando conforme estos se cojan (tambien por referencia para los borrados)
//contador -> lleva el numero de iteraciones que hace la funcion para condicion de parada. Como maximo puede ser recursiva 100000 veces
void solucion_BL( double **matriz_dist, vector<int> &Solucion, vector<int> &S, int contador){ //luego cambiar el tipo de lo que devuelve y que el vector sea por referencia

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
    //ACTUALIZACION: AHORA LA CANTIDAD MAXIMA DE VECINOS QUE EVALUAMOS ES 400
    while(simula_i< S.size() && simula_i<400 && seguir){

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
    if(!seguir && contador <99999)    solucion_BL(matriz_dist, Solucion, S, contador+1);

}


//Funcion que a partir de un vector binario de seleccionados devuelve el vector de no seleccionados (necesario para BL)
/////vecinos -> Vector en el que se almacena el vector complementario al de seleccionados (conjunto de vecinos no seleccionados)
/////Solucion -> Vector del que obtendremos los seleccionados para saber cuales no estan en el conjunto
void rellena_vecindario_binario( vector<int> &vecinos, vector<int> Solucion){

    //Creo un vector auxiliar que contendra el contrario de Solucion
    vector<int> vecindario_temporal;
    //Variable con las iteraciones para recorrer el vector de seleccionados 'Solucion'
    int i = 0;

    //Recorremos todos los seleccionados
    while(i < Solucion.size()){

    	//Si en la posicion i de los seleccionados hay un 0 es porque este elemento no fue escogido y deberia estar en el vector de vecinos
        if(Solucion[i] == 0) vecindario_temporal.push_back(1);
		//Si el elemento es un 1 es porque esta seleccionado y no estara en el vector de vecinos
		else vecindario_temporal.push_back(0);

		//Avanzamos las iteraciones
		i++;

    }

    //Metemos en el vector por referencia el vector armado con el contrario de Solucion
    vecinos = vecindario_temporal;

}




//Funcion que recibe un vector en decimal y devuelve uno en binario equivalente que contiene los mismos elementos que el pasado
/////Binario -> vector del que obtener los seleccionados en decimal que se pasaran a binario
/////n -> indica el numero total de elementos que debera tener el vector en binario, independientemente de los seleccionados que saque de 'Binario'
/////////return -> vector binario que contiene un 1 en la posicion equivalente a cada numero contenido en 'Binario'
vector<int> pasar_decimal_a_binario(vector<int> Binario, int n){

	//Vector en binario que sera devuelto. Incialmente tiene n 0's y se le introduciran los 1 en las posiciones necesarias
    vector<int> decimal(n, 0);
    //Variable con las iteraciones a traves de 'Sel'
    int j = 0;

    //Recorremos todos los numeros para saber las posiciones en las que poner los 1's en el que vector binario que se devuelve
    while(j < Binario.size()){

    	//Metemos en la posicion Sel[j] del vector binario un 1
        decimal[Binario[j]] = 1;

        //Avanzamos las iteraciones
        j++;

    }

    //Devolvemos el vector en binario que contiene un 1 en las posiciones equivalentes a cada elemento del vector decimal como parametro
    return decimal;

}

//Funcion que devuelve un conjunto de padres seleccionados de una poblacion para realizar el cruce
/////Poblacion -> vector de cromosomas que representa a la poblacion total de la que seleccionamos estos padres
/////cantidad_padres -> numero de padres que queremos seleccionar de la poblacion 'Poblacion'
//////////return -> vector de cromosomas con aquellos de la poblacion seleccionados por torneo para ser padres
vector<Cromosoma> seleccionar_padres(vector<Cromosoma> Poblacion, int cantidad_padres){

	//Vector de padres que sera devuelto
    vector<Cromosoma> padres;
    //segundo_padre -> indice del segundo padre //// primer_padre -> indice del primer padre //// contador -> cantidad de padres generada
    int segundo_padre, primer_padre, contador;
    //Inicialmente hemos generado 0 padres
    contador = 0;

    //Generamos tantos padres como se indique en los parametros
    while(contador < cantidad_padres){

		//Genero un numero aleatorio correspondiente al indice del primer padre selccionado
		primer_padre = Randint (0,Poblacion.size()-1);

		//Genero numeros aleatorios para el indice del segundo padre hasta que este sea diferente (y no hacer el torneo con el mismo elemento dos veces)
        do{
            segundo_padre = Randint (0,Poblacion.size()-1);
        }while(segundo_padre==primer_padre);

        //Si el segundo padre es mejor que el primero, el primero pasa a ser el segundo tambien porque es el que añadimos como padre seleccionado
        if(Poblacion[segundo_padre].get_coste() > Poblacion[primer_padre].get_coste())  primer_padre = segundo_padre;

        //Avanzamos el contador para seguir generando padres
        contador++;
        //Introducimos el cromosoma seleccionado en el vector de padres que devolveremos
        padres.push_back(Poblacion[primer_padre]);

    }

    //Devuelvo el vector con los 'cantidad_padres' padres seleccionados
    return padres;

}

//Funcion que calcula la cantidad de 1's que tiene el vector 'genes' de un cromosoma
/////cromosoma_comprobar -> cromosoma del que indicar el numero de 1's contenido en 'genes'
//////////return -> devuelve un entero con la cantidad de 1's que contiene 'cromosoma_comprobar' en sus genes
int cantidad_unos_en_cromosoma(Cromosoma cromosoma_comprobar){

	//Variable con la cantidad de 1's
    int contador = 0;

    //Recorremos el vector de genes de este cromosoma
    for(int i = 0 ; i < cromosoma_comprobar.get_genes().size(); i++)
    	//Si encontramos un 1, aumentamos el contador
        if(cromosoma_comprobar.get_gen_concreto(i) == 1) contador++;

    //Devolvemos la cantidad de 1's encontrada
    return contador;

}

//Funcion que calcula el aporte de un vecino si este formase parte del conjunto de los seleccionados
/////seleccionados_recorridos -> punto del vecindario cuyo aporte evaluamos
///// seleccionados_decimal -> vector con los puntos seleccionados en formato digital
/////aporte_actual -> valor por referencia que contendra el aporte de este punto con los seleccionados
/////distancias -> matriz que contiene la distancia entre todos los puntos
void calcula_aporte_vecino(int seleccionados_recorridos, vector<int> seleccionados_decimal, double &aporte_actual,double **distancias ){

	//Iteraciones que recorren los puntos seleccionados del vector 'seleccionados_decimal'
	int contador = 0;
	//Ponemos a 0 el aporte por si traia algun valor erroneo previo
	aporte_actual = 0;

	//Recorremos todos los puntos seleccionados
    while(contador < seleccionados_decimal.size()){
    	//Sumamos la distancia de este vecino a todos los puntos seleccionados para ver su aporte
        if(seleccionados_recorridos<seleccionados_decimal[contador])
        	aporte_actual+=distancias[seleccionados_recorridos][seleccionados_decimal[contador]];
        else
        	aporte_actual+=distancias[seleccionados_decimal[contador]][seleccionados_recorridos];

        //Avanzamos las iteraciones
        contador++;
    }

}

//Funcion que calcula el aporte de un elemento seleccionado dentro del conjunto
/////seleccionados_recorridos -> punto de la solucion al que calculamos el aporte
///// seleccionados_decimal -> vector con los puntos seleccionados en formato digital
/////aporte_actual -> valor por referencia que contendra el aporte de este punto con los seleccionados
/////distancias -> matriz que contiene la distancia entre todos los puntos
void calcula_aporte_seleccionado(int seleccionados_recorridos, vector<int> seleccionados_decimal, double &aporte_actual,double **distancias ){

	//Iteraciones que recorren los puntos seleccionados del vector 'seleccionados_decimal'
    int seleccionados_para_distancia = 0;
    //Ponemos a 0 el aporte por si traia algun valor erroneo previo
    aporte_actual = 0;

    //Recorremos todos los puntos seleccionados
	while(seleccionados_para_distancia < seleccionados_decimal.size()){

		//Evaluamos solo la distancia con los puntos que no son el seleccionado, pues esta es 0
        if(seleccionados_recorridos != seleccionados_para_distancia){

        	//Sumamos la distancia de este vecino a todos los puntos seleccionados para ver su aporte
            if(seleccionados_decimal[seleccionados_para_distancia]<seleccionados_decimal[seleccionados_recorridos])
            	aporte_actual+=distancias[seleccionados_decimal[seleccionados_para_distancia]][seleccionados_decimal[seleccionados_recorridos]];
            else
            	aporte_actual+=distancias[seleccionados_decimal[seleccionados_recorridos]][seleccionados_decimal[seleccionados_para_distancia]];

        }

        //Avanzamos las iteraciones
        seleccionados_para_distancia++;
    }


}

//Funcion que devuelve el mejor elemento de un conjunto de puntos para ser eliminado (para cuando sobran 1's en 'genes')
/////aportes-> vector de pair que contiene parejas de indices-aportes que actualizaremos en cada momento para saber cual es el de mayor aporte
/////ultimo_valor_introducido -> ultimo punto que se elimino de la solucion para actualizar los aportes restando la distancia de todos los puntos a este
/////distancias -> matriz con la distancia entre todos los puntos
/////posicion -> posicion del vector de aportes en el que se encuentra el mejor elemento (la posicion no tiene que equivaler al indice del punto) para elimiar el pair de esta posicion de 'aportes'
//////////return -> devolvemos el punto con mayor aporte de los que se encuentra dentro de la solucion
int obtener_nuevo_mejor_elemento_eliminar(vector<pair<int, int>> &aportes, int ultimo_valor_introducido, double **distancias, int &posicion){

	//El mayor aporte inicial es un numero negativo para que el primero, sea cual sea, ya sea mayor (las distancias son como minimo 0)
	int mayor_aporte = -1;
	//Contador para iterar por todos los aportes
	int seleccionados_recorridos = 0;
	//El mejor elemento inicialmente es el primero hasta que alguno de los restantes le supere
	int mejor_elemento = aportes[seleccionados_recorridos].first;

	//Recorremos todos los puntos de la solucion
    while(seleccionados_recorridos < aportes.size()){

    	//Actualizamos el valor de cada pareja indice-valor tras eliminar un elemento de la solucion
        if(ultimo_valor_introducido<aportes[seleccionados_recorridos].first) aportes[seleccionados_recorridos].second -=distancias[ultimo_valor_introducido][aportes[seleccionados_recorridos].first];
        else aportes[seleccionados_recorridos].second -= distancias[aportes[seleccionados_recorridos].first][ultimo_valor_introducido];

        //Si este nuevo aporte actualizado es mejor que el mejor que tenia almacenado
        if(aportes[seleccionados_recorridos].second> mayor_aporte){

        	//actualizamos el mejor aporte
            mayor_aporte = aportes[seleccionados_recorridos].second;
            //actualizamos la posicion de este para poder eliminarlo de entre los aportes
            posicion = seleccionados_recorridos;
            //ademas del indice guardamos el punto que mejor aporte ofrece para saber cual sera el siguiente en ser eliminado
            mejor_elemento = aportes[seleccionados_recorridos].first;

        }

        //Avanzamos las iteraciones
        seleccionados_recorridos++;

    }

    //Devolvemos el elemento a eliminar
    return mejor_elemento;

}

//Funcion que devuelve el mejor elemento del vecindario para ser introducido (para cuando faltan 1's en 'genes')
/////aportes-> vector de pair que contiene parejas de indices-aportes que actualizaremos en cada momento para saber cual es el de mayor aporte
/////ultimo_valor_introducido -> ultimo punto que se introdujo en la solucion para actualizar los aportes sumando la distancia de todos los puntos a este
/////distancias -> matriz con la distancia entre todos los puntos
/////posicion -> posicion del vector de aportes en el que se encuentra el mejor elemento (la posicion no tiene que equivaler al indice del punto) para elimiar el pair de esta posicion de 'aportes'
//////////return -> devolvemos el punto con mayor aporte de los que se encuentra en el vecindario
int obtener_nuevo_mejor_elemento_introducir(vector<pair<int, int>> aportes, int ultimo_valor_introducido, double **distancias, int &posicion){

	//El mayor aporte inicial es un numero negativo para que el primero, sea cual sea, ya sea mayor (las distancias son como minimo 0)
	int mayor_aporte = -1;
	//Contador para iterar por todos los aportes
	int seleccionados_recorridos = 0;
	//El mejor elemento inicialmente es el primero hasta que alguno de los restantes le supere
	int mejor_elemento = aportes[seleccionados_recorridos].first;

	//Recorremos todos los puntos de la solucion
    while(seleccionados_recorridos < aportes.size()){

        //Actualizamos el valor de cada pareja indice-valor tras añadir un vecino en la solucion
        if(ultimo_valor_introducido<aportes[seleccionados_recorridos].first) aportes[seleccionados_recorridos].second +=distancias[ultimo_valor_introducido][aportes[seleccionados_recorridos].first];
        else aportes[seleccionados_recorridos].second += distancias[aportes[seleccionados_recorridos].first][ultimo_valor_introducido];

        //Si este nuevo aporte actualizado es mejor que el mejor que tenia almacenado
        if(aportes[seleccionados_recorridos].second> mayor_aporte){

        	//actualizamos el mejor aporte
            mayor_aporte = aportes[seleccionados_recorridos].second;
            //actualizamos la posicion de este para poder eliminarlo de entre los aportes
            posicion = seleccionados_recorridos;
            //ademas del indice guardamos el punto que mejor aporte ofrece para saber cual sera el siguiente en ser eliminado
            mejor_elemento = aportes[seleccionados_recorridos].first;

        }

        //Avanzamos las iteraciones
        seleccionados_recorridos++;

    }

    //Devolvemos el elemento a eliminar
    return mejor_elemento;

}


//Funcion de reparacion para el cruce uniforme
/////cromosoma_reparar -> cromosoma que modificaremos si tiene una cantidad erronea de 1's
/////distancias -> matriz que contiene la distancias entre todos los puntos
void funcion_reparacion_cuniforme(Cromosoma &cromosoma_reparar, int m, double **distancias){

	//vector que contendra parejas de indice-aporte para saber en cada caso cual es el mejor a añadir o eliminar
    vector<pair<int, int>> aportes;
    //vector que contendra los puntos en decimal seleccionados (coincide con las posiciones de 'genes' en los que hay un 1)
    vector<int> seleccionados_decimal;
    //aporte de un punto seleccionado con el resto o de un vecino con todos los de la solucion
    double aporte_actual = 0;
    //el mayor aporte encontrado hasta ahora (inicialmente en negativo para escoger el primer punto sea el aporte que sea)
    double mayor_aporte = -1;
    //mejor_elemento -> punto que mas aporte tenga // posicion -> la posicion de este dentro del vector al que pertenezca para eliminarlo
    //contador -> contara la de 1's que se han arreglado hasta el momento
    // ultimo_valor_introducido -> ultimo punto que se introduzca o elimine de la solucion para actualizar los aportes cada vez
    int mejor_elemento, posicion, contador, ultimo_valor_introducido;
    mejor_elemento = posicion = contador = ultimo_valor_introducido = 0;
    //Calculamos la cantidad de 1's que tiene este cromosoma en 'genes' para ver si realmente hay algo que reparar
    int cantidad_unos = cantidad_unos_en_cromosoma(cromosoma_reparar);
    //Pair auxiliar que para ir añadiendo cada indice con su coste al vector de aportes
    pair<int, int> auxiliar;
    //Variable para recorrer cada punto de la solucion o del vecindario, dependiendo del caso que queramos reparar
    int seleccionados_recorridos = 0;

    //Pasamos el vector 'genes' a decimal para hacer m iteraciones y no n
    seleccionados_decimal = pasar_binario_a_decimal(cromosoma_reparar.get_genes());

    //Si estamos en el caso de que nuestro cromosoma tiene mas 1's de los que deberia
    if( cantidad_unos> m){

    	//Vamos a recorrer los elementos que son 1 en 'genes' para ver cual aporta mas y eliminarlo
    	while(seleccionados_recorridos < seleccionados_decimal.size()){

    		//Calculamos el aporte que proporciona este gen
            calcula_aporte_seleccionado(seleccionados_recorridos, seleccionados_decimal, aporte_actual, distancias);

            //lo almaceno en el vector de aportes junto a su indice
            auxiliar.first = seleccionados_decimal[seleccionados_recorridos];
            auxiliar.second = aporte_actual;
            aportes.push_back(auxiliar);

            //Si es el mayor aporte hasta ahora
            if(aporte_actual> mayor_aporte){
            	//actualizo el mayor aporte hasta el momento
                mayor_aporte = aporte_actual;
                //guardo la posicion en la que se encuentra para eliminarlo de 'aportes' y 'seleccionados_decimal'
                posicion = seleccionados_recorridos;
                //ademas guardo el punto que genera este aporte
                mejor_elemento = seleccionados_decimal[seleccionados_recorridos];
            }

            //Avanzamos las iteraciones
            seleccionados_recorridos++;

        }

        //Una vez tenemos el mejor elemento de la solucion, lo ponemos a 0 indicando que ya no pertenece a esta
        cromosoma_reparar.set_gen_concreto(mejor_elemento, 0);
        //Guardo cual fue el ultimo elemento para actualizar los aportes que tenia almacenados
        ultimo_valor_introducido = mejor_elemento;

        //Ahora quitamos tantos unos como sobre aun (la diferencia entre los que tiene, los que deberia y uno menos por el que acabamos de corregir arriba)
        while(contador < cantidad_unos-m-1){

        	//Avanzamos el contador de los 1's corregidos
           	contador++;

           	//Eliminamos del vector en decima aquel que hemos puesto a 0 en 'genes'
            seleccionados_decimal.erase(seleccionados_decimal.begin()+posicion);
            //Eliminamos el par de 'aportes' que contenia el indice del elemento que ya hemos borrado, pues su aporte ya no nos interesa
    		aportes.erase(aportes.begin()+posicion);

    		//calculamos el mejor elemento de los que quedan en 'genes' para ser eliminado
            mejor_elemento = obtener_nuevo_mejor_elemento_eliminar(aportes, ultimo_valor_introducido, distancias, posicion);

            //El obtenido como mejor elemento se pone a 0
            cromosoma_reparar.set_gen_concreto(mejor_elemento, 0);
            //Guardo cual fue el ultimo elemento para actualizar los aportes que tenia almacenados
            ultimo_valor_introducido = mejor_elemento;


        }

    //Si estamos en el caso de que nuestro cromosoma tiene menos 1's de los que deberia
    }else if(cantidad_unos < m){

    	//Obtenemos el vecindario de este cromosoma concreto porque ahora necesitamos meter el mejor vecino de todos
    	vector<int> no_seleccionados_binario;
        rellena_vecindario_binario(no_seleccionados_binario, cromosoma_reparar.get_genes());

        //Pasamos a decimal los vecino para recorrerlos en m iteraciones y no en n
        vector<int> no_seleccionados_decimal = pasar_binario_a_decimal(no_seleccionados_binario);

        //Vamos a recorrer los vecinos para ver cual aporta mas y añadirlo
        while(seleccionados_recorridos < no_seleccionados_decimal.size()){

        	//Calculamos el aporte que proporcionaria este vecino si entrase al conjunto
			calcula_aporte_vecino(no_seleccionados_decimal[seleccionados_recorridos], seleccionados_decimal, aporte_actual, distancias);

			//lo almaceno en el vector de aportes junto a su indice
            auxiliar.first = no_seleccionados_decimal[seleccionados_recorridos];
            auxiliar.second = aporte_actual;
            aportes.push_back(auxiliar);

			//Si es el mayor aporte hasta ahora
            if(aporte_actual> mayor_aporte){
            	//actualizo el mayor aporte hasta el momento
                mayor_aporte = aporte_actual;
                //guardo la posicion en la que se encuentra para eliminarlo de 'aportes' y 'seleccionados_decimal'
                posicion = seleccionados_recorridos;
                //ademas guardo el punto que genera este aporte
                mejor_elemento = no_seleccionados_decimal[seleccionados_recorridos];
            }

            //Avanzamos las iteraciones
            seleccionados_recorridos++;

        }

        //Una vez tenemos el mejor elemento de la solucion, lo ponemos a 0 indicando que ya no pertenece a esta
        cromosoma_reparar.set_gen_concreto(mejor_elemento, 1);
        //Guardo cual fue el ultimo elemento para actualizar los aportes que tenia almacenados
        ultimo_valor_introducido = mejor_elemento;

        //Ahora añadimos tantos vecinos como falten aun (la diferencia entre los que deberia tener, los que tiene y uno menos por el que acabamos de corregir arriba)
        while(contador < m-cantidad_unos-1){

        	//Avanzamos el contador de los 1's corregidos
            contador++;

            //Añadimos al vector que contiene los elementos equivalentes a las posiciones donde hay un 1 en 'genes' la posicion en la que acabamos de poner un 1 nuevo
            seleccionados_decimal.push_back(ultimo_valor_introducido);
            //Eliminamos del vector en decima aquel que hemos puesto a 1 en 'genes'
            no_seleccionados_decimal.erase(no_seleccionados_decimal.begin()+posicion);
            //Eliminamos el par de 'aportes' que contenia el indice del elemento que ya hemos borrado, pues su aporte ya no nos interesa
    		aportes.erase(aportes.begin()+posicion);


            //calculamos el mejor elemento de los que quedan en el vecindario para ser añadido
            mejor_elemento = obtener_nuevo_mejor_elemento_introducir(aportes, ultimo_valor_introducido, distancias, posicion);

            //El obtenido como mejor elemento se pone a 1
            cromosoma_reparar.set_gen_concreto(mejor_elemento, 1);
            //Guardo cual fue el ultimo elemento para actualizar los aportes que tenia almacenados
            ultimo_valor_introducido = mejor_elemento;

        }

    }

}


//Funcion que introduce en un hijo los elementos no comunes entre los padres en un orden aleatorio
/////padre_auxiliar -> vector que contiene de UNO de los padres, los valores encontrados en las posiciones que no coincidian en ambos padres
/////poblacion_intermedia -> vector que contiene todos los hijos y que sera modificado al alterar los genes concretos
/////contador -> indica el cromosoma concreto del que modificar lo genes con los del padre
void introduce_del_padre(vector<int> &padre_auxiliar, vector<Cromosoma> &poblacion_intermedia, int contador){

	//Mezclamos los elementos del padre que vamos a introducir en el hijo
	random_shuffle( padre_auxiliar.begin(), padre_auxiliar.end() );

	//Contador para avanzar en el vecto de los elementos a añadir
    int contador_padre = 0;
    //Contador para ir gen por gen del cromosoma especificado introduciendo valores
    int iteraciones = 0;

    //Mientras no se haya recorrido el gen completo añadiendo o no se hayan metido todos los valores del padre en el hijo
    while(iteraciones < poblacion_intermedia[contador].get_genes().size() && contador_padre<padre_auxiliar.size()){

    	//Si hay un -1 en la posicion del hijo, significara que en esta posicion hay que meter valor del padre
        if(poblacion_intermedia[contador].get_gen_concreto(iteraciones) == -1){
        	//Añadimos ese gen
            poblacion_intermedia[contador].set_gen_concreto(iteraciones, padre_auxiliar[contador_padre]);
            //Avanzamos el contador del numero de genes del padre añadidos
            contador_padre++;

        }

        //Seguimos avanzando por el gen en busca de mas -1 para introducir valores
        iteraciones++;

    }

}

//Funcion encargada de realizar el cruce uniforme entre un conjunto de padres previamente seleccionado
/////padres -> conjunto de cromosomas elegidos como padres para realizar el cruce
/////distancias -> matriz que contiene la distancia entre todos los puntos
/////n -> cantidad de elementos total que hay en el vector de genes de cada cromosoma
/////m -> cantidad de 1's que tiene que haber en el vector de genes de cada cromosoma
//////////return -> devuelve un vector con los cromosomas correspondientes a los hijos generados tras el cruce
vector<Cromosoma> cruce_uniforme(vector<Cromosoma> padres, double **distancias, int n){

	//contador ->cantidad de hijos generados     ////  num_esperado_cruce -> cantidad de cruces que se llevaran a cabo
    int contador, num_esperado_cruce;
    //Inicialmente hemos generado 0 hijos
    contador = 0;
    //Vector que se devolvera con los hijos generados del cruce de padres. Este en un comienzo es igual al de padres
    vector<Cromosoma> poblacion_intermedia = padres;
    //La probabilidad de cruce proporcionada en PRADO es 0.7
    double prob_cruce=0.7;

    //Si la cantidad de padres que tenemos es 1, nos encontramos en el estacionario y se tienen que cruzar si o si, por lo que la probabilidad pasa a 1
    if(padres.size()==2)  prob_cruce = 1;

    //Una vez sepa que probabilidad es definitivamente, obtenemos la cantidad de cruces que van a llevarse a cabo
    num_esperado_cruce = prob_cruce * padres.size()/2;

    //Mientras no se hayan generado todos los hijos (2 hijos por cruce, por eso num_esperado_cruce * 2)
    while(contador< num_esperado_cruce*2 ){

    	//Recorremos gen a gen el vector de genes del hijo para ver si coincide con el del padre
        for(int i = 0 ; i < n; i++){

        	//Si el primer hijo (que es una copia del primer padre) tiene un elemento diferente al segundo padre, es porque
        	//el valor entre los padres en esta posicion es diferente y si, ademas, se cumple que al generar un numero aleatorio
        	//sale par, le cambiamos el valor del gen en esa posicion (habra salido que elige el gen del segundo padre)
            if(poblacion_intermedia[contador].get_gen_concreto(i) != padres[contador+1].get_gen_concreto(i) && (Randint(0,10) %2 ) == 0)
                poblacion_intermedia[contador].set_gen_concreto(i, padres[contador+1].get_gen_concreto(i));

         	//Si el segundo hijo (que es una copia del segundo padre) tiene un elemento diferente al primer padre, es porque
        	//el valor entre los padres en esta posicion es diferente y si, ademas, se cumple que al generar un numero aleatorio
        	//sale par, le cambiamos el valor del gen en esa posicion (habra salido que elige el gen del primer padre)
            if(poblacion_intermedia[contador+1].get_gen_concreto(i) != padres[contador].get_gen_concreto(i)  && (Randint(0,10) %2 ) == 0)
				poblacion_intermedia[contador+1].set_gen_concreto(i, padres[contador].get_gen_concreto(i));

        }

        //Avanazmos el contador de hijos dos valores debido a que se generan dos hijos por cada cruce
        contador+=2;

    }

    //Cuando hemos hecho las modificaciones necesarias se devuelve el vector que contiene los hijos generados del cruce
    return poblacion_intermedia;

}

//Funcion encargada de realizar el cruce posicional entre un conjunto de padres previamente seleccionado
/////padres -> conjunto de cromosomas elegidos como padres para realizar el cruce
/////distancias -> matriz que contiene la distancia entre todos los puntos
/////n -> cantidad de elementos total que hay en el vector de genes de cada cromosoma
//////////return -> devuelve un vector con los cromosomas correspondientes a los hijos generados tras el cruce
vector<Cromosoma> cruce_posicional(vector<Cromosoma> padres, double **distancias, int n){


	//contador -> cantidad de hijos generados     ////  num_esperado_cruce -> cantidad de cruces que se llevaran a cabo
	int contador, num_esperado_cruce;
    //Inicialmente hemos generado 0 hijos
    contador = 0;
    //Vector que se devolvera con los hijos generados del cruce de padres. Este en un comienzo es igual al de padres
    vector<Cromosoma> poblacion_intermedia = padres;
    //La probabilidad de cruce proporcionada en PRADO es 0.7
    double prob_cruce=0.7;
    //Vector en el que se meteran los elementos de UNO de los padres que esten en las posiciones en las que los valores de ambos padres no coincidan
    vector<int> padre_auxiliar;

    //Si la cantidad de padres que tenemos es 1, nos encontramos en el estacionario y se tienen que cruzar si o si, por lo que la probabilidad pasa a 1
    if(padres.size()==2)  prob_cruce = 1;

    //Una vez sepa que probabilidad es definitivamente, obtenemos la cantidad de cruces que van a llevarse a cabo
    num_esperado_cruce = prob_cruce * padres.size()/2;

    //Mientras no se hayan generado todos los hijos (2 hijos por cruce, por eso num_esperado_cruce * 2)
    while(contador< num_esperado_cruce*2 ){

    	//Limpiamos el vector en cada iteracion porque se rellena con push_backs que añaden al final, no sobreescriben
        padre_auxiliar.clear();

        //Bucle que recorre cada gen de ambos hijos que se estan montando
        for(int i = 0 ; i < n; i++){

        	//Si el primer hijo (que es una copia del primer padre) tiene el gen que se esta evaluando diferente a ese gen del segundo padre (por lo que en esa posicion hay una diferencia entre padres)
            if(poblacion_intermedia[contador].get_gen_concreto(i) != padres[contador+1].get_gen_concreto(i)){

            	//Guardamos ese valor que era diferente para luego ser añadido en orden aleatorio en cada hijo
                padre_auxiliar.push_back(padres[contador].get_gen_concreto(i));
                //Añadimos un valor distintivo en las posiciones de los hijos en los que deberiamos introducir los valores del padre reordenados
                poblacion_intermedia[contador].set_gen_concreto(i, -1);
                poblacion_intermedia[contador+1].set_gen_concreto(i, -1);

            }


        }

        //Metemos los valores del padre en el primer hijo
        introduce_del_padre(padre_auxiliar, poblacion_intermedia, contador);

        //Metemos los valores del padre en el segundo hijo
        introduce_del_padre(padre_auxiliar, poblacion_intermedia, contador+1);

        //Avanzamos el numero de hijos generados
        contador+=2;

    }

    //Devolvemos el conjunto de los hijos
    return poblacion_intermedia;

}

//Funcion que recorre una poblacion completa buscando los cromosomas que tienen el flag de reevaluar a true y corrigiendo sus costes (lo cual cuenta como una evaluacion de las 100000 que tenemos que hacer)
/////poblacion_mutada -> poblacion que recorremos buscando cromosomas que reajustar
/////distancias -> matriz con las distancias entre todos los puntos
/////evaluaciones -> contador de las veces que se calcula el coste de un cromosoma (100000 evaluaciones maximo) que se incrementa cada vez que encontremos un cromosoma que necesita recalcular su coste
void reparar_coste(vector<Cromosoma> &poblacion_mutada, double **distancias, int &evaluaciones){

	//Recorremos la poblacion completa
	for(int i = 0; i < poblacion_mutada.size(); i++){

		//Si ese cromosoma tiene que ser reevaluado
        if(poblacion_mutada[i].get_reevaluar()){

        	//Aumentamos el numero de evaluaciones que se han llevado a cabo
            evaluaciones++;
            //Le introducimos un nuevo coste a ese cromosoma que sera el resultado de calcularle el coste de nuevo
            poblacion_mutada[i].set_coste(poblacion_mutada[i].calcula_coste(pasar_binario_a_decimal(poblacion_mutada[i].get_genes()),distancias));
            //Ponemos su flag a false por haberle recalculado ya el coste correcto
            poblacion_mutada[i].set_reevaluar(false);
        }

    }

}


//Funcion que lleva a cabo la mutacion de una poblacion
/////poblacion_intermedia -> poblacion de la que partir y a la que realizar las mutaciones
/////distancias -> matriz que contiene la distancia entre todos los puntos
/////n -> cantidad de elementos que tiene un vector de genes
/////m -> cantidad de 1's que tiene un vector de genes (elementos seleccionados en una solucion)
/////evaluaciones -> evaluaciones -> contador de las veces que se calcula el coste de un cromosoma (100000 evaluaciones maximo) que se incrementa cada vez que encontremos un cromosoma que necesita recalcular su coste
//////////return -> Devuelve la poblacion de los elementos mutados
vector<Cromosoma> mutacion(vector<Cromosoma> poblacion_intermedia, double **distancias, int n, int m, int &evaluaciones){

	//Contador del numero de mutaciones realizadas
    int contador = 0;
    //Contador de la cantidad de 1's que hemos leido en el vector 'genes' buscando aquel en la posicion generada aleatoriamente
    int unos_pasados = 0;
	//Probabilidad de mutacion proporcionada en PRADO
	const double prob_mutacion = 0.1/n;
	//La cantidad de mutaciones que se van a hacer es la probabilidad de mutar un gen por la cantidad de genes que hay
    int num_esperado_mutacion = prob_mutacion * 50*n;
    //Variables para generar que cromosoma y que gen de este se muta
    int gen_aleatorio, cromosoma_aleatorio;
    //Condicion de parada para cuando ya hayamos cambiado lo que queremos, no recorrer mas genes del mismo cromosoma
    bool continuar;
    //Booleano para saber si hemos cambiado ya algun 0 por un 1 o seguimos buscandolo dentro de 'genes'
    bool busca_0 ;
    //Booleano para saber su hemos cambiado ya el 1 en la posicion generada o seguimos iterando hacia el
    bool uno_cambiado = false;
    //Variable para iterar por cada gen del cromosoma aleatorio
    int i = 0;

    //En un principio la poblacion mutada es identica a la de hijos
    vector<Cromosoma> poblacion_mutada = poblacion_intermedia;

    //Mientras no se hayan generado todas las mutaciones que queremos
    while(contador < num_esperado_mutacion){

    	//Para cada cromosoma reestablecemos el valor inicial de las condiciones de parada
        uno_cambiado = false;
        continuar = true;
        unos_pasados = 0;
        busca_0 = true;
        i = 0;

        //Generamos aleatoriamente el cromosoma que vamos a modificar
        cromosoma_aleatorio = Randint (0,poblacion_mutada.size()-1);
        //Generamos aleatoriamente el gen que vamos a modificar
        gen_aleatorio = Randint (0,m-1);

        //Mientras no hayamos recorrido el cromosoma completo o no hayamos dado con la condicion de parada (haber cambiado ya tanto el 1 como el 0)
        while(i < poblacion_mutada[cromosoma_aleatorio].get_genes().size() && continuar){

        	//Si aun no he cambiado el 0 y encuentro uno de ellos
            if(busca_0 && poblacion_mutada[cromosoma_aleatorio].get_gen_concreto(i) == 0){

            	//Ya no estare buscando mas 0's porque lo he cambiado ya
                busca_0 = false;
                //Metemos un 1 en esa posicion
                poblacion_mutada[cromosoma_aleatorio].set_gen_concreto(i,1);
                //Avanzamos la iteracion para no evaluar el 1 recien metido como uno de los que busco
                i++;

            }

            //Si aun no he cambiado el 1 y encuentro un 1
            if(poblacion_mutada[cromosoma_aleatorio].get_gen_concreto(i) == 1 && !uno_cambiado){

            	//Aumento la cantidad de 1's que he cruzado hasta ahora
                unos_pasados++;

                //Si he encontrado el 1 que buscaba
                if(unos_pasados == gen_aleatorio+1){

                	//Meto un 0 en esa posicion
                    poblacion_mutada[cromosoma_aleatorio].set_gen_concreto(i,0);
                    //Marco que ya he cambiado el 1 que buscaba
                    uno_cambiado = true;

                }

            }

            //Si ya he cambiado el 1 y he metido el 0, no hay que seguir recorriendo el vector
            if(uno_cambiado && !busca_0) continuar = false;

            //Avanzamos al siguiente gen
            i++;

        }

        //Avanzamos al siguiente cromosoma
        contador++;

    }

    //Despues de mutar vamos a recalcular el coste de todos aquellos cromosomas que han sufrido alguna modificacion, en cruce, reparacion o mutacion
    reparar_coste(poblacion_mutada, distancias, evaluaciones);

    //Devolvemos la poblacion mutada y con los costes actualizados
    return poblacion_mutada;

}


//Funcion que dada una poblacion te saca el mejor cromosoma segun su coste
/////poblacion -> poblacion de la que se obtiene el mejor elemento
///////////return -> devuelve el cromosoma que tiene el mejor coste de esta poblacion
Cromosoma get_mejor_elemento_poblacion(vector<Cromosoma> poblacion){

	//Indice del cromosoma que tiene el mejor coste. Por defecto es el primero
    int indice_mejor = 0;
    //El mayor aporte empieza en negativo para coger el primero que se evalue por muy pequeño que sea (distancias >= 0 siempre)
    int mayor_aporte = -1;

    //Recorremos toda la poblacion
    for(int i = 0 ; i < poblacion.size(); i++){

    	//Si el cromosoma que evaluamos es mejor que el mejor hasta ahora
        if(poblacion[i].get_coste()>mayor_aporte){

        	//Actualizamos el valor del mejor
            mayor_aporte = poblacion[i].get_coste();
            //Almacenamos que cromosoma dentro de la poblacion es el mejor hasta ahora
            indice_mejor = i;

        }

    }

    //Devolvemos el mejor cromosoma
    return poblacion[indice_mejor];

}

//Funcion que genera una poblacion de 50 cromosomas diferentes
/////m -> numero de 1's que tiene que tener el cromosoma que generemos
/////n -> numero de elementos que tiene que tener el cromosoma que generemos
/////evaluaciones -> numero de evaluaciones que se hace por cada modelo (maximo 100000) que incrementa cada vez que se calcula un coste, por lo que se aumenta al crear un cromosoma nuevo
/////distancias -> matriz que contiene las distancias entre todos los puntos
/////poblacion -> vector de cromosomas en el que se devuelve por referencia la poblacion generada
void genera_poblacion(int m, int n, double **distancias, int &evaluaciones, vector<Cromosoma> &poblacion){

    //Limpiamos la poblacion por si tiene algun cromosoma por error dentro
    poblacion.clear();

    //Durante 50 veces...
	for(int i = 0 ; i < 50; i++){

		//Creamos un nuevo cromosoma
        Cromosoma cromosoma(m, n, distancias);
        //Al crearlo este calcula su coste, por lo que las evaluaciones aumentan
        evaluaciones++;
        //Introducimos este cromosoma en la poblacion
        poblacion.push_back(cromosoma);

    }

}

//MODELO GENERACIONAL UNIFORME
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma modelo_generacional_uniforme(int m, int n, double **distancias){

	//Numero de evaluaciones realizadas
    int contador = 0;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

    	//Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  50);

        //Los cruzamos con cruce uniforme
        poblacion_intermedia = cruce_uniforme(padres, distancias, n);

        //Para cada cromosoma de la poblacion de hijos
        for(int j = 0; j < poblacion_intermedia.size(); j++){

        	//Si ha sido modificado
            if(poblacion_intermedia[j].get_reevaluar()){

            	//Lo mando a reparar para que tenga exactamente m 1's en 'genes'
                funcion_reparacion_cuniforme(poblacion_intermedia[j], m, distancias);

            }

        }

        //Mutamos la poblacion de hijos ya reparada
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

        //Introducimos en la poblacion mutada, sustituyendo a un cromosoma aleatorio, el mejor de la poblacion original
        poblacion_mutada[Randint (0,poblacion_mutada.size()-1)] = get_mejor_elemento_poblacion(poblacion);

        //Sustituimos la poblacion original con la que tenemos tras la mutacion y empezamos la iteracion de nuevo
        poblacion = poblacion_mutada;

    }

    //Devolvemos el mejor cromosoma de la poblacion
    return get_mejor_elemento_poblacion(poblacion);

}

//MODELO ESTACIONARIO UNIFORME
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma modelo_estacionario_uniforme(int m, int n, double **distancias){

    //Numero de evaluaciones realizadas
    int contador = 0;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

    	//Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  2);

        //Los cruzamos con cruce uniforme
        poblacion_intermedia = cruce_uniforme(padres, distancias, n);


        //Para cada cromosoma de la poblacion de hijos
        for(int j = 0; j < poblacion_intermedia.size(); j++){

        	//Si ha sido modificado
            if(poblacion_intermedia[j].get_reevaluar()){

            	//Lo mando a reparar para que tenga exactamente m 1's en 'genes'
                funcion_reparacion_cuniforme(poblacion_intermedia[j], m, distancias);

            }

        }

        //Mutamos la poblacion de hijos ya reparada
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

        //Introducimos en la poblacion original el mejor hijo generado
        poblacion[Randint (0,poblacion.size()-1)] = get_mejor_elemento_poblacion(poblacion_mutada);

    }

    //Devolvemos el mejor cromosoma de la poblacion tras 100000 iteraciones
    return get_mejor_elemento_poblacion(poblacion);

}

//MODELO GENERACIONAL POSICIONAL
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma modelo_generacional_posicional(int m, int n, double **distancias){

    //Numero de evaluaciones realizadas
    int contador = 0;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

    	//Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  50);

        //Los cruzamos con cruce posicional
        poblacion_intermedia = cruce_posicional(padres, distancias, n);

        //Mutamos la poblacion de hijos
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

		//Introducimos en la poblacion mutada, sustituyendo a un cromosoma aleatorio, el mejor de la poblacion original
        poblacion_mutada[Randint (0,poblacion.size()-1)] = get_mejor_elemento_poblacion(poblacion);

        //Sustituimos la poblacion original con la que tenemos tras la mutacion y empezamos la iteracion de nuevo
        poblacion = poblacion_mutada;

    }

    //Devolvemos el mejor elemento de la poblacion tras 100000 iteraciones
    return get_mejor_elemento_poblacion(poblacion);

}

//MODELO ESTACIONARIO POSICIONAL
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma modelo_estacionario_posicional(int m, int n, double **distancias){

    //Numero de evaluaciones realizadas
    int contador = 0;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

    	//Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  2);

        //Los cruzamos con cruce posicional
        poblacion_intermedia = cruce_posicional(padres, distancias, n);


        //Mutamos la poblacion de hijos ya reparada
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

        //Introducimos en la poblacion original el mejor hijo generado
        poblacion[Randint (0,poblacion.size()-1)] = get_mejor_elemento_poblacion(poblacion_mutada);

    }

    //Devolvemos el mejor elemento de la poblacion tras 100000 iteraciones
    return get_mejor_elemento_poblacion(poblacion);

}

//MODELO MEMETICO 10_1 QUE REALIZA BL A TODOS LOS CROMOSOMAS CADA 10 GENERACIONES
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma algotimo_memetico_10_1(int m, int n, double **distancias){

    //Numero de evaluaciones realizadas
    int contador = 0;
    //Numero de generaciones que se han generado
    int generacion = 1;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original
    //Vector que contiene los elementos no seleccionados por un cromosoma, equivalente al vecindario de 'genes' de cada elemento de la poblacion
    vector<int> S;
    //Vector que sera equivalente a 'genes' de cada cromosoma pero en decima en vez de en binario
    vector<int> poblacion_decimal;

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

        //Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  50);

        //Los cruzamos con cruce uniforme
        poblacion_intermedia = cruce_uniforme(padres, distancias, n);

        //Para cada cromosoma de la poblacion de hijos
        for(int j = 0; j < poblacion_intermedia.size(); j++){

        	//Si ha sido modificado
            if(poblacion_intermedia[j].get_reevaluar()){

            	//Lo mando a reparar para que tenga exactamente m 1's en 'genes'
                funcion_reparacion_cuniforme(poblacion_intermedia[j], m, distancias);

            }

        }

        //Mutamos la poblacion de hijos ya reparada
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

        //Si han pasado ya 10 generaciones
        if(generacion%10 == 0){

        	//Para cada cromosoma de la poblacion
            for(int j = 0; j < poblacion_mutada.size(); j++){

            	//Obtenemos a raiz de 'genes' el vector de los puntos no seleccionados
                rellena_vecindario_binario(S, poblacion_mutada[j].get_genes());

                //Pasamos el vector de 'genes' a su equivalente decimal para hacer m iteraciones en lugar de n
                poblacion_decimal = pasar_binario_a_decimal(poblacion_mutada[j].get_genes());

                //Pasamos el vector de vecinos a decimal para hacer n-m iteraciones en lugar de n
                S = pasar_binario_a_decimal(S);

                //Llamos a solucion BL para cada cromosoma
                solucion_BL(distancias, poblacion_decimal, S, 0);

                //cambiamos los genes del cromosoma por aquellos obtenidos de BL
                poblacion_mutada[j].set_genes(pasar_decimal_a_binario(poblacion_decimal, n), distancias);

            }

        }

        //Aumento la generacion por la que vamos
        generacion++;

        //Introducimos en la poblacion mutada, sustituyendo a un cromosoma aleatorio, el mejor de la poblacion original
        poblacion_mutada[Randint (0,poblacion.size()-1)] = get_mejor_elemento_poblacion(poblacion);

        //Sustituimos la poblacion original con la que tenemos tras la mutacion y empezamos la iteracion de nuevo
        poblacion = poblacion_mutada;

    }

    //Devolvemos el mejor elemento de la poblacion tras 100000 iteraciones
    return get_mejor_elemento_poblacion(poblacion);

}

//MODELO MEMETICO 10_0.1 QUE REALIZA BL A 10 CROMOSOMAS ALEATORIOS CADA 10 GENERACIONES
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma algotimo_memetico_10_01(int m, int n, double **distancias){

    //Numero de evaluaciones realizadas
    int contador = 0;
    //Numero de generaciones que se han generado
    int generacion = 1;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original
    //Vector que contiene los elementos no seleccionados por un cromosoma, equivalente al vecindario de 'genes' de cada elemento de la poblacion
    vector<int> S;
    //Vector que sera equivalente a 'genes' de cada cromosoma pero en decima en vez de en binario
    vector<int> poblacion_decimal;

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

        //Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  50);

        //Los cruzamos con cruce uniforme
        poblacion_intermedia = cruce_uniforme(padres, distancias, n);

        //Para cada cromosoma de la poblacion de hijos
        for(int j = 0; j < poblacion_intermedia.size(); j++){

        	//Si ha sido modificado
            if(poblacion_intermedia[j].get_reevaluar()){

            	//Lo mando a reparar para que tenga exactamente m 1's en 'genes'
                funcion_reparacion_cuniforme(poblacion_intermedia[j], m, distancias);

            }

        }

        //Mutamos la poblacion de hijos ya reparada
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

        //Si han pasado ya 10 generaciones
        if(generacion%10 == 0){

			//Para 10 cromosomas de la poblacion
            for(int j = 0; j < (int)(0.1*poblacion_mutada.size()); j++){

            	//Obtenemos a raiz de 'genes' el vector de los puntos no seleccionados
                rellena_vecindario_binario(S, poblacion_mutada[j].get_genes());

                //Pasamos el vector de 'genes' a su equivalente decimal para hacer m iteraciones en lugar de n
                poblacion_decimal = pasar_binario_a_decimal(poblacion_mutada[j].get_genes());

                //Pasamos el vector de vecinos a decimal para hacer n-m iteraciones en lugar de n
                S = pasar_binario_a_decimal(S);

                //Llamos a solucion BL para cada cromosoma
                solucion_BL(distancias, poblacion_decimal, S, 0);

                //cambiamos los genes del cromosoma por aquellos obtenidos de BL
                poblacion_mutada[j].set_genes(pasar_decimal_a_binario(poblacion_decimal, n), distancias);

            }

        }

        //Aumento la generacion por la que vamos
        generacion++;

        //Introducimos en la poblacion mutada, sustituyendo a un cromosoma aleatorio, el mejor de la poblacion original
        poblacion_mutada[Randint (0,poblacion.size()-1)] = get_mejor_elemento_poblacion(poblacion);

        //Sustituimos la poblacion original con la que tenemos tras la mutacion y empezamos la iteracion de nuevo
        poblacion = poblacion_mutada;

    }

    //Devolvemos el mejor elemento de la poblacion tras 100000 iteraciones
    return get_mejor_elemento_poblacion(poblacion);

}

//MODELO MEMETICO 10_0.1MEJ QUE REALIZA BL A LOS 10 MEJORES CROMOSOMAS DE LA POBLACION CADA 10 GENERACIONES
/////m -> numero de 1's que debe tener 'genes' de cada cromosoma de la poblacion (cantidad de elementos que se escogen por solucion)
/////n -> numero de elementos en total que tiene nuestro vecto 'genes' en cada cromosoma de la poblacion
/////distancias -> matriz que contiene las distancias entre todos los puntos
//////////return -> mejor cromosoma de la poblacion tras las 100000 evaluaciones
Cromosoma algotimo_memetico_10_01mej(int m, int n, double **distancias){

    //Numero de evaluaciones realizadas
    int contador = 0;
    //Numero de generaciones que se han generado
    int generacion = 1;
    //Vector de cromosomas correspondiente a cada fase de la iteracion
    vector<Cromosoma> poblacion; 				//original
    vector<Cromosoma> padres ;					//padres
    vector<Cromosoma> poblacion_intermedia;		//hijos
    vector<Cromosoma> poblacion_mutada;			//hijos mutados que sustituiran la poblacion original
    //Vector que contiene los elementos no seleccionados por un cromosoma, equivalente al vecindario de 'genes' de cada elemento de la poblacion
    vector<int> S;
    //Vector que sera equivalente a 'genes' de cada cromosoma pero en decima en vez de en binario
    vector<int> poblacion_decimal;

    //Generamos una poblacion original antes de empezar las iteraciones
    genera_poblacion(m, n, distancias, contador, poblacion);

    //Mientras no se hayan realizado 100000 evaluaciones de la funcion objetivo
    while(contador<100000){

        //Escogemos por torneo a los padres
        padres = seleccionar_padres(poblacion,  50);

        //Los cruzamos con cruce uniforme
        poblacion_intermedia = cruce_uniforme(padres, distancias, n);

        //Para cada cromosoma de la poblacion de hijos
        for(int j = 0; j < poblacion_intermedia.size(); j++){

        	//Si ha sido modificado
            if(poblacion_intermedia[j].get_reevaluar()){

            	//Lo mando a reparar para que tenga exactamente m 1's en 'genes'
                funcion_reparacion_cuniforme(poblacion_intermedia[j], m, distancias);

            }

        }

        //Mutamos la poblacion de hijos ya reparada
        poblacion_mutada = mutacion(poblacion_intermedia, distancias, n,m, contador);

        //Si han pasado ya 10 generaciones
        if(generacion%10 == 0){

        	//Vector que contendra los indices de los 10 cromosomas con mas coste
            vector<int> mejores;

            //Para 10 cromosomas de la poblacion
            while(mejores.size()<(int)(0.1*poblacion_mutada.size())){

            	//Indice del mejor cromosoma de la poblacion en cada momento, inicialmente el primero
                int indice_mejor = 0;
                //El mayor coste inicial es un numero negativo para que el primero, sea cual sea, ya sea mayor (las distancias son como minimo 0)
                int mayor_coste = -1;
                //Booleano que indica si este elemento que hemos encontrado esta ya seleccionado o no
                bool encontrado = false;

                //Recorremos todos los cromosomas en busca del que tenga el aporte mas alto
                for(int j = 0 ; j < poblacion_mutada.size(); j++){

                	//Reestablezco el valor de haberlo encontrado previamente o no
                    encontrado = false;

                    //Si el elemento que estamos evaluando tiene un coste mayor que el mayor que hemos registrado hasta ahora
                    if(poblacion_mutada[j].get_coste()>mayor_coste){

                    	//Recorremos el vector con los indices de los mejores ya seleccionados
                        for(int i = 0 ; i < mejores.size() && !encontrado; i++){

                        	//Si encontramos que este indice ya ha sido seleccionado dejamos de recorre e indicamos que ha sido encontrado ya
                            if(mejores[i] == j) encontrado = true;

                        }

                        //Si no habia sido encontrado antes
                        if(!encontrado){

                        	//El mayor coste se actualiza
                            mayor_coste = poblacion_mutada[j].get_coste();
                            //Se guarda el indice del elemento
                            indice_mejor = j;

                        }

                    }

                }

                //Al recorrerlos todos, meto en 'mejores' el indice del punto que mayor aporte tenia y no habia sido seleccionado antes
                mejores.push_back(indice_mejor);

            }

            //Para cada uno de los 10 mejores elementos
            for(int j = 0; j < (mejores.size()); j++){

            	//Obtenemos a raiz de 'genes' el vector de los puntos no seleccionados
                rellena_vecindario_binario(S, poblacion_mutada[mejores[j]].get_genes());

                //Pasamos el vector de 'genes' a su equivalente decimal para hacer m iteraciones en lugar de n
                poblacion_decimal = pasar_binario_a_decimal(poblacion_mutada[mejores[j]].get_genes());

                //Pasamos el vector de vecinos a decimal para hacer n-m iteraciones en lugar de n
                S = pasar_binario_a_decimal(S);

                //Llamos a solucion BL para cada cromosoma
                solucion_BL(distancias, poblacion_decimal, S, 0);

                //cambiamos los genes del cromosoma por aquellos obtenidos de BL
                poblacion_mutada[mejores[j]].set_genes(pasar_decimal_a_binario(poblacion_decimal, n), distancias);

            }

        }

        //Aumento la generacion por la que vamos
        generacion++;

        //Introducimos en la poblacion mutada, sustituyendo a un cromosoma aleatorio, el mejor de la poblacion original
        poblacion_mutada[Randint (0,poblacion.size()-1)] = get_mejor_elemento_poblacion(poblacion);

        //Sustituimos la poblacion original con la que tenemos tras la mutacion y empezamos la iteracion de nuevo
        poblacion = poblacion_mutada;

    }

    //Devolvemos el mejor elemento de la poblacion tras 100000 iteraciones
    return get_mejor_elemento_poblacion(poblacion);

}


int main(int argc, char *argv[]){

	//Establezco semilla por defecto por si no la recibe como argumento de la ejecucion
    int semilla =5;

    //Si tiene mas argumentos de los que debe
    if(argc > 3){
        cout << "NUMERO DE ARGUMENTOS ERRONEO: EJECUTABLE NOMBRE_FICHERO SEMILLA" << endl;
        exit(1);

    //Si se le introducen correctamente
    }else  if(argc == 3)  semilla = atoi(argv[2]);

    //Si son menos argumentos de la cuenta
    else{
        cout << "NUMERO DE ARGUMENTOS ERRONEO: EJECUTABLE NOMBRE_FICHERO SEMILLA" << endl;
    }

    //Inicializamos la semilla de numeros aleatorios
    Set_random(semilla);

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

    //***********************************MODELO GENERACIONAL UNIFORME*********************************************
    cout << "MODELO GENERACIONAL UNIFORME "<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_mgu = high_resolution_clock::now();

    //Llamamos al modelo generacional uniforme
    Cromosoma crom2 = modelo_generacional_uniforme(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_mgu = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_mgu = duration_cast<milliseconds>(end_mgu - begin_mgu);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_mgu.count()   << endl;
    cout << "---------------------------------------"<< endl;


    //***********************************MODELO GENERACIONAL POSICIONAL*********************************************
    cout << "MODELO GENERACIONAL POSICIONAL "<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_mgp = high_resolution_clock::now();

    //Llamamos al modelo generacional posicional
    crom2 = modelo_generacional_posicional(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_mgp = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_mgp = duration_cast<milliseconds>(end_mgp - begin_mgp);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_mgp.count()   << endl;
    cout << "---------------------------------------"<< endl;


    //***********************************MODELO ESTACIONARIO UNIFORME*********************************************
    cout << "MODELO ESTACIONARIO UNIFORME "<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_meu = high_resolution_clock::now();

    //Llamamos al modelo estacionario uniforme
    crom2 = modelo_estacionario_uniforme(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_meu = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_meu = duration_cast<milliseconds>(end_meu - begin_meu);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_meu.count()   << endl;
    cout << "---------------------------------------"<< endl;

    //***********************************MODELO ESTACIONARIO POSICIONAL*********************************************
    cout << "MODELO ESTACIONARIO POSICIONAL "<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_mep = high_resolution_clock::now();

    //Llamamos al modelo estacionario posicional
    crom2 = modelo_estacionario_posicional(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_mep = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_mep = duration_cast<milliseconds>(end_mep - begin_mep);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_mep.count()   << endl;
    cout << "---------------------------------------"<< endl;

    //***********************************MODELO MEMETICO 10_1*********************************************
    cout << "MODELO MEMETICO 10_1.0"<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_mm1 = high_resolution_clock::now();

    //Llamamos al modelo memetico 10_1
    crom2 = algotimo_memetico_10_1(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_mm1 = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_mm1 = duration_cast<milliseconds>(end_mm1 - begin_mm1);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_mm1.count()   << endl;
    cout << "---------------------------------------"<< endl;

    //***********************************MODELO MEMETICO 10_0.1*********************************************
    cout << "MODELO MEMETICO 10_0.1"<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_mm2 = high_resolution_clock::now();

    //Llamamos al modelo memetico 10_0.1
    crom2 = algotimo_memetico_10_01(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_mm2 = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_mm2 = duration_cast<milliseconds>(end_mm2 - begin_mm2);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_mm2.count()   << endl;
    cout << "---------------------------------------"<< endl;

    //***********************************MODELO MEMETICO 10_0.1mej*********************************************
    cout << "MODELO MEMETICO 10_0.1mej"<<endl;
    //Obtenemos la hora del sistema actual para saber cuando empieza la ejecucion de este modelo
    auto begin_mm3 = high_resolution_clock::now();

    //Llamamos al modelo memetico 10_0.1mej
    crom2 = algotimo_memetico_10_01mej(m, n, matriz_dist);

    //Obtenemos la hora del sistema actual para saber cuando termina la ejecucion de este modelo
    auto end_mm3 = high_resolution_clock::now();

    //Restamos la hora del final con la del inicio para saber cuanto tiempo de ejecucion estuvo este modelo
    auto elapsed_mm3 = duration_cast<milliseconds>(end_mm3 - begin_mm3);

    //Mostramos el tiempo de ejecucion empleado para ello
    cout<<"\nCoste: "<<crom2.get_coste()<<endl;
    cout << "\tMilisegundos: " << elapsed_mm3.count()   << endl;
    cout << "---------------------------------------"<< endl;

    //Limpiamos la matriz de distancias
    vaciar_matriz(matriz_dist, n);

    return 0;
}
