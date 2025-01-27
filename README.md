# ka_tilecut

`ka_tilecut` is a package for cutting map data into square tiles.

All algorithms are oriented towards fast and robust processing of geometries.
To preserve the simplicity and speed of the algorithms, the package requires imposing constraints on the source data.
It is expected that these constraints will be natural for most users.

The source code is divided into several semantic components:
* [`exact`](src/exact/README.md) - floating-point algorithms;
* [`generate_grid`](src/generate_grid/README.md) - generation of specific constants for `exact` component;
* [`geometry_types`](src/geometry_types/README.md) - Vec2, Segmen2, etc;
* [`tilecut`](src/tilecut/README.md) - algorithms for cutting geometries into tiles.

The code is licensed under the [MIT license](LICENSE).
However, tests and utilities may have some LGPL dependencies.
