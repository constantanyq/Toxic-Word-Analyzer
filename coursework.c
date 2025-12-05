#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100    // max length of words
#define MAX_STOP 50    // max length of stopwords
#define MAX_WORDS 7000 // max number of words
#define MAX_BAR 80     // max number of bar chart

typedef struct {
  char *word; // dynamically allocated
  int count;
} Word;

typedef struct {
  char ID[17];
  bool toxic;
  bool severe_toxic;
  bool obscene;
  bool threat;
  bool insult;
  bool identity_hate;
} Comment_Flag;

typedef struct {
  char *word;
  char id[17];
  int count;
  bool toxic;
  bool severe_toxic;
  bool obscene;
  bool threat;
  bool insult;
  bool identity_hate;
} Toxic_Word;

void reset_data(Word *words, int *total, int *unique, int *capacity,
                int *toxic_count, int *severe_count, int *obscene_count,
                int *identity_count, int *insult_count, int *threat_count,
                int *ID_count, int *ID_capacity, char ***ID_array,
                int *toxic_found_count, int *toxic_found_element);
void readFile1(char stopwords[][MAX_STOP], int stopword_count, int *total,
               int *unique, int *capacity, Word **words, int record_len,
               int record_capacity, bool multiline_comment,
               int comment_capacity, int *ID_count, char ***ID_array,
               int *ID_capacity, int *toxic_word_count, char ***toxic_word_list,
               Toxic_Word **toxic_words_found, int *toxic_found_element,
               int *toxic_found_capacity, int *toxic_found_count,
               Comment_Flag **flags, int *flag_count);
Comment_Flag *find_comment_flag(const char *id, Comment_Flag **flags,
                                int *flag_count);
int load_toxicwords(int *toxic_word_count, int *toxic_word_capaccaity,
                    char ***toxic_word_list);
int add_toxicwords(char *update_word, int *toxic_word_count,
                   int *toxic_word_capacity, char ***toxic_word_list);
int is_toxicwords(char *comment, int *toxic_word_count,
                  char ***toxic_word_list);
void toxic_category(int *toxic_count, int *severe_count, int *obscene_count,
                    int *identity_count, int *insult_count, int *threat_count,
                    int *capacity_flag, Comment_Flag **flags, int *flag_count);
bool detection(char *comment, const char *id, int *toxic_word_count,
               char ***toxic_word_list, Toxic_Word **toxic_words_found,
               int *toxic_found_element, int *toxic_found_capacity,
               int *toxic_found_count, Comment_Flag **flags, int *flag_count);
int read_stopwords(char stopwords[][MAX_STOP]);
int add_stopwords(char stopwords[][MAX_STOP], char *update_word,
                  int stopword_count);
void clean_comment(char *comment);
void ID_return(char *id, char ***ID_array, int *ID_count, int *ID_capacity);
int is_stopword(char *comment, char stopwords[][MAX_STOP], int stopword_count);
void total_words(char *comment, const char *id, int *total, int *unique,
                 int *capacity, Word **words_ptr);
void SelectionSort_Toxic(Toxic_Word toxic_words_found[], int n);
void SelectionSort_Unique(Word words[], int *n);
void copywords_toxic(Toxic_Word *newword, Toxic_Word *oldword, int n);
void copywords_unique(Word *newword, Word *oldword, int n);
int comparewords(const void *a, const void *b);
void result_1(int *unique, int *total, int *toxic_found_count,
              int *toxic_found_element);
void result_2(Toxic_Word newword[], int m, int *total);
void result_3(Toxic_Word words[], int m, int *total);
void result_4(Word word[], int *unique, int *total);
void result_5(Word tempword[], int *unique, int *total);
void result_6(int *toxic_count, int *severe_count, int *obscene_count,
              int *threat_count, int *insult_count, int *identity_count,
              int *ID_count);
void save_report(Toxic_Word words[], Toxic_Word newword[], Word word[],
                 Word tempword[], int m, int *unique, int *total,
                 int *toxic_count, int *severe_count, int *obscene_count,
                 int *threat_count, int *insult_count, int *identity_count,
                 int *toxic_found_count, int *toxic_found_element,
                 int *ID_count);
void save_csv(Toxic_Word words[], int m, int *total, int *toxic_found_count);

int main() {
  Toxic_Word *toxic_words_found = NULL; // toxic words detected
  int toxic_found_count = 0;
  int toxic_found_element = 0;
  int toxic_found_capacity = MAX_WORDS;

  char **toxic_word_list = NULL; // toxic words from toxicwords.txt
  int toxic_word_count = 0;
  int toxic_word_capacity = MAX_WORDS;

  Comment_Flag *flags = NULL; // comment flags from test_labels.csv
  int flag_count = 0;
  int capacity_flag = MAX_WORDS;

  char **ID_array = NULL; // array to store comment IDs
  int ID_count = 0;
  int ID_capacity = MAX_WORDS;

  int total = 0;
  int unique = 0;
  int capacity = 1000; // initial capacity for words
  int record_len = 0;
  int record_capacity = MAX_WORDS;
  bool multiline_comment = false;
  int comment_capacity = MAX_WORDS;
  int stopword_count = 0;
  int toxic_count = 0, severe_count = 0, obscene_count = 0, identity_count = 0,
      insult_count = 0, threat_count = 0;

  // memory allocation
  char(*stopwords)[MAX_STOP] = malloc(MAX_WORDS * sizeof(*stopwords));
  if (stopwords == NULL) {
    printf("Memory allocation failed for stopwords.\n");
    return 0;
  }

  Word *words = calloc(capacity, sizeof(Word));
  if (words == NULL) {
    printf("Memory allocation failed for words.\n");
    free(words);
    words = NULL;
    return 0;
  }
  // allocate strings of MAX_LEN for words
  for (int i = 0; i < capacity; i++) {
    words[i].word = malloc(MAX_LEN * sizeof(char));
    if (!words[i].word) {
      printf("Memory allocation failed for words elements.\n");
      for (int j = 0; j < i; j++) {
        free(words[j].word);
      }
      free(words);
      words = NULL;
      return 0;
    }
  }

  Toxic_Word *newword = calloc(20, sizeof(Toxic_Word));
  if (newword == NULL) {
    printf("Memory allocation failed for newword.\n");
    free(words);
    free(newword);
    words = NULL;
    newword = NULL;
    return 0;
  }
  // allocate strings of MAX_LEN for newword
  for (int i = 0; i < 20; i++) {
    newword[i].word = malloc(MAX_LEN * sizeof(char));
    if (!newword[i].word) {
      printf("Memory allocation failed for newwords elements.\n");
      for (int j = 0; j < i; j++) {
        free(newword[j].word);
      }
      free(words);
      free(newword);
      words = NULL;
      newword = NULL;
      return 0;
    }
  }

  Word *tempword = calloc(20, sizeof(Word));
  if (tempword == NULL) {
    printf("Memory allocation failed for tempwords.\n");
    free(words);
    free(newword);
    free(tempword);
    tempword = NULL;
    words = NULL;
    newword = NULL;
    return 0;
  }
  // allocate strings of MAX_LEN for tempword
  for (int i = 0; i < 20; i++) {
    tempword[i].word = malloc(MAX_LEN * sizeof(char));
    if (!tempword[i].word) {
      printf("Memory allocation failed for tempwords elements.\n");
      for (int j = 0; j < i; j++) {
        free(tempword[j].word);
      }
      free(words);
      free(newword);
      free(tempword);
      tempword = NULL;
      words = NULL;
      newword = NULL;
      return 0;
    }
  }
  toxic_words_found = calloc(toxic_found_capacity, sizeof(Toxic_Word));
  if (toxic_words_found == NULL) {
    printf("Memory allocation failedfor toxic_words_found.\n");
    free(newword);
    free(words);
    free(tempword);
    free(toxic_words_found);
    tempword = NULL;
    words = NULL;
    toxic_words_found = NULL;
    newword = NULL;
    return 0;
  }
  // allocate strings of MAX_LEN for toxic_words_found
  for (int i = 0; i < toxic_found_capacity; i++) {
    toxic_words_found[i].word = malloc(MAX_LEN * sizeof(char));
    if (!toxic_words_found[i].word) {
      printf("Memory allocation failed for toxic_word_list elements.\n");
      for (int j = 0; j < i; j++) {
        free(toxic_words_found[j].word);
      }
      free(newword);
      free(words);
      free(tempword);
      free(toxic_words_found);
      tempword = NULL;
      words = NULL;
      toxic_words_found = NULL;
      newword = NULL;
      return 0;
    }
  }

  toxic_word_list = calloc(toxic_word_capacity, sizeof(char *));
  if (toxic_word_list == NULL) {
    printf("Memory allocation failed for toxic_word_list.\n");
    free(words);
    free(toxic_words_found);
    words = NULL;
    toxic_words_found = NULL;
    return 0;
  }
  // allocate strings of MAX_LEN for toxic_word_list
  for (int i = 0; i < toxic_word_capacity; i++) {
    toxic_word_list[i] = malloc(MAX_LEN * sizeof(char));
    if (!toxic_word_list[i]) {
      printf("Memory allocation failed for toxic_word_list elements.\n");
      for (int j = 0; j < i; j++) {
        free(toxic_word_list[j]);
      }
      free(words);
      free(toxic_words_found);
      free(toxic_word_list);
      free(tempword);
      free(newword);
      free(flags);
      flags = NULL;
      tempword = NULL;
      newword = NULL;
      words = NULL;
      toxic_words_found = NULL;
      toxic_word_list = NULL;
      return 0;
    }
  }

  flags = calloc(capacity_flag, sizeof(Comment_Flag));
  if (flags == NULL) {
    printf("Memory allocation failed for flags\n");
    for (int i = 0; i < toxic_word_capacity; i++) {
      free(toxic_word_list[i]);
    }
    free(words);
    free(toxic_words_found);
    free(toxic_word_list);
    free(tempword);
    free(newword);
    free(flags);
    flags = NULL;
    tempword = NULL;
    newword = NULL;
    words = NULL;
    toxic_words_found = NULL;
    toxic_word_list = NULL;
    return 0;
  }

  ID_array = calloc(ID_capacity, sizeof(char *));
  if (ID_array == NULL) {
    printf("Memory allocation failed for ID array.\n");
    for (int i = 0; i < toxic_word_capacity; i++) {
      free(toxic_word_list[i]);
    }
    free(words);
    free(toxic_words_found);
    free(toxic_word_list);
    free(flags);
    free(tempword);
    free(newword);
    free(ID_array);
    ID_array = NULL;
    tempword = NULL;
    newword = NULL;
    words = NULL;
    toxic_words_found = NULL;
    toxic_word_list = NULL;
    flags = NULL;
    return 0;
  }
  // allocate strings of 17 characters for ID_array
  for (int i = 0; i < ID_capacity; i++) {
    (ID_array)[i] = malloc(17 * sizeof(char));
    if (!(ID_array)[i]) {
      printf("Memory allocation failed ID array elements.\n");
      for (int j = 0; j < i; j++) {
        free((ID_array)[j]);
      }
      for (int k = 0; k < toxic_word_capacity; k++) {
        free(toxic_word_list[k]);
      }
      free(words);
      free(toxic_words_found);
      free(toxic_word_list);
      free(flags);
      free(tempword);
      free(newword);
      free(ID_array);
      ID_array = NULL;
      tempword = NULL;
      newword = NULL;
      words = NULL;
      toxic_words_found = NULL;
      toxic_word_list = NULL;
      flags = NULL;
      return 0;
    }
  }

  int choice = 0;
  char answer = '\0';
  char update_word[MAX_LEN] = {0};

  // user interface
  do {
    printf("\n### Toxic Words Analysis Menu ###\n");
    printf("1. Load Text Files\n");
    printf("2. Update Dictionaries\n");
    printf("3. Analyse files\n");
    printf("4. Total unique and toxic words\n");
    printf("5. Bar Chart of Toxic Words\n");
    printf("6. Bar Chart of Unique Words\n");
    printf("7. Percentage of Toxic Sentences by Severity\n");
    printf("8. Save results to txt file\n");
    printf("9. Save results to CSV file\n");
    printf("0. Exit\n");
    printf("\nEnter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
    case 1:
      // load text files
      do {
        printf("\n=== Load Text Files ===\n");
        printf("a. Load Stop Words\n");
        printf("b. Load Toxic Words\n");
        printf("e. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%c", &answer);
        getchar();
        answer = tolower(answer);

        switch (answer) {

        case 'a':
          stopword_count = read_stopwords(stopwords);
          break;

        case 'b':
          //  reset toxic words found before loading new list
          for (int i = 0; i < toxic_found_capacity; i++) {
            toxic_words_found[i].word[0] = '\0';
            toxic_words_found[i].count = 0;
          }
          toxic_word_count = load_toxicwords(
              &toxic_word_count, &toxic_word_capacity, &toxic_word_list);
          break;

        case 'e':
          break;

        default:
          printf("\nInvalid choice! Please select again\n");
          break;
        }
      } while (answer != 'e');
      break;

    case 2:
      // update dictionaries
      do {
        printf("\n=== Update Dictionaries ===\n");
        printf("a. Update Stop Words\n");
        printf("b. Update Toxic Words\n");
        printf("e. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%c", &answer);
        getchar();
        answer = tolower(answer);

        switch (answer) {

        case 'a':
          printf("Words to be added to stopwords.txt:");
          fgets(update_word, sizeof(update_word), stdin);
          update_word[strlen(update_word) - 1] = '\0';
          add_stopwords(stopwords, update_word, stopword_count);
          break;

        case 'b':
          printf("Words to be added to toxicwords.txt:");
          fgets(update_word, sizeof(update_word), stdin);
          update_word[strlen(update_word) - 1] = '\0';
          add_toxicwords(update_word, &toxic_word_count, &toxic_word_capacity,
                         &toxic_word_list);
          break;

        case 'e':
          break;

        default:
          printf("\nInvalid choice! Please select again\n");
          break;
        }
      } while (answer != 'e');
      break;

    case 3:
      // analyse files
      printf("Loading...\n");
      reset_data(words, &total, &unique, &capacity, &toxic_count, &severe_count,
                 &obscene_count, &identity_count, &insult_count, &threat_count,
                 &ID_count, &ID_capacity, &ID_array, &toxic_found_count,
                 &toxic_found_element);

      toxic_category(&toxic_count, &severe_count, &obscene_count,
                     &identity_count, &insult_count, &threat_count,
                     &capacity_flag, &flags, &flag_count);
      readFile1(stopwords, stopword_count, &total, &unique, &capacity, &words,
                record_len, record_capacity, multiline_comment,
                comment_capacity, &ID_count, &ID_array, &ID_capacity,
                &toxic_word_count, &toxic_word_list, &toxic_words_found,
                &toxic_found_element, &toxic_found_capacity, &toxic_found_count,
                &flags, &flag_count);
      SelectionSort_Toxic(toxic_words_found, toxic_found_element);
      SelectionSort_Unique(words, &unique);
      copywords_toxic(newword, toxic_words_found, 20);
      copywords_unique(tempword, words, 20);
      qsort(newword, 20, sizeof(Toxic_Word), comparewords);
      qsort(tempword, 20, sizeof(Word), comparewords);
      printf("Files analyze successfully.\n");
      break;

    case 4:
      // total unique and toxic words
      result_1(&unique, &total, &toxic_found_count, &toxic_found_element);
      break;

    case 5:
      // bar chart of toxic words
      do {
        printf("\n=== Bar Chart of Toxic Words ===\n");
        printf("a. With Alphabetical Order (Top 20)\n");
        printf("b. Only based on Frequencies (Top 20)\n");
        printf("e. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%c", &answer);
        getchar();
        answer = tolower(answer);

        switch (answer) {

        case 'a':
          result_2(newword, toxic_found_element, &total);
          break;

        case 'b':
          result_3(toxic_words_found, toxic_found_element, &total);
          break;

        case 'e':
          break;
        default:
          printf("\nInvalid choice! Please select again\n");
          break;
        }
      } while (answer != 'e');
      break;

    case 6:
      // bar chart of unique words
      do {
        printf("\n=== Bar Chart of Unique Words ===\n");
        printf("a. With Alphabetical Order (Top 20)\n");
        printf("b. Only based on Frequencies (Top 20)\n");
        printf("e. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%c", &answer);
        getchar();
        answer = tolower(answer);

        switch (answer) {

        case 'a':
          result_5(tempword, &unique, &total);
          break;

        case 'b':
          result_4(words, &unique, &total);
          break;

        case 'e':
          break;

        default:
          printf("\nInvalid choice! Please select again\n");
          break;
        }
      } while (answer != 'e');
      break;

    case 7:
      // percentage of toxic sentences by severity
      result_6(&toxic_count, &severe_count, &obscene_count, &threat_count,
               &insult_count, &identity_count, &ID_count);
      break;

    case 8:
      // save results to txt file
      save_report(toxic_words_found, newword, words, tempword,
                  toxic_found_element, &unique, &total, &toxic_count,
                  &severe_count, &obscene_count, &threat_count, &insult_count,
                  &identity_count, &toxic_found_count, &toxic_found_element,
                  &ID_count);
      break;

    case 9:
      // save results to CSV file
      save_csv(toxic_words_found, toxic_found_element, &total,
               &toxic_found_count);
      break;

    case 0:
      // exit
      printf("\nThank you for using!\n");
      break;

    default:
      printf("\nInvalid choice! Please select again\n");
      break;
    }
  } while (choice != 0);

  // free memory
  for (int i = 0; i < unique; i++) {
    free(words[i].word);
  }
  for (int i = 0; i < ID_count; i++) {
    free((ID_array)[i]);
  }
  free(ID_array);
  ID_array = NULL;
  free(words);
  words = NULL;
  free(flags);
  flags = NULL;
  free(toxic_words_found);
  toxic_words_found = NULL;
  for (int i = 0; i < toxic_word_count; i++) {
    free(toxic_word_list[i]);
  }
  free(toxic_word_list);
  toxic_word_list = NULL;
  free(stopwords);
  // printf("done freeing memory\n");

  return 0;
}
void reset_data(Word *words, int *total, int *unique, int *capacity,
                int *toxic_count, int *severe_count, int *obscene_count,
                int *identity_count, int *insult_count, int *threat_count,
                int *ID_count, int *ID_capacity, char ***ID_array,
                int *toxic_found_count, int *toxic_found_element) {
  *total = 0;
  *total = 0;
  *unique = 0;
  *ID_count = 0;
  *toxic_found_count = 0;
  *toxic_found_element = 0;
  *toxic_count = 0;
  *severe_count = 0;
  *obscene_count = 0;
  *identity_count = 0;
  *insult_count = 0;
  *threat_count = 0;

  // reset words[]
  for (int i = 0; i < *unique; i++) {
    words[i].word[0] = '\0';
    words[i].count = 0;
  }

  // reset ID_array[]
  for (int i = 0; i < *ID_capacity; i++) {
    (*ID_array)[i][0] = '\0';
  }
}

// read file 1
void readFile1(char stopwords[][MAX_STOP], int stopword_count, int *total,
               int *unique, int *capacity, Word **words, int record_len,
               int record_capacity, bool multiline_comment,
               int comment_capacity, int *ID_count, char ***ID_array,
               int *ID_capacity, int *toxic_word_count, char ***toxic_word_list,
               Toxic_Word **toxic_words_found, int *toxic_found_element,
               int *toxic_found_capacity, int *toxic_found_count,
               Comment_Flag **flags, int *flag_count) {
  FILE *pFile = fopen("test.csv", "r");
  char line[2048];

  if (pFile == NULL) {
    printf("Error opening dataset file.\n");
    return;
  }

  char *comment = malloc(comment_capacity);
  if (!comment) {
    printf("Memory allocation failed for comment.\n");
    return;
  }

  char *record = malloc(record_capacity);
  if (!record) {
    printf("Memory allocation failed for record.\n");
    return;
  }

  // skip header
  fgets(line, sizeof(line), pFile);

  // read line by line
  while (fgets(line, sizeof(line), pFile) != NULL) {
    line[strcspn(line, "\r\n")] = '\0';

    // skip empty lines
    if (line[0] == '\0') {
      continue;
    }

    int line_len = strlen(line);

    // handle multi-line comments
    if (multiline_comment) {
      if (record_len + line_len + 2 > record_capacity) {
        record_capacity = record_len + line_len + 2;
        char *temp = realloc(record, record_capacity);
        if (temp == NULL) {
          printf("Error: Reallocation failed for record.\n");
          free(record);
          free(comment);
          return;
        }
        record = temp;
      }

      strcat(record, " ");
      strcat(record, line);
      record_len = strlen(record);
      // printf("multiline comment copied.\n");

      // check if line ends with closing quote
      if (line_len > 0 && line[line_len - 1] == '"') {
        multiline_comment = false;     // end of multiline comment
        record[record_len - 1] = '\0'; // remove closing quote

        size_t need = strlen(record) + 1;
        char *tmp = realloc(comment, need);
        if (!tmp) {
          printf("Error: Reallocation failed for comment.");
          return;
        }
        comment = tmp;
        comment_capacity = need;
        memcpy(comment, record, need);
        // printf("multiline comment last line copied.\n");
        record[0] = '\0'; // reset record
        record_len = 0;
      } else {
        continue;
      }
    } else {
      // reset comment at the start of new line
      comment[0] = '\0';

      if (line[0] == '"') {
        if (line_len + 1 > comment_capacity) {
          comment_capacity = line_len + 1;
          char *temp1 = realloc(comment, comment_capacity);
          if (temp1 == NULL) {
            printf("Error: Reallocation failed for comment.\n");
            free(comment);
            free(record);
            comment = NULL;
            record = NULL;
            return;
          }
          comment = temp1;
        }
        // Case 1: one line comment with id and comment
        if (line[line_len - 1] == '"') {
          char *id_token = strtok(line, ","); // extract ID
          ID_return(id_token, ID_array, ID_count, ID_capacity);

          // get the rest of the line
          char *comment_part = line + strlen(id_token) + 1;
          if (comment_part != NULL) {
            strcpy(comment, comment_part);
            //  printf("\nsingle line comment copied at ID: %s.\n", id_token);
          }
        }
        // Case 2: multi-line comment
        else {
          multiline_comment = true;
          char *id_token = strtok(line, ","); // extract ID
          ID_return(id_token, ID_array, ID_count, ID_capacity);

          // initilaize record
          record[0] = '\0';
          record_len = 0;

          // first line of comment
          char *first =
              line + strlen(id_token) +
              1; // get all comments first then only seperate word by word
          strcat(record, first); // record = record + line
          record_len = strlen(record);
          // printf("\nmultiline comment first line copied at ID: %s.\n",
          //        id_token);
          continue;
        }
      }
    }
    // process comment word by word
    if (comment[0] != '\0') {
      char *sentence = strtok(comment, " ");
      while (sentence != NULL) {
        clean_comment(sentence);
        if (!is_stopword(sentence, stopwords, stopword_count)) {
          if (*ID_count > 0) {
            bool is_toxic1 = detection(
                sentence, (*ID_array)[*ID_count - 1], toxic_word_count,
                toxic_word_list, toxic_words_found, toxic_found_element,
                toxic_found_capacity, toxic_found_count, flags, flag_count);
            total_words(sentence, (*ID_array)[*ID_count - 1], total, unique,
                        capacity, words);
            /*  printf("detection complete at ID: %s, words %s\n",
                     ID_array[ID_count - 1], sentence);
              if (is_toxic1) {
                printf("toxic words added.\n");
              } */
          }
        }
        sentence = strtok(NULL, " ");
      }
    }
    // reset after processing
    comment[0] = '\0';
  }

  // free memory
  free(record);
  free(comment);
  record = NULL;
  comment = NULL;

  fclose(pFile);
  // printf("Read file 1 is done\n");

  return;
}

// extract ID
void ID_return(char *id, char ***ID_array, int *ID_count, int *ID_capacity) {
  if (id != NULL && id[0] == '"') {
    int len = strlen(id);
    if (len > 1 && id[len - 1] == '"') {
      id[len - 1] = '\0';
    }

    // realloc ID_array if exceed capacity
    if (*ID_count >= *ID_capacity) {
      *ID_capacity *= 4;
      char **temp = realloc(*ID_array, *ID_capacity * sizeof(char *));
      if (temp == NULL) {
        printf("Memory reallocation failed for ID_array.\n");
        return;
      }

      *ID_array = temp;

      for (int i = *ID_count; i < *ID_capacity; i++) {
        (*ID_array)[i] = malloc(17 * sizeof(char));
        if (!(*ID_array)[i]) {
          printf("Memory allocation failed for ID_array[i].\n");
          return;
        }
        (*ID_array)[i][0] = '\0';
      }
    }

    // store ID without quotes
    if ((*ID_array)[*ID_count] != NULL) {
      strncpy((*ID_array)[*ID_count], id + 1, 16);
      (*ID_array)[*ID_count][16] = '\0';
      //   printf("ID_array updated at ID: %s", id);
      (*ID_count)++;
    }
  }
  return;
}

// Get comment ID and flags
void toxic_category(int *toxic_count, int *severe_count, int *obscene_count,
                    int *identity_count, int *insult_count, int *threat_count,
                    int *capacity_flag, Comment_Flag **flags, int *flag_count) {
  FILE *pFile = fopen("test_labels.csv", "r");
  if (pFile == NULL) {
    printf("Error opening test_labels.csv\n");
    return;
  }

  char line[50];

  fgets(line, sizeof(line), pFile); // skip header
  while (fgets(line, sizeof(line), pFile) != NULL) {
    line[strcspn(line, "\r\n")] = '\0';
    if (line[0] == '\0') {
      continue;
    }

    char temp[50];
    strncpy(temp, line, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    // tokenize line
    char *id1 = strtok(temp, ",");
    char *toxic1 = strtok(NULL, ",");
    char *severe1 = strtok(NULL, ",");
    char *obscene1 = strtok(NULL, ",");
    char *threat1 = strtok(NULL, ",");
    char *insult1 = strtok(NULL, ",");
    char *identity1 = strtok(NULL, ",");

    // if line lack of either one flag
    if (!id1 || !toxic1 || !severe1 || !obscene1 || !threat1 || !insult1 ||
        !identity1) {
      // printf("Skipped line at %s\n", id1);
      continue;
    }

    // realloc space if exceed
    if (*flag_count >= *capacity_flag) {
      *capacity_flag *= 2;
      Comment_Flag *tempf =
          realloc(*flags, (*capacity_flag) * sizeof(Comment_Flag));
      if (tempf == NULL) {
        printf("Error: Reallocation failed for flags.\n");
        return;
      }
      *flags = tempf;
    }

    // store flags and flag counts
    strncpy((*flags)[*flag_count].ID, id1, 16);
    (*flags)[*flag_count].ID[16] = '\0';
    if (strcmp(toxic1, "1") == 0) {
      (*flags)[*flag_count].toxic = true;
      (*toxic_count)++;
    }
    if (strcmp(severe1, "1") == 0) {
      (*flags)[*flag_count].severe_toxic = true;
      (*severe_count)++;
    }
    if (strcmp(obscene1, "1") == 0) {
      (*flags)[*flag_count].obscene = true;
      (*obscene_count)++;
    }
    if (strcmp(threat1, "1") == 0) {
      (*flags)[*flag_count].threat = true;
      (*threat_count)++;
    }
    if (strcmp(insult1, "1") == 0) {
      (*flags)[*flag_count].insult = true;
      (*insult_count)++;
    }
    if (strcmp(identity1, "1") == 0) {
      (*flags)[*flag_count].identity_hate = true;
      (*identity_count)++;
    }
    (*flag_count)++;
  }
  fclose(pFile);
  // printf("Loaded %d flags successfully", flag_count);
  return;
}

// find comment flag by id
Comment_Flag *find_comment_flag(const char *id, Comment_Flag **flags,
                                int *flag_count) {
  for (int i = 0; i < *flag_count; i++) {
    if (strcmp((*flags)[i].ID, id) == 0) {
      return &(*flags)[i];
    }
  }
  return NULL;
}

// load toxicwords.txt into global array
int load_toxicwords(int *toxic_word_count, int *toxic_word_capacity,
                    char ***toxic_word_list) {
  FILE *pFile1 = fopen("toxicwords.txt", "r");
  if (pFile1 == NULL) {
    printf("Error opening toxicwords file.\n");
    return 0;
  }

  char line1[MAX_LEN];
  *toxic_word_count = 0;

  while (fgets(line1, sizeof(line1), pFile1)) {
    line1[strcspn(line1, "\r\n")] = '\0';

    int length = strlen(line1);

    // realloc toxic_word_list if exceed capacity
    if (*toxic_word_count >= *toxic_word_capacity) {
      *toxic_word_capacity = (*toxic_word_count) * 2;
      char **temp =
          realloc(toxic_word_list, (*toxic_word_capacity) * sizeof(char *));
      if (temp == NULL) {
        printf("Memory reallocation failed for toxic_word_list.\n");
        fclose(pFile1);
        return 0;
      }

      *toxic_word_list = temp;

      // allocate new string slots for the new pointers
      for (int i = *toxic_word_count; i < *toxic_word_capacity; i++) {
        (*toxic_word_list)[i] = malloc(MAX_LEN * sizeof(char));
        if ((*toxic_word_list)[i] == NULL) {
          printf("Memory allocation failed for toxic_word_list[i].\n");
          return 0;
        }
        (*toxic_word_list)[i][0] = '\0';
      }
    }

    // store toxic word
    strncpy((*toxic_word_list)[*toxic_word_count], line1, length);
    (*toxic_word_list)[*toxic_word_count][length] = '\0';
    (*toxic_word_count)++;
  }

  fclose(pFile1);
  printf("Loaded %d toxic words from toxicwords.txt successfully\n",
         *toxic_word_count);
  return *toxic_word_count;
}

int add_toxicwords(char *update_word, int *toxic_word_count,
                   int *toxic_word_capacity, char ***toxic_word_list) {
  // Error Handling
  if (update_word[0] == '\0') {
    printf("No word entered to add to toxic words.\n");
    return *toxic_word_count;
  }

  // Clean the word
  clean_comment(update_word);

  // Check if toxicword already exists
  if (is_toxicwords(update_word, toxic_word_count, toxic_word_list)) {
    printf("Toxic word already exists in toxicwords.txt.\n");
    return *toxic_word_count;
  }

  // realloc toxic_word_list if exceed capacity
  if (*toxic_word_count >= *toxic_word_capacity) {
    (*toxic_word_capacity) *= 2;
    char **temp =
        realloc(*toxic_word_list, (*toxic_word_capacity) * sizeof(char *));
    if (temp == NULL) {
      printf("Memory reallocation failed for toxic_word_list.\n");
      return *toxic_word_count;
    }

    *toxic_word_list = temp;

    // allocate new string slots for the new pointers
    for (int i = *toxic_word_count; i < *toxic_word_capacity; i++) {
      (*toxic_word_list)[i] = malloc(MAX_LEN * sizeof(char));
      if ((*toxic_word_list)[i] == NULL) {
        printf("Memory allocation failed for toxic_word_list[i].\n");
        return *toxic_word_count;
      }
      (*toxic_word_list)[i][0] = '\0';
    }
  }

  FILE *pFile = fopen("toxicwords.txt", "a");
  char line[MAX_LEN];

  if (pFile == NULL) {
    printf("Error opening toxicwords file.\n");
    return 0;
  }

  // Append the new stopword
  fprintf(pFile, "%s\n", update_word);
  strcpy((*toxic_word_list)[*toxic_word_count], update_word);
  (*toxic_word_count)++;

  fclose(pFile);
  printf("Toxic Words updated.\n");
  return *toxic_word_count;
}

// check stopword
int is_toxicwords(char *comment, int *toxic_word_count,
                  char ***toxic_word_list) {
  for (int i = 0; i < *toxic_word_count; i++) {
    if (strcmp(comment, (*toxic_word_list)[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

// Toxic word detection and add into toxic_words_found
bool detection(char *comment, const char *id, int *toxic_word_count,
               char ***toxic_word_list, Toxic_Word **toxic_words_found,
               int *toxic_found_element, int *toxic_found_capacity,
               int *toxic_found_count, Comment_Flag **flags, int *flag_count) {
  bool is_toxic = false;
  for (int i = 0; i < *toxic_word_count; i++) {
    if (strcmp(comment, (*toxic_word_list)[i]) == 0) {
      is_toxic = true;
      break;
    }
  }

  if (!is_toxic) {
    return false;
  } else {
    // check if already in toxic_words_found
    for (int i = 0; i < *toxic_found_element; i++) {
      if ((*toxic_words_found)[i].word != NULL &&
          strcmp((*toxic_words_found)[i].word, comment) == 0) {
        (*toxic_words_found)[i].count++;
        (*toxic_found_count)++;
        return true;
      }
    }

    // check capacity and reallocate for toxic_words_found
    if (*toxic_found_element >= *toxic_found_capacity) {
      *toxic_found_capacity = *toxic_found_element + 1;
      Toxic_Word *temp = realloc(*toxic_words_found,
                                 (*toxic_found_capacity) * sizeof(Toxic_Word));
      if (temp == NULL) {
        printf("Memory reallocation failed for toxic_words_found.\n");
        return false;
      }
      *toxic_words_found = temp;

      // initialize new elements
      for (int i = *toxic_found_element; i < *toxic_found_capacity; i++) {
        (*toxic_words_found)[i].word = NULL;
        (*toxic_words_found)[i].count = 0;
      }
    }

    int index = *toxic_found_element;

    // add new toxic word
    (*toxic_words_found)[index].word = malloc(strlen(comment) + 1);
    if (!(*toxic_words_found)[index].word) {
      printf("Memory allocation failed for toxic_words_found[index].word!\n");
      return false;
    }

    // store into toxic_words_found
    Comment_Flag *cf = find_comment_flag(id, flags, flag_count);
    strcpy((*toxic_words_found)[index].word, comment);
    strcpy((*toxic_words_found)[index].id, id);
    (*toxic_words_found)[index].count = 1;

    if (cf != NULL) {
      (*toxic_words_found)[index].toxic = cf->toxic;
      (*toxic_words_found)[index].severe_toxic = cf->severe_toxic;
      (*toxic_words_found)[index].obscene = cf->obscene;
      (*toxic_words_found)[index].threat = cf->threat;
      (*toxic_words_found)[index].insult = cf->insult;
      (*toxic_words_found)[index].identity_hate = cf->identity_hate;
    }
    (*toxic_found_element)++;
    (*toxic_found_count)++;
  }
  return true;
}

// lowercase + remove punctuation
void clean_comment(char *comment) {
  int i, j = 0;
  // convert to lowercase
  for (i = 0; comment[i]; i++) {
    comment[i] = tolower(comment[i]);
    // remove punctuation + number
    if (!ispunct(comment[i]) && !isdigit(comment[i])) {
      comment[j] = comment[i];
      j++;
    }
  }
  comment[j] = '\0';
  return;
}

// read stopwords
int read_stopwords(char stopwords[][MAX_STOP]) {
  FILE *pFile = fopen("stopwords.txt", "r");
  char line[MAX_LEN];

  if (pFile == NULL) {
    printf("Error opening stopwords file.\n");
    return 0;
  }

  // read stopwords line by line and store into array
  int read_stopword = 0;
  while (fgets(line, sizeof(line), pFile) != NULL) {
    line[strcspn(line, "\r\n")] = '\0';
    strcpy(stopwords[read_stopword], line);
    read_stopword++;
  }

  fclose(pFile);
  printf("Stopwords done allocate\n");
  return read_stopword;
}

int add_stopwords(char stopwords[][MAX_STOP], char *update_word,
                  int stopword_count) {
  // Error Handling
  if (update_word[0] == '\0') {
    printf("No word entered to add to stopwords.\n");
    return stopword_count;
  }

  // Clean the word
  clean_comment(update_word);

  // Check if stopword already exists
  if (is_stopword(update_word, stopwords, stopword_count)) {
    printf("Stop word already exists in stopwords.txt.\n");
    return stopword_count;
  }

  FILE *pFile = fopen("stopwords.txt", "a");
  char line[MAX_LEN];

  if (pFile == NULL) {
    printf("Error opening stopwords file.\n");
    return 0;
  }

  // Append the new stopword
  fprintf(pFile, "%s\n", update_word);
  strcpy(stopwords[stopword_count], update_word);
  stopword_count++;

  fclose(pFile);
  printf("Stopwords updated.\n");
  return 1;
}

// check stopword
int is_stopword(char *comment, char stopwords[][MAX_STOP], int stopword_count) {
  for (int i = 0; i < stopword_count; i++) {
    if (strcmp(comment, stopwords[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

// count words
void total_words(char *comment, const char *id, int *total, int *unique,
                 int *capacity, Word **words_ptr) {
  Word *words = *words_ptr;
  if (comment[0] == '\0') {
    return;
  }

  (*total)++;

  // check if word more than 1
  for (int i = 0; i < *unique; i++) {
    if (strcmp(words[i].word, comment) == 0) {
      words[i].count++;
      return;
    }
  }

  // realloc words array if exceed capacity
  if (*unique >= *capacity) {
    *capacity *= 2;
    *words_ptr = realloc(words, (*capacity) * sizeof(Word));
    if (*words_ptr == NULL) {
      printf("Memory allocation failed for words!\n");
      return;
    }
    words = *words_ptr;
  }

  // add new unique word
  words[*unique].word = malloc(strlen(comment) + 1);
  if (!words[*unique].word) {
    printf("Memory allocation failed for words[*unique].word!\n");
    return;
  }
  strcpy(words[*unique].word, comment);

  words[*unique].count = 1;
  (*unique)++;
  return;
}

// sort toxic words based on number of counts
void SelectionSort_Toxic(Toxic_Word toxic_words_found[], int n) {
  for (int i = 0; i < n; i++) {
    int max_idx = i;
    for (int j = i + 1; j < n; j++) {
      if (toxic_words_found[j].count > toxic_words_found[max_idx].count) {
        max_idx = j;
      }
    }
    // swap entire struct
    Toxic_Word temp = toxic_words_found[max_idx];
    toxic_words_found[max_idx] = toxic_words_found[i];
    toxic_words_found[i] = temp;
  }
  // printf("Selection sort for toxic words done\n");
  return;
}

// sort unique words based on number of counts
void SelectionSort_Unique(Word words[], int *n) {
  for (int i = 0; i < *n; i++) {
    int max_idx = i;
    for (int j = i + 1; j < *n; j++) {
      if (words[j].count > words[max_idx].count) {
        max_idx = j;
      }
    }
    // swap entire struct
    Word temp = words[max_idx];
    words[max_idx] = words[i];
    words[i] = temp;
  }
  // printf("Selection sort for unique words done\n");
  return;
}

// applied code from https://www.geeksforgeeks.org/c/qsort-function-in-c/
// copy toxic words array
void copywords_toxic(Toxic_Word *newword, Toxic_Word *oldword, int n) {
  for (int i = 0; i < n; i++) {
    if (oldword[i].word == NULL) {
      printf("Array of oldword is empty.\n");
      newword[i].word[0] = '\0';
      newword[i].count = 0;
    }

    // int length = strlen(oldword[i].word);
    strcpy(newword[i].word, oldword[i].word);
    newword[i].count = oldword[i].count;
  }
  // printf("Copy words for toxic done.\n");
  return;
}

// copy unique words array
void copywords_unique(Word *tempword, Word *oldword, int n) {
  for (int i = 0; i < n; i++) {
    if (oldword[i].word == NULL) {
      printf("Array of oldword is empty.\n");
      tempword[i].word[0] = '\0';
      tempword[i].count = 0;
    }

    strcpy(tempword[i].word, oldword[i].word);
    tempword[i].count = oldword[i].count;
  }
  //  printf("Copy words for unique done.\n");
  return;
}

// compare words for qsort
int comparewords(const void *a, const void *b) {
  Word *worda = (Word *)a;
  Word *wordb = (Word *)b;

  return strcmp(worda->word, wordb->word);
}

// print summary
void save_report(Toxic_Word words[], Toxic_Word newword[], Word word[],
                 Word tempword[], int m, int *unique, int *total,
                 int *toxic_count, int *severe_count, int *obscene_count,
                 int *threat_count, int *insult_count, int *identity_count,
                 int *toxic_found_count, int *toxic_found_element,
                 int *ID_count) {
  FILE *pFile = fopen("analysis_report.tst", "w");
  if (pFile == NULL) {
    printf("Error creating analysis_report.tst.\n");
    return;
  }

  char symbol[400] = "";

  fprintf(pFile, "==================================== TEXT ANALYSIS RESULT "
                 "==============================================\n");
  fprintf(pFile, "Total Words Without Stopwords: %d\n", *total);
  fprintf(pFile, "Total Unique Words: %d\n", *unique);
  fprintf(pFile, "Total Toxic Words: %d\n", *toxic_found_count);
  fprintf(pFile, "Total Unique Toxic Words: %d\n", *toxic_found_element);
  fprintf(pFile, "Percentage of Toxic Words: %.2f%%\n\n",
          ((float)(*toxic_found_count) / *total) * 100);

  fprintf(pFile, "Percentage of Toxic Sentences by Severity\n");
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "%-10s | %-10s | %-10s | %-10s | %-10s | %-10s | %-10s |\n",
          "   Flags  ", "  Toxic  ", " Severe Toxic ", "  Obscene", "  Threat",
          "  Insult", "Identity Hate");
  fprintf(
      pFile,
      "%-10s |   %-8d |     %-10d |   %-8d |   %-8d |   %-8d |     %-9d |\n",
      "   Count", *toxic_count, *severe_count, *obscene_count, *threat_count,
      *insult_count, *identity_count);
  fprintf(pFile,
          "%-10s |   %-8.2f |     %-10.2f |   %-8.2f |   %-8.2f |   %-8.2f |   "
          "  %-9.2f |\n",
          "Percentage", ((float)*toxic_count / *ID_count) * 100,
          ((float)*severe_count / *ID_count) * 100,
          ((float)*obscene_count / *ID_count) * 100,
          ((float)*threat_count / *ID_count) * 100,
          ((float)*insult_count / *ID_count) * 100,
          ((float)*identity_count / *ID_count) * 100);

  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile,
          "\nBar Chart of Toxic Words with Alphabetical Order (Top 20)\n");
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
          "Percentage (%)");
  for (int i = 0; i < *unique && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength =
        (int)(log(newword[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && newword[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate toxicity percentage
    float percent = ((float)newword[i].count / *total) * 100;

    fprintf(pFile, "%-15s | %-60s | %-5d | %-6.2f\n", newword[i].word, symbol,
            newword[i].count, percent);
  }

  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "\n");
  fprintf(pFile, "\n");
  fprintf(pFile, "\n");
  fprintf(pFile,
          "Bar Chart of Toxic Words only based on Frequencies (Top 20)\n");
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
          "Percentage (%)");
  for (int i = 0; i < m && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength = (int)(log(words[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && words[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate toxicity percentage
    float percent = ((float)words[i].count / *total) * 100;

    fprintf(pFile, "%-15s | %-60s | %-5d | %-6.2f\n", words[i].word, symbol,
            words[i].count, percent);
  }
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "\n");
  fprintf(pFile, "\n");
  fprintf(pFile, "\n");
  fprintf(pFile,
          "Bar Chart of Unique Words only based on Frequencies (Top 20)\n");
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
          "Percentage (%)");
  for (int i = 0; i < *unique && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength = (int)(log(word[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && word[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate percentage
    float percent = ((float)word[i].count / *total) * 100;

    fprintf(pFile, "%-15s | %-60s | %-5d | %-6.2f\n", word[i].word, symbol,
            word[i].count, percent);
  }
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "\n");
  fprintf(pFile, "\n");
  fprintf(pFile, "\n");
  fprintf(pFile,
          "Bar Chart of Unique Words with Alphabetical Order (Top 20)\n");
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fprintf(pFile, "%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
          "Percentage (%)");
  for (int i = 0; i < *unique && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength =
        (int)(log(tempword[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && tempword[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate percentage
    float percent = ((float)tempword[i].count / *total) * 100;

    fprintf(pFile, "%-15s | %-60s | %-5d | %-6.2f\n", tempword[i].word, symbol,
            tempword[i].count, percent);
  }
  fprintf(pFile, "-------------------------------------------------------------"
                 "------------------------------------------\n");
  fclose(pFile);
  printf("Saved to analysis_report.txt\n");
  return;
}

void save_csv(Toxic_Word words[], int m, int *total, int *toxic_found_count) {
  FILE *csv = fopen("analysis_report.csv", "w");
  if (csv == NULL) {
    printf("Error creating analysis_report.csv\n");
    return;
  }

  fprintf(csv, "Toxic Word,Count,Percentage(%%)\n");
  for (int i = 0; i < m; i++) {
    // calculate toxicity percentage
    float percent = ((float)words[i].count / *total) * 100;

    fprintf(csv, "%s,%d,%f\n", words[i].word, words[i].count, percent);
  }

  fclose(csv);
  printf("Saved to analysis_report.csv\n");
  return;
}

// print summary
void result_1(int *unique, int *total, int *toxic_found_count,
              int *toxic_found_element) {
  printf("\n\n=== TEXT ANALYSIS RESULT ===\n\n");
  printf("Total Words Without Stopwords: %d\n", *total);
  printf("Total Unique Words: %d\n", *unique);
  printf("Total Toxic Words: %d\n", *toxic_found_count);
  printf("Total Unique Toxic Words: %d\n", *toxic_found_element);
  printf("Percentage of Toxic Words: %.2f%%\n\n",
         ((float)(*toxic_found_count) / *total) * 100);

  return;
}

// print bar chart of toxic words with alphabetical order
void result_2(Toxic_Word newword[], int m, int *total) {
  char symbol[400] = "";
  printf("\n\n=== TEXT ANALYSIS RESULT ===\n\n");
  printf("Bar Chart of Toxic Words with Alphabetical Order (Top 20)\n");
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  printf("%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
         "Percentage (%)");
  for (int i = 0; i < m && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength =
        (int)(log(newword[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && newword[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate toxicity percentage
    float percent = ((float)newword[i].count / *total) * 100;

    printf("%-15s | %-60s | %-5d | %-6.2f\n", newword[i].word, symbol,
           newword[i].count, percent);
  }
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  return;
}

// print bar chart of toxic words based on frequency
void result_3(Toxic_Word words[], int m, int *total) {
  char symbol[400] = "";
  printf("\n\n=== TEXT ANALYSIS RESULT ===\n\n");
  printf("Bar Chart of Toxic Words only based on Frequencies (Top 20)\n");
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  printf("%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
         "Percentage (%)");
  for (int i = 0; i < m && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length 
    int barlength = (int)(log(words[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && words[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate toxicity percentage
    float percent = ((float)words[i].count / *total) * 100;

    printf("%-15s | %-60s | %-5d | %-6.2f\n", words[i].word, symbol,
           words[i].count, percent);
  }
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  return;
}

// print bar chart of unique words based on frequency
void result_4(Word word[], int *unique, int *total) {
  char symbol[400] = "";
  printf("\n\n=== TEXT ANALYSIS RESULT ===\n\n");
  printf("Bar Chart of Unique Words only based on Frequencies (Top 20)\n");
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  printf("%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
         "Percentage (%)");
  for (int i = 0; i < *unique && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength = (int)(log(word[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && word[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate percentage
    float percent = ((float)word[i].count / *total) * 100;

    printf("%-15s | %-60s | %-5d | %-6.2f\n", word[i].word, symbol,
           word[i].count, percent);
  }
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  return;
}

// print bar chart of unique words with alphabetical order
void result_5(Word tempword[], int *unique, int *total) {
  char symbol[400] = "";
  printf("\n\n=== TEXT ANALYSIS RESULT ===\n\n");
  printf("Bar Chart of Unique Words with Alphabetical Order (Top 20)\n");
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  printf("%-15s | %-60s | %-5s | %-6s\n", "Words", "Bar Chart", "Count",
         "Percentage (%)");
  for (int i = 0; i < *unique && i < 20; i++) {
    // reset
    symbol[0] = '\0';

    // calculate bar length
    int barlength =
        (int)(log(tempword[i].count + 1) / log(*total + 1) * MAX_BAR);
    if (barlength == 0 && tempword[i].count > 0) {
      barlength = 1;
    }

    // build bar chart
    for (int j = 0; j < barlength; j++) {
      symbol[j] = '#';
    }
    symbol[barlength] = '\0';

    // calculate percentage
    float percent = ((float)tempword[i].count / *total) * 100;

    printf("%-15s | %-60s | %-5d | %-6.2f\n", tempword[i].word, symbol,
           tempword[i].count, percent);
  }
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  return;
}

// print toxic severity result
void result_6(int *toxic_count, int *severe_count, int *obscene_count,
              int *threat_count, int *insult_count, int *identity_count,
              int *ID_count) {
  printf("\n\n=== TEXT ANALYSIS RESULT ===\n\n");
  printf("Percentage of Toxic Sentences by Severity\n");
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
  printf("%-10s | %-10s | %-10s | %-10s | %-10s | %-10s | %-10s |\n",
         "   Flags  ", "  Toxic  ", " Severe Toxic ", "  Obscene", "  Threat",
         "  Insult", "Identity Hate");
  printf("%-10s |   %-8d |     %-10d |   %-8d |   %-8d |   %-8d |     %-9d |\n",
         "   Count", *toxic_count, *severe_count, *obscene_count, *threat_count,
         *insult_count, *identity_count);
  printf("%-10s |   %-8.2f |     %-10.2f |   %-8.2f |   %-8.2f |   %-8.2f |    "
         " %-9.2f |\n",
         "Percentage", ((float)*toxic_count / *ID_count) * 100,
         ((float)*severe_count / *ID_count) * 100,
         ((float)*obscene_count / *ID_count) * 100,
         ((float)*threat_count / *ID_count) * 100,
         ((float)*insult_count / *ID_count) * 100,
         ((float)*identity_count / *ID_count) * 100);
  printf("---------------------------------------------------------------------"
         "----------------------------------\n");
}