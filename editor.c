/**
 * Machine Problem: Text Editor
 * CS 241 - Fall 2016
 */
/*
#include "document.h"
#include "editor.h"
#include "format.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_filename(int argc, char *argv[]) {
  // TODO implement get_filename
  // take a look at editor_main.c to see what this is used for
  return NULL;
}

void handle_display_command(Document *document, const char *command) {
  // TODO implement handle_display_command
}

void handle_write_command(Document *document, const char *command) {
  // TODO implement handle_write_command
}

void handle_append_command(Document *document, const char *command) {
  // TODO implement handle_append_command
}

void handle_delete_command(Document *document, const char *command) {
  // TODO implement handle_delete_command
}

void handle_search_command(Document *document, const char *command) {
  // TODO implement handle_search_command
}

void handle_save_command(Document *document, const char *filename) {
  // TODO implement handle_save_command
}
*/
#include "editor.h"
#include "document.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ctype.h>

char *get_filename(int argc, char *argv[]) {
  // TODO implement get_filename
  // take a look at editor_main.c to see what this is used for
  return argv[argc-1];
}

void handle_display_command(Document *document, const char *command) {
  // TODO implement handle_display_command

	char * error = calloc(1, sizeof(char) * 64);
	int length = strlen(command);
	char letter = 0;
	int index = -1;
	int count = 0;
	int doc_lines = Document_size(document);

	sscanf(command, "%c %d%s", &letter, &index, error);

	for(int i = 0; i < length; i++){
		if(isspace(command[i])) count++;
	}

	if(count > 1 || (count == 1 && index == -1 && length != 1)){
		invalid_command(command);
		free(error);
		return;
	}

	if(doc_lines == 0){
		print_document_empty_error();
		free(error);
		return;
	}

	if(length == 1) for(int i = 0; i < doc_lines; i++) print_line(document, i+1);

	else if(index > doc_lines) invalid_line();

	else if(index == 0 || strlen(error) != 0) invalid_command(command);

	else if(index > 0){
		if(index - 5 < 0){
			if(index + 5 > doc_lines) for(int i = 0; i < doc_lines; i++) print_line(document, i+1);
			else for(int i = 0; i < index+5; i++) print_line(document,i+1);
		}
		else{
			if(index+5 <= doc_lines){
				if(index - 5 == 0) for(int i = index-5;i < index+5; i++) print_line(document, i+1);
				else for(int i = index-6; i < index+5; i++) print_line(document, i+1);
			}
			else{
				if(index - 5 == 0) for(int i = index-5;i < index; i++) print_line(document, i+1);
				else for(int i = index-6; i < index; i++) print_line(document, i+1);
			}
		}
	}
	free(error);
	return;
}

void handle_write_command(Document *document, const char *command) {
  // TODO implement handle_write_command

	char * text = calloc(1, sizeof(char) * strlen(command));
	char letter = 0;
	int index = -1;
	int length = strlen(command);
	int beginning = -1;

	sscanf(command, "%c %d %s", &letter, &index, text);
//	printf("%c,%d,%s,%d.\n", letter, index, error, count);

	int count = 0;
	for(int i = 0; i < length; i++){
		if(isspace(command[i])){
			count++;
			if(count == 2) beginning = i+1;		//index of text write
		}
	}
//	bool bullshit = false;
//	for(int i = 2; i < length; i++){
//		if(isspace(command[i])) return 
//	}

	if(length < 4 || index == 0 || count < 2){
		invalid_command(command);
		free(text);
		return;
	}

	strcpy(text, &command[beginning]);

	const char * delim = "$";
	size_t numtokens = 0;

	char ** tokens = strsplit(text, delim, &numtokens);



	Document_set_line(document, index, tokens[0]);
	free(tokens[0]);

	for(int i = 1; i < (int)numtokens; i++){
		Document_insert_line(document, index+i, tokens[i]);
		free(tokens[i]);
	}

	free(tokens);
	free(text);
}

void handle_append_command(Document *document, const char *command) {
  // TODO implement handle_append_command

	int doc_lines = Document_size(document);
	char letter = 0;
	int index = -1;
	int length = strlen(command);
	int beginning = -1;

	char * text = calloc(1, sizeof(char) * (strlen(command)));

	sscanf(command, "%c %d %s", &letter, &index, text);

	int count = 0;
	for(int i = 0; i < length; i++){
		if(isspace(command[i])){
			count++;
			if(count == 2) beginning = i+1;		//index of text write
		}
	}

	strcpy(text, &command[beginning]);

	if(index > doc_lines){
		handle_write_command(document, command);
		//Document_set_line(document, index-1, text);
		free(text);
	}
	else{
		const char * cur_line = Document_get_line(document, index);

		const char * delim = "$";
		size_t numtokens = 0;

		char ** tokens = strsplit(text, delim, &numtokens);

		char * append = calloc(1, sizeof(char) * ((strlen(tokens[0]) + strlen(cur_line) + 1)));
		strcpy(append, cur_line);
//		strcpy(text, tokens[0]);
		strcat(append, tokens[0]);
		Document_set_line(document, index, append);

		for(int i = 1; i < (int)numtokens; i++){
			Document_insert_line(document, index+i, tokens[i]);
			free(tokens[i]);
		}
		free(tokens[0]);
		free(tokens);
		free(append);
		free(text);
	}

	return;

}

void handle_delete_command(Document *document, const char *command) {
  // TODO implement handle_delete_command

	int doc_lines = Document_size(document);
	char letter = 0;
	int index = -1;
	int length = strlen(command);
//	int beginning = -1;

	char * text = calloc(1, sizeof(char) * (strlen(command)));

	sscanf(command, "%c %d %s", &letter, &index, text);

	int count = 0;

	for(int i = 0; i < length; i++){
		if(isspace(command[i])) count++;
	}

	if(index > doc_lines){
		invalid_line();
		free(text);
		return;
	}

	if(index < 1 || length < 3 || !(isdigit(command[2])) || count > 1){
		invalid_command(command);
		free(text);
		return;
	}

	Document_delete_line(document, index);
	free(text);
	return;
}

void handle_search_command(Document *document, const char *command) {
  // TODO implement handle_search_command

	char * text = malloc(sizeof(char) * strlen(command));

	strcpy(text, command+1);

	if(!strcmp(text, "")){
		free(text);
		return;
	}

	char * booty = malloc(sizeof(char) * 1024);
	char * ass = NULL;
	int doc_lines = Document_size(document);
	for(int i = 0; i < doc_lines; i++){
		ass = NULL;
		strcpy(booty,Document_get_line(document, i+1));
		ass = strstr(booty, text);

		if(ass){
			print_search_line(i+1, booty, ass, text);
		}
	}

	free(text);
	free(booty);
	return;
}

void handle_save_command(Document *document, const char *filename) {
  // TODO implement handle_save_command

	Document_write_to_file(document, filename);
}
