import pandas as pd
import re

# --- 0. Configuration ---
# path to the text file
file_path = 'text.csv'
# paths to the filter files
csv_files = ['jmena.csv', 'kraje.csv', 'KU.csv', 'okresy.csv', 'obce.csv', 'prijmeni.csv']
# encoding of the files
utf_code = 'utf-8'
# tag for the name
name_tag = '<TokenJmeno>'
# tag for the number
number_tag = '<TokenCislo>'
# path to the output file
output_file = 'text_final.csv'

# --- 1. Load the main text data ---
# load the .csv file into a pandas dataframe
df = pd.read_csv(file_path, encoding=utf_code)
# convert the entire text column to lowercase
df['text'] = df['text'].astype(str).str.lower()
# print a message
print(f"Successfully loaded {file_path} and converted text to lowercase.")



# --- 2. Gather and combine all words from the filter CSVs ---
# create a set to store the words to filter
words_to_filter = set()

# loop through the filter files
for file in csv_files:
    # remove header=None so pandas reads the first row as column names
    temp_df = pd.read_csv(file, encoding=utf_code)
    # target the 'JMENO' column explicitly
    if 'JMENO' in temp_df.columns:
        # drop empty, convert to string, strip spaces, and convert to lowercase
        words = temp_df['JMENO'].dropna().astype(str).str.strip().str.lower().tolist()
        # update the set with the words
        words_to_filter.update(words)
        # print a message with the number of words loaded   
        print(f"Loaded {len(words)} items from {file}")
    else:
        # print a message if the column 'JMENO' is not found
        print(f"Warning: Column 'JMENO' not found in {file}. Skipping this file.")


# --- 3. Prepare the dictionary ---
# build the regex pattern using word boundaries (\b) and the words to filter
name_pattern = r'\b(' + '|'.join(words_to_filter) + r')\b'

# --- 4. Replace names ---
# print a message that the names are being filtered
print("\nFiltering names. This might take a moment...")
# replace the names with the tag '<TokenJmeno>'
df['text'] = df['text'].str.replace(name_pattern, name_tag, regex=True)

# --- 5. Replace numbers with 3 or MORE digits with the tag ---
# print a message that the numbers are being filtered
print("Filtering numbers...")
# filter the numbers with 3 or more digits and replace them with the tag '<TokenCislo>'
df['text'] = df['text'].str.replace(r'\b\d{3,}\b', number_tag, regex=True)

# --- 6. Delete numbers with 1 or 2 digits ---
# delete the numbers with 1 or 2 digits
df['text'] = df['text'].str.replace(r'\b\d{1,2}\b', '', regex=True)

# --- 7. Clean up the spacing ---
# replace multiple spaces with a single space and strip the spaces from the very ends of the text
df['text'] = df['text'].str.replace(r'\s+', ' ', regex=True).str.strip()

# --- 8. Save the final dataset ---
# save the final dataset to a .csv file
df.to_csv(output_file, index=False, encoding=utf_code)
# print a message that the final dataset has been saved
print(f"\nDone! Final dataset saved as: {output_file}")
