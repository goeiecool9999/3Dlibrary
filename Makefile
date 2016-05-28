sources = $(wildcard *.cpp)
headers = $(wildcard *.hpp)
objects = $(sources:.cpp=.o)

CXXFLAGS = -ggdb

lib3D.a: $(objects) | dependencies.txt
	rm -f lib3D.a
	ar -csvq lib3D.a $(objects)

dependencies.txt: $(headers) $(sources)
	g++ -MM *.cpp > dependencies.txt


-include dependencies.txt

	
clean:
	rm -f *.o *.a dependencies.txt
