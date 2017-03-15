for i in `seq 1 10000`;
do
    #force the seed
    #echo $i > ../bin/seeds.txt

    #duration=`/usr/bin/time -f "%E" 
    ./localscript.sh > tmpout

    seed=`grep "SIMULATION SEED" tmpout | cut -d " " -f3`

    frompool=`grep "FROM POOL" tmpout | wc -l`
    topool=`grep "TO POOL" tmpout | wc -l`
    limitreached=`grep "NO CONN AVAILABLE" tmpout | wc -l`

    # Error handling
    handle=0
    err="ok"

    if [ $frompool -eq '1' ]; then
        err="failed"
        handle=1
    fi

    if [ $topool -eq '1' ]; then
        err="failed"
        handle=1
    fi

    if [ $limitreached -eq '1' ]; then
        err="limit"
        handle=1
    fi

    if [ $handle -eq '1' ]; then
      # make a folder for the bad seed
      folder="badseeds/$err$seed"

      mkdir -p $folder
      cp circuit.raw.js* $folder
      cp circuit.raw.in.geom.io.box.0 $folder
      cp tmpout $folder
    fi
    
    echo "$i $seed $err"
done
