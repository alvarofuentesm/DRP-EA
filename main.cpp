#include "includes.h"
#include "globals.h"

/* calculo de distancias euclideanas */
double euclideanDistance(double lat1, double lon1, double lat2, double lon2){
    double a = pow(lat1 - lat2, 2) + pow(lon1 - lon2, 2);
    return sqrt(a);
}

/* liberar la memoria dinámica */
void _exit(void){
   
    delete[] vertex; 
    for (int i = 0; i < N_eventosOHCA; i++){
        delete distances[i];
    }
    delete[] distances;

    delete[] poblacion_child;
    
    /* TODO: fix this */
    //delete[] poblacion_actual; // signal 11 en instancia Instancias/SJC-DRP-AEDs/SJC500-7.txt

    //salida sin error
    exit(0);
}


/* Leer parametros del algoritmo */
int read_params(int argc, char **argv){

    
    //archivo con la instancia del problema
    configuration_file=(char *)(argv[1]);
    
    //SEMILLA
    seed=atoi(argv[2]);

    // debug
    debug=atoi(argv[3]);

    // n of iterations
    iterations=atoi(argv[4]);

    // costos
    costo_nuevo=atof(argv[5]);
    costo_reubicar=atof(argv[6]);

    // AG config
    population_size=atoi(argv[7]);
    child_size=atoi(argv[8]);

    prob_mutation=atoi(argv[9]);
    prob_mutation_original_AED=atoi(argv[10]);

    return 1;
}


/* Split string en un arreglo*/
template <size_t N>
void splitString(string (&arr)[N], string str){
    int n = 0;
    istringstream iss(str);
    for (auto it = istream_iterator<string>(iss); it != istream_iterator<string>() && n < N; ++it, ++n)
        arr[n] = *it;
}


/*
Leer instancia.

<#eventosOHCA> <presupuesto> <Radio de Cobertura>
<coordenada-x-OHCA #1> <coordenada-y-OHCA #1> <existeAED? #1>
<coordenada-x-OHCA #2> <coordenada-y-OHCA #2> <existeAED? #2>
<coordenada-x-OHCA #3> <coordenada-y-OHCA #3> <existeAED? #3>
*/
void read_config_file(void){
    
    ifstream config(configuration_file);
    string str; 

    getline(config, str);
    
    string arr[3];
    splitString(arr, str);

    N_eventosOHCA = stoi(arr[0]);
    presupuesto = stof(arr[1]);
    radio = stoi(arr[2]);

    cout << endl;
    cout << "Instancia: " << configuration_file << endl;
    
    cout << "N_eventosOHCA " << N_eventosOHCA << endl;
    cout << "presupuesto " << presupuesto << endl;
    cout << "radio " << radio << endl;
    cout << "costo nuevo: " << costo_nuevo << endl;
    cout << "costo reubicar: " << costo_reubicar << endl;

    

    //Reserva de espacio para el arreglo de coordenadas y la matriz de distancia
    vertex = new node[N_eventosOHCA];
    distances = new double*[N_eventosOHCA]; // matrix of distances 
    for( int i = 0 ; i < N_eventosOHCA ; i++ ) distances[i] = new double [N_eventosOHCA];

    int id, aed;
    double x,y;
    int cont = 0;
    string arr2[3], temp;    

    max_X = std::numeric_limits<double>::infinity();
    min_X = -1 * std::numeric_limits<double>::infinity();
    max_Y =  std::numeric_limits<double>::infinity();
    min_Y = -1 *  std::numeric_limits<double>::infinity();

    if (0 < min_X)cout << "yes" << endl;

    originalsAEDs = 0;

    while (getline(config, str) && cont < N_eventosOHCA) {
        temp = str;
        splitString(arr2, temp);
        
        if (debug){
            cout << arr2[0]  <<  " ";
            cout << arr2[1]  <<  " ";
            cout << arr2[2]  <<  "\n";
        }

        id = cont;

        x = stof(arr2[0]);
        y = stof(arr2[1]);
        aed = stof(arr2[2]);

        if (x < max_X){
            max_X = x;
        }
        if (x > min_X){
            min_X = x;
        }
        if (y < max_Y){
            max_Y = y;
        }
        if (y > min_Y){
            min_Y = y;
        }
        
        vertex[cont].id = id;
        vertex[cont].x = x;
        vertex[cont].y = y;
        
        vertex[cont].aed = aed;
        vertex[cont].original_aed = aed;

        if (vertex[cont].aed == 1) { // previous AED 
            originalsAEDs++;

            vertex[cont].x_original = x;
            vertex[cont].y_original = y;
            cout << "Original pos AED: (" << x << ", " << y << ")" << endl;  
        }
        else{
            vertex[cont].x_original = -1;
            vertex[cont].y_original = -1;
        }

        //vertex[cont].pos = cont;

        cont++;

    }
    config.close();

    cout << "Number of Original AED's: " << originalsAEDs << endl;

    //cout << "x range: " << min_X << " " << max_X << endl;
    //cout << "y range: " << min_Y << " " << max_Y << endl;

    // distances matrix 
    for(int i = 0; i < N_eventosOHCA; i++){
        for (int j = 0; j < N_eventosOHCA; j++){            
            //distances[i][j] =  haversineDistance(vertex[i].x, vertex[i].y, vertex[j].x, vertex[j].y);
            distances[i][j] =  euclideanDistance(vertex[i].x, vertex[i].y, vertex[j].x, vertex[j].y);
        }
    }

    int disable_debug = 1;

    if (debug && !disable_debug) {
        for(int i=0; i < N_eventosOHCA; i++)
            for(int j = 0; j < N_eventosOHCA; j++)
            cout << distances[i][j] << endl;
        cout << endl;  
    }

}

/* generar población incial */
void generate_initial_population(){
    poblacion_actual = new Cromosoma[population_size];
    
    int disable_debug = 1;

    if (debug) cout << "Initial population" << endl;
    
    for (int i = 0; i < population_size; i++){
        //cout << poblacion_actual[i].eval_function << endl;
        //cout << rand() % 2 << endl;
    
        // set cromosoma
        poblacion_actual[i].set_initial_cromosoma();
    
        // calculate evaluation
        poblacion_actual[i].recalculate_eval_function();
        
        if (debug && !disable_debug){
            cout << "Individuo " << i;
            poblacion_actual[i].stats_cromosoma();
    
            //poblacion_actual[i].print_debug_cromosoma();
        
        }
        
    }
}

/* inicializar población child */
void init_childs(){
    
    int disable_debug = 1;

    for (int i = 0; i < child_size; i++){

        // set cromosoma
        poblacion_child[i].set_initial_cromosoma(1);
        
        if (debug && !disable_debug){
            poblacion_child[i].stats_cromosoma();
        }
    }
}



/* Algoritmo k-tournament. 
Retorna un arreglo con las posiciones de los dos padres ganadores*/
void k_tournament(int k, int *parents){
    int random_num; 
    int repetitions, replace_first, replace_second; // flags

    int parents_eval[2];

    parents_eval[0] = -1.0;
    parents_eval[1] = -1.0;
    

    int* prev_random = new int[k]; // para no contar traslape
    for (int j = 0; j < k; j++) prev_random[j] = - 1;

    for (int i = 0; i < k; i++) {
        while(1) { // to prevent repetitions
            random_num =  rand() % population_size;
            repetitions = 0;
            for (int j = 0; j < k; j++){
                if (prev_random[j] == random_num) repetitions = 1; 
            }
            if (repetitions == 0) break;
        }
        prev_random[i] = random_num;

        replace_first = 0;
        replace_second = 0;

        if ( poblacion_actual[random_num].get_eval_function() >  parents_eval[0]) replace_first = 1;
            
        if ( poblacion_actual[random_num].get_eval_function() >  parents_eval[1]) replace_second = 1;

        if (replace_first && replace_second){ // replace the worst 
            if (parents_eval[0] < parents_eval[1]){ // replace first
                parents_eval[0] = poblacion_actual[random_num].get_eval_function();
                parents[0] = random_num;
            }

            else{ // replace second
                parents_eval[1] = poblacion_actual[random_num].get_eval_function();
                parents[1] = random_num;
            }
        }
        else if(replace_first){ // replace first
            parents_eval[0] = poblacion_actual[random_num].get_eval_function();
            parents[0] = random_num;
        }
        else if (replace_second){ // replace second 
            parents_eval[1] = poblacion_actual[random_num].get_eval_function();
            parents[1] = random_num;
        }


    }
    delete[] prev_random;
}

/*
Cruzamiento en un punto aleatorio.

Se multiples varios hijos con cruzamiento en un punto aleatorio para cada uno. 

Aquellos hijos infactibles tendran presupuesto de cromosoma negativo o 
AED's originales duplicados.

Notar que el cruzamiento en un punto no cambia de posicion los AEDs originales
*/

void one_point_crossover(int *parents){
    int point;
    int disable_debug = 1;

    if (debug){
        cout << "-----------------------------------------------------------------------------" << endl;
        cout << "Cruzamiento en un punto " << endl;
        cout << "-----------------------------------------------------------------------------" << endl;
    } 

    if (debug && !disable_debug){
        cout << "Parents AEDs: "<< endl;
        poblacion_actual[ parents[0] ].print_debug_cromosoma(77);
        cout << endl;
        poblacion_actual[ parents[1] ].print_debug_cromosoma(77);
    }

    for (int i = 0; i < child_size; i = i + 2){ // if child_size == 10, needs 5 crossover 
        point = rand() % N_eventosOHCA; // punto en posicion aleatoria

        for(int j = 0; j < point; j++) {
            
            poblacion_child[i].copy_AED_status(j, poblacion_actual[ parents[0] ].cromosoma[j]);
            poblacion_child[1 + i].copy_AED_status(j, poblacion_actual[ parents[1] ].cromosoma[j]);
            
        }
        
        for (int j = point; j < N_eventosOHCA; j++) {
            poblacion_child[i].copy_AED_status(j, poblacion_actual[ parents[1] ].cromosoma[j]);
            poblacion_child[1 + i].copy_AED_status(j, poblacion_actual[ parents[0] ].cromosoma[j]);
        }

        
        // recalcular el presupuesto contando los AEDs del cruzamiento
        //cout << "i: " << i << endl;
        poblacion_child[i].recalculate_presupuesto();

        poblacion_child[1 + i].recalculate_presupuesto();
        
        if (debug && i == 0 && !disable_debug){
            cout << "First and second child (cross point: " << point << ") " << endl;
            poblacion_child[i].print_debug_cromosoma(point);
            cout << endl;
            poblacion_child[1 + i].print_debug_cromosoma(point);
        }
    }
    if (debug){
        for (int i = 0; i < child_size; i++){
            cout << "Child " << i << " after cross";
            poblacion_child[i].stats_cromosoma();
        }
    }
}


/*
Mutación sobre hijos factibles resultantes del cruzamiento

Se definen dos tipos de mutaciones iterando sobre todos los OHCA del cromosoma:
1) Mutacion sin AED original: agregar/quitar AED segun corresponda de un OHCA.
2) Mutacion con AED original: cambiar de posicion el AED a una nueva posicion sin AED previo, restando del presupuesto el movimiento.

Estos dos tipos de mutación tienen una probabilidad distinta, siendo mucho menor en el caso de 1).
para evitar demasiados hijos infactibles y favorecer la exploración en espacios con AED's reubicados. 
*/

void mutation(){
    int random_num, result;

    int* random_position = new int[originalsAEDs]; // para no mutar dos veces un mismo AED
    int n_random_position;

    if (debug){
        cout << "-----------------------------------------------------------------------------" << endl;
        cout << "Mutación de solo cruzamientos previamente factibles " << endl;
        cout << "-----------------------------------------------------------------------------" << endl;
    }

    for (int i = 0; i < child_size; i++){
        
        n_random_position = 0;
        for(int z = 0; z < originalsAEDs; z++) random_position[z] = -1;

        if (poblacion_child[i].get_presupuesto() < 0 || 
             poblacion_child[i].count_originals_AED() != originalsAEDs) continue; // para ignorar cruzamientos infactibles
        
        poblacion_child[i].child_cruzamiento_factible = 1;

        
        for (int j = 0; j < N_eventosOHCA; j++){
            
            if (poblacion_child[i].cromosoma[j].original_aed == 1){ // AED original (caso 2)
                
                for(int z = 0; z < originalsAEDs; z++){
                    if (random_position[z] == j)continue; // para no mutar dos veces un mismo nodo
                }
                

                random_num = rand() % 100;

                if (random_num < prob_mutation_original_AED){
                    random_position[n_random_position] = poblacion_child[i].move_Original_AED(j);
                    n_random_position++;
                }
            }
            else{ // no original AED (caso 1)
                random_num = rand() % N_eventosOHCA;
                
                if (random_num < prob_mutation){
                    result = abs(poblacion_child[i].cromosoma[j].aed - 1);
                
                    poblacion_child[i].cromosoma[j].aed = result;
                }  
            }
        }

        // recalcular presupuesto tras mutar 
        poblacion_child[i].recalculate_presupuesto();

        poblacion_child[i].recalculate_eval_function();
       

        if (debug){
            cout << "Child " << i << " after mutation";
            poblacion_child[i].stats_cromosoma();
        }

        

    }
    delete[] random_position;
}

/*
Reemplazar a lo mas los peores k individuos de la población actual por 
los hijos factibles resultantes del cruzamiento y mutacion.  

De momento el valor de k es el tamaño de la poblacion child. 
*/

void reemplazo(int k){
    int* worst_cromosomas = new int[k];
    int worst_pos, flag;
    float worst_eval;
    
    // buscar peores cromosomas de la poblacion
    for (int i = 0; i < k; i++){
        worst_eval = std::numeric_limits<double>::infinity();
        worst_pos = -1;

        for (int j = 0; j < population_size; j++){
            flag = 0;
            
            for (int x = 0; x < i; x++){
                if (worst_cromosomas[x] == j) flag = 1;
            }

            if (flag) continue;
            
            if (poblacion_actual[j].get_eval_function() < worst_eval){
                
                worst_eval = poblacion_actual[j].get_eval_function();
                worst_pos = j;
            }
        }
        worst_cromosomas[i] = worst_pos;
    }

    // reemplazar peores de la poblacion actual por hijos factibles 
    int x = 0;
    for (int i = 0; i < k; i++) { 
        flag = 0; // para evitar salirse del rango de arreglo
        
        if (x == child_size)break;

        // para solo reemplazar por hijos factibles
        while (poblacion_child[x].get_presupuesto() < 0 || 
            poblacion_child[x].count_originals_AED() != originalsAEDs ||
            poblacion_child[x].child_cruzamiento_factible != 1){

            if (x+1 == child_size){
                flag = 1;
                break;
            }
            x++;
        }
        
        if (flag) break;

        poblacion_child[x].copy_to_cromosoma(&poblacion_actual[ worst_cromosomas[i] ]);
        x++;

    }
    delete[] worst_cromosomas;
}


/* seleccionar mejor individuos localmente y reemplazar si es mejor que el 
mejor individuo global */
void select_best_of_population(){
    int best_eval = 0;
    int individuo = 0;

    for (int i = 0; i < population_size; i++){
        if (poblacion_actual[i].get_eval_function() > best_eval){
            best_eval = poblacion_actual[i].get_eval_function();
            poblacion_actual[i].copy_to_cromosoma( &best_cromosoma_local );
            individuo = i;
        }
    }
    if (debug){
        cout << "Best cromosoma local (Individuo " << individuo << "): " <<  endl;
        best_cromosoma_local.stats_cromosoma();
    }

    // reemplazo
    if (best_cromosoma_local.get_eval_function() > best_cromosoma_global.get_eval_function()){
        best_cromosoma_local.copy_to_cromosoma(&best_cromosoma_global);
    }

}

int main (int argc, char *argv[]){

    
    //lectura de parametros
    if(!read_params(argc,argv)){
        cout<<"Problemas en la lectura de los parametros";
        exit(1);
    }

    
    //semilla aleatoria
    srand48 (seed);
    

    read_config_file();
    

    // force initial child size even number
    if (child_size%2 != 0){ 
        child_size++;
    } 


    // definir la cantidad de individuos a reemplazar 
    int k_reemplazo = child_size;
    //cout << "k_reemplazo: " << k_reemplazo << endl;

    // init mejor evaluacion
    best_cromosoma_global.set_eval_function(-1);


    // generar poblacion inicial
    generate_initial_population();

    // buscar el mejor cromosoma en poblacion inicial
    select_best_of_population();

    // reservar poblacion hijos
    poblacion_child = new Cromosoma[child_size];

    // iterar generaciones
    for (int generation = 0; generation < iterations; generation ++){

        if (debug){
            cout << "-----------------------------------------------------------------------------" << endl;
            cout << "Generation: " <<  generation + 1 << endl;
            cout << "-----------------------------------------------------------------------------" << endl;
        }

        if (debug){
            for (int i = 0; i < population_size; i++){
                cout << "Individuo " << i;
                poblacion_actual[i].stats_cromosoma();
            }
        }

        // inicializar childs 
        init_childs();


        // select parents
        int* parents = new int[2];
        //k_tournament(2, parents);
        //k_tournament(population_size, parents);
        k_tournament((int)population_size/2, parents);

        if (debug){
            cout << "Parents [" << parents[0] << "," << parents[1] << "]" <<  endl;  
        }

        // cruzamiento
        one_point_crossover(parents);

        if (debug) cout << endl;
        
        // mutacion
        mutation();

        // reemplazo incremental, reemplazar a los hijos factibles por los peores de la poblacion actual

        reemplazo(k_reemplazo);

        // buscar el mejor cromosoma en poblacion actual y reemplazar el mejor global si es necesario 
        select_best_of_population();

        delete[] parents;
        

    }

    finished_iterations = iterations;

    int activate_debug = 0;
    if (debug || activate_debug){
        cout << "Final best stats: " << endl;
        best_cromosoma_global.stats_cromosoma();
    }
    
    // mostrar por pantalla mejor individuo
    best_cromosoma_global.print_best();
    
    _exit();
}