cmake_minimum_required(VERSION 3.16)
project(SistemaPrefeitura LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# As três mágicas: geram moc, uic e rcc automaticamente pra qualquer
# arquivo que precise (Q_OBJECT -> moc, .ui incluído -> uic, .qrc -> rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Ajuda o CMake a achar o Qt6 instalado pelo MSYS2
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# libpqxx não vem com um pacote CMake padrão no MSYS2, então localizamos
# manualmente o header e a lib dentro do prefixo do MinGW
find_path(PQXX_INCLUDE_DIR pqxx/pqxx
    PATHS "C:/msys64/mingw64/include"
)
find_library(PQXX_LIBRARY NAMES pqxx
    PATHS "C:/msys64/mingw64/lib"
)
find_library(PQ_LIBRARY NAMES pq
    PATHS "C:/msys64/mingw64/lib"
)

if(NOT PQXX_INCLUDE_DIR OR NOT PQXX_LIBRARY)
    message(FATAL_ERROR "libpqxx não encontrada. Confira se mingw-w64-x86_64-libpqxx está instalado no MSYS2.")
endif()

# Lista explícita é mais segura que GLOB (CMake não recompila
# automaticamente se você usar GLOB e só adicionar um arquivo novo)
add_executable(sistema_prefeitura
    src/main.cpp
    src/MainWindow.cpp
    src/ViewPacientes.cpp
    src/ViagemRepository.cpp
    src/ViewPacientes.ui
)

target_include_directories(sistema_prefeitura PRIVATE
    src
    ${PQXX_INCLUDE_DIR}
)

target_link_libraries(sistema_prefeitura PRIVATE
    Qt6::Widgets
    ${PQXX_LIBRARY}
    ${PQ_LIBRARY}
)