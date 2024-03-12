abcsort2:
	gcc main.c sort_engine.c sys_utils_wrappers.c -o abcsort2
clean:
	rm abcsort2
rebuild: clean abcsort2