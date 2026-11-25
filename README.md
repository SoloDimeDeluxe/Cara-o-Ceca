# Cara o Ceca

Juego de combate por turnos con clash de monedas, hecho en C++ con SFML 3.1 para el TP Bonus de SFML de Desarrollo de Videojuegos I (Image Campus, 2026).

Yi Sang pelea contra el jefe Lei Heng. Cada turno Lei Heng hace hasta 6 ataques, uno por columna, y vos tenés hasta 3 acciones para frenarlos. Cuando dos skills chocan se tiran las monedas: cada cara suma poder, el que saca más le rompe una moneda al otro, y así hasta que uno se queda sin monedas. El que gana pega con las que le quedan. La pelea se ve en una arena con los dos personajes, ronda por ronda.

## Modos de juego

- **Jugar**: el modo Normal, con ayudas para aprender: Yi Sang arranca con +45 de cordura, se cura 15 HP por cada clash ganado y recibe la mitad de Burn.
- **Jugar en Difícil**: para cuando ya sabés las mecánicas. Sin esas ayudas: Yi Sang arranca con 0 de cordura, se cura solo 5 HP por clash ganado y recibe el Burn completo.

En los dos modos Lei Heng arranca al 30% de su fuerza y se hace más fuerte con cada turno.

Al terminar la partida (ganes o pierdas) se muestran las estadísticas: turnos jugados, daño hecho y recibido, clashes ganados y perdidos, golpes esquivados, prescriptos cumplidos y veces que usaste Furioso-Replica.

## Cómo se juega

- Cada acción de Yi Sang trae **2 skills sorteadas** (S1 sale seguido, S2 a veces y S3 poco). **Esquivar** siempre está disponible.
- **Arrastrá** una skill hasta la columna del ataque que querés frenar (o hacé clic en una columna y después en la skill). Clic derecho la saca.
- La skill que usás se gasta y el turno siguiente sale otra en su lugar. Esquivar gasta la primera opción de esa acción.
- Si Lei Heng tiene un solo ataque ese turno, tenés una sola acción.
- Entre las dos filas se ve tu **chance de ganar** cada clash: buscá los verdes.
- Si una columna queda sin acción, ese ataque te pega directo.
- Ganar un clash da **+10 de cordura** y Yi Sang se cura (**15 HP** en Normal, **5 HP** en Difícil). Perderlo saca 3 o 4 de cordura.
- **Esquivar** no es un clash: tirás una vez y los golpes que no superen tu número no te hacen daño. Si esquivás todo, la esquiva sigue activa y cubre los próximos ataques directos hasta que un golpe la supere o termine el turno.
- Las monedas **irrompibles** de Lei Heng pegan aunque él pierda el clash, pero con la mitad de daño.
- **Prescripto**: cada turno se marcan hasta 2 skills de tu mano. Usarlas pega +15% y suma una carga; con 9 cargas aparece **Furioso-Replica**, un ataque de 9 monedas irrompibles.

## Estados

- **Poise** (Yi Sang): chance de crítico y más poder por moneda.
- **Sinking** (Lei Heng): pierde cordura cada vez que lo golpean.
- **Cordura (SP)**: cambia la chance de sacar cara.
- **Stagger**: las marcas en las barras de vida. Si la vida baja de una, esa unidad pierde el turno y recibe el doble de daño.
- **Tremor** y **Tremor Burst** (Yi Sang): acercan su próximo umbral de Stagger.
- **Tremor - Scorch**: si Lei Heng no pierde el clash con Tajo o Cortador, cada Burst además hace daño.
- **Burn** (Yi Sang): daño al final de cada turno.

En el juego, el botón **Cómo se juega** tiene todas las reglas, separadas en páginas.

## Controles

| Tecla / acción | Qué hace |
| --- | --- |
| Arrastrar con clic izquierdo | Pone una skill en una columna |
| Clic derecho | Saca la skill de una columna |
| Espacio o Enter | ¡Combatir! (o Saltear mientras se ve la pelea) |
| Esc | Pausa (o cierra el cartel abierto) |
| M | Silencia o vuelve a prender la música |
| ← → | Cambia de página en Cómo se juega |

## Cómo compilar

1. Abrir `CaraOCeca.slnx` con Visual Studio 2026.
2. Elegir **Release | x64** y compilar (Debug anda, pero mucho más lento).
3. El ejecutable queda en `bin/x64/Release/`, con las DLL de SFML y la carpeta `res` copiadas al lado.

## Créditos

- Programación y diseño: Valentin Reyes Cuadra.
- Sprites de Yi Sang y Lei Heng: Valentin Reyes Cuadra, basados en los diseños originales de Project Moon.
- Música: tema de batalla del Canto VIII (jefe 2) de Limbus Company, de Project Moon.
- Biblioteca: [SFML](https://www.sfml-dev.org/) 3.1 (licencia zlib, ver `lib/sfml/license.md`).
- Fuente: Crimson Text (SIL Open Font License, ver `res/fonts/OFL.txt`).
- Fan game sin fines de lucro, inspirado en el sistema de combate de Limbus Company. Yi Sang, Lei Heng, sus skills y la música pertenecen a Project Moon.

<img width="606" height="268" alt="IV5PBY" src="https://github.com/user-attachments/assets/25b0b1c7-59d7-41e0-8fd5-7fde7b764d14" />

