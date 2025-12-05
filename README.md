# Toxic-Word-Analyzer
A C-based application designed to analyze text datasets for toxic content. This tool processes comments from CSV files, filters stopwords, detects toxic terminology based on a customizable dictionary, and generates statistical reports including ASCII bar charts and severity categorization.

# Features

- Text Processing: removing punctuation and digits, case normalization, and stop word filtering.
- Toxic Detection: Identifies toxic words based on a loaded dictionary.
- Dynamic Dictionaries: Allows users to add new stop words or toxic words during runtime.
- Statistical Analysis:
  - Total unique words vs. toxic words.
  - Classification of toxicity severity (Toxic, Severe, Obscene, Threat, Insult, Identity Hate).
- Reporting:
  - ASCII Bar Charts for unique words and toxic words frequency, with or without alphabetical order. 
  - Exports results to `.txt` and `.csv` formats.

# Prerequisites
- GCC Compiler
- Standard C Libraries (`stdlib.h`, `stdio.h`, `string.h`, `math.h`, `ctype.h`, `stdbool.h`)
- Input Files (must be in the same directory):
  - test.csv (The dataset containing ID and comments)
  - test_labels.csv (The dataset labels)
  - stopwords.txt (List of words to ignore)
  - toxicwords.txt (List of words to flag)

# Important Workflow Instruction
To ensure accurate analysis, please follow this specific order of operations, especially when updating dictionaries:
1.	Standard Run:
-	Select Option 1 to Load Text Files.
-	Select Option 3 to Analyze Files.
  
2.	Updating Dictionaries (Crucial Step):
If you use Option 2 (Update Dictionaries) to add new toxic words or stopwords, you MUST reload the files for the changes to take effect in the analysis.
-	Step A: Run Option 2 (Update Dictionaries).
-	Step B: Run Option 1 (Load Text Files) immediately after. This reloads the updated toxicwords.txt or stopwords.txt into memory based on option chosen.
-	Step C: Run Option 3 (Analyze Files). This performs the detection logic using the new data and resets internal pointers for a clean state.

# Menu Options
-	Option 1. Load Text Files: Reads stopwords.txt and toxicwords.txt into memory.
-	Option 2. Update Dictionaries: Append new words to the text files. (Requires reloading Option 1 afterwards).
-	Option 3. Analyze Files: Processes test.csv and test_labels.csv, matches flags, and calculates statistics.
-	Options 4-7. View Statistics: Display various counts, percentages, and ASCII bar charts in the console.
-	Options 8-9. Save Reports: Export the findings to external files.
-	Option 0: Exit Program.

# Output
The program generates the following reports:
-	analysis_report.tst: A detailed summary including text statistics and ASCII charts.
-	analysis_report.csv: A raw data dump of toxic words and sorted according to their frequencies.

# Dataset Source
This project uses the following datasets:
- Jigsaw Toxic Comment Classification Challenge:
  https://www.kaggle.com/datasets/julian3833/jigsaw-toxic-comment-classification-challenge?select=test.csv
