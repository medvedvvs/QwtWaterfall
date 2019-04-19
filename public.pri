UI_DIR = ./
RCC_DIR = ./
MOC_DIR = ./

 INCLUDEPATH +=./

macx{

    OBJECTS_DIR = ./
}

win32{
    OBJECTS_DIR = ./

    CONFIG -= flat
}
