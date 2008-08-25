#/bin/sh
cd ../group
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_s60.pkg
signsis ayfly_s60.sis ayfly_s60_0.0.16.sisx gencert.cert genkey.key

