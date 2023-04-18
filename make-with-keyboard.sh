cd mailfail-keyboard
./make.sh
cd ..
rm -rf mailfail-installer/resource
mkdir mailfail-installer/resource
cp mailfail-keyboard/build/mailfail/index.html mailfail-installer/resource/index.html
cd mailfail-installer
./make.sh
cd ..