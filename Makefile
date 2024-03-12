abcsort2:
	gcc main.c sort_engine.c sys_utils_wrappers.c file_cutter.c -o abcsort2
clean:
	rm abcsort2
rebuild: clean abcsort2