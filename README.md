# tqec geometry generator

Software for generating a geometric description corresponding to 
a TQEC circuit.


Please reference http://www.nature.com/articles/srep30600 (http://arxiv.org/abs/1604.08621) if you use the software.

To use it:

1) make all

2) make cpbin (copies some files into bin)

3) have a look in the visual folder (visual/localscript.sh)

The quantum circuit file is bin/circuit.raw.in
It is possible to generate adders and other circuits directly in the code (see code starting at [here](https://github.com/alexandrupaler/tqec/blob/180fcfddf377c2543d231aa6cd6c92cfe0ba6edd/oldconvertft.cpp#L440)).
