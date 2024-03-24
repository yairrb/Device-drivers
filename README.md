# Device-drivers

Este repostorio consta de dos modulo con funcionalidades distintas siendo el mas avanzado el *Char Device*

## Hola Mundo

Primer acercamiento con los modulos. El objetivos es cargar y probar un modulo simple. Consta de algunos printK para chequear su correcto funcionamiento.

## Char Device

Consta de la implementacion de un dispositivo de caracteres. Este debe recibir lo que le escribamos e imprimirlo en el kernel. Ademas, al leerlo mostrara el ultimo mensaje al reves.

### Set up

Para compilar los modulos necesitamos ejecutar el comando *make* dentro de carpeta deseada. Esto generara el archivo que vamos a instalar a continuacion con el comando *insmod*:
```
  make
  sudo insmod miModulo.ko
```

Para validar la instalacion se puede ejecutar el siguiente comando que mostrar toda la informacion necesaria:

```
  dmesg | tail
```

Para crear un archivo:

```
  sudo mknod /dev/YRUIZ
  sudo chmod 666 /dev/YRUIZ
```
*nb: YRUIZ es el el nombre del char device*

Para escribir:
```
  echo "Hola, mundo!" > /dev/YRUIZ

```

Para leer:

```
  cat /dev/YRUIZ

```
