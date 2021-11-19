
// configuration
char* configuration_file;
char* results_file;
int seed;
int debug;

// parameters AG
int iterations;
int finished_iterations;
int population_size; 
int child_size;
int prob_mutation;
int prob_mutation_original_AED; 

// instance
int N_eventosOHCA, radio;
float presupuesto, costo_nuevo, costo_reubicar;
int originalsAEDs;


// utils
double min_X, max_X, min_Y, max_Y;


/* EDD nodo para almacenar informacion en cada posicion del cromosoma */

struct node{
    //int pos; // position in vertex array
    int id;
    int aed; // 0 not aed, 1 with aed
    int original_aed; // 1 if previous AED in instance
    double x_original, y_original; // coordinates of original aed
    double x, y; // coordinates
};

node* vertex;
double **distances;

/*
La clase Cromosoma corresponde a un individuo. 

Contiene su funcion de evaluacion, cromosoma y presupuesto_cromosoma 
(costo del cromosoma). Se definen metodos para encapsular las operaciones 
sobre los individuos. 
*/

class Cromosoma {
  public:
    int eval_function; // numero de OHCA cubiertos
    float presupuesto_cromosoma; // costo de la combinaciond de AED's en el cromosoma

    int child_cruzamiento_factible; // 1: si es un hijo esta dado por un cruzamiento factible, 0 si no

    vector<node> cromosoma; // vector cromosoma

    Cromosoma() {   // Constructor
        eval_function = 0;
        cromosoma = vector<node>(N_eventosOHCA);
        
        presupuesto_cromosoma = presupuesto;
    }

    /* recalcular la funcion de evaluacion */
    void recalculate_eval_function(){
        int eval = 0;
        //cout << "radio: " << radio << endl;

        int* covered = new int[N_eventosOHCA]; // para no contar traslape
        for (int i = 0; i < N_eventosOHCA; i++) covered[i] = 0;    

        for (int i = 0; i < N_eventosOHCA; i++){
            if (cromosoma[i].aed == 1) {
                for (int j = 0; j < N_eventosOHCA; j++){
                    //cout << "distances[cromosoma[i].id][j] : " << distances[cromosoma[i].id][j]  << endl;
                    if (distances[cromosoma[i].id][j] <= radio && covered[j] == 0){
                        covered[j] = 1;
                        eval++;
                    }
                }        
            }
        }

        delete[] covered;

        eval_function = eval;
    }

    /* inicializar posiciones de cromosomas. se diferencian el caso de inicializar 
    la poblacion inicial e inicializar la poblacion child */
    void set_initial_cromosoma(int childs = 0){
        
        for (int i = 0; i < N_eventosOHCA; i++){
            cromosoma[i].x = vertex[i].x;
            cromosoma[i].y = vertex[i].y;
            cromosoma[i].id = vertex[i].id;

            cromosoma[i].x_original =  vertex[i].x_original;
            cromosoma[i].y_original =  vertex[i].y_original;

            //cromosoma[i].pos = vertex[i].pos;

            if (childs == 0){ // not childs cromosoma (initial population)
                if (vertex[i].aed == 1){ // previous AED
                    cromosoma[i].aed =  1; // mantener el AED original
                    cromosoma[i].original_aed = 1;
                }
                else{
                    cromosoma[i].aed =  0; 
                    cromosoma[i].original_aed = 0;
                }
            }
            else{ // child cromosoma
                cromosoma[i].aed =  0;
                cromosoma[i].original_aed = 0;
                child_cruzamiento_factible = 0;
            }
        }

        if (childs == 0) { // poblacion incial
            int random_num, random_num2;

            int trys = (int)presupuesto; // para agregar aleatoridad

            // agregar AED's nuevos segun presupuesto y aleatoriedad
            while(presupuesto_cromosoma > 0 && trys > 0){
                if (presupuesto_cromosoma - costo_nuevo >= 0){ // agregar nuevo AED en lugar aleatorio
                    random_num = rand() % N_eventosOHCA;
                    random_num2 = rand() % 2;
                    if (cromosoma[random_num].aed ==  0 && random_num2 == 0) { // solo si no hay un AED previamente
                        cromosoma[random_num].aed = 1;                    
                        presupuesto_cromosoma -= costo_nuevo;
                    }
                }
                trys--;
            }
        }
        
    
    }

    /* copiar informacion de un cromosoma en un cromosoma destino */
    void copy_to_cromosoma(Cromosoma* dest){
        dest->cromosoma = cromosoma;
        dest->eval_function = eval_function;
        dest->presupuesto_cromosoma = presupuesto_cromosoma;
    }

    /* copiar informacion de un nodo original en un nodo ubicado en la posicion pos */
    void copy_AED_status(int pos, node original){
        cromosoma[pos].aed = original.aed;
        cromosoma[pos].original_aed = original.original_aed;
        cromosoma[pos].x_original = original.x_original;
        cromosoma[pos].y_original = original.y_original;
    }

    /* reubicacion de AED original en una posicion aleatoria sin AED previo 
    retorna la posicion random para no volver a mutar sobre la nueva posicion
    */

    int move_Original_AED(int original_pos){ // mover AED a una posicion aleatoria sin AED previo
      
        int random_position;
        while(1){
            random_position = rand()%N_eventosOHCA;
            if (random_position == original_pos || cromosoma[random_position].aed == 1) continue;

            cromosoma[random_position].aed = 1;
            cromosoma[random_position].x_original = cromosoma[original_pos].x_original;
            cromosoma[random_position].y_original = cromosoma[original_pos].y_original;
            cromosoma[random_position].original_aed = 1; 

            // reset original position
            cromosoma[original_pos].aed = 0;
            cromosoma[original_pos].x_original = -1;
            cromosoma[original_pos].y_original = -1;
            cromosoma[original_pos].original_aed = 0; 

            
            break;
        }
        return random_position;
    }

    
    /* funcion debug de la informacion de un cromosoma */
    void print_debug_cromosoma(int pos = -1){ // pos es para indicar una posicion del cromosoma especifica
        cout << "[";
        for (int i = 0; i < N_eventosOHCA; i++) {
            if (pos == i) cout << endl;
            //cout << "(" << cromosoma[i].id << ", " << cromosoma[i].aed << "), " ; 
            cout << cromosoma[i].aed << ", " ; 
            
        }
        cout << "]" << endl;
    }

    /* getter presupuesto del individuo */
    float get_presupuesto(){        
        return presupuesto_cromosoma;
    }

    /* setter presupuesto del individuo */
    void set_presupuesto(float p){
        presupuesto_cromosoma = p;
    }

    /* recalcular presupuesto del cromosoma */
    void recalculate_presupuesto(){
        //cout << "presupuesto: " << presupuesto << endl;
        presupuesto_cromosoma = presupuesto;

        for (int i = 0; i < N_eventosOHCA; i++){
            
            // notar que el cruzamiento en un punto no cambia de posicion los AEDs originales, por lo que no necesito
            // restarle el costo de moverlo 

            if (cromosoma[i].original_aed == 0) { // no AED original
                if (cromosoma[i].aed == 1 )  presupuesto_cromosoma -= costo_nuevo;
            }
            else{ // AED original
                
                // restar del presupuesto solo 1 vez (si se mueve multiples veces cuesta lo mismo que moverlo directamente al ultimo lugar)
                if (cromosoma[i].x != cromosoma[i].x_original || 
                    cromosoma[i].y != cromosoma[i].y_original){
                    
                    presupuesto_cromosoma-= costo_reubicar;
                }
            }
        }
    }

    /* getter funcion de evaluacion del individuo */
    double get_eval_function(){
        return eval_function;
    }

    /* setter funcion de evaluacion del individuo */
    void set_eval_function(int eval){
        eval_function = eval;
    }

    /* retorna el conteo de AED's en un cromosoma */
    int get_number_of_AED(){
        int AEDs = 0;
        for (int i = 0; i < N_eventosOHCA; i++) {
            if (cromosoma[i].aed == 1) AEDs++;
        }
        return AEDs;
    }

    /* retorna el conteo de AED's originales en un cromosoma */
    int count_originals_AED(){
        int orig = 0;
        for (int i = 0; i < N_eventosOHCA; i++){
            if (cromosoma[i].original_aed == 1) orig++;
        }
        return orig;
    }


    /*Para debug*/
    void stats_cromosoma(){
        cout <<  " Eval: " <<  get_eval_function() << " (" << 100 * get_eval_function()/N_eventosOHCA << "%)"  ;
        if (get_presupuesto() <= 0.001){
            cout << " presupuesto: 0" ;
        }
        else{
            cout << " presupuesto: " <<  get_presupuesto();
        }
        
        cout << " AEDs: " << get_number_of_AED() << " count_originals_AED: " << count_originals_AED();

        if ( get_presupuesto() < 0 || count_originals_AED() != originalsAEDs) cout << " (INFACTIBLE)" << endl;
        else cout << endl;  
    }

    /*Mostar por pantalla mejor solucion*/
    void print_best(){
        cout << "---------------------------------------------" << endl;
        cout << "Número de eventos cubiertos: " << get_eval_function()  << endl;
        cout << "Porcentaje de eventos cubiertos: " <<  100 * get_eval_function()/N_eventosOHCA  << "%" << endl ;
        cout << "Cantidad iteraciones: " << finished_iterations << endl;
        
        // coordinates
        for (int i = 0; i < N_eventosOHCA; i++){
            if (cromosoma[i].aed == 0) continue;

            if (cromosoma[i].original_aed == 1) {

                if (cromosoma[i].x != cromosoma[i].x_original || 
                    cromosoma[i].y != cromosoma[i].y_original) { // AED reposicionado

                    cout << "AED Reposicionado: (" <<  cromosoma[i].x << ", " << cromosoma[i].y << ") " << 
                    " Original coords: (" << cromosoma[i].x_original << ", " << cromosoma[i].y_original << ")" << endl;    
                }

                else{ // AED no reposicionado
                    cout << "AED no Reposicionado: (" <<  cromosoma[i].x << ", " << cromosoma[i].y << ")" << endl; 
                    
                }
            }
            else { // AED nuevo
                cout << "AED Nuevo: (" << cromosoma[i].x << ", " << cromosoma[i].y << ")" << endl;
            }
        }

        if (get_presupuesto() <= 0.001){
            cout << "Presupuesto sobrante: 0" << endl;
        }
        else{
            cout << "Presupuesto sobrante: " <<  get_presupuesto() << endl;
        }

        cout << "---------------------------------------------" << endl;
    }

    

};

// mejor cromosoma local y global
Cromosoma best_cromosoma_global; 
Cromosoma best_cromosoma_local;

// f. de evaluacion del mejor global
int best_global_eval; 

// poblacion de hijos 
Cromosoma* poblacion_child;

// poblacion actual
Cromosoma* poblacion_actual; 
