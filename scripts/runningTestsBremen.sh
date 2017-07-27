
#PATH=~/Library/Developer/Xcode/DerivedData/flood-dqmloywuwiinozbwnqplvucqmekw/Build/Products/Debug
PATH=~/Desktop/files/${DEM}
EXECUTABLE_PATH=flood/flood


DEM_FILE=bremen_dem.asc
DEM=bremen
PATH=~/Desktop/files/${DEM}


OUTPUT_PATH=~/Desktop/${DEM}

ORIG_FZ_FILE=bremen_floodzones.asc

#output files
INTERP_FZ_NN=bremen_interp_floodzones_nn.asc
INTERP_FZ_IDW=bremen_interp_floodzones_idw.asc



echo "Start"
#slr

$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_6ft.asc --norender
echo


#interp NN

$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_FZ_FILE -o $OUTPUT_PATH/$INTERP_FZ_NN --norender
echo

#interp idw

$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_FZ_FILE -o $OUTPUT_PATH/$INTERP_FZ_IDW --norender --approx
echo


#slr+floodzones NN
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_nn_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_nn_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_nn_6ft.asc --norender
echo

#slr+floodzones IDW
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDW -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_idw_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDW -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_idw_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDW -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_fz_idw_6ft.asc --norender

echo "End"















