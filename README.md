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
build/creator <nombre_del_buffer> <tamaño_del_buffer>

# Una vez creado el buffer necesitamos crear el productor y consumidor:
# La creación de un productor es de la siguiente manera:
build/producer <nombre_del_buffer> <delay_en_ms>

# La creación de un consumidor es de la siguiente manera:
build/producer <nombre_del_buffer> <delay_en_ms>

#Se tiene otro componente que es el que finaliza el buffer creado:
build/finalizer <nombre_del_buffer>
```

## Descripción de Interfaz Gráfica de Usuario

**Se encuentra dividida en 3 `Secciones`**

- Primera Sección:

  - Compuesta por una cuadrícula que contiene el `Nombre del Buffer`, la `Cantidad de Productores` y la `Cantidad de Consumidores` que se están ejecutando.

- Segunda Sección:

  - En este componente se va a mostrar todo el contenido del `Buffer`

- Tercera Sección:

  - En este componente se va a mostrar todo lo relacionado con la ejecución de los Productores y Consumidores, que está pasando con cada uno de ellos.

![image](https://user-images.githubusercontent.com/37723586/236951166-c94b290b-4d7c-408a-8cb3-f3c10ab36ec0.png)

![image](https://user-images.githubusercontent.com/37723586/236951239-d4f67b99-16ab-46b7-a9f2-94a422fa99eb.png)
