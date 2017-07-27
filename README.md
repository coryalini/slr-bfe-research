# slr-bfe-research



This repository holds the code written by Corinne Alini for the Rusack Coastal Studies Fellowship with adviors Laura Toma and Eileen Johnson.

This program contains three executables, interp, modelflood, and renderflood.

##Interp:
Interp uses either nearest neighbor, inverse distance weighted, or approximate inverse distance weighted to interpolate either a flood zones grid or tidal grid. Inputs are an elevation grid and a grid to be interpolated.


The interpolation simulator takes the following command-line arguments: 
    -e <e>: Elevation grid flag 
    -i <i>: Grid that will be interpolated(input) 
    -o <o>: Filename you wish to write your grid to(output). If not given, the code will not write to output 
    --norender <a>: Dont render the grid! 
    --idw <c>: Run interpolation with the idw method 
    --approx <d>: Run interpolation with the approximate idw method 

usage: [-c] -e elevname -b orignalname -w file_to_write [--norender] [--approx] [--idw]

The rendering map takes the following commands: 
    q: quit 
    e: Draw Elevgrid 
    s: Draw the sea 
    b: Draw the original grid on top of the elevgrid 
    i: Draw interpolated grid with original on top 


##Modelflood
Flooding simulates different flooding senarios depending on what was given. It can simulate flooding with storm or normal waves by providing either an interpolated flood zone or an interpolated tidal grid. The inputs are an elevation grid and an optional interpolated grid.
The flooding simulator takes the following command-line arguments: 
    -e <e>: Elevation grid flag 
    -i <i>: Tidal grid 
    -r <r>: The desired rise amount 
    -o <o>: Filename you wish to write your grid to 
    --norender <a>: Dont render and just write the flooded grid 

usage: -e elevname [-i interpname] -r rise -o file_to_write [--norender]

The rendering map takes the following commands: 
    q: quit 
    w: write to files 
    e: Draw Elevgrid 
    f: Draw Flooded grid 
    g: Draw Flooded grid with gray land 
    j: Draw just flooded grid (no elev) 
    s: Draw the sea 
    h: Draw just flooded grid displaying water 
    i: Draw interpolated bfe 
    +: increase the sea level rise 
    =: increase the sea level rise 
    -: decrease the sea level rise 

##renderflood
Render flood renders an image of the elevation grid with orignal flood zones grid andflooding on top. 

The flooding simulator takes the following command-line arguments: 
    -e <e>: Elevation grid flag 
    -i <i>: Original bfe grid 
    -f <f>: The flooded grid 
    -o <o>: Filename you wish to write your grid to 

usage: -e elevname -i interpname -f floodedname -o file_to_write 

The rendering map takes the following commands: 
    q: quit 
    w: write to files 
    e: Redraw Elevgrid with the bfe grid with flooded 




