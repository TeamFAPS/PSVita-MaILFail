INDEX_HTML_PATH=index.html
rm -rf build
mkdir build
mkdir build/mailfail
touch build/mailfail/$INDEX_HTML_PATH
touch build/$INDEX_HTML_PATH
cat base/head.html >> build/$INDEX_HTML_PATH
cat base/DOM_updater.js >> build/$INDEX_HTML_PATH
cat base/magictouch.js >> build/$INDEX_HTML_PATH
cat base/keyboard.js >> build/$INDEX_HTML_PATH
printf '</script></body></html>' >> build/$INDEX_HTML_PATH
cat build/$INDEX_HTML_PATH >> build/mailfail/$INDEX_HTML_PATH
rm -rf build/$INDEX_HTML_PATH