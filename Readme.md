# Emulador CHIP-8

Esta es mi versión para C++ de un emulador de la máquina virtual CHIP-8, desarrollada a mediados de los años '70.

## Advertencia

Esto no es más que un proyecto "de juguete", hecho principalmente para aprender algunos conceptos básicos del funcionamiento interno del hardware, por lo que en ningún caso está hecho para ser amigable con el usuario.

### Prerequisitos

Para poder compilar el código fuente es necesario tener una versión actual de CMake, y un compilador de C/C++ que permita utilizar C++17.

### Instalación

Para compilar el código sólo hay que abrir el archivo CMakelists.txt con CMake y dejar que haga su trabajo.

### Abrir un rom

Para poder correr algún juego en este emulador, debes editar el archivo src/main.cpp y modificar el valor de la variable "path"

```
// Escribir la ruta del rom entre las comillas
    path = "";
```

Finalmente, se debe recompilar el código para que sea capaz de abrir el rom.

Algunos roms de prueba pueden ser encontrados [aquí](https://github.com/loktar00/chip8/tree/master/roms).

## Tecnología utilizada

* C++17
* SDL2

## Autores

* **Valentín Vargas** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

## Recursos utilizados

* [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [loktar00's CHIP-8 ROMs](https://github.com/loktar00/chip8/tree/master/roms)
