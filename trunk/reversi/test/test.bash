for VAR in $(ls test/saves)
do
echo $VAR
./bin/reversi -c test/saves/$VAR
echo
echo
done
