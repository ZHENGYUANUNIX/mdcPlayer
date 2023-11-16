#########################################################################
# File Name: build.sh
# Created Time: 2023年11月16日 星期四 19时49分53秒
#########################################################################
#!/bin/bash
/usr/local/mdc_sdk/dp_gea/mdc_cross_compiler/bin/clang++  src/main.cpp src/msgDef.cpp src/player.cpp -I include -lpthread  -o mdcPlayer
