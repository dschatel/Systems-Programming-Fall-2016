/*
 * pointersorter.c
 *
 *  Created on: Sep 15, 2016
 *      Author: Falcon24
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct _node {
	char * data;
	struct _node * next;
} Node;


Node * createList (Node * nodeHead, char * input) {

	char * ptr = input;
	int tokLength = 0;
	int compare;
	Node *traverse, *prev;

	while (*input != '\0') {
		if(isalpha(*ptr)) {
			ptr++;
			tokLength++;
		}
		else {
			if(tokLength > 0) {
				Node * newNode = malloc(sizeof(Node));
				newNode->data = malloc(tokLength + 1);
				strncpy(newNode->data, input, tokLength);
				newNode->data[tokLength] = '\0';

				if(nodeHead == NULL) {
					newNode->next = nodeHead;
					nodeHead = newNode;
				}
				else {
					traverse = nodeHead;
					prev = nodeHead;

					while (traverse != NULL) {
						compare = strcmp(traverse->data, newNode->data);
						if (compare >= 0) {
							newNode->next = traverse;
							if (traverse == nodeHead)
								nodeHead = newNode;
							else
								prev->next = newNode;
							break;
					}
						else {
							prev = traverse;
							traverse = traverse->next;
						}
					}

					if(traverse == NULL) {
						newNode->next = traverse;
						prev->next = newNode;
					}
				}

				//Sort on insert
				//newNode->next = nodeHead;
				//nodeHead = newNode;
				}

			ptr++;
			input = ptr;
			tokLength = 0;
			}

		}

	return nodeHead;

}

void printList(Node *head) {

	Node * ptr = head;

	while (ptr != NULL) {
		printf("Token: %s\n", ptr->data);
		ptr = ptr->next;
	}

}

void freeNodes(Node *head) {
	Node *tmp = head;

	while (head != NULL) {
		head = head->next;
		free(tmp->data);
		tmp->data = NULL;
		free(tmp);
		tmp = NULL;
		tmp = head;
	}

}

int main (int arc, char ** argv) {

	char * fullstring = malloc((sizeof (char) * strlen(argv[1])));
	strcpy(fullstring, argv[1]);
	fullstring[strlen(argv[1])] = '\0';

	Node * head = NULL;
	head = createList(head, fullstring);
	printList(head);
	freeNodes(head);
	head = NULL;

	return 0;

}

