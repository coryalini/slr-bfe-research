
PATH=~/Library/Developer/Xcode/DerivedData/flood-dqmloywuwiinozbwnqplvucqmekw/Build/Products/Debug

EXECUTABLE_PATH=flood/flood


DEM_FILE=southport_dem_2cellsize.asc
DEM=southport


OUTPUT_PATH=~/Desktop/${DEM}

ORIG_FZ_FILE=southport_floodzones_2cellsize.asc
ORIG_HAT_FILE=southport_hat_2cellsize.asc

#output files
INTERP_FZ_NN=southport_interp_floodzones_nn.asc
INTERP_FZ_IDW=southport_interp_floodzones_idw.asc

INTERP_HAT_NN=southport_interp_hat_nn.asc
INTERP_HAT_IDW=southport_interp_hat_idw.asc


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
$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_HAT_FILE -o $OUTPUT_PATH/$INTERP_HAT_NN --norender
echo
#interp idw

$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_FZ_FILE -o $OUTPUT_PATH/$INTERP_FZ_IDW --norender --approx
echo
$EXECUTABLE_PATH/interp -e $PATH/$DEM_FILE -i $PATH/$ORIG_HAT_FILE -o $OUTPUT_PATH/$INTERP_HAT_IDW --norender --approx
echo

#slr+floodzones NN
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_nn_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_nn_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_NN -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_nn_6ft.asc --norender
echo
#slr+hat NN
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_HAT_NN -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_hat_nn_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_HAT_NN -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_hat_nn_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_HAT_NN -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_hat_nn_6ft.asc --norender
echo

#slr+floodzones IDW
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDW -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_idw_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDW -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_idw_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_FZ_IDW -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_bfe_idw_6ft.asc --norender
echo
#slr+hat IDW
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_HAT_IDW -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_hat_idw_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_HAT_IDW -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_hat_idw_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -i $OUTPUT_PATH/$INTERP_HAT_IDW -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_hat_idw_6ft.asc --norender

echo "End"















