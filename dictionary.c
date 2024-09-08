#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

table_t *create_table() {
    table_t *table = malloc(sizeof(table_t));
    if (table == NULL) {
        return NULL;
    }
    table->array = malloc(1301 * sizeof(list_node_t*));
    for (int i = 0; i<1301; i++) {
        table->array[i] = NULL;
    }
    table->length = 1301;
    return table;

}

dictionary_t *create_dictionary() {
    dictionary_t *dict = malloc(sizeof(dictionary_t));
    if (dict == NULL) {
        return NULL;
    }
    dict->table = create_table();
    if (dict->table == NULL) {
        return NULL;
    }
    dict->size = 0;
    return dict;
}

int hash_code(const char* word) {
    int hash;

    for (hash = 0; *word != '\0'; word++) {
        hash = *word + 31*hash;
    }
    return hash;
}

int dict_insert(dictionary_t *dict, const char *word) {
    int hash = hash_code(word);
    hash = hash % dict->table->length;
    list_node_t *node = malloc(sizeof(list_node_t));
    if (node == NULL) {
        return -1;
    }
    node->word[127] = '\0';
    strcpy(node->word, word);
    node->next = NULL;
    if (dict->table->array[hash] == NULL) {
        dict->table->array[hash] = node;
    } else {
        list_node_t *cur = dict->table->array[hash];
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = node;
    }
    dict->size = dict->size + 1;
    if (((1.0 * dict->size) / dict->table->length) > 0.8 ) {
	dict->table = resize_table(dict->table);
    } 
    return 0;
}

int dict_find(const dictionary_t *dict, const char *query) {
    if (dict == NULL || query == NULL) {
    	return 0;
    }
    int hash = hash_code(query);
    hash = hash % dict->table->length;
    if (dict->table->array[hash] == NULL) {
    	return 0;
    } else {
	list_node_t *cur = dict->table->array[hash];
    	do {
	    if (strcmp(cur->word, query) == 0) {
	        return 1;
	    }
	    cur = cur->next;
	} while (cur != NULL);
    }
    return 0;
}

void dict_print(const dictionary_t *dict) {
    if (dict != NULL) {
    	for (int i = 0; i< dict->table->length; i++) {
            if (dict->table->array[i] == NULL) {
                continue;
            }
            list_node_t *cur = dict->table->array[i];
            do {
                printf("%s\n", cur->word);
                cur = cur->next;
            } while (cur != NULL);
        }
    }
}

void dict_free(dictionary_t *dict) {
    for (int i = 0; i < dict->table->length; i++) {
        if (dict->table->array[i] == NULL) {
	    continue;
	}
	list_node_t *cur = dict->table->array[i];
	
	list_node_t *runner = dict->table->array[i];
	while (runner->next != NULL) {
	    runner = runner->next;
	    free(cur);
	    cur = runner;
	}
	if (cur->next == NULL) {
            free(cur);
            continue;
        }
    }
    free(dict->table->array);
    free(dict->table);
    free(dict);
}

dictionary_t *read_dict_from_text_file(const char *file_name) {
    FILE *file_handle = fopen(file_name, "r");
    if (file_handle == NULL) {
    	return NULL;
    }
    dictionary_t *dict = create_dictionary();
    if (dict == NULL) {
        return NULL;
    }
    char word[128] = "\0";
    while (fscanf(file_handle, "%s", word) != EOF) {
	dict_insert(dict, word);
    }
    fclose(file_handle);
    return dict;
}

table_t* resize_table(table_t* orig) {
    table_t *newtable = malloc(sizeof(table_t));
    if (newtable == NULL) {
        return NULL;
    }
    newtable->length = (orig->length * 2);
    newtable->array = malloc(newtable->length * sizeof(list_node_t*));
    for (int i = 0; i<newtable->length; i++) {
        newtable->array[i] = NULL;
    }
    int hash = 0;
    for (int i = 0; i < orig->length; i++) {
    	if (orig->array[i] == NULL) {
	    continue;
	}
	list_node_t *cur = orig->array[i];
	do {
	    list_node_t *temp = cur->next;
	    cur->next = NULL;
	    hash = hash_code(cur->word) % newtable->length;
            if (newtable->array[hash] == NULL) {
	    	newtable->array[hash] = cur;
	    } else {
		list_node_t *runner = newtable->array[hash];
	    	while (runner->next != NULL) {
		    runner = runner->next;
		}
		runner->next = cur;
	    }
            cur = temp;
        } while (cur != NULL);
    }
    free(orig->array);
    free(orig);
    return newtable;
}

int write_dict_to_text_file(const dictionary_t *dict, const char *file_name) {
    if (dict == NULL || file_name == NULL) {
    	return -1;
    }
    FILE *file_handle = fopen(file_name, "w");
    if (file_handle == NULL) {
    	return -1;
    }
    for (int i = 0; i< dict->table->length; i++) {
        if (dict->table->array[i] == NULL) {
            continue;
        }
        list_node_t *cur = dict->table->array[i];
        do {
            fputs(cur->word, file_handle);
            cur = cur->next;
        } while (cur != NULL);
    }
    fclose(file_handle);
    return 0;
}

