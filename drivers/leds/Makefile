KERN_DIR:=/home/fenghan/arm-linux/linux-2.6.22.6

all:
	make -C $(KERN_DIR) M=`pwd` modules

clean:
	
	make -C $(KERN_DIR) M=`pwd` modules clean

obj-m	+=led.o
