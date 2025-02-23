all:demo1 demo2 demo3 incache outcache

demo1:demo1.cpp _public.h _public.cpp
	g++ -g -o demo1 demo1.cpp _public.cpp

demo2:demo2.cpp _public.h _public.cpp
	g++ -g -o demo2 demo2.cpp _public.cpp

demo3:demo3.cpp _public.h _public.cpp
	g++ -g -o demo3 demo3.cpp _public.cpp

incache:demo4_producer.cpp _public.h _public.cpp
	g++ -g -o incache demo4_producer.cpp _public.cpp

outcache:demo4_consumer.cpp _public.h _public.cpp
	g++ -g -o outcache demo4_consumer.cpp _public.cpp

clean:
	rm -f demo1 demo2 demo3 incache outcache




