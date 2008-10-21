#/bin/sh
export EPOCROOT=/home/andrew/symbian-sdks/s60_28/
cd ../group_s60_2nd
bldmake bldfiles
abld build armi urel
cd ../sis
makesis ayfly_s60_2nd.pkg
mv ayfly_s60_2nd.sis ayfly_s60_2nd_0.0.19.sis
