# herramientas_proyecto

Herramientas comunes para varios proyectos. Casi todas ellas dependen en cierta medida de la libdansdl2 pero tienen funcionalidades muy específicas como para incluirlas dentro de la librería. 

La idea es que el código sea reutilizable en varios proyectos con tanta independencia como sea posible. Con esta finalidad, el makefile de estas herramientas genera tan sólo un puñado de object files que hay que linkar posteriormente con el proyecto de turno (en el que deberíamos incluir los headers, por supuesto).,

Hay cosas como listados verticales y en rejilla, un parser de "casi JSON", una matriz 2d basada en std::map, tablas de animaciones y sprites, un compositor de vistas basado en ficheros... Estas herramientas están en constante expansión :).
