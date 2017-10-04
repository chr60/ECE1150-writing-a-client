
int parse_content_length(char* buf)
{
	int length = 0;
	char* leng_start;
	char contStr[] = "Content-Length: ";

	leng_start = strstr(buf, contStr);
	leng_start = leng_start+16;

	length = atoi(leng_start);

	return length;
}

void print_weather(char* buf, char* city, char* state)
{
	//create an output file
	FILE *f = fopen("output.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	//introduce
	printf("\n\nThe weather for %s, %s is....\n\n", city, state);
	fprintf(f, "\n\nThe weather for %s, %s is....\n\n", city, state);

	//find start and stop of forcast within buffer
	char weatherTag[] = "<weather>";
	char* start = strstr(buf, weatherTag);
	char stopTag[] = "<icon>";
	char* stop = strstr(buf, stopTag);

	//parse out the good stuff
	int i = 0;
	int close_tag = 0;
	for(int i = 0; i < strlen(buf); i++) {
		if(&start[i] == stop)
		{
			break;
		}
		else if(start[i] == ' ' || start[i] == '\n' || start[i] == '\t' || start[i] == '<')
		{
			//dont print these chars 
		}
    	else if(start[i] == '/')
    	{	
    		//keep track of close tags for new line logic
    		close_tag = 1;
    	}
    	else if (start[i] == '>')
    	{
    		if(close_tag == 1)		//new line logic
    		{
    			close_tag = 0;
    			printf("\n");
    			fprintf(f, "\n");
    		}
    		else
    		{
    			printf(": ");
    			fprintf(f, ": ");
    		}
    	}
    	else if(close_tag == 0)
    	{
    		printf("%c", start[i]);
    		fprintf(f, "%c", start[i]);
    	}
	}

	printf("\nEnjoy your day!\n\n");
	fprintf(f, "\nEnjoy your day!\n\n");
	fclose(f);
}