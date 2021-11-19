# DRP-EA: Defibrillator Relocation Problem (DRP) con Algoritmo Evolutivo.


TO-DO: translate to english. 

En el escenario de emergencias de paros cardiacos fuera del hospital (OHCA), en donde un testigo de la emergencia debe ir a buscar un Desfibrilador Externo Automático (AED),
es crítica la cobertura de los AEDs instalados en una región específica debido a que la probabilidad de supervivencia de la víctima disminuye a medida que aumenta la distancia que
recorre el testigo. 

Los estudios concluyen que los métodos de optimización de cobertura pueden identificar áreas para la asignación de recursos de emergencias como AED considerando
una mayor cobertura de eventos OHCA a un menor costo financiero en escenarios donde se pueden comprar nuevos AEDs y/o reubicar los ya existentes. 

El propósito de este trabajo es un exponer un trabajo investigativo acerca de este problema y una implementación para encontrar soluciones utilizando un Algoritmo Evolutivo.


### Instrucciones de ejecución:

Para compilar ejecutar:

```
make
```

Para correr el algoritmo sobre una instancia particular descomentar el parámetro config correspondiente a la instancia en el archivo Makefile. Por defecto esta descomentada la instancia SJC-DRP-AEDs/SJC324-3. Posteriormente ejecutar:

```
make run
```

Para limpiar los archivos de compilación ejecutar:

```
make clean
```

###  Parámetros necesarios para ejecutar el algoritmo

config: ruta de la instancia

semilla: semilla para números aleatorios

debug: flag para activar output debug. 1 para activar, 0 para desactivar. 

iteration: número de iteraciones del algoritmo.

costo_nuevo: costo de un nuevo AED.

costo_reubicar: costo de reubicar un AED.

population_size: tamaño de población

child_size: tamaño máximo de población de hijos.

prob_mutation: probabilidad de aplicar operador de mutación sobre un AED nuevo (alpha n). El valor esta dado por  prob_mutation / número de eventos OHCA. 


prob_mutation_original_AED: probabilidad de aplicar operador de mutación (reubicar) sobre un AED original (alpha o). El valor esta dado por prob_mutation_original_AED 100

### Ejemplos de comandos de ejecución:

Ejecutar la instancia SJC-DRP-AEDs/SJC324-3 usando MakeFile:

```
make run
```

Ejecutar la instancia SJC-DRP-AEDs/SJC324-3  sin MakeFile:

```
/usr/bin/time -p ./DRP+AE Instancias/SJC-DRP-AEDs/SJC324-3.txt 123 0 5000 1 0.2 200 50 1 10
```

### Datos adicionales output:

Para faciliar la lectura de resultados, el output incluye:

Instancia: ruta de la instancia

N_eventosOHCA: número de eventos OHCA de la instancia

presupuesto: presupuesto de la instancia

radio: radio de cobertura

costo nuevo: costo de colocar un AED nuevo

costo reubicar: costo de reubicar AED

Number of Original AED's: número de AED's originales

### Observaciones:

Por motivos desconocidos la instancia SJC-DRP-AEDs/SJC500-7 tiene problemas para hacer free() sobre la poblacion_actual, por lo que no se realiza esta liberación. 


