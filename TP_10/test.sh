echo "####################################"
echo "####################################"
echo "####################################"

echo "Debut du test de SWAP"

make clean
make

./mmu_manager | ./oracle