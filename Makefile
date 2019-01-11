# Quais objetos têm que ser compilados
OBJS = main.cpp biblioteca.cpp

# Qual compilador usar
CC = g++

# Flags para o compilador
COMPILER_FLAGS = -Wall

# aqui eu recebo o uname para depois usar sabendo se é macOS ou linux. Windows não trabalho no momento
os = $(shell uname -s)

ifeq ($(os), Darwin)
	
	# Flags de linkagem de biblioteca
	LINKER_FLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

	# Caminhos de include que precisam
	INCLUDE_PATHS = -I/usr/local/include -I/usr/local/include/opencv
	# Caminhos de bibliotecas que precisam
	LIBRARY_PATHS = -L/usr/local/lib
else
	# Flags de linkagem de biblioteca
	LINKER_FLAGS = $(shell pkg-config --libs --cflags opencv)

	@echo Nao totalmente configurado. Testar!

endif

# Nome da saída
OUTPUT_NAME = saida

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUTPUT_NAME)

	@echo Done.
