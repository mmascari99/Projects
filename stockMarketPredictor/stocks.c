#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DAY{
	char date[12];
	double open;
	double high;
	double low;
	double close;
	double change;
}DAY;
DAY list[2267];

void read_file();
void compare();
void printDAY(int i);

int main(){
	read_file();
	compare();
	return 0;
}

void compare(){
	double change, average = 0.0;
	printf("\n\n\nenter S&P 500 change for the day:");
	scanf("%lf", &change);
	int numerator = 1, denominator = 1;
	for(int i = 0; i < 2265; i++){
		if(list[i].change > (change - 0.01) && list[i].change < (change + 0.01)){
			if(list[i+1].change > 0.0){
				numerator++;
				average += list[i+1].change;
				printDAY((i+1));
				
			}else{
				denominator++;
				average += list[i+1].change;
				printDAY((i+1));
			} 
		}
	}
	if(denominator > numerator){
		printf("Buy Puts: %d/%d average: %lf\n", numerator, denominator, (average/(numerator + denominator)));
	}else if(numerator > denominator){
		printf("Buy Calls: %d/%d average: %lf\n", numerator, denominator, (average/(numerator + denominator)));
	}else{
		printf("Take a gamble: %d/%d average: %lf\n", numerator, denominator, (average/(numerator + denominator)));
	}
	return;
}

void read_file(){
	const char s[2] = ",";
	char *token, *ptr;	
	char line[100];
	FILE * data_input = fopen("stocks.csv", "r");
	
	fgets(line, 100, data_input);
	for(int i = 0; i < 2266; i++){
		fgets(line, 100, data_input);
		
		token = strtok(line, s);
		strcpy(list[i].date,token);
		token = strtok(NULL, s);
		list[i].open = strtod(token, &ptr);
		token = strtok(NULL, s);
		list[i].high = strtod(token, &ptr);
		token = strtok(NULL, s);
		list[i].low = strtod(token, &ptr);
		token = strtok(NULL, s);
		list[i].close = strtod(token, &ptr);
		list[i].change = ((list[i].open - list[i].close)/list[i].open) * 100;
		printDAY(i);
	}
	return;
}

void printDAY(int i){
	printf("%s: Open:%.2lf High:%.2lf Low:%.2lf Close:%.2lf Change:%.4lf\n", list[i].date, list[i].open, list[i].high, list[i].low, list[i].close, list[i].change);
	return;
}
