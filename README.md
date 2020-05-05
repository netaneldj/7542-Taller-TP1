Protocolo DBUS
============

**Alumno**:  Netanel David Jamilis           
**Número de Padrón**: 99093            
**Email**: njamilis@fi.uba.ar  
**Github**: netaneldj/tp1


Introducción
============

Objetivos
---------

Los objetivos del presente trabajo son los siguientes:

-   Buenas prácticas en programación de Tipos de Datos Abstractos (TDAs)

-   Modularización de sistemas

-   Correcto uso de recursos (memoria dinámica y archivos)

-   Encapsulación y manejo de Sockets

En las siguientes secciones se verá plasmado el cumplimiento de los
objetivos.

Secciones
---------

El presente trabajo práctico está dividido en diferentes secciones las cuales se pueden
agrupar en dos grandes categorías. La primera es la introducción en
donde se enumeran los objetivos y se informa como se estructurará el
informe. La segunda es el analisis del desarrollo del proyecto. Allí se relatará todo el proceso de desarrollo del proyecto destacando los puntos ms importantes del mismo y los desafios que se debio superar.

Desarrollo del proyecto
==========================

Primeras impresiones
------------------------

Al comienzo se dificulto entender la tarea a realizar. Se estuvo varios dias diseñando la resolucion del proyecto. Esto se refiere a comprender el enunciado y poder concluir que objetos habŕia que desarrollar, que rol cumpliría cada uno, como interactuarian entre si, etc.

Resolución adoptada
------------------------

La solucón que implemente útiliza los siguientes objetos:

-   Vector Dinamico

-   Mensaje DBUS

-   Socket

-   Cliente

-   Servidor

A continuación explicare como utilice cada uno de ellos.

### Vector Dinamico

El vector dinamico lo utilice a modo de lista para almacenar bytes, caracteres. Al momento de desarrollar el Mensaje DBUS me ví en la necesidad de desarrolarlo ya que necesitaba poder almacenar bytes/caracteres sin saber de antemano la cantidad de ellos. Se me ocurrio usar este objeto porque en una materia previa ya habíamos trabajado con el mismo.

### Mensaje DBUS

El Mensaje DBUS es el encargado de tomar las lineas de instrucciones que le manda el cliente y devolverlas en formato DBUS para que el cliente se las pueda enviar al servidor. Luego si del lado del servidor se le ingresa dicha instrucción al mensaje, luego se le pueden solicitar los datos de interes como el id, destino, ruta, interfaz, metodo y argumentos (si los tuviese).

### Socket

El Socket es el encargado de establecer la comunicación entre el cliente y el servidor. Ambos de ellos cuentan con objetos de este tipo entre sus atributos.

### Cliente

El Cliente es el encargado de recibir por consola o por parametro las instrucciones que se deben enviar al servidor, procesarlas y enviarselas de a una en protocolo DBUS. Por cada instrucción la traduce, la envia y espera la respuesta del servidor, antes de continuar con la proxima instrucción. Para realizar la conversión al formato del protocolo utiliza al Mensaje DBUS y para la comunicación Socket.

### Servidor

El Servidor es el encargado de recibir la información del cliente, traducirla y obtener la información de su interes para luego mostarla por pantalla. Luego de recibirla debe notificarle la recepción al cliente. Para realizar la conversión al formato del protocolo utiliza al Mensaje DBUS y para la comunicación Socket.

Dificultades abordadas
------------------------

A lo largo de todo el trabajo me fui topando con una serie de dificultades que de no superarlas no superarlas no hubiese podido progresar con el trabajo. 

Algunas de ellas se deberieron en la falta de conocimiento, como por ejemplo no saber como se manejan los sockets o no saber como construir un protocolo capaz de poder convertir un mensaje en su formato y luego recuperarlo. 

Otras se debieron a dificultades propias del lenguaje como por ejemplo manejar punteros y direcciones de memoria.

Por último, pero no menos importante se debieron enfrentar las problematicas tipicas de un programa de tal complejidad como por ejemplo defasajes de los indices del protocolo, entre otros.
