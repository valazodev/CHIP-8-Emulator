# Emulador CHIP-8

Esta es mi versi�n para C++ de un emulador de la m�quina virtual CHIP-8, desarrollada a mediados de los a�os '70.

## Advertencia

Esto no es m�s que un proyecto "de juguete", hecho principalmente para aprender algunos conceptos b�sicos del funcionamiento interno del hardware, por lo que en ning�n caso est� hecho para ser amigable con el usuario.

### Prerequisitos

Para poder compilar el c�digo fuente es necesario tener una versi�n actual de CMake, y un compilador de C/C++ que permita utilizar C++17.

### Instalaci�n

Para compilar el c�digo s�lo hay que abrir el archivo CMakelists.txt con CMake y dejar que haga su trabajo.

### Abrir un rom

Para poder correr alg�n juego en este emulador, debes editar el archivo src/main.cpp y modificar el valor de la variable "path"

```
// Escribir la ruta del rom entre las comillas
    path = "";
```

Finalmente, se debe recompilar el c�digo para que sea capaz de abrir el rom.

Algunos roms de prueba pueden ser encontrados [aqu�](https://github.com/loktar00/chip8/tree/master/roms).

## Tecnolog�a utilizada

* C++17
* SDL2

## Autores

* **Valent�n Vargas** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

## Recursos utilizados

* [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [loktar00's CHIP-8 ROMs](https://github.com/loktar00/chip8/tree/master/roms)
