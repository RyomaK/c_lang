# c言語

1.othello(report2.c)

#シェルコマンド

NAME=report2

echo $NAME

arm-none-eabi-gcc -S $NAME.c

arm-none-eabi-as -o $NAME.o $NAME.s

arm-none-eabi-as -o crt.o crt.S

arm-none-eabi-ld -T gcc.ls -o $NAME.out crt.o $NAME.o

arm-none-eabi-objcopy -O binary $NAME.out $NAME.mb
