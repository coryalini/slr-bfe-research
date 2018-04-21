STARTPATH=/home/calini/slr-bfe-research
EXECUTABLE_PATH=flood/flood

DEM=southport
DEM_FILE=${DEM}_dem_2cellsize.asc
PATH=/home/calini/Data


OUTPUT_PATH=/home/calini/Data/${DEM}

ORIG_FZ_FILE=${DEM}_floodzones_2cellsize.asc
#output files
INTERP_FZ_NN=${DEM}_interp_floodzones_nn.asc
INTERP_FZ_IDWA=${DEM}_interp_floodzones_idwapprox.asc
INTERP_FZ_IDW=${DEM}_interp_floodzones_idw.asc

echo "Start"
#slr
echo "slr 1ft"
$STARTPATH/$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_1ft.asc --norender
echo

#interp NN
echo "interp nn"
$STARTPATH/$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_FZ_FILE -o $OUTPUT_PATH/$INTERP_FZ_NN --norender
echo
#approx interp idw
echo "interp idw approx"
$STARTPATH/$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_FZ_FILE -o $OUTPUT_PATH/$INTERP_FZ_IDW --norender --approx

echo "interp idw"
$STARTPATH/$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_FZ_FILE -o $OUTPUT_PATH/$INTERP_FZ_IDW --norender
#slr+floodzones NN
echo "slr + NN"
$STARTPATH/$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_nn_1ft.asc --norender
echo

echo "slr+idwapprox"
$STARTPATH/$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDWA -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_idwapprox_1ft.asc --norender
echo "End"
