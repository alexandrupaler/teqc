#!/bin/bash
#file="$1"
#afail=$2
#tfail=$3
#yfail=$4
#pfail=$5

file="circuit.raw"
afail=0.5
tfail=0.001
yfail=0.5
pfail=0.001
schedulingType=2

cd ../bin
#./processraw $file > /dev/null
#valgrind --leak-check=full --show-leak-kinds=all \
./convertft "$file.in" xxx $afail $tfail $yfail $pfail $schedulingType 
# > /dev/null


echo "converted" 

#./boxworld "$file.in.geom.io" "$file.in.geom.bounding" $afail $tfail $yfail $pfail $schedulingType

echo "scheduled"

#./connectpins "$file.in.geom.io.toconnect"

cp $file.in.* ../visual

cd ../visual

#generate .js files for visualisation
#./graph2js.sh "$file.in.geom" > "$file.js"
./graph2js.sh "$file.in.geom" graph > "$file.js_1"
./graph2js.sh "$file.in.geom.io.toconnect.geom" graph2 > "$file.js_2"
./graph2js.sh "$file.in.geom.io.toconnect.geomdebug1" graph3 > "$file.js_3"
./graph2js.sh "$file.in.geom.io.toconnect.geomdebug2" graph4 > "$file.js_4"

