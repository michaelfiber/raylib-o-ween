gcc -o ./raylib-o-ween ./src/raylib-o-ween.c -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -s -O1 -std=gnu99 -I. -I/home/pi/raylib/src -I/home/pi/raylib/src/external -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -L. -L/home/pi/raylib -L/home/pi/raylib/src -L/opt/vc/lib -lraylib -lbrcmGLESv2 -lbrcmEGL -lpthread -lrt -lm -lbcm_host -ldl -DPLATFORM_RPI