# closedCurves


## Minimal canonical representation of closed curves bounding contiguously classed regions in pixelated space.


The minimal canonical representation will be "closed curves" not "simple closed curves". As "simple closed curves" are non-self-intersecting, "closed curves" may be self-intersecting, an interesting artifact of this minimal canonical representation.


### To make run

`make`

this creates two executables `closed_curves` and `closed_curves_parallel`


### To demo run

`./closed_curves demo`

or

`./closed_curves_parallel demo`


### To use run 

`./closed_curves <PARAMS>`

or

`./closed_curves_parallel <PARAMS>`


where `<PARAMS>` is of the form of the `debug_data` within the `debug_data.h`. AKA an unorderd list of ordered (COLORTYPE COORDINATE) pairs.

Not audited, but WELL used. Still, use at your own risk.

Support my work on this library by donating ETH or other coins to

`0x1331DA733F329F7918e38Bc13148832D146e5adE`
