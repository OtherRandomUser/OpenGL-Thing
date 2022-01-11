# Pré-Requisitos

* Conan 1.30.0 ou superior (https://docs.conan.io/en/latest/installation.html#install-with-pip-recommended)
* CMake 3.10.2 ou superior (https://cmake.org/download/)

# Instruções de Instalação

```bash
# primeiro crie uma pasta para a compilação
$ mkdir build && cd build

# instale os pacote através do Conan
$ conan install ..

# configure o projeto através do CMake
$ cmake ..

# compile o projeto
$ cmake --build . --parallel

# vá para a pasta contendo o executável
$ cd bin

# e por fim execute o projeto
$ ./exe
```

# Utilizando o VS Code como ide

Para utilizar o Visual Studio code como ide, baixe as extenções `ms-vscode.cpptools` e `ms-vscode.cmake-tools`. altere o arquivo `.vscode/settings.json` como preferir, lembrando que o compilador usado deve ser o mesmo usado de referência na instalação dos pacotes via Conan (se nenhum perfil for criado, ele usará o compilador padrão do sistema)
