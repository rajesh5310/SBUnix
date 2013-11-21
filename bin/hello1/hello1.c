int main(int argc, char* argv[]) {
		volatile char * video_memory =  (volatile char *) (0xFFFFFFFF800B8000 + 160);
		*video_memory = '2';
	    video_memory += 1;
	    *video_memory = 0x0f;
	    video_memory += 1;

	    *video_memory = '3';
	    	    video_memory += 1;
	    	    *video_memory = 0x0f;
	    	    video_memory += 1;


	    while(1);
	    return 0;
}
