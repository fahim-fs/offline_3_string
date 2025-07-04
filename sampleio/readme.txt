For the tf, idf, tfidf, and stat commands to work, you need to process your documents through the NLP pipeline steps in order:

1. set: Load your documents.
2. normalize: (Optional, but good practice for consistency) Convert documents to lowercase.
3. tokenize: Break documents into words.
4. remove_stop: Filter out common stop words.
5. stem: Apply stemming to reduce words to their base form.
tf / idf / tfidf / stat: Now, these commands will have the fully processed (normalized, tokenized, stop-word removed, and stemmed) data to work with.