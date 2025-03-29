
all : catch2

src/catch2.tar.gz :
	# wget https://github.com/catchorg/Catch2/archive/v2.11.1.tar.gz -O src/catch2.tar.gz
	wget https://github.com/catchorg/Catch2/archive/v3.8.0.tar.gz -O src/catch2.tar.gz

.PHONY : catch2
catch2 : src/catch2.tar.gz
	mkdir -p src/lib/Catch2
	tar -C src/lib/Catch2 --strip-components=1 -xf src/catch2.tar.gz


.PHONY : clean
clean :
	rm src/catch2.tar.gz
	rm -rf src/lib/Catch2
