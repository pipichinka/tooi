import os
import string
import nltk
from nltk.corpus import stopwords
from nltk.stem import SnowballStemmer

nltk.download('stopwords')

# NORM

def remove_punctuation(text):
    translator = str.maketrans('', '', string.punctuation)
    return text.translate(translator)


def remove_stopwords(text):
    stop_words = set(stopwords.words('russian'))
    words = text.split()
    filtered_words = [word for word in words if word.lower() not in stop_words]
    return ' '.join(filtered_words)


def lemmatize_text(text):
    stemmer = SnowballStemmer('russian')
    words = text.split()
    stemmed_words = [stemmer.stem(word) for word in words]
    return ' '.join(stemmed_words)


def process_file(input_file, output_dir):
    file_name = os.path.splitext(os.path.basename(input_file))[0]
    output_file = os.path.join(output_dir, file_name + '.txt')

    with open(input_file, 'r', encoding='utf-8') as file:
        text = file.read()
        if file_name == "info":
            ready_text = text
        else:
            text_without_punctuation = remove_punctuation(text)
            text_without_stopwords = remove_stopwords(text_without_punctuation)
            stemmed_text = lemmatize_text(text_without_stopwords)
            ready_text = stemmed_text

    with open(output_file, 'w', encoding='utf-8') as file:
        file.write(ready_text)


def process_files_in_directory(input_dir, output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for file_name in os.listdir(input_dir):
        input_file = os.path.join(input_dir, file_name)
        if os.path.isfile(input_file):
            process_file(input_file, output_dir)


input_directory = '/home/mariia/TOOI_TEXTS'
output_directory = input_directory + '_PROCESSED_ALTERNATIVE'

process_files_in_directory(input_directory, output_directory)
