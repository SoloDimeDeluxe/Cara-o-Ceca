# Cara o Ceca

Juego de combate por turnos con clash de monedas, hecho en C++ con SFML 3.1 para el TP Bonus de SFML de Desarrollo de Videojuegos I (Image Campus, 2026).

Yi Sang pelea contra el jefe Lei Heng. Cada turno Lei Heng hace hasta 6 ataques, uno por columna, y vos tenés 3 acciones para frenarlos. Cuando dos skills chocan se tiran las monedas: cada cara suma poder, el que saca más le rompe una moneda al otro, y así hasta que uno se queda sin monedas. El que gana pega con las que le quedan.

## Cómo se juega

- Cada acción de Yi Sang trae **2 skills sorteadas** (S1 sale seguido, S2 a veces y S3 poco). **Esquivar** siempre está disponible.
- Hacé clic en una columna de Yi Sang (fila de abajo) y después en una skill de alguna acción para ponerla ahí. Clic derecho la saca.
- La skill que usás se gasta y el turno siguiente sale otra en su lugar. Esquivar gasta la primera opción de esa acción.
- Entre las dos filas se ve tu **chance de ganar** cada clash: buscá los verdes.
- Si una columna queda sin acción, ese ataque te pega directo.
- **Esquivar** no es un clash: tirás una vez y los golpes que no superen tu número no te hacen daño. Si esquivás todo, la esquiva sigue activa y cubre los próximos ataques directos hasta que un golpe la supere o termine el turno.
- Las monedas **irrompibles** de Lei Heng pegan aunque él pierda el clash.
- Apretá **¡Combatir!** para resolver el turno y mirá el clash ronda por ronda (o **Saltear**).
- **Esc** pausa la partida. Pasá el mouse por cualquier skill para ver todos sus efectos.

## Estados

- **Poise** (Yi Sang): chance de crítico y más poder por moneda.
- **Sinking** (Lei Heng): pierde cordura cada vez que lo golpean.
- **Cordura (SP)**: cambia la chance de sacar cara.
- **Stagger**: las marcas en las barras de vida. Si la vida baja de una, esa unidad pierde el turno y recibe el doble de daño.
- **Tremor** y **Tremor Burst** (Yi Sang): acercan su próximo umbral de Stagger.
- **Tremor - Scorch**: si Lei Heng no pierde el clash con Tajo o Cortador, cada Burst además hace daño.
- **Burn** (Yi Sang): daño al final de cada turno.

En el juego, el botón **Cómo se juega** tiene todas las reglas.

## Cómo compilar

1. Abrir `CaraOCeca.slnx` con Visual Studio 2026.
2. Elegir **Release | x64** y compilar (Debug anda, pero mucho más lento).
3. El ejecutable queda en `bin/x64/Release/`, con las DLL de SFML y la carpeta `res` copiadas al lado.

## Créditos

- Programación y diseño: Valentin Reyes Cuadra.
- Biblioteca: [SFML](https://www.sfml-dev.org/) 3.1 (licencia zlib, ver `lib/sfml/license.md`).
- Fuente: Crimson Text (SIL Open Font License, ver `res/fonts/OFL.txt`).
- Fan game sin fines de lucro, inspirado en el sistema de combate de Limbus Company (Project Moon). Yi Sang, Lei Heng y sus skills pertenecen a Project Moon. No usa assets del juego original.
