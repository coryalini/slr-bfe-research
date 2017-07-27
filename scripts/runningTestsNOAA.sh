
#PATH=~/Library/Developer/Xcode/DerivedData/flood-dqmloywuwiinozbwnqplvucqmekw/Build/Products/Debug
PATH=~/Desktop/files/${DEM}
EXECUTABLE_PATH=flood/flood


DEM_FILE=noaa_coast_southport_10m.asc
DEM=noaa_southport
PATH=~/Desktop/files/${DEM}


OUTPUT_PATH=~/Desktop/${DEM}



echo "Start"
#slr

$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 1 -o $OUTPUT_PATH/${DEM}_flooded_slr_1ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 3 -o $OUTPUT_PATH/${DEM}_flooded_slr_3ft.asc --norender
echo
$EXECUTABLE_PATH/modelflood -e $PATH/$DEM_FILE -r 6 -o $OUTPUT_PATH/${DEM}_flooded_slr_6ft.asc --norender
echo


echo "End"










