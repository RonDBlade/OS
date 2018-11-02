rm -rf ${HW1DIR}
mkdir ${HW1DIR}
chmod 755 ${HW1DIR}
cd ${HW1DIR}
cp ${1} ${HW1TF}
chmod 711 ${HW1TF}
cd ..
./hw1_subs ${2} ${3}
