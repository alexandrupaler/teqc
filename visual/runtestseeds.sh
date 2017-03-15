# The seeds.txt next to executable contains srand48 seeds which resulted
# into failed synthesis results. This script takes one a time and checks if
# the ...

while IFS='' read -r line || [[ -n "$line" ]]; do
    echo "$line" > ../bin/seeds.txt
    ./localscript.sh > tmpout

    frompool=`grep "FROM POOL" tmpout | wc -l`
    topool=`grep "TO POOL" tmpout | wc -l`

    # Error handling
    handle=0
    err="ok"

    if [ $frompool -eq '1' ]; then
        handle=1
    fi

    if [ $topool -eq '1' ]; then
        handle=1
    fi

    if [ $handle -eq '1' ]; then
      #break
      err="failed"
    fi
    
    echo "$i $line $err"

done < "badseeds.txt"

# make everything default. no testing
#echo "0" > ../bin/seeds.txt
