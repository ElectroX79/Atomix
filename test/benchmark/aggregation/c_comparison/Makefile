all: float64_array_sum intenger32_array_sum

intenger32_array_sum: intenger32_array_sum.c
	gcc -O3 -march=native intenger32_array_sum.c -o intenger32_array_sum

float64_array_sum: float64_array_sum.c
	gcc -O3 -march=native float64_array_sum.c -o float64_array_sum

clean:
	rm -f float64_array_sum intenger32_array_sum
