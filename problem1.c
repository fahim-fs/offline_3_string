#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define MAX_DOCS 50
#define MAX_LEN 5000
#define MAX_TOKENS 500
#define MAX_TOKEN_LEN 50
#define NUM_STOP_WORDS 8

char documents[MAX_DOCS][MAX_LEN];
char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
char tokens_except_stop_words[MAX_TOKENS][MAX_TOKEN_LEN];
char stemmed_tokens[MAX_TOKENS][MAX_TOKEN_LEN];
char stop_words[NUM_STOP_WORDS][MAX_TOKEN_LEN] = {"the", "is", "a", "an", "and", "in", "of", "to"};

void help();
void normalize_case_all();
void tokenize_all();
void remove_stop_words_all();
void stem_all_tokens();
double compute_tf(char word[], int doc_id);
double compute_idf(char word[]);
void compute_tfidf_all(char word[]);

int main()
{
    printf("Welcome to the Document Processing System!\n");
    int f_1 = 0, num_doc, token_count, token_c_except_stop;
    char cmd[30];
    help();
    while (1)
    {
        printf("\nEnter command: ");
        scanf("%s", cmd);
        if (strcmp(cmd, "set") == 0)
        {
            for (int i = 0; i < MAX_DOCS; i++)
                documents[i][0] = '\0';
            while (1)
            {
                printf("Enter number of documents (1-50): ");
                scanf("%d", &num_doc);
                if (num_doc > 0 && num_doc <= 50)
                {
                    getchar();
                    for (int i = 0; i < num_doc; i++)
                    {
                        printf("Enter document %d: ", i + 1);
                        fgets(documents[i], MAX_LEN, stdin);
                        documents[i][strcspn(documents[i], "\n")] = '\0';
                    }
                    printf("Documents set successfully.");
                    f_1 = 1;
                    break;
                }
                else
                {
                    printf("Invalid number of documents. Must be from 1 to 50.\n");
                    continue;
                }
            }
        }
        else if (strcmp(cmd, "normalize") == 0)
        {
            if (f_1)
            {
                normalize_case_all(num_doc);
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "tokenize") == 0)
        {
            if (f_1)
            {
                token_count = 0;
                tokenize_all(num_doc, &token_count);
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "remove_stop") == 0)
        {
            if (f_1)
            {
                token_c_except_stop = 0;
                remove_stop_words_all(&token_count, &token_c_except_stop);
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "stem") == 0)
        {
            if (f_1)
            {
                stem_all_tokens(token_c_except_stop);
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "tf") == 0)
        {
            if (f_1)
            {
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "idf") == 0)
        {
            if (f_1)
            {
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "tfidf") == 0)
        {
            if (f_1)
            {
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "help") == 0)
        {
            help();
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            printf("Exiting program.\n");
            break;
        }
        else
            printf("Unknown command. Type\"help\" for list of commands.");
    }
}

void help()
{
    printf("set \t\t- Prompt for the number of documents and their text.\n");
    printf("normalize \t- Convert all documents to lowercase and display them.\n");
    printf("tokenize \t- Tokenize all documents into words and display the tokens.\n");
    printf("remove_stop \t- Remove stop words from tokens and display the filtered tokens.\n");
    printf("stem \t\t- Apply simple stemming (remove suffixes like \"ing\", \"ed\", \"s\") and display results.\n");
    printf("tf \t\t- Compute and display Term Frequency for a specified word across documents.\n");
    printf("idf \t\t- Compute and display Inverse Document Frequency for a specified word.\n");
    printf("stat \t\t- Display TF, IDF, and TF-IDF for all tokens across all documents in a matrix format.\n");
    printf("help \t\t- Print the list of all available commands.\n");
    printf("exit \t\t- Exit the program.\n");
}

void normalize_case_all(int s)
{
    for (int i = 0; i < s; i++)
    {
        for (int j = 0; j < MAX_LEN; j++)
        {
            documents[i][j] = tolower(documents[i][j]);
            if (documents[i][j] == '\0')
                break;
        }
    }
    printf("Normalized Documents:\n");
    for (int i = 0; i < s; i++)
    {
        printf("Document %d: %s\n", i + 1, documents[i]);
    }
    printf("Documents normalized.");
}

void tokenize_all(int s, int *t_c)
{
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens[i][0] = '\0';
    int k;
    for (int i = 0; i < s; i++)
    {
        k = 0;
        for (int j = 0; j < MAX_LEN; j++)
        {
            if (isalnum(documents[i][j]))
            {
                tokens[*t_c][k] = documents[i][j];
                k++;
            }
            else if (k > 0)
            {
                tokens[*t_c][k] = '\0';
                k = 0;
                (*t_c)++;
            }
            if (documents[i][j] == '\0')
                break;
            if (*t_c >= MAX_TOKENS)
                break;
        }
        if (*t_c >= MAX_TOKENS)
            break;
    }
    printf("Tokens:\n");
    for (int i = 0; i < *t_c; i++)
    {
        printf("%d. %s\n", i + 1, tokens[i]);
    }
    printf("Tokenization complete. Total tokens: %d", *t_c);
}

void remove_stop_words_all(int *a, int *t_s)
{
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens_except_stop_words[i][0] = '\0';
    if (tokens[0][0] != 0)
    {
        for (int i = 0; i < *a; i++)
        {
            int is_num_stop = 0;
            for (int j = 0; j < NUM_STOP_WORDS; j++)
            {
                if (strcmp(tokens[i], stop_words[j]) == 0)
                {
                    is_num_stop = 1;
                    break;
                }
            }
            if (!is_num_stop)
            {
                strcpy(tokens_except_stop_words[*t_s], tokens[i]);
                (*t_s)++;
            }
        }
        printf("Tokens after stop-word removal:\n");
        for (int i = 0; i < *t_s; i++)
        {
            printf("%d. %s\n", i + 1, tokens_except_stop_words[i]);
        }
        printf("Stop-word removal complete. Tokens remaining: %d", *t_s);
        for (int i = 0; i < MAX_TOKENS; i++)
            tokens[i][0] = '\0';
        for (int i = 0; i < *t_s; i++)
        {
            strcpy(tokens[i], tokens_except_stop_words[i]);
        }
        *a = *t_s;
    }
    else
        printf("No tokens available. Use\'tokenize\' command first");
}

void stem_all_tokens(int b)
{
    for (int i = 0; i < MAX_TOKENS; i++)
        stemmed_tokens[i][0] = '\0';
    char suffix[4][10] = {"ing", "ed", "s"};
    if (tokens[0][0] != 0)
    {
        for (int i = 0; i < b; i++)
        {
            strcpy(stemmed_tokens[i], tokens_except_stop_words[i]);
            int len = strlen(stemmed_tokens[i]);
            if (len > 3 && strcmp(&stemmed_tokens[i][len - 3], "ing") == 0)
                stemmed_tokens[i][len - 3] = '\0';
            else if (len > 2 && strcmp(&stemmed_tokens[i][len - 2], "ed") == 0)
                stemmed_tokens[i][len - 2] = '\0';
            else if (len > 1 && stemmed_tokens[i][len - 1] == 's')
                stemmed_tokens[i][len - 1] = '\0';
        }
        printf("Stemmed Tokens:\n");
        for (int i = 0; i < b; i++)
        {
            printf("%d. %s\n", i + 1, stemmed_tokens[i]);
        }
        printf("Stemming complete. Total stemmed tokens: %d", b);
    }
    else
        printf("No tokens available. Use\'tokenize\' command first");
}

double compute_tf(char word[], int doc_id)
{
    
}

double compute_idf(char word[])
{
}

void compute_tfidf_all(char word[])
{
}