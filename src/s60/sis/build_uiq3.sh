#/bin/sh
cd ../group
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_s60.pkg
signsis ayfly_s60.sis ayfly_uiq3_0.0.15.sisx gencert.cert genkey.key

