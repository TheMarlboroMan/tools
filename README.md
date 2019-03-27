# tools

Common tools for various projects. Most of them depend on libdansdl2 but are developed to fullfill very specifics needs (much too specific to get a part in the main library).

With the purpose of reusability and independence the makefile only generate object files that have to be linked with the project later. Of course, this does not apply to the templates.

As for July 2016 these tools have been translated into english and retooled to some point (some things have been changed, all interfaces have been redone and some utilities have been deleted). As a result of this, all compatibility with the previous versions has been broken. There should still be copies in the branches "master" and "classic" of the "herramientas_proyecto" repository, but these will be developed no more.

# building

Must be built with a compiler that accepts the c++14 standard.

## TODO

- Test all functionality in the test directory.
- Properly document the use of the more complicated tools (like the menu).
