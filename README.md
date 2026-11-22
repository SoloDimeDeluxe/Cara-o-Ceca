# Cara o Ceca

Juego de combate por turnos con clash de monedas, hecho en C++ con SFML 3.1 para el TP Bonus de SFML de Desarrollo de Videojuegos I (Image Campus, 2026).

Yi Sang pelea contra Lei Heng. Cada turno tenés 6 slots: elegís una skill para cada uno y se enfrentan con la skill de Lei Heng que está arriba. Si las dos skills chocan, tiran monedas hasta que a una se le acaban; la que gana pega con las monedas que le quedan.

## Cómo se juega

- Hacé clic en un slot de Yi Sang (fila de abajo) para seleccionarlo.
- Hacé clic en una skill de la lista para asignársela al slot seleccionado (pasa solo al siguiente slot).
- Apretá **¡Combatir!** para resolver el turno.
- Las skills de defensa (esquivar) no pegan, pero si ganan el clash evitan el daño.
- Las monedas irrompibles de Lei Heng pegan aunque él pierda el clash.

## Cómo compilar

1. Abrir `CaraOCeca.slnx` con Visual Studio 2026.
2. Elegir Debug o Release (x64) y compilar.
3. El ejecutable queda en `bin/x64/<Configuración>/`, con las DLL de SFML y la carpeta `res` copiadas al lado.

## Créditos

- Programación: Valentin Reyes Cuadra.
- Biblioteca: [SFML](https://www.sfml-dev.org/) 3.1 (licencia zlib, ver `lib/sfml/license.md`).
- Fuente: Crimson Text (SIL Open Font License, ver `res/fonts/OFL.txt`).
- Fan game sin fines de lucro, inspirado en el sistema de combate de Limbus Company (Project Moon). No usa assets del juego original.
