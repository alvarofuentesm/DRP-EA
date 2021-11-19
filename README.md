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

# Represetación

ón apropiada para este problema es una adaptación de la descrita por Jaramillo et al. \cite{Jaramillo2002} para algoritmos genéticos en problemas de ubicación Maximum Covering. Proponen el uso de un string binario como cromosoma de largo equivalente a los potenciales sitios de instalaciones, en donde un valor 1 en el i-ésimo bit implica una instalación en el i-ésimo sitio.


Se extiende esta representación para aplicarla en el problema DRP utilizando un arreglo de largo equivalente a la cantidad de eventos OHCA de la instancia tal que cada posición del arreglo corresponda a un único evento OHCA en el mapa, luego se almacena en cada posición del arreglo un bit con valor 1 si se posiciona un AED sobre el evento OHCA, y un valor 0 en caso contrario. Al trabajar la variante flexible location es útil almacenar en cada posición del arreglo un bit para indicar si el AED corresponde a uno nuevo o a uno original de la instancia, almacenando adicionalmente las coordenadas originales del AED en caso de reubicación. Este arreglo, denominado de aquí en adelante como cromosoma, permite diferenciar los dos tipos de AED's, mantener rastreo de los AED's originales de la instancia y calcular el costo de la combinación de AED's en el cromosoma.

A continuación se presenta un ejemplo de cromosoma para una instancia de 5 eventos OHCA con un nuevo AED en las coordenadas $(x_2, y_2)$ y un AED original reubicado en las coordenadas $(x_4, y_4)$ que originalmente se ubicada en las coorenadas $(x_1, y_1)$. En la parte superior se especifican las coordenadas correspondiente al evento OHCA, en el medio el bit que indica la presencia de un AED sobre el evento, y en la parte inferior las coordenadas originales para el AED reubicado.       

\hfill 
\begin{center}
\begin{tabular}{|c|c|c|c|c|c|}

    \multicolumn{1}{c}{$(x_1, y_1)$} & \multicolumn{1}{c}{$(x_2, y_2)$} & \multicolumn{1}{c}{$(x_3, y_3)$} & \multicolumn{1}{c}{($x_4, y_4$)} & \multicolumn{1}{c}{($x_5, y_5$)} \\ \hline
    0 & 1 & 0 & 1 & 0 \\ \hline
    \multicolumn{1}{c}{} & \multicolumn{1}{c}{} & \multicolumn{1}{c}{} & \multicolumn{1}{c}{$(x_1, y_1)$} & \multicolumn{1}{c}{} 
\end{tabular}
\end{center}
\hfill 


