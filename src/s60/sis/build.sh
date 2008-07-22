#/bin/sh
cd ../group
abld build gcce urel
cd ../sis
makesis ayfly_s60_S60_3_X_v_1_0_0.pkg
signsis ayfly_s60_S60_3_X_v_1_0_0.sis ayfly_s60_S60_3_X_v_1_0_0.sisx gencert.cert genkey.key

