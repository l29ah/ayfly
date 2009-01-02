#/bin/sh
export EPOCROOT=/home/ander/symbian-sdks/uiq31/
cd ../group_uiq3
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_uiq3.pkg
signsis ayfly_uiq3.sis ayfly-uiq3-0.0.23.sisx gensign.cer gensign.key
rm ayfly_uiq3.sis
