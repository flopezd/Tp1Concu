cmake_minimum_required(VERSION 3.6)
project(tp1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/Pilon.cpp src/Pilon.h src/Pipe.cpp src/Pipe.h src/Mesa.cpp src/Mesa.h src/MemoriaCompartida.h src/Jugador.cpp src/Jugador.h src/Mensaje.h src/Semaforo.cpp src/Semaforo.h src/LockFile.cpp src/LockFile.h src/Arbitro.cpp src/Arbitro.h)
add_executable(tp1 ${SOURCE_FILES})