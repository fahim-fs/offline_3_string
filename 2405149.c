#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
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
char stop_words[NUM_STOP_WORDS][MAX_TOKEN_LEN] = { "the", "is", "a", "an", "and", "in", "of", "to" };
char sorted_tokens[MAX_TOKENS][MAX_TOKEN_LEN], unique_tokens[MAX_TOKENS][MAX_TOKEN_LEN];
int token_flag[MAX_TOKENS] = { 0 }, token_num_per_doc[MAX_DOCS] = { 0 };
int num_doc, token_count, token_c_except_stop;

void help();
void normalize_case_all();
void tokenize_all();
void remove_stop_words_all();
void stem_all_tokens(int b);
double compute_tf(char word[], int doc_id);
double compute_idf(char word[]);
void compute_tfidf_all(char word[]);
void display_stat();
int cmpstring(const void* a, const void* b);

int main()
{
    printf("Welcome to the Document Processing System!\n");
    int f_1 = 0;
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
                        int l = strlen(documents[i]);
                        if (l > 0 && documents[i][l - 1] == '\n')
                        {
                            documents[i][l - 1] = '\0';

                        }
                        else
                        {
                            printf("Document too long.");
                            --i;
                            int ch;
                            while ((ch = getchar()) != '\n' && ch != EOF);
                        }
                    }
                    printf("Documents set successfully. Please, enter \'preprocess\' command now. It will not take other commands.");
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
        else if (strcmp(cmd, "preprocess") == 0)
        {
            if (f_1)
            {
                normalize_case_all();
                token_count = 0;
                tokenize_all();
                token_c_except_stop = 0;
                remove_stop_words_all();
                stem_all_tokens(token_c_except_stop);
                for (int i = 0; i < MAX_TOKENS; i++)
                {
                    if (token_flag[i + 1] > 0)
                    {
                        token_num_per_doc[i] = token_flag[i + 1] - token_flag[i];
                    }
                    else
                        break;
                }
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "tf") == 0)
        {
            if (f_1)
            {
                char wrd1[50];
                printf("Enter word to compute TF: ");
                scanf("%s", wrd1);
                for (int i = 0; i < num_doc; i++)
                {
                    printf("Document %d: %.4f\n", i + 1, compute_tf(wrd1, i));
                }

            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "idf") == 0)
        {
            if (f_1)
            {
                char wrd2[50];
                printf("Enter word to compute IDF: ");
                scanf("%s", wrd2);
                printf("IDF for '%s': %.4f", wrd2, compute_idf(wrd2));
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "tfidf") == 0)
        {
            if (f_1)
            {
                char wrd3[50];
                printf("Enter word to compute TF-IDF: ");
                scanf("%s", wrd3);
                printf("TF-IDF for '%s':\n", wrd3);
                compute_tfidf_all(wrd3);
            }
            else
                printf("No documents set. Use \'set\' command first.");
        }
        else if (strcmp(cmd, "stat") == 0)
        {
            if (f_1)
            {
                display_stat();
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
    printf("preprocess \t- Apply normalization, tokenization, stop-words removal, and stemming orderly.\n");
    printf("tf \t\t- Compute and display Term Frequency for a specified word across documents.\n");
    printf("idf \t\t- Compute and display Inverse Document Frequency for a specified word.\n");
    printf("stat \t\t- Display TF, IDF, and TF-IDF for all tokens across all documents in a matrix format.\n");
    printf("help \t\t- Print the list of all available commands.\n");
    printf("exit \t\t- Exit the program.\n");
}

void normalize_case_all()
{
    for (int i = 0; i < num_doc; i++)
    {
        for (int j = 0; j < MAX_LEN; j++)
        {
            documents[i][j] = tolower(documents[i][j]);
            if (documents[i][j] == '\0')
                break;
        }
    }
    printf("Normalized Documents:\n");
    for (int i = 0; i < num_doc; i++)
    {
        printf("Document %d: %s\n", i + 1, documents[i]);
    }
    printf("Documents normalized.\n");
}

void tokenize_all()
{
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens[i][0] = '\0';
    int k;
    for (int i = 0; i < num_doc; i++)
    {
        k = 0;
        for (int j = 0; j < MAX_LEN; j++)
        {
            if (isalnum(documents[i][j]))
            {
                tokens[token_count][k] = documents[i][j];
                k++;
            }
            else if (k > 0)
            {
                tokens[token_count][k] = '\0';
                k = 0;
                token_count++;
            }
            if (documents[i][j] == '\0')
                break;
            if (token_count >= MAX_TOKENS)
                break;
        }
        token_flag[i + 1] = token_count;
        if (token_count >= MAX_TOKENS)
            break;
    }
    printf("Tokens:\n");
    for (int i = 0; i < token_count; i++)
    {
        printf("%d. %s\n", i + 1, tokens[i]);
    }
    printf("Tokenization complete. Total tokens: %d\n", token_count);
}

void remove_stop_words_all()
{
    int start, end;
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens_except_stop_words[i][0] = '\0';
    if (tokens[0][0] != 0)
    {
        for (int doc = 0; doc < num_doc; doc++)
        {
            start = token_flag[doc];
            end = token_flag[doc + 1];
            token_flag[doc] = token_c_except_stop;
            for (int i = start; i < end; i++)
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
                    strcpy(tokens_except_stop_words[token_c_except_stop], tokens[i]);
                    token_c_except_stop++;
                }

            }
        }
        token_flag[num_doc] = token_c_except_stop;
        printf("Tokens after stop-word removal:\n");
        for (int i = 0; i < token_c_except_stop; i++)
        {
            printf("%d. %s\n", i + 1, tokens_except_stop_words[i]);
        }
        printf("Stop-word removal complete. Tokens remaining: %d\n", token_c_except_stop);
        for (int i = 0; i < MAX_TOKENS; i++)
            tokens[i][0] = '\0';
        for (int i = 0; i < token_c_except_stop; i++)
        {
            strcpy(tokens[i], tokens_except_stop_words[i]);
        }
        token_count = token_c_except_stop;
    }
    else
        printf("No tokens available. Use\'tokenize\' command first.\n");
}

void stem_all_tokens(int b)
{
    for (int i = 0; i < MAX_TOKENS; i++)
        stemmed_tokens[i][0] = '\0';
    char suffix[4][10] = { "ing", "ed", "s" };
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
        printf("Stemming complete. Total stemmed tokens: %d\n", b);
    }
    else
        printf("No tokens available. Use\'tokenize\' command first.\n");
}

double compute_tf(char word[], int doc_id)
{
    int count = 0;
    for (int i = token_flag[doc_id]; i < token_flag[doc_id + 1]; i++)
    {
        if (strcmp(word, stemmed_tokens[i]) == 0)
            count++;
    }
    if (count > 0)
        return (double)count / token_num_per_doc[doc_id];
    else
        return 0.0;

}

double compute_idf(char word[])
{
    int count = 0, idf = 0;
    for (int i = 0; i < num_doc; i++)
    {
        for (int j = token_flag[i]; j < token_flag[i + 1]; j++)
        {
            if (strcmp(word, stemmed_tokens[j]) == 0)
            {
                count++;
                break;
            }
        }
    }
    if (count > 0)
    {
        return log10((double)MAX_DOCS / (1 + count));
    }
    else
        return 0.0;

}

void compute_tfidf_all(char word[])
{
    for (int i = 0; i < num_doc; i++)
    {
        double tf = compute_tf(word, i);
        double idf = compute_idf(word);
        if (tf > 0 && idf > 0)
        {
            printf("Document %d: %.4f\n", i + 1, tf * idf);
        }
        else
        {
            printf("Document %d: 0.0000\n", i + 1);
        }
    }
}

void display_stat()
{
    int j = 0, unq_tok_num;
    for (int i = 0; i < token_c_except_stop; i++)
    {
        strcpy(sorted_tokens[i], stemmed_tokens[i]);
    }
    qsort(sorted_tokens, token_c_except_stop, sizeof(sorted_tokens[0]), cmpstring);
    for (int i = 0; i < token_c_except_stop; i++)
    {
        if (strcmp(sorted_tokens[i], sorted_tokens[i + 1]) != 0)
        {
            strcpy(unique_tokens[j], sorted_tokens[i]);
            j++;
        }
    }
    unq_tok_num = j;

    //tf_printing
    for (int i = 0; i < 14; i++)
    {
        printf("=");
    }
    printf(" TF ");
    for (int i = 0; i < 14; i++)
    {
        printf("=");
    }
    printf("\n");
    printf("%30s", "");
    for (int i = 0; i < num_doc; i++)
    {
        printf("%9s%d", "doc", i + 1);
    }
    printf("\n");
    for (int i = 0; i < unq_tok_num; i++)
    {
        printf("%-30s", unique_tokens[i]);
        for (int t = 0; t < num_doc; t++)
        {
            printf("%10.4f", compute_tf(unique_tokens[i], t));
        }
        printf("\n");
    }

    //idf_printing
    for (int i = 0; i < 14; i++)
    {
        printf("=");
    }
    printf(" IDF ");
    for (int i = 0; i < 14; i++)
    {
        printf("=");
    }
    printf("\n");
    printf("%41s", "IDF\n");
    for (int i = 0; i < unq_tok_num; i++)
    {
        printf("%-33s %.4f", unique_tokens[i], compute_idf(unique_tokens[i]));
        printf("\n");
    }

    //tf-idf_printing
    for (int i = 0; i < 14; i++)
    {
        printf("=");
    }
    printf(" TF-IDF ");
    for (int i = 0; i < 14; i++)
    {
        printf("=");
    }
    printf("\n");
    printf("%30s", "");
    for (int i = 0; i < num_doc; i++)
    {
        printf("%9s%d", "doc", i + 1);
    }
    printf("\n");
    for (int i = 0; i < unq_tok_num; i++)
    {
        printf("%-30s", unique_tokens[i]);
        double idf_st = compute_idf(unique_tokens[i]);
        for (int j = 0; j < num_doc; j++)
        {
            printf("%10.4f", compute_tf(unique_tokens[i], j) * idf_st);
        }
        printf("\n");
    }

}

int cmpstring(const void* a, const void* b)
{
    return strcmp((const char*)a, (const char*)b);
}