/* Restaurant recommender:
 *
 * Skeleton code written by Jianzhong Qi, April 2022
 * Edited by: [Add your name and student ID here]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* stage numbers */
#define STAGE_NUM_ONE 1
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5
#define HEADER "=========================Stage %d=========================\n"

#define MAX_WORD_LENGTH 20
#define MAX_FORM_LENGTH 95
#define MAX_SCORE_LENGTH 2
#define MAX_WORD_NUM 100
#define SENTENCE_WORD_LEN 23
#define OVERALL_SCORE "Overall score:"
#define NOT_FOUND 0
#define FOUND 1
#define END_OF_DICT '%'
#define SUCCESS 0
#define FIRST_WORD 0
#define ASTERISK "*"
#define NOT_FOUND_STR "NOT_FOUND"
#define END_OF_STR '\0'
#define NEXT 1

typedef int data_t;
typedef struct dictionary dict_t;
typedef struct sentence sentence_t;
typedef char word_t[MAX_WORD_LENGTH+1];
typedef char form_t[MAX_FORM_LENGTH+1];
typedef char score_t[MAX_SCORE_LENGTH+1];

/****************************************************************/

/* structures */
struct dictionary {
	word_t word;
	score_t score;
	form_t form;
};

typedef struct {
	sentence_t *head;
	sentence_t *foot;
} list_t;

struct sentence {
	char word[SENTENCE_WORD_LEN+1];
	sentence_t *next;
};

/****************************************************************/

/* function prototypes */
void stage_one(dict_t dict[], int *num_word);
void stage_two(dict_t dict[], int num_word);
void stage_three(list_t *sentence);
void stage_four(dict_t dict[], list_t *sentence, int num_word);
void stage_five(dict_t dict[], list_t *sentence, int num_word);

list_t *create_empty_list();
void print_stage_header(int stage_num);
void read_dict(dict_t dict[], int *num_word);
void read_one_word(char str[]);
void insert_sentence(list_t *list);
int getword(char W[], int limit);
int b_search(dict_t dict[], int lo, int hi, char target[], int *pos);
int form_matching(form_t form, const char word[]);
void free_list(list_t *mylist);


/****************************************************************/

int
main(int argc, char *argv[]) {
	/* The input starts with a list of at least 1 and up to 99 restaurant records sorted by the restaurant IDs(e.g., ABNs) in ascending order. */
	dict_t dict[MAX_WORD_NUM];
	int num_word = 0;

	/* Stage 1 - Read Restaurant Records */
	stage_one(dict, &num_word);

	/* Stage 2 - Read Dining Transactions */
	stage_two(dict, num_word);

	list_t *sentence = create_empty_list();
	assert(sentence);

	/*  Stage 3 - Recommend Based on Restaurant Similarity */
	stage_three(sentence);

	/* Stage 4 - Recommend Based on Customer Similarity */
	stage_four(dict, sentence, num_word);

	/*  Stage 5 - Recommend by Matrix Factorisation */
	stage_five(dict, sentence, num_word);

	free_list(sentence);

	return 0;
}

/****************************************************************/


void
print_stage_header(int stage_num) {
	printf(HEADER, stage_num);
}

/****************************************************************/

list_t *
create_empty_list() {
	list_t *new_list = (list_t *)malloc(sizeof(list_t));
	assert(new_list);
	new_list->head = new_list->foot = NULL;
	return new_list;
}

/****************************************************************/


void
read_dict(dict_t dict[], int *num_word) {
	int c, count = 0;

	while ((c = getchar()) != END_OF_DICT) {
	    read_one_word(dict[count].word);
	    read_one_word(dict[count].score);
	    read_one_word(dict[count].form);
	    count++;
	}

	*num_word = count;
}

/****************************************************************/

/* read a line of input into the given char array */
/* function adapted from Assignment 1 code and modified */
void
read_one_word(char str[]) {
	int i = 0, c;
	while (((c = getchar()) != EOF ) && c != '\n' && c != '\r' && c != ' ') {
		if (c != '$') {
			str[i++] = c;
		}
	}
	str[i] = END_OF_STR;
}

/****************************************************************/


void
insert_sentence(list_t *list) {
	sentence_t *new = (sentence_t *)malloc(sizeof(sentence_t));
	assert(new && list);


	while (getword(new->word, SENTENCE_WORD_LEN) == SUCCESS) {


	    new->next = NULL;
        if (list->head == NULL) {
		    list->head = list->foot = new;
	    }
	    else {
		    list->foot->next = new;
		    list->foot = new;
	    }

	    new = (sentence_t *)malloc(sizeof(sentence_t));
	    assert(new);
	}
	free(new);
}

/****************************************************************/

int
getword(char W[], int limit) {
	int c, len = 0;

	while ((c = getchar()) != EOF && !isalpha(c)) {
	}
	if (c == EOF) {
		return EOF;
	}

	W[len++] = c;
	while (len < limit && (c = getchar()) != EOF && isalpha(c)) {

		W[len++] = c;
	}

	W[len] = END_OF_STR;
	return SUCCESS;
}

/****************************************************************/

/* Stage 1 - Read Restaurant Records */
void
stage_one(dict_t dict[], int *num_word) {
	print_stage_header(STAGE_NUM_ONE);

	read_dict(dict, num_word);


	printf("First word: %s\n", dict[FIRST_WORD].word);
	printf("Sentiment score: %d\n", atoi(dict[FIRST_WORD].score));
	printf("Forms: %s\n\n", dict[FIRST_WORD].form);
}

/****************************************************************/

/* Stage 2 - Read Dining Transactions */
void
stage_two(dict_t dict[], int num_word) {
	print_stage_header(STAGE_NUM_TWO);
	double score_total = 0, word_len = 0; int i;

	for (i = 0; i < num_word; i++) {
		word_len += strlen(dict[i].word);
		score_total += atoi(dict[i].score);
	}
	/* print out the result */
	printf("Number of words: %d\n", num_word);
	printf("Average number of characters: %.2f\n", word_len/num_word);
	printf("Average sentiment score: %.2f\n\n", score_total/num_word);
}

/****************************************************************/

/* Stage 3 - Recommend Based on Restaurant Similarity */
void
stage_three(list_t *sentence) {
	print_stage_header(STAGE_NUM_THREE);
	int c;
	assert(sentence);


	while ((c = getchar()) == END_OF_DICT) {
	}


	insert_sentence(sentence);


	sentence_t *new = sentence->head;
	while (new) {
		printf("%s\n", new->word);
		new = new->next;
	}
	printf("\n");
}

/****************************************************************/

/* Stage 4 - Recommend Based on Customer Similarity */
void
stage_four(dict_t dict[], list_t *sentence, int num_word) {
	print_stage_header(STAGE_NUM_FOUR);
	assert(sentence);
	sentence_t *new = sentence->head;
	int tot_score = 0, pos;

	int lo = 0;

	while (new) {


		if (b_search(dict, lo, num_word, new->word, &pos) == NOT_FOUND) {
			printf("%-25s0\n", new->word);
		}

		else {
			printf("%-25s%d\n", new->word, atoi(dict[pos].score));
			tot_score += atoi(dict[pos].score);
		}
		new = new->next;
	}


	printf("%-25s%d\n\n", OVERALL_SCORE, tot_score);
}

/****************************************************************/

/* Stage 5 - Recommend by Matrix Factorisation */
void
stage_five(dict_t dict[], list_t *sentence, int num_word) {
	print_stage_header(STAGE_NUM_FIVE);
	assert(sentence);
	int i, flag, tot_score = 0;
	sentence_t *new = sentence->head;


	while (new) {
		flag = NOT_FOUND;


		for (i = 0; i < num_word; i++) {
			if (strcmp(dict[i].word, new->word) == 0) {
				tot_score += atoi(dict[i].score);
				printf("%-25s%-25s%d\n", new->word, dict[i].word,
					atoi(dict[i].score));
				flag = FOUND;
			}
			else if (form_matching(dict[i].form, new->word) == FOUND) {
				tot_score += atoi(dict[i].score);
				printf("%-25s%-25s%d\n", new->word, dict[i].word,
					atoi(dict[i].score));
				flag = FOUND;
			}
		}

		if (flag == NOT_FOUND) {
			printf("%-25s%-25s0\n", new->word, NOT_FOUND_STR);
		}
		new = new->next;
	}

	printf("%-50s%d\n", OVERALL_SCORE, tot_score);
}

/****************************************************************/

int
form_matching(form_t form, const char word[]) {
	form_t temp;
	strcpy(temp, form);


	char *token = strtok(temp, ASTERISK);
	while (token) {

		if (strcmp(token, word) == 0) {
			return FOUND;
		}
		token = strtok(NULL, ASTERISK);
    }
    return NOT_FOUND;
}

/****************************************************************/


int
b_search(dict_t dict[], int lo, int hi, char target[], int *pos) {
	int mid, outcome;
	if (lo >= hi) {
		return NOT_FOUND;
	}

	mid = (lo + hi)/2;


	if ((outcome = strcmp(target, dict[mid].word)) < 0) {
		return b_search(dict, lo, mid, target, pos);
	}
	else if (outcome > 0) {
		return b_search(dict, mid+NEXT, hi, target, pos);
	}

	else {
		*pos = mid;
		return FOUND;
	}
}

/****************************************************************/

void
free_list(list_t *list) {
    sentence_t *new;
    assert(list);
    while (list->head != NULL) {
        new = list->head->next;
        free(list->head);
        list->head = new;
    }
    free(list);
    list = NULL;
}

/****************************************************************/
