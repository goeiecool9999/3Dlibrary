include = include

lib/lib3D.a: obj/perspective.o obj/tesselation.o obj/objloading.o
	touch lib/lib3D.a
	rm lib/lib3D.a
	ar -cvq lib/lib3D.a obj/perspective.o obj/tesselation.o obj/objloading.o

obj/perspective.o: source/perspective.cpp
	g++ -c source/perspective.cpp -Iinclude -o obj/perspective.o
obj/tesselation.o: source/tesselation.cpp
	g++ -c source/tesselation.cpp -Iinclude -o obj/tesselation.o
obj/objloading.o: source/objloading.cpp
	g++ -c source/objloading.cpp -Iinclude -o obj/objloading.o
	
clean:
	rm obj/*.o
	rm lib/*.a
