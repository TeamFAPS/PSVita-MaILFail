rm -rf mailfail-installer/resource
mkdir mailfail-installer/resource
cp mailfail-anchor-lister/build/mailfail/index.html mailfail-installer/resource/index.html
cd mailfail-installer
./make.sh
cd ..