file=$1
nrinj=`head -1 $file | tail -1`
edges=`head -2 $file | tail -1`
nodes=`head -3 $file | tail -1`

echo -n "var $2={\"inj\":["
inj=`head -4 $file | tail -1`
echo -n $inj
echo -n "],\"nodes\":["

sed -n "s/^/[/; s/$/]/; $((4 + edges + 1)),$((4+edges+nodes-1))p" $1| tr '\n' ','
sed -n "s/^/[/; s/$/]/; $((4 + edges + nodes)),$((4+edges+nodes))p" $1| tr -d '\n'

echo -n "],\"edges\":["

if [ $edges -ne 0 ]; then
  sed -n "s/^/[/; s/$/]/; $((4 + 1)),$((4 + edges-1))p" $1| tr '\n' ','
  sed -n "s/^/[/; s/$/]/; $((4 + edges)),$((4 + edges))p" $1| tr -d '\n'
fi
echo -n "],\"names\":["

if [ $nrinj -ne 0 ]; then
    sed -n "s/^/[/; s/$/]/; $((4 + edges + nodes + 1)),$((4+edges+nodes+nrinj-1))p" $1| tr '\n' ','
    sed -n "s/^/[/; s/$/]/; $((4 + edges + nodes + nrinj)),$((4+edges+nodes+nrinj))p" $1| tr -d '\n'
fi

echo -n "]}"

