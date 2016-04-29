#!/bin/bash
#file="$1"
#afail=$2
#tfail=$3
#yfail=$4
#pfail=$5

file="circuit.raw"
afail=0.0001
tfail=0.2
yfail=0.0001
pfail=0.2

cd ..
./processraw $file > /dev/null
./convertft "$file.in" 0 > /dev/null

./boxworld "$file.in.geom.io" "$file.in.geom.bounding" $afail $tfail $yfail $pfail
./faildistillations "$file.in.geom.io" "$file.in.geom.io.io" $afail $yfail
./connectpins "$file.in.geom.io.toconnect"

 mv $file.* visual

cd visual

#generate .js files for visualisation
./graph2js.sh "$file.in.geom" > "$file.js"
./graph2js.sh "$file.in.geom" graph > "$file.js_1"
./graph2js.sh "$file.in.geom.io.toconnect.geom" graph2 > "$file.js_2"

