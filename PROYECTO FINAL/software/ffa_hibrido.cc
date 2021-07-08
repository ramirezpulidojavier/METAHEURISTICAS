extern "C" {
    #include "cec17.h"
}
#include <assert.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <functional>


using namespace std;

void clip(vector<double> &sol, int lower, int upper) {
  for (auto &val : sol) {
    if (val < lower) {
      val = lower;
    }
    else if (val > upper) {
      val = upper;
    }
  }
}

void increm_bias(vector<double> &bias, vector<double> dif) {
  for (unsigned i = 0; i < bias.size(); i++) {
    bias[i] = 0.2*bias[i]+0.4*(dif[i]+bias[i]);
  }
}

void decrement_bias(vector<double> &bias, vector<double> dif) {
  for (unsigned i = 0; i < bias.size(); i++) {
    bias[i] = bias[i]-0.4*(dif[i]+bias[i]);
  }
}

/**
 * Aplica el Solis Wets
 *
 * @param sol solucion a mejorar.
 * @param fitness fitness de la solución.
 * @param
 * @param maxevals número máximo de evaluaciones que ejecutará el algoritmo
 * @param lower ínfimo del espacio de búsqueda
 * @param upper máximo del espacio de búsqueda
 *
 * @details Ejemplo de uso: `bl_soliswets(sol, fitness, 0.2, 1000, -100, 100, generador);`
 */
template <class Random>
double bl_soliswets(vector<double> &sol, double &fitness, double delta, int maxevals, int lower, int upper, Random &random) {
  const size_t dim = sol.size();
  vector<double> bias (dim), dif (dim), newsol (dim);
  double newfit;
  size_t i;

  int evals = 0;
  int num_success = 0;
  int num_failed = 0;

  while (evals < maxevals) {
    std::uniform_real_distribution<double> distribution(0.0, delta);

    for (i = 0; i < dim; i++) {
      dif[i] = distribution(random);
      newsol[i] = sol[i] + dif[i] + bias[i];
    }

    clip(newsol, lower, upper);
    newfit = cec17_fitness(&newsol[0]);
    evals += 1;

    if (newfit < fitness) {
      sol = newsol;
      fitness = newfit;
      increm_bias(bias, dif);
      num_success += 1;
      num_failed = 0;
    }
    else if (evals < maxevals) {

      for (i = 0; i < dim; i++) {
        newsol[i] = sol[i] - dif[i] - bias[i];
      }

      clip(newsol, lower, upper);
      newfit = cec17_fitness(&newsol[0]);
      evals += 1;

      if (newfit < fitness) {
        sol = newsol;
        fitness = newfit;
        decrement_bias(bias, dif);
        num_success += 1;
        num_failed = 0;
      }
      else {
        for (i = 0; i < dim; i++) {
          bias[i] /= 2;
        }

        num_success = 0;
        num_failed += 1;
      }
    }

    if (num_success >= 5) {
      num_success = 0;
      delta *= 2;
    }
    else if (num_failed >= 3) {
      num_failed = 0;
      delta /= 2;
    }
  }

  return evals;
}


//Estructura para cada luciernaga
//solucion -> vector con elementos aleatorios entre -100 y 100 que representa a cada luciernaga
//fitness -> valor de la funcion objetivo de cada luciernaga segun su 'solucion'
struct Luciernaga {

    vector<double> solucion;       //tiene 'dim' elementos
    double fitness;                //valor a minimizar

    //Operador necesario para ordenar un vector de luciernagas segun su fitness
    bool operator<(const Luciernaga& luciernaga2) const {

        return (fitness < luciernaga2.fitness);  
    
    }

    
};


//FUNCION PARA CREAR EL VECTOR DE UN ELEMENTO (EN ESTE CASO DE UNA LUCIERNAGA)
//cantidad_elementos -> numero de elementos que queremos generar dentro de nuestro vector 'sol'
//generador -> utilizado para la creacion de valores dentro de la distribucion real de -100 a 100
/////return -> vector de doubles con la solucion que acabamos de generar aleatoriamente
template <class Random>
auto solucion_aleatoria (int cantidad_elementos, Random &generador) -> vector<double> {

    //Definimos el rango de valores entre los que puede generar los numeros. Especificado en el problema
    std::uniform_real_distribution<> uniforme_real(-100.0, 100.0);

    //Vector solucion en el que meteremos los valores y que mas tarde devolveremos
    vector<double> solucion_elemento;

    //Se generan  'cantidad_elementos' aleatorios en la distribucion real de -100 a 100 y se introducen en la solucion
    for (int i = 0; i < cantidad_elementos; i++) {
        solucion_elemento.push_back(uniforme_real(generador));
    }

    //Devolvemos la solucion generada
    return solucion_elemento;
}


//FUNCION PARA CREAR UN CONJUNTO DE ELEMENTOS SIMULANDO UNA POBLACION
//cantidad_elementos -> cantidad real de elementos que debe tener nuestra poblacion
//cantidad_excesiva_elementos -> cantidad de elementos que creamos, superior a cantidad_elementos y de entre los
//                               que elegiremos los cantidad_elementos mejores
//dim -> cantidad de elementos que tendra cada objeto de la poblacion
//generador -> objeto de tipo Random para generar numeros aleatorios dentro de cada elemento de la poblacion
/////return -> Vector de Luciernagas que hace de poblacion con 'cantidad_elementos' elementos dentro
template <class Random>
auto crea_poblacion (int cantidad_elementos, int cantidad_excesiva_elementos, int dim, Random &generador) -> vector<Luciernaga> {
    
    //Vector que contendra todos los elementos de la poblacion
    vector<Luciernaga> poblacion;

    //Generamos mas elementos de los que va a tener realmente nuestra poblacion
    for (int i = 0; i < cantidad_excesiva_elementos; i++) {

        //Llamamos a la funcion que cree e inicialice un elemento con numeros aleatorios
        auto sol = solucion_aleatoria(dim, generador);

        //Añadimos este elemento recien creado a la poblacion y le calculamos su coste(gasto de evaluacion)
        poblacion.push_back({ .solucion = sol, .fitness = cec17_fitness(&sol[0])});

    }

    //Cuando tenemos la poblacion completa (con mas elementos de los que necesitamos realmente) la ordenamos
    sort(poblacion.begin(), poblacion.end());

    //Y eliminamos del final (las peores) el sobrante del que hablabamos. Asi nos quedamos con las mejores
    if (cantidad_excesiva_elementos > cantidad_elementos) 
        poblacion.erase(poblacion.begin() + cantidad_elementos, poblacion.end());

    //Si despues de este proceso la poblacion tiene tantos elementos como necesitamos de verdad, sigue adelante    
    assert(poblacion.size() == cantidad_elementos);

    //Devolvemos la poblacion una vez rellena
    return poblacion;

}

//FUNCION PARA MOVER LAS LUCIERNAGAS ENTRE ELLAS
//poblacion -> conjunto en el que se encuentran todos los elementos que son los que iremos moviendo
//dim -> cantidad de valores dentro del vector de cada elemento. Es el tamaño de cada elementos por asi decirlo
//evaluaciones -> se pasa como referencia para llevar una cuenta de las evaluaciones que hace (condicion de parada)
void mover_luciernaga(vector<Luciernaga> &poblacion, int dim, int &evaluaciones){

    //Variable que controla el tamaño del paso al acercar las luciernagas y pertenece a [0; 1]
    double alpha = 0.5;    
    //Para el calculo de B (atraccion de la luciernaga)
    double betamin = 0.2; 
    //Coeficiente de absorcion de la luz       
    double gama = 1.0;     
    //Hasta que se demuestre lo contrario, es el mejor
    mejor = true; 

    //Para mantener los valores en la escala de -100 y 100
    double escala = 200;

    //Para cada luciernaga de la poblacion
    for(int i = 0 ; i < poblacion.size(); i++){

        //Hasta que se demuestre lo contrario, es el mejor
        mejor = true;

        //Comparamos con cada una de las otras luciernagas de la poblacion
        for(int j = 0 ; j < poblacion.size(); j++){
            
            //Para no comparar una luciernaga con ella misma, saltamos esa iteracion
            if(i == j && j < poblacion.size()-1) j++;            

            //Para cada par i,j de luciernagas que comparamos restauramos la distancia a 0
            double distancia_entre_elementos = 0.0;
            
            //La distancia es la suma de las diferencias entre cada elemento de la solucion de ambas luciernagas
            //al cuadrado
            for(int k = 0; k < dim; k++){

                distancia_entre_elementos += (poblacion[i].solucion[k]-poblacion[j].solucion[k])*(poblacion[i].solucion[k]-poblacion[j].solucion[k]);

            }

            //Y luego la raiz de esta suma es la distancia definitiva
            distancia_entre_elementos = sqrt(distancia_entre_elementos);

            //Si la luciernaga j tiene menos fitness, la i se acerca a esta
            if(poblacion[j].fitness < poblacion[i].fitness){

                //ponemos a falso porque ya es peor que al menos 1
                mejor = false;

                //Calculamos el atractivo de esas luciernagas a esa distancia
                double beta0 = 1.0;
                double beta = (beta0-betamin)*exp(-gama*pow(distancia_entre_elementos, 2.0))+betamin;

                //Para cada elemento de la solucion de la luciernaga i (la que movemos hacia la otra)
                for(int k=0;k<dim;k++){

                    //Escalamos el valor y movemos la luciernaga i en direccion de la j (formula explicada en memoria)
                    distancia_entre_elementos = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
                    double aleatorizacion = alpha*(distancia_entre_elementos-0.5)*escala;
                    poblacion[i].solucion[k] = poblacion[i].solucion[k]*(1.0-beta)+poblacion[j].solucion[k]*beta+aleatorizacion;
                
                }

                //Para cada elemento de la solucion del hijo comprobamos que este dentro del rango de valores permitidos
                for(int h=0;h<dim;h++){

                    //Si excede el limite inferior, satura a este
                    if(poblacion[i].solucion[h] < -100)
                        poblacion[i].solucion[h] = -100;

                    //Si excede el limite superior, satura a este
                    if(poblacion[i].solucion[h] > 100)
                        poblacion[i].solucion[h] = 100;

                }

                //Recalculamos el fitness del elemento que hemos movido porque hemos modificado su solucion
                poblacion[i].fitness = cec17_fitness(&poblacion[i].solucion[0]);
                //contamos la evaluacion
                evaluaciones++;
                
            }

        }

        if(mejor){

            for(int k=0;k<dim;k++){

                //Defino un valor aleatorio entre -10 y 10
                int aleatorio1 = -10 + rand()%21;

                poblacion[i].solucion[k] = poblacion[i].solucion[k] + aleatorio1;

                if(poblacion[i].solucion[k] > 100) poblacion[i].solucion[k] = 100;
                else if (poblacion[i].solucion[k] < -100) poblacion[i].solucion[k]= -100;
                
            
            }

        }

    }
    
}

int main(int argc, char *argv[]) {
    
    //Se inicializa por defecto en 10 por si no se le pasa como parametro
    int dim = 10;

    //Podemos pasar la dimension como parametro y sobreescribir la definida por defecto (valor 10)
    if (argc > 1) {
        dim = atoi(argv[1]);
    }

    //Distribucion real de los datos de los que generaremos numeros aleatorios para cada luciernaga
    std::uniform_real_distribution<> uniforme_real(-100.0, 100.0);

    //Cantidad de elementos que tendra nuestra poblacion
    const int poblacion_inicial = 50;
    //Cantidad de elementos que creamos realmente de entre los que seleccionaremos los mejores
    const int cantidad_excesiva_elementos = 100;  
    //Evaluaciones maximas permitidas a BL para evitar que este se coma todas las que tenemos
    const int evaluaciones_bl_maximas = 100;
    //Parametro necesario para la busqueda local
    const double delta = 0.4;


    //Repetimos la funcion 30 veces con diferentes parametros
    for (int id_funcion = 1; id_funcion <= 30; id_funcion++) {
       
        //Los datos saldran en ficheros de la carpeta 'results_Firefly_mejorado'
        //Inicializamos el benchmark
        cec17_init("Firefly_bl", id_funcion, dim);

        //Generador pseudoaleatorio de numeros de 32 bits 
        std::mt19937 generador;

        //Creamos una poblacion inicial con nuestra funcion
        vector<Luciernaga> poblacion = crea_poblacion(poblacion_inicial, cantidad_excesiva_elementos, dim, generador);
        //Variable que contendra las evaluaciones realizadas para la condicion de parada
        //Empieza con tantos elementos como hayamos generado para la poblacion (cantidad real creada, no
        //la cantidad con la que nos quedamos finalmente) porque para cada una se calcula un fitness
        int evaluaciones = cantidad_excesiva_elementos;

        //Mientras no se cumpla la condicion de parada asociada a la cantidad de evaluaciones a hacer
        while (evaluaciones < 10000 * dim) {

            //Aplicamos la BL para cada elemento de la poblacion
            for (int i = 0; i < poblacion.size(); i++) {

                evaluaciones += bl_soliswets(
                    poblacion[i].solucion,   poblacion[i].fitness,
                    delta, evaluaciones_bl_maximas,
                    -100, 100, generador
                );
                
            }   

            //Llamamos a la funcion que movera a las luciernagas como es debido (Recorre una vez el conjunto completo)
            mover_luciernaga(poblacion, dim, evaluaciones);

        }

        //Una vez hemos realizado todas las comprobaciones, ordenamos la poblacion de mejor a peor
        //(eso quiere decir que la que tiene un fitness mas bajo se coloca al principio)
        sort(poblacion.begin(), poblacion.end());
        //El mejor elemento se encuentra ahora en la primera casilla de poblacion, por lo que obtenemos su fitness
        auto mejor = poblacion[0].fitness;

        //Mostramos este valor por pantalla
        cout <<"Mejor solución [F" <<id_funcion <<"]: " << scientific <<cec17_error(mejor) <<endl;
    }
}