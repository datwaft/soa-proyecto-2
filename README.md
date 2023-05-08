# `[SOA]` - Proyecto 2: Memoria Compartida: "Productor - Consumidor"

Proyecto creado por:

- Abreu Cárdenas, Miguel Guillermo
- Guevara Sánchez, David Alberto
- Ortíz Ruiz, Victor Adolfo
- Yip Chen, Michael

## Cómo ejecutar este proyecto

```sh
# Primero se necesita o clonar el repositorio o extraer el tarball. e.g.
tar -xvf Abreu-Guevara-Ortiz-Yip.tgz

# Luego es necesario entrar al directorio extraído. e.g.
cd Abreu-Guevara-Ortiz-Yip

# Luego de eso se necesita compilar el proyecto.
# Si clang no está instalado o se desea usar otro compilador ejecutar como: make CC=gcc
make

# Antes de comenzar a producir y/o consumir (Productor - Consumidor) se necesita necesita crear el buffer donde se van a publicar los diferentes elementos del productor.
build/creator <nombre_buffer> <tamaño_buffer>

# Una vez creado el buffer necesitamos crear el productor y consumidor:
#- La creación de un productor queda de la siguiente manera:
build/producer <nombre_productor> <delay en ms>

#- La creación de un consumidor queda de la siguiente manera:
build/producer <nombre_consumidor> <delay en ms>

#Se tiene otro componente que es el que finaliza el buffer creado:
build/finalizer <nombre_buffer>  
```

## Descripción de Interfaz Gráfica de Usuario

**Se encuentra dividida en 3 `Secciones`**

- Primera Sección:
  
  - Compuesta por una cuadrícula que contiene el `Nombre del Buffer`, la `Cantidad de Productores` y la `Cantidad de Consumidores` que se están ejecutando.

- Segunda Sección:

  - En este componente se va a mostrar todo el contenido del `Buffer`

- Tercera Sección:
  
  - En este componente se va a mostrar todo lo relacionado con la ejecución de los Productores y Consumidores, que está pasando con cada uno de ellos.

### Ejecución
