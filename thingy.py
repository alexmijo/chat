import random
import numpy as np

# Set the number of possible features (word characteristics) and actions (generated words)
NUM_FEATURES = 3
NUM_ACTIONS = 10

# Initialize the Q table (a matrix of size NUM_FEATURES x NUM_ACTIONS)
# Each row represents a feature, and each column represents an action
# The values in the matrix represent the expected reward for taking an action given a certain feature
# The Q table is initialized to small random values
Q = np.random.rand(NUM_FEATURES, NUM_ACTIONS) * 0.1

def generate_word():
    vowels = 'aeiouy'
    consonants = 'bcdfghjklmnpqrstvwxyz'
    word = ''
    length = random.randint(3, 10)  # generate a random word length between 3 and 10
    for i in range(length):
        if i == 0:  # first letter must be a consonant
            word += random.choice(consonants)
        elif i == length - 1:  # last letter must be a vowel
            word += random.choice(vowels)
        else:  # for all other letters, alternate between vowels and consonants
            if word[-1] in vowels:
                word += random.choice(consonants)
            else:
                word += random.choice(vowels)
    return word

def is_good_word(word):
    if len(word) < 3 or len(word) > 10:  # check if word length is within the desired range
        return False
    if word.endswith('ing'):  # check if word ends with the suffix "-ing"
        return False
    for i in range(len(word)-1):
        if word[i] in 'aeiouy' and word[i+1] in 'aeiouy':  # check if there are at least two consecutive vowels
            return True
    return False

def generate_good_words(num_words):
    words = []
    while len(words) < num_words:
        word = generate_word()
        if is_good_word(word) and word not in words:
            words.append(word)
    return words

def get_features(word):
    # Define the features of a word as a list of binary values
    # The first feature is whether the word has at least two consecutive vowels
    # The second feature is whether the word ends with a vowel
    # The third feature is whether the word contains the letter 'q'
    features = []
    for i in range(NUM_FEATURES):
        if i == 0:
            features.append(int(any(word[i] in 'aeiouy' and word[i+1] in 'aeiouy' for i in range(len(word)-1))))
        elif i == 1:
            features.append(int(word[-1] in 'aeiouy'))
        elif i == 2:
            features.append(int('q' in word))
    return features

def get_action(features, epsilon):
    # Choose an action (word) based on the current state (features) and the exploration-exploitation tradeoff (epsilon)
    if random.random() < epsilon:
        # Choose a random action with probability epsilon
        action = random.choice(range(NUM_ACTIONS))
    else:
        # Choose the action with the highest expected reward given the current state (with ties broken randomly)
        max_q = np.max(Q[:, features], axis=1)
        indices = np.where(Q[:, features] == max_q)[0]
        action = random.choice(indices)
    return action

def update_q_table(features, action, reward, alpha, gamma):
    # Update the Q table based on the current state (features), action, reward, learning rate (alpha), and discount factor (gamma)
    Q[features, action] = (1 - alpha) * Q[features, action] + alpha * (reward + gamma * np.max(Q[:, features]))

def choose_best_word(words):
    print('Please choose the best word from the following list:')
    for i, word in enumerate(words):
        print(f'{i+1}. {word}')
    while True:
        choice = input('Enter the number of your choice: ')
        if choice.isdigit() and int(choice) in range(1, len(words)+1):
            return words[int(choice)-1]

def generate_good_words_with_feedback(num_words, num_episodes, alpha=0.1, gamma=0.9, epsilon=0.1):
    # Train the Q table using reinforcement learning
    for episode in range(num_episodes):
        # Generate a list of "good" words
        words = generate_good_words(num_words)
        # Get the user's ratings of the words
        ratings = []
        for word in words:
            print(f'How much do you like the word "{word}"? (1-10)')
            while True:
                rating = input()
                if rating.isdigit() and int(rating) in range(1, 11):
                    ratings.append(int(rating))
                    break
        # Update the Q table based on the user's ratings
        for i, word in enumerate(words):
            features = get_features(word)
            action = i
            reward = ratings[i]
            update_q_table(features, action, reward, alpha, gamma)
    # Use the trained Q table to generate new lists of words based on the user's preferences
    while True:
        # Get the current state (features) by asking the user for their preferences
        print('Please rate the following features on a scale of 1-10:')
        features = []
        for i in range(NUM_FEATURES):
            print(f'{i+1}. {FEATURE_NAMES[i]}')
            while True:
                rating = input()
                if rating.isdigit() and int(rating) in range(1, 11):
                    features.append(int(rating))
                    break
        # Choose a list of words based on the current state (features)
        actions = np.argsort(Q[:, features])[-NUM_ACTIONS:][::-1]
        words = [WORDS[i] for i in actions]
        best_word = choose_best_word(words)
        print(f'You chose: {best_word}')
        input_text = input('Type "again" to generate a new list of words, or "quit" to exit: ')
        if input_text.lower() == 'again':
            continue
        elif input_text.lower() == 'quit':
            break

# Define the possible feature names and words
FEATURE_NAMES = ['contains two consecutive vowels', 'ends with a vowel', 'contains the letter "q"']
WORDS = generate_good_words(100)

# Example usage:
generate_good_words_with_feedback(10, 1000)
