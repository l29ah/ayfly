#/bin/sh
export EPOCROOT=/home/ander/symbian-sdks/s60_28/
cd ../group_s60_2nd
bldmake bldfiles
abld build armi urel
cd ../sis
makesis ayfly_s60_2nd.pkg
mv ayfly_s60_2nd.sis ayfly-s60-2nd-0.0.22.sis
