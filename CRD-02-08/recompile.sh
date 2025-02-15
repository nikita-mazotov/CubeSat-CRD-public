# run from folder above build with `sh recompile.sh`
cd build &&
rm -r * &&
cmake .. &&
make -j 16 exampleB1 &&
echo "Compiled!" &&
./exampleB1